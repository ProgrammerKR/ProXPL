#include "transpiler_ui.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

static void transpileStmt(Stmt *stmt, FILE *html, FILE *js, int indent);
static void transpileExpr(Expr *expr, FILE *out);

static void printIndent(FILE *out, int level) {
    for (int i = 0; i < level; i++) fprintf(out, "  ");
}

void transpileUIApp(Stmt *appStmt, const char *outputDir) {
    if (appStmt->type != STMT_UI_APP) return;

    MKDIR(outputDir);

    char htmlPath[512], jsPath[512];
    sprintf(htmlPath, "%s/index.html", outputDir);
    sprintf(jsPath, "%s/app.js", outputDir);

    FILE *html = fopen(htmlPath, "w");
    FILE *js = fopen(jsPath, "w");

    if (!html || !js) return;

    // HTML Boilerplate
    fprintf(html, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(html, "  <title>%s</title>\n", appStmt->as.ui_app.name);
    fprintf(html, "  <meta charset=\"UTF-8\">\n");
    fprintf(html, "  <script src=\"https://cdn.jsdelivr.net/npm/alpinejs@3.x.x/dist/cdn.min.js\" defer></script>\n");
    fprintf(html, "</head>\n<body x-data=\"app()\">\n");

    // JS Boilerplate
    fprintf(js, "function app() {\n  return {\n");

    // Transpile body
    StmtList *body = appStmt->as.ui_app.body;
    for (int i = 0; i < body->count; i++) {
        transpileStmt(body->items[i], html, js, 1);
    }

    // Close JS
    fprintf(js, "  };\n}\n");

    // Close HTML
    fprintf(html, "  <script src=\"app.js\"></script>\n");
    fprintf(html, "</body>\n</html>\n");

    fclose(html);
    fclose(js);
}

static const char *mapTag(const char *tag) {
    if (strcmp(tag, "Container") == 0) return "div";
    if (strcmp(tag, "Button") == 0) return "button";
    if (strcmp(tag, "Text") == 0) return "span";
    if (strcmp(tag, "Input") == 0) return "input";
    if (strcmp(tag, "Image") == 0) return "img";
    if (strcmp(tag, "Link") == 0) return "a";
    if (strcmp(tag, "List") == 0) return "ul";
    if (strcmp(tag, "Item") == 0) return "li";
    if (strcmp(tag, "Section") == 0) return "section";
    if (strcmp(tag, "Nav") == 0) return "nav";
    if (strcmp(tag, "Header") == 0) return "header";
    if (strcmp(tag, "Footer") == 0) return "footer";
    if (strcmp(tag, "Icon") == 0) return "i";
    if (strcmp(tag, "Form") == 0) return "form";
    if (strcmp(tag, "Label") == 0) return "label";
    if (strcmp(tag, "Select") == 0) return "select";
    if (strcmp(tag, "Option") == 0) return "option";
    return tag;
}

static void transpileStmt(Stmt *stmt, FILE *html, FILE *js, int indent) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_UI_STATE:
            printIndent(js, indent);
            fprintf(js, "%s: ", stmt->as.ui_state.name);
            if (stmt->as.ui_state.initializer) {
                transpileExpr(stmt->as.ui_state.initializer, js);
            } else {
                fprintf(js, "null");
            }
            fprintf(js, ",\n");
            break;

        case STMT_UI_ACTION:
            printIndent(js, indent);
            fprintf(js, "%s() {\n", stmt->as.ui_action.name);
            // Simple translation for now: just print statements inside
            // In a real impl, we'd transpile ProXPL logic to JS logic
            fprintf(js, "    console.log('Action %s triggered');\n", stmt->as.ui_action.name);
            printIndent(js, indent);
            fprintf(js, "},\n");
            break;

        case STMT_UI_COMPONENT: {
            const char *htmlTag = mapTag(stmt->as.ui_component.tag);
            printIndent(html, indent);
            fprintf(html, "<%s", htmlTag);
            
            // Props
            DictPairList *props = stmt->as.ui_component.props;
            for (int i = 0; i < props->count; i++) {
                fprintf(html, " ");
                transpileExpr(props->items[i].key, html);
                fprintf(html, "=\"");
                transpileExpr(props->items[i].value, html);
                fprintf(html, "\"");
            }
            
            if (strcmp(htmlTag, "img") == 0 || strcmp(htmlTag, "input") == 0) {
                fprintf(html, " />\n");
                break;
            }
            
            fprintf(html, ">");

            // Children
            if (stmt->as.ui_component.children) {
                fprintf(html, "\n");
                for (int i = 0; i < stmt->as.ui_component.children->count; i++) {
                    transpileStmt(stmt->as.ui_component.children->items[i], html, js, indent + 1);
                }
                printIndent(html, indent);
            }
            fprintf(html, "</%s>\n", htmlTag);
            break;
        }

        case STMT_UI_WINDOW: {
            // Treat Window as a div container for now
            fprintf(html, "  <div id=\"%s\" style=\"border: 1px solid #ccc; padding: 20px;\">\n", stmt->as.ui_window.name);
            for (int i = 0; i < stmt->as.ui_window.body->count; i++) {
                transpileStmt(stmt->as.ui_window.body->items[i], html, js, indent + 1);
            }
            fprintf(html, "  </div>\n");
            break;
        }

        default:
            // Skip non-UI statements or handle them if needed
            break;
    }
}

static void transpileExpr(Expr *expr, FILE *out) {
    if (!expr) return;
    switch (expr->type) {
        case EXPR_LITERAL:
            if (IS_STRING(expr->as.literal.value)) {
                fprintf(out, "%s", AS_CSTRING(expr->as.literal.value));
            } else if (IS_NUMBER(expr->as.literal.value)) {
                fprintf(out, "%g", AS_NUMBER(expr->as.literal.value));
            } else if (IS_BOOL(expr->as.literal.value)) {
                fprintf(out, "%s", AS_BOOL(expr->as.literal.value) ? "true" : "false");
            }
            break;
        case EXPR_VARIABLE:
            if (strcmp(expr->as.variable.name, "className") == 0) {
                fprintf(out, "class");
            } else if (strcmp(expr->as.variable.name, "onClick") == 0) {
                fprintf(out, "@click");
            } else {
                fprintf(out, "%s", expr->as.variable.name);
            }
            break;
        default:
            fprintf(out, "[complex_expr]");
            break;
    }
}
