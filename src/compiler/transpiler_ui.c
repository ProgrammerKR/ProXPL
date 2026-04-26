#include "transpiler_ui.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

static void transpileStmt(Stmt *stmt, FILE *html, FILE *js, int indent);
static void transpileExpr(Expr *expr, FILE *out, bool isJS);

static void escapeHtml(const char* str, FILE* out) {
    while (*str) {
        if (*str == '<') fprintf(out, "&lt;");
        else if (*str == '>') fprintf(out, "&gt;");
        else if (*str == '&') fprintf(out, "&amp;");
        else if (*str == '"') fprintf(out, "&quot;");
        else if (*str == '\\'') fprintf(out, "&#39;");
        else fputc(*str, out);
        str++;
    }
}

static void printIndent(FILE *out, int level) {
    for (int i = 0; i < level; i++) fprintf(out, "  ");
}

// =====================================================
//  Tag Mapper: ProXPL Component Names → HTML Tags
// =====================================================
static const char *mapTag(const char *tag) {
    // Layout
    if (strcmp(tag, "Container") == 0) return "div";
    if (strcmp(tag, "Section")   == 0) return "section";
    if (strcmp(tag, "Main")      == 0) return "main";
    if (strcmp(tag, "Aside")     == 0) return "aside";
    if (strcmp(tag, "Article")   == 0) return "article";
    if (strcmp(tag, "Header")    == 0) return "header";
    if (strcmp(tag, "Footer")    == 0) return "footer";
    if (strcmp(tag, "Nav")       == 0) return "nav";
    if (strcmp(tag, "Details")   == 0) return "details";
    if (strcmp(tag, "Summary")   == 0) return "summary";
    if (strcmp(tag, "Dialog")    == 0) return "dialog";
    if (strcmp(tag, "Figure")    == 0) return "figure";
    if (strcmp(tag, "Figcaption") == 0) return "figcaption";

    // Typography
    if (strcmp(tag, "Text")      == 0) return "span";
    if (strcmp(tag, "P")         == 0) return "p";
    if (strcmp(tag, "H1")        == 0) return "h1";
    if (strcmp(tag, "H2")        == 0) return "h2";
    if (strcmp(tag, "H3")        == 0) return "h3";
    if (strcmp(tag, "H4")        == 0) return "h4";
    if (strcmp(tag, "H5")        == 0) return "h5";
    if (strcmp(tag, "H6")        == 0) return "h6";
    if (strcmp(tag, "Strong")    == 0) return "strong";
    if (strcmp(tag, "Em")        == 0) return "em";
    if (strcmp(tag, "Small")     == 0) return "small";
    if (strcmp(tag, "Mark")      == 0) return "mark";
    if (strcmp(tag, "Code")      == 0) return "code";
    if (strcmp(tag, "Pre")       == 0) return "pre";
    if (strcmp(tag, "Kbd")       == 0) return "kbd";
    if (strcmp(tag, "Hr")        == 0) return "hr";
    if (strcmp(tag, "Br")        == 0) return "br";
    if (strcmp(tag, "Span")      == 0) return "span";

    // Interaction
    if (strcmp(tag, "Button")    == 0) return "button";
    if (strcmp(tag, "Link")      == 0) return "a";
    if (strcmp(tag, "Icon")      == 0) return "i";

    // Forms
    if (strcmp(tag, "Form")      == 0) return "form";
    if (strcmp(tag, "Input")     == 0) return "input";
    if (strcmp(tag, "Textarea")  == 0) return "textarea";
    if (strcmp(tag, "Label")     == 0) return "label";
    if (strcmp(tag, "Select")    == 0) return "select";
    if (strcmp(tag, "Option")    == 0) return "option";
    if (strcmp(tag, "Fieldset")  == 0) return "fieldset";
    if (strcmp(tag, "Legend")    == 0) return "legend";

    // Lists
    if (strcmp(tag, "List")      == 0) return "ul";
    if (strcmp(tag, "OrderedList") == 0) return "ol";
    if (strcmp(tag, "Item")      == 0) return "li";

    // Tables
    if (strcmp(tag, "Table")     == 0) return "table";
    if (strcmp(tag, "Thead")     == 0) return "thead";
    if (strcmp(tag, "Tbody")     == 0) return "tbody";
    if (strcmp(tag, "Tfoot")     == 0) return "tfoot";
    if (strcmp(tag, "Tr")        == 0) return "tr";
    if (strcmp(tag, "Th")        == 0) return "th";
    if (strcmp(tag, "Td")        == 0) return "td";
    if (strcmp(tag, "Caption")   == 0) return "caption";

    // Media
    if (strcmp(tag, "Image")     == 0) return "img";
    if (strcmp(tag, "Video")     == 0) return "video";
    if (strcmp(tag, "Audio")     == 0) return "audio";
    if (strcmp(tag, "Canvas")    == 0) return "canvas";
    if (strcmp(tag, "Iframe")    == 0) return "iframe";
    if (strcmp(tag, "Source")    == 0) return "source";
    if (strcmp(tag, "Track")     == 0) return "track";

    // Progress
    if (strcmp(tag, "Progress")  == 0) return "progress";
    if (strcmp(tag, "Meter")     == 0) return "meter";

    // Misc
    if (strcmp(tag, "Script")    == 0) return "script";
    if (strcmp(tag, "Style")     == 0) return "style";
    if (strcmp(tag, "Template")  == 0) return "template";

    // Fallback: use tag as-is (supports lowercase HTML tags directly)
    return tag;
}

// Self-closing HTML tags
static int isSelfClosing(const char *htmlTag) {
    return (
        strcmp(htmlTag, "img")    == 0 ||
        strcmp(htmlTag, "input")  == 0 ||
        strcmp(htmlTag, "br")     == 0 ||
        strcmp(htmlTag, "hr")     == 0 ||
        strcmp(htmlTag, "meta")   == 0 ||
        strcmp(htmlTag, "link")   == 0 ||
        strcmp(htmlTag, "source") == 0 ||
        strcmp(htmlTag, "track")  == 0
    );
}

// =====================================================
//  Prop / Attribute Name Mapping
// =====================================================
static const char *mapPropName(const char *name) {
    if (strcmp(name, "className") == 0) return "class";
    if (strcmp(name, "onClick")   == 0) return "@click";
    if (strcmp(name, "onChange")  == 0) return "@change";
    if (strcmp(name, "onInput")   == 0) return "@input";
    if (strcmp(name, "onSubmit")  == 0) return "@submit";
    if (strcmp(name, "onFocus")   == 0) return "@focus";
    if (strcmp(name, "onBlur")    == 0) return "@blur";
    if (strcmp(name, "onHover")   == 0) return "@mouseenter";
    if (strcmp(name, "xBind")     == 0) return ":";
    if (strcmp(name, "xModel")    == 0) return "x-model";
    if (strcmp(name, "xShow")     == 0) return "x-show";
    if (strcmp(name, "xIf")       == 0) return "x-if";
    if (strcmp(name, "xFor")      == 0) return "x-for";
    if (strcmp(name, "xText")     == 0) return "x-text";
    if (strcmp(name, "xHtml")     == 0) return "x-html";
    if (strcmp(name, "xOn")       == 0) return "x-on";
    if (strcmp(name, "xData")     == 0) return "x-data";
    if (strcmp(name, "xRef")      == 0) return "x-ref";
    if (strcmp(name, "xTransition") == 0) return "x-transition";
    if (strcmp(name, "htmlFor")   == 0) return "for";
    if (strcmp(name, "tabIndex")  == 0) return "tabindex";
    if (strcmp(name, "readOnly")  == 0) return "readonly";
    if (strcmp(name, "autoFocus") == 0) return "autofocus";
    if (strcmp(name, "autoPlay")  == 0) return "autoplay";
    return name;
}

// =====================================================
//  Shared Statement Transpiler
// =====================================================
static void transpileStmt(Stmt *stmt, FILE *html, FILE *js, int indent) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_UI_STATE:
            if (!js) break;
            printIndent(js, indent);
            fprintf(js, "%s: ", stmt->as.ui_state.name);
            if (stmt->as.ui_state.initializer) {
                transpileExpr(stmt->as.ui_state.initializer, js, true);
            } else {
                fprintf(js, "null");
            }
            fprintf(js, ",\n");
            break;

        case STMT_UI_ACTION:
            if (!js) break;
            printIndent(js, indent);
            fprintf(js, "%s() {\n", stmt->as.ui_action.name);
            // Transpile action body statements if they exist
            if (stmt->as.ui_action.body) {
                for (int i = 0; i < stmt->as.ui_action.body->count; i++) {
                    transpileStmt(stmt->as.ui_action.body->items[i], html, js, indent + 1);
                }
            } else {
                fprintf(js, "    console.log('Action %s triggered');\n", stmt->as.ui_action.name);
            }
            printIndent(js, indent);
            fprintf(js, "},\n");
            break;

        case STMT_UI_COMPONENT: {
            if (!html) break;
            const char *htmlTag = mapTag(stmt->as.ui_component.tag);
            printIndent(html, indent);
            fprintf(html, "<%s", htmlTag);

            // Emit props/attributes
            DictPairList *props = stmt->as.ui_component.props;
            Expr *innerTextValue = NULL;

            for (int i = 0; i < props->count; i++) {
                const char *rawKey  = (props->items[i].key && props->items[i].key->type == EXPR_VARIABLE)
                                       ? props->items[i].key->as.variable.name : NULL;
                const char *mappedKey = rawKey ? mapPropName(rawKey) : NULL;
                bool isVariableValue = (props->items[i].value && props->items[i].value->type == EXPR_VARIABLE);

                // Special handling for 'value' prop on non-form elements
                if (rawKey && strcmp(rawKey, "value") == 0) {
                    if (strcmp(htmlTag, "input") != 0 && strcmp(htmlTag, "option") != 0 && 
                        strcmp(htmlTag, "textarea") != 0 && strcmp(htmlTag, "select") != 0 &&
                        strcmp(htmlTag, "progress") != 0 && strcmp(htmlTag, "meter") != 0) {
                        
                        // If literal string, inject as inner text
                        if (props->items[i].value && props->items[i].value->type == EXPR_LITERAL && IS_STRING(props->items[i].value->as.literal.value)) {
                            innerTextValue = props->items[i].value;
                            continue; // Skip emitting this attribute
                        } else {
                            // Any other expression becomes a reactive x-text attribute
                            mappedKey = "x-text";
                            isVariableValue = false; // x-text is natively reactive, do not prefix with ':'
                        }
                    }
                }

                fprintf(html, " ");
                if (mappedKey) {
                    // Only add ':' reactive prefix for plain HTML attribute bindings.
                    // Event handlers (@click etc.) and Alpine directives (x-model, x-show etc.)
                    // must NOT be prefixed with ':'.
                    bool isEventHandler = (mappedKey[0] == '@');
                    bool isAlpineDirective = (mappedKey[0] == 'x' && mappedKey[1] == '-');
                    if (isVariableValue && !isEventHandler && !isAlpineDirective) {
                         fprintf(html, ":%s", mappedKey);
                    } else {
                         fprintf(html, "%s", mappedKey);
                    }
                } else {
                    transpileExpr(props->items[i].key, html, false);
                }
                fprintf(html, "=\"");
                transpileExpr(props->items[i].value, html, false);
                fprintf(html, "\"");
            }

            if (isSelfClosing(htmlTag)) {
                fprintf(html, " />\n");
                break;
            }

            fprintf(html, ">");

            if (innerTextValue) {
                transpileExpr(innerTextValue, html, false);
            }

            // Children
            if (stmt->as.ui_component.children && stmt->as.ui_component.children->count > 0) {
                if (!innerTextValue) fprintf(html, "\n");
                else fprintf(html, "\n");
                for (int i = 0; i < stmt->as.ui_component.children->count; i++) {
                    transpileStmt(stmt->as.ui_component.children->items[i], html, js, indent + 1);
                }
                printIndent(html, indent);
            }
            fprintf(html, "</%s>\n", htmlTag);
            break;
        }

        case STMT_UI_WINDOW: {
            if (!html) break;
            printIndent(html, indent);
            fprintf(html, "<div id=\"window-%s\" class=\"prox-window\">\n", stmt->as.ui_window.name);
            for (int i = 0; i < stmt->as.ui_window.body->count; i++) {
                transpileStmt(stmt->as.ui_window.body->items[i], html, js, indent + 1);
            }
            printIndent(html, indent);
            fprintf(html, "</div>\n");
            break;
        }

        case STMT_EXPRESSION:
            // Handle text content or JS expressions
            if (html && !js) { 
                if (stmt->as.expression.expression->type == EXPR_VARIABLE) {
                    // Reactive text
                    fprintf(html, "<span x-text=\"%s\"></span>", stmt->as.expression.expression->as.variable.name);
                } else if (stmt->as.expression.expression->type == EXPR_LITERAL) {
                    transpileExpr(stmt->as.expression.expression, html, false);
                }
                // Skip assignments or other complex exprs in HTML output
            } else if (js) {
                printIndent(js, indent);
                transpileExpr(stmt->as.expression.expression, js, true);
                fprintf(js, ";\n");
            }
            break;

        default:
            // Non-UI statements silently skipped in UI context
            break;
    }
}

static void transpileExpr(Expr *expr, FILE *out, bool isJS) {
    if (!expr) return;
    switch (expr->type) {
        case EXPR_LITERAL:
            if (IS_STRING(expr->as.literal.value)) {
                if (isJS) {
                    fprintf(out, "\\"");
                    const char* s = AS_CSTRING(expr->as.literal.value);
                    while (*s) {
                        if (*s == '"' || *s == '\\\\') fputc('\\\\', out);
                        if (*s == '\\n') fprintf(out, "\\\\n");
                        else if (*s == '\\r') fprintf(out, "\\\\r");
                        else if (*s != '\\n' && *s != '\\r') fputc(*s, out);
                        s++;
                    }
                    fprintf(out, "\\"");
                } else {
                    escapeHtml(AS_CSTRING(expr->as.literal.value), out);
                }
            } else if (IS_NUMBER(expr->as.literal.value)) {
                double n = AS_NUMBER(expr->as.literal.value);
                if (n == (long long)n) {
                    fprintf(out, "%lld", (long long)n);
                } else {
                    fprintf(out, "%g", n);
                }
            } else if (IS_BOOL(expr->as.literal.value)) {
                fprintf(out, "%s", AS_BOOL(expr->as.literal.value) ? "true" : "false");
            } else if (IS_NIL(expr->as.literal.value)) {
                fprintf(out, "null");
            }
            break;

        case EXPR_VARIABLE:
            if (isJS) fprintf(out, "this.");
            fprintf(out, "%s", expr->as.variable.name);
            break;

        case EXPR_ASSIGN:
            if (isJS) fprintf(out, "this.");
            fprintf(out, "%s = ", expr->as.assign.name);
            if (expr->as.assign.value) transpileExpr(expr->as.assign.value, out, isJS);
            break;

        case EXPR_BINARY:
            transpileExpr(expr->as.binary.left, out, isJS);
            fprintf(out, " %s ", expr->as.binary.operator);
            transpileExpr(expr->as.binary.right, out, isJS);
            break;

        default:
            fprintf(out, "[expr]");
            break;
    }
}

static void emitBaseStyles(FILE *css, const char *appName) {
    fprintf(css, "/* ============================================\n");
    fprintf(css, "   ProXPL UI — Premium Generated style.css\n");
    fprintf(css, "   App: %s\n", appName);
    fprintf(css, "   ============================================ */\n\n");

    // Modern Design Tokens
    fprintf(css, ":root {\n");
    fprintf(css, "  --primary: #6366f1;\n");
    fprintf(css, "  --primary-hover: #4f46e5;\n");
    fprintf(css, "  --bg-gradient: linear-gradient(135deg, #f8fafc 0%%, #e2e8f0 100%%);\n");
    fprintf(css, "  --card-bg: rgba(255, 255, 255, 0.8);\n");
    fprintf(css, "  --text-main: #0f172a;\n");
    fprintf(css, "  --text-muted: #64748b;\n");
    fprintf(css, "}\n\n");

    // CSS Reset
    fprintf(css, "*, *::before, *::after {\n");
    fprintf(css, "  box-sizing: border-box;\n");
    fprintf(css, "  margin: 0;\n");
    fprintf(css, "  padding: 0;\n");
    fprintf(css, "}\n\n");

    // Base Styles
    fprintf(css, "html {\n");
    fprintf(css, "  font-size: 16px;\n");
    fprintf(css, "  scroll-behavior: smooth;\n");
    fprintf(css, "}\n\n");

    fprintf(css, "body {\n");
    fprintf(css, "  font-family: 'Outfit', 'Inter', system-ui, sans-serif;\n");
    fprintf(css, "  color: var(--text-main);\n");
    fprintf(css, "  background: var(--bg-gradient);\n");
    fprintf(css, "  min-height: 100vh;\n");
    fprintf(css, "  display: flex;\n");
    fprintf(css, "  align-items: center;\n");
    fprintf(css, "  justify-content: center;\n");
    fprintf(css, "}\n\n");

    // ProXPL Window Component (Glassmorphism)
    fprintf(css, ".prox-window {\n");
    fprintf(css, "  background: var(--card-bg);\n");
    fprintf(css, "  backdrop-filter: blur(12px);\n");
    fprintf(css, "  -webkit-backdrop-filter: blur(12px);\n");
    fprintf(css, "  border-radius: 2rem;\n");
    fprintf(css, "  box-shadow: 0 25px 50px -12px rgba(0, 0, 0, 0.1);\n");
    fprintf(css, "  border: 1px solid rgba(255, 255, 255, 0.3);\n");
    fprintf(css, "  padding: 3rem;\n");
    fprintf(css, "  margin: 1rem;\n");
    fprintf(css, "  max-width: 600px;\n");
    fprintf(css, "  width: 90%%;\n");
    fprintf(css, "  animation: zoomIn 0.5s cubic-bezier(0.16, 1, 0.3, 1);\n");
    fprintf(css, "}\n\n");

    // Basic components
    fprintf(css, "button {\n");
    fprintf(css, "  background: var(--primary);\n");
    fprintf(css, "  color: white;\n");
    fprintf(css, "  border: none;\n");
    fprintf(css, "  padding: 0.75rem 1.5rem;\n");
    fprintf(css, "  border-radius: 1rem;\n");
    fprintf(css, "  font-weight: 600;\n");
    fprintf(css, "  cursor: pointer;\n");
    fprintf(css, "  transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);\n");
    fprintf(css, "  box-shadow: 0 10px 15px -3px rgba(99, 102, 241, 0.3);\n");
    fprintf(css, "}\n\n");

    fprintf(css, "button:hover {\n");
    fprintf(css, "  background: var(--primary-hover);\n");
    fprintf(css, "  transform: translateY(-2px);\n");
    fprintf(css, "  box-shadow: 0 20px 25px -5px rgba(99, 102, 241, 0.4);\n");
    fprintf(css, "}\n\n");

    fprintf(css, "button:active {\n");
    fprintf(css, "  transform: translateY(0);\n");
    fprintf(css, "}\n\n");

    fprintf(css, "span {\n");
    fprintf(css, "  font-size: 1.1rem;\n");
    fprintf(css, "}\n\n");
}

// =====================================================
//  BASIC MODE: transpileUIApp()
//  Used by runFile() for quick previews
// =====================================================
void transpileUIApp(Stmt *appStmt, const char *outputDir) {
    if (appStmt->type != STMT_UI_APP) return;

    MKDIR(outputDir);

    char htmlPath[512], cssPath[512], jsPath[512];
    sprintf(htmlPath, "%s/index.html", outputDir);
    sprintf(cssPath,  "%s/style.css",  outputDir);
    sprintf(jsPath,   "%s/app.js",     outputDir);

    FILE *html = fopen(htmlPath, "w");
    FILE *css  = fopen(cssPath,  "w");
    FILE *js   = fopen(jsPath,   "w");
    if (!html || !css || !js) {
        if (html) fclose(html);
        if (css)  fclose(css);
        if (js)   fclose(js);
        return;
    }

    fprintf(html, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    fprintf(html, "  <meta charset=\"UTF-8\">\n");
    fprintf(html, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(html, "  <title>%s</title>\n", appStmt->as.ui_app.name);
    fprintf(html, "  <link rel=\"stylesheet\" href=\"style.css\">\n");
    fprintf(html, "  <script src=\"https://cdn.jsdelivr.net/npm/alpinejs@3.x.x/dist/cdn.min.js\" defer></script>\n");
    fprintf(html, "</head>\n<body x-data=\"app()\">\n");

    fprintf(js, "function app() {\n  return {\n");

    StmtList *body = appStmt->as.ui_app.body;

    // Pass 1: emit Window/Component HTML structure (no JS)
    for (int i = 0; i < body->count; i++) {
        transpileStmt(body->items[i], html, NULL, 1);
    }

    // Pass 2: emit State + Action into app.js
    for (int i = 0; i < body->count; i++) {
        Stmt *s = body->items[i];
        if (s->type == STMT_UI_STATE || s->type == STMT_UI_ACTION) {
            transpileStmt(s, NULL, js, 2);
        }
    }

    fprintf(js,   "  };\n}\n");
    fprintf(html, "  <script src=\"app.js\"></script>\n");
    fprintf(html, "</body>\n</html>\n");

    // Emit base styles to style.css
    emitBaseStyles(css, appStmt->as.ui_app.name);

    fclose(html);
    fclose(css);
    fclose(js);

    printf("[UI] Generated: %s/index.html\n", outputDir);
    printf("[UI] Generated: %s/style.css\n", outputDir);
    printf("[UI] Generated: %s/app.js\n", outputDir);
}

// =====================================================
//  WEB BUILD MODE: transpileUIAppWeb()
//  Used by: prm build web [--output <dir>]
//  Generates: index.html + style.css + app.js
//  with Google Fonts, Font Awesome, Alpine.js, CSS reset
// =====================================================
void transpileUIAppWeb(Stmt *appStmt, const char *outputDir) {
    if (appStmt->type != STMT_UI_APP) return;

    const char *appName = appStmt->as.ui_app.name;

    // Create output directory
    MKDIR(outputDir);

    char htmlPath[512], cssPath[512], jsPath[512];
    sprintf(htmlPath, "%s/index.html", outputDir);
    sprintf(cssPath,  "%s/style.css",  outputDir);
    sprintf(jsPath,   "%s/app.js",     outputDir);

    FILE *html = fopen(htmlPath, "w");
    FILE *css  = fopen(cssPath,  "w");
    FILE *js   = fopen(jsPath,   "w");

    if (!html || !css || !js) {
        fprintf(stderr, "[PRM] Error: Could not create output files in '%s'\n", outputDir);
        if (html) fclose(html);
        if (css)  fclose(css);
        if (js)   fclose(js);
        return;
    }

    // ─────────────────────────────────────────────
    //  index.html
    // ─────────────────────────────────────────────
    fprintf(html, "<!DOCTYPE html>\n");
    fprintf(html, "<html lang=\"en\">\n");
    fprintf(html, "<head>\n");
    fprintf(html, "  <meta charset=\"UTF-8\">\n");
    fprintf(html, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(html, "  <meta name=\"description\" content=\"%s — built with ProXPL UI\">\n", appName);
    fprintf(html, "  <meta name=\"generator\" content=\"ProXPL prm build web\">\n");
    fprintf(html, "  <title>%s</title>\n", appName);
    fprintf(html, "\n");
    fprintf(html, "  <!-- ProXPL Web Build: Google Fonts -->\n");
    fprintf(html, "  <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n");
    fprintf(html, "  <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n");
    fprintf(html, "  <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800;900&family=Fira+Code:wght@400;500&family=Outfit:wght@400;600;700;900&display=swap\" rel=\"stylesheet\">\n");
    fprintf(html, "\n");
    fprintf(html, "  <!-- Font Awesome Icons -->\n");
    fprintf(html, "  <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css\">\n");
    fprintf(html, "\n");
    fprintf(html, "  <!-- App Styles -->\n");
    fprintf(html, "  <link rel=\"stylesheet\" href=\"style.css\">\n");
    fprintf(html, "\n");
    fprintf(html, "  <!-- Alpine.js (Reactive State) -->\n");
    fprintf(html, "  <script src=\"https://cdn.jsdelivr.net/npm/alpinejs@3.x.x/dist/cdn.min.js\" defer></script>\n");
    fprintf(html, "</head>\n");
    fprintf(html, "<body x-data=\"app()\">\n");
    fprintf(html, "\n");

    // ─────────────────────────────────────────────
    //  Transpile UI body statements
    // ─────────────────────────────────────────────
    StmtList *body = appStmt->as.ui_app.body;
    for (int i = 0; i < body->count; i++) {
        transpileStmt(body->items[i], html, NULL, 1);
    }

    // Close HTML
    fprintf(html, "\n  <!-- ProXPL App Script -->\n");
    fprintf(html, "  <script src=\"app.js\"></script>\n");
    fprintf(html, "</body>\n");
    fprintf(html, "</html>\n");

    // ─────────────────────────────────────────────
    //  style.css — CSS Reset + ProXPL Base Styles
    // ─────────────────────────────────────────────
    emitBaseStyles(css, appName);

    // Additional Rich Styles for Web Build
    fprintf(css, "/* === Rich Web Build Extras === */\n");
    fprintf(css, "body {\n");
    fprintf(css, "  -webkit-font-smoothing: antialiased;\n");
    fprintf(css, "  -moz-osx-font-smoothing: grayscale;\n");
    fprintf(css, "}\n\n");

    fprintf(css, "html {\n");
    fprintf(css, "  -webkit-text-size-adjust: 100%%;\n");
    fprintf(css, "}\n\n");

    fprintf(css, "a {\n");
    fprintf(css, "  color: inherit;\n");
    fprintf(css, "  text-decoration: none;\n");
    fprintf(css, "}\n\n");

    fprintf(css, "ul, ol {\n");
    fprintf(css, "  list-style: none;\n");
    fprintf(css, "}\n\n");

    fprintf(css, "table {\n");
    fprintf(css, "  border-collapse: collapse;\n");
    fprintf(css, "}\n\n");

    // Global keyframe animations (extended)
    fprintf(css, "@keyframes slideUp {\n");
    fprintf(css, "  from { transform: translateY(20px); opacity: 0; }\n");
    fprintf(css, "  to   { transform: translateY(0);    opacity: 1; }\n");
    fprintf(css, "}\n\n");

    fprintf(css, "@keyframes slideDown {\n");
    fprintf(css, "  from { transform: translateY(-20px); opacity: 0; }\n");
    fprintf(css, "  to   { transform: translateY(0);     opacity: 1; }\n");
    fprintf(css, "}\n\n");

    fprintf(css, "@keyframes zoomIn {\n");
    fprintf(css, "  from { transform: scale(0.85); opacity: 0; }\n");
    fprintf(css, "  to   { transform: scale(1);    opacity: 1; }\n");
    fprintf(css, "}\n\n");

    fprintf(css, "@keyframes bounce {\n");
    fprintf(css, "  0%%, 100%% { transform: translateY(0); }\n");
    fprintf(css, "  50%%       { transform: translateY(-10px); }\n");
    fprintf(css, "}\n\n");

    fprintf(css, "@keyframes spin {\n");
    fprintf(css, "  from { transform: rotate(0deg); }\n");
    fprintf(css, "  to   { transform: rotate(360deg); }\n");
    fprintf(css, "}\n\n");

    fprintf(css, "@keyframes pulse {\n");
    fprintf(css, "  0%%, 100%% { opacity: 1; transform: scale(1); }\n");
    fprintf(css, "  50%%       { opacity: 0.8; transform: scale(1.03); }\n");
    fprintf(css, "}\n\n");

    fprintf(css, "@keyframes shake {\n");
    fprintf(css, "  0%%, 100%% { transform: translateX(0); }\n");
    fprintf(css, "  25%%       { transform: translateX(-6px); }\n");
    fprintf(css, "  75%%       { transform: translateX(6px); }\n");
    fprintf(css, "}\n\n");

    fprintf(css, "@keyframes loadingShimmer {\n");
    fprintf(css, "  from { background-position: 200%% 0; }\n");
    fprintf(css, "  to   { background-position: -200%% 0; }\n");
    fprintf(css, "}\n\n");

    // Focus styles
    fprintf(css, "/* === Focus & Accessibility === */\n");
    fprintf(css, ":focus-visible {\n");
    fprintf(css, "  outline: 2px solid #3b82f6;\n");
    fprintf(css, "  outline-offset: 2px;\n");
    fprintf(css, "}\n\n");

    // Scrollbar
    fprintf(css, "/* === Custom Scrollbar === */\n");
    fprintf(css, "::-webkit-scrollbar {\n");
    fprintf(css, "  width: 6px;\n");
    fprintf(css, "  height: 6px;\n");
    fprintf(css, "}\n");
    fprintf(css, "::-webkit-scrollbar-track {\n");
    fprintf(css, "  background: #f1f5f9;\n");
    fprintf(css, "}\n");
    fprintf(css, "::-webkit-scrollbar-thumb {\n");
    fprintf(css, "  background: #94a3b8;\n");
    fprintf(css, "  border-radius: 9999px;\n");
    fprintf(css, "}\n");
    fprintf(css, "::-webkit-scrollbar-thumb:hover {\n");
    fprintf(css, "  background: #64748b;\n");
    fprintf(css, "}\n\n");

    // Selection color
    fprintf(css, "/* === Selection === */\n");
    fprintf(css, "::selection {\n");
    fprintf(css, "  background: #3b82f6;\n");
    fprintf(css, "  color: #ffffff;\n");
    fprintf(css, "}\n\n");

    // Utility classes
    fprintf(css, "/* === ProXPL Utility Classes === */\n");
    fprintf(css, ".prox-center { display: flex; align-items: center; justify-content: center; }\n");
    fprintf(css, ".prox-card   { background: #fff; border-radius: 1rem; padding: 1.5rem; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.1); border: 1px solid #e5e7eb; }\n");
    fprintf(css, ".prox-glass  { backdrop-filter: blur(16px) saturate(180%%); background: rgba(255,255,255,0.25); border: 1px solid rgba(255,255,255,0.3); }\n");
    fprintf(css, ".prox-btn    { display: inline-flex; align-items: center; justify-content: center; padding: 0.5rem 1.25rem; border-radius: 0.5rem; font-weight: 600; cursor: pointer; border: none; transition: all 0.2s ease; text-decoration: none; }\n");
    fprintf(css, ".prox-input  { width: 100%%; padding: 0.625rem 1rem; border: 2px solid #e5e7eb; border-radius: 0.5rem; font-size: 1rem; outline: none; transition: border-color 0.2s ease; }\n");
    fprintf(css, ".prox-input:focus { border-color: #3b82f6; box-shadow: 0 0 0 3px rgba(59,130,246,0.15); }\n");
    fprintf(css, ".prox-badge  { display: inline-flex; align-items: center; padding: 0.2rem 0.75rem; border-radius: 9999px; font-size: 0.75rem; font-weight: 700; text-transform: uppercase; }\n\n");

    // ─────────────────────────────────────────────
    //  app.js — Alpine.js data function
    // ─────────────────────────────────────────────
    fprintf(js, "/**\n");
    fprintf(js, " * ProXPL Web Build — Generated app.js\n");
    fprintf(js, " * App: %s\n", appName);
    fprintf(js, " * Built with: prm build web\n");
    fprintf(js, " */\n\n");
    fprintf(js, "document.addEventListener('alpine:init', () => {\n");
    fprintf(js, "  Alpine.data('app', () => ({\n");

    // Re-scan body to emit state + actions only into JS
    for (int i = 0; i < body->count; i++) {
        Stmt *s = body->items[i];
        if (s->type == STMT_UI_STATE || s->type == STMT_UI_ACTION) {
            transpileStmt(s, NULL, js, 2);
        }
    }

    fprintf(js, "  }));\n");
    fprintf(js, "});\n");

    fclose(html);
    fclose(css);
    fclose(js);

    printf("\n");
    printf("[PRM] ✓ Web build complete!\n");
    printf("[PRM] ─────────────────────────────────────\n");
    printf("[PRM] App:      %s\n", appName);
    printf("[PRM] Output:   %s/\n", outputDir);
    printf("[PRM]   ├── index.html   (markup)\n");
    printf("[PRM]   ├── style.css    (styles + animations)\n");
    printf("[PRM]   └── app.js       (Alpine.js reactive state)\n");
    printf("[PRM] ─────────────────────────────────────\n");
    printf("[PRM] Open %s/index.html in a browser to preview.\n\n", outputDir);
}
