#include "../include/wasm_gen.h"
#include "../include/object.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dynamic byte buffer helper
typedef struct {
    uint8_t* data;
    size_t size;
    size_t capacity;
} ByteBuffer;

static void initBuffer(ByteBuffer* buf) {
    buf->capacity = 1024;
    buf->size = 0;
    buf->data = (uint8_t*)malloc(buf->capacity);
}

static void freeBuffer(ByteBuffer* buf) {
    if (buf->data) free(buf->data);
    buf->data = NULL;
    buf->size = 0;
    buf->capacity = 0;
}

static void writeByte(ByteBuffer* buf, uint8_t byte) {
    if (buf->size + 1 > buf->capacity) {
        buf->capacity *= 2;
        buf->data = (uint8_t*)realloc(buf->data, buf->capacity);
    }
    buf->data[buf->size++] = byte;
}

static void writeBytes(ByteBuffer* buf, const uint8_t* bytes, size_t count) {
    for (size_t i = 0; i < count; i++) {
        writeByte(buf, bytes[i]);
    }
}

static void writeU32Leb128(ByteBuffer* buf, uint32_t value) {
    do {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if (value != 0) byte |= 0x80;
        writeByte(buf, byte);
    } while (value != 0);
}

static void writeI32Leb128(ByteBuffer* buf, int32_t value) {
    bool more = true;
    while (more) {
        uint8_t byte = value & 0x7F;
        value >>= 7;
        if ((value == 0 && !(byte & 0x40)) || (value == -1 && (byte & 0x40))) {
            more = false;
        } else {
            byte |= 0x80;
        }
        writeByte(buf, byte);
    }
}

static void writeWasmString(ByteBuffer* buf, const char* str) {
    uint32_t len = (uint32_t)strlen(str);
    writeU32Leb128(buf, len);
    writeBytes(buf, (const uint8_t*)str, len);
}

static void emitSection(ByteBuffer* out, uint8_t sectionId, ByteBuffer* sectionContent) {
    if (sectionContent->size == 0) return;
    writeByte(out, sectionId);
    writeU32Leb128(out, (uint32_t)sectionContent->size);
    writeBytes(out, sectionContent->data, sectionContent->size);
}

// Compile program statements to Wasm bytecode
static void compileExprToWasm(Expr* expr, ByteBuffer* code, uint32_t* stringOffset, ByteBuffer* dataContent) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_LITERAL: {
            if (IS_NUMBER(expr->as.literal.value)) {
                writeByte(code, 0x41); // i32.const
                writeI32Leb128(code, (int32_t)AS_NUMBER(expr->as.literal.value));
            } else if (IS_BOOL(expr->as.literal.value)) {
                writeByte(code, 0x41); // i32.const
                writeI32Leb128(code, AS_BOOL(expr->as.literal.value) ? 1 : 0);
            } else if (IS_STRING(expr->as.literal.value)) {
                ObjString* str = AS_STRING(expr->as.literal.value);
                uint32_t offset = *stringOffset;
                // Write string into data section
                for (int i = 0; i < str->length; i++) {
                    writeByte(dataContent, (uint8_t)str->chars[i]);
                }
                writeByte(dataContent, 0); // null terminator
                *stringOffset += str->length + 1;

                // Push offset to stack
                writeByte(code, 0x41); // i32.const
                writeI32Leb128(code, (int32_t)offset);
            } else {
                writeByte(code, 0x41); // i32.const
                writeI32Leb128(code, 0);
            }
            break;
        }
        case EXPR_BINARY: {
            compileExprToWasm(expr->as.binary.left, code, stringOffset, dataContent);
            compileExprToWasm(expr->as.binary.right, code, stringOffset, dataContent);
            if (strcmp(expr->as.binary.operator, "+") == 0) writeByte(code, 0x6A); // i32.add
            else if (strcmp(expr->as.binary.operator, "-") == 0) writeByte(code, 0x6B); // i32.sub
            else if (strcmp(expr->as.binary.operator, "*") == 0) writeByte(code, 0x6C); // i32.mul
            else if (strcmp(expr->as.binary.operator, "/") == 0) writeByte(code, 0x6D); // i32.div_s
            else if (strcmp(expr->as.binary.operator, "%") == 0) writeByte(code, 0x6F); // i32.rem_s
            else if (strcmp(expr->as.binary.operator, "==") == 0) writeByte(code, 0x46); // i32.eq
            else if (strcmp(expr->as.binary.operator, "!=") == 0) writeByte(code, 0x47); // i32.ne
            else if (strcmp(expr->as.binary.operator, "<") == 0) writeByte(code, 0x48); // i32.lt_s
            else if (strcmp(expr->as.binary.operator, "<=") == 0) writeByte(code, 0x4C); // i32.le_s
            else if (strcmp(expr->as.binary.operator, ">") == 0) writeByte(code, 0x4E); // i32.gt_s
            else if (strcmp(expr->as.binary.operator, ">=") == 0) writeByte(code, 0x4E); // i32.ge_s
            break;
        }
        case EXPR_UNARY: {
            compileExprToWasm(expr->as.unary.right, code, stringOffset, dataContent);
            if (strcmp(expr->as.unary.operator, "-") == 0) {
                // Negate: 0 - val
                writeByte(code, 0x41); writeI32Leb128(code, 0); // i32.const 0
                compileExprToWasm(expr->as.unary.right, code, stringOffset, dataContent);
                writeByte(code, 0x6B); // i32.sub
            } else if (strcmp(expr->as.unary.operator, "!") == 0) {
                writeByte(code, 0x45); // i32.eqz
            }
            break;
        }
        case EXPR_GROUPING:
            compileExprToWasm(expr->as.grouping.expression, code, stringOffset, dataContent);
            break;
        case EXPR_CALL: {
            if (expr->as.call.callee->type == EXPR_VARIABLE) {
                const char* fnName = expr->as.call.callee->as.variable.name;
                int argCount = expr->as.call.arguments ? expr->as.call.arguments->count : 0;
                for (int i = 0; i < argCount; i++) {
                    compileExprToWasm(expr->as.call.arguments->items[i], code, stringOffset, dataContent);
                }
                if (strcmp(fnName, "print") == 0) {
                    writeByte(code, 0x10); // call
                    writeU32Leb128(code, 0); // import index 0: env.print_num
                } else if (strcmp(fnName, "print_str") == 0) {
                    writeByte(code, 0x10); // call
                    writeU32Leb128(code, 1); // import index 1: env.print_str
                }
            }
            break;
        }
        default:
            writeByte(code, 0x41); writeI32Leb128(code, 0);
            break;
    }
}

static void compileStmtToWasm(Stmt* stmt, ByteBuffer* code, uint32_t* stringOffset, ByteBuffer* dataContent) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_EXPRESSION:
            compileExprToWasm(stmt->as.expression.expression, code, stringOffset, dataContent);
            writeByte(code, 0x1A); // drop
            break;
        case STMT_PRINT:
            compileExprToWasm(stmt->as.print.expression, code, stringOffset, dataContent);
            writeByte(code, 0x10); // call
            writeU32Leb128(code, 0); // import index 0 (env.print_num)
            break;
        case STMT_RETURN:
            if (stmt->as.return_stmt.value) {
                compileExprToWasm(stmt->as.return_stmt.value, code, stringOffset, dataContent);
            }
            writeByte(code, 0x0F); // return
            break;
        case STMT_BLOCK:
            if (stmt->as.block.statements) {
                for (int i = 0; i < stmt->as.block.statements->count; i++) {
                    compileStmtToWasm(stmt->as.block.statements->items[i], code, stringOffset, dataContent);
                }
            }
            break;
        case STMT_VAR_DECL:
            if (stmt->as.var_decl.initializer) {
                compileExprToWasm(stmt->as.var_decl.initializer, code, stringOffset, dataContent);
                writeByte(code, 0x1A); // drop for now in main
            }
            break;
        default:
            break;
    }
}

bool compileToWasm(StmtList* program, const char* outWasmPath, const WasmOptions* options) {
    ByteBuffer wasm;
    initBuffer(&wasm);

    // Magic number: \0asm
    writeBytes(&wasm, (const uint8_t*)"\0asm", 4);
    // Version: 1
    uint8_t version[] = {0x01, 0x00, 0x00, 0x00};
    writeBytes(&wasm, version, 4);

    // 1. Type Section (id: 1)
    // Types:
    // 0: (i32) -> void  [print_num, print_str]
    // 1: () -> void     [main / _start]
    // 2: (i32, i32, i32, i32) -> i32 [wasi fd_write]
    ByteBuffer typeSection;
    initBuffer(&typeSection);
    writeU32Leb128(&typeSection, 3); // 3 types

    // Type 0: (i32) -> void
    writeByte(&typeSection, 0x60); // func
    writeU32Leb128(&typeSection, 1); // 1 param
    writeByte(&typeSection, 0x7F); // i32
    writeU32Leb128(&typeSection, 0); // 0 return

    // Type 1: () -> void
    writeByte(&typeSection, 0x60); // func
    writeU32Leb128(&typeSection, 0); // 0 param
    writeU32Leb128(&typeSection, 0); // 0 return

    // Type 2: (i32, i32, i32, i32) -> i32
    writeByte(&typeSection, 0x60); // func
    writeU32Leb128(&typeSection, 4); // 4 params
    writeByte(&typeSection, 0x7F); writeByte(&typeSection, 0x7F);
    writeByte(&typeSection, 0x7F); writeByte(&typeSection, 0x7F);
    writeU32Leb128(&typeSection, 1); // 1 return
    writeByte(&typeSection, 0x7F); // i32

    emitSection(&wasm, 1, &typeSection);
    freeBuffer(&typeSection);

    // 2. Import Section (id: 2)
    // Imports:
    // 0: env.print_num : Type 0
    // 1: env.print_str : Type 0
    ByteBuffer importSection;
    initBuffer(&importSection);
    writeU32Leb128(&importSection, 2); // 2 imports

    // Import 0: env.print_num
    writeWasmString(&importSection, "env");
    writeWasmString(&importSection, "print_num");
    writeByte(&importSection, 0x00); // func import
    writeU32Leb128(&importSection, 0); // type index 0

    // Import 1: env.print_str
    writeWasmString(&importSection, "env");
    writeWasmString(&importSection, "print_str");
    writeByte(&importSection, 0x00); // func import
    writeU32Leb128(&importSection, 0); // type index 0

    emitSection(&wasm, 2, &importSection);
    freeBuffer(&importSection);

    // 3. Function Section (id: 3)
    // 1 function: main (_start), signature index 1 (() -> void)
    ByteBuffer funcSection;
    initBuffer(&funcSection);
    writeU32Leb128(&funcSection, 1); // 1 function
    writeU32Leb128(&funcSection, 1); // type index 1 (() -> void)
    emitSection(&wasm, 3, &funcSection);
    freeBuffer(&funcSection);

    // 5. Memory Section (id: 5)
    ByteBuffer memSection;
    initBuffer(&memSection);
    writeU32Leb128(&memSection, 1); // 1 memory
    writeByte(&memSection, 0x00); // flags: min only
    writeU32Leb128(&memSection, 1); // min 1 page (64KB)
    emitSection(&wasm, 5, &memSection);
    freeBuffer(&memSection);

    // 7. Export Section (id: 7)
    // Exports: "memory" (mem 0), "_start" (func 2), "main" (func 2)
    ByteBuffer exportSection;
    initBuffer(&exportSection);
    writeU32Leb128(&exportSection, 3); // 3 exports

    // Export memory
    writeWasmString(&exportSection, "memory");
    writeByte(&exportSection, 0x02); // memory export
    writeU32Leb128(&exportSection, 0);

    // Export _start (func index = 2, because 2 imported funcs)
    writeWasmString(&exportSection, "_start");
    writeByte(&exportSection, 0x00); // func export
    writeU32Leb128(&exportSection, 2);

    // Export main
    writeWasmString(&exportSection, "main");
    writeByte(&exportSection, 0x00); // func export
    writeU32Leb128(&exportSection, 2);

    emitSection(&wasm, 7, &exportSection);
    freeBuffer(&exportSection);

    // Compile Code and Data
    uint32_t stringOffset = 1024;
    ByteBuffer dataRaw;
    initBuffer(&dataRaw);

    ByteBuffer fnBody;
    initBuffer(&fnBody);
    writeU32Leb128(&fnBody, 0); // 0 local variable declarations

    if (program) {
        for (int i = 0; i < program->count; i++) {
            compileStmtToWasm(program->items[i], &fnBody, &stringOffset, &dataRaw);
        }
    }
    writeByte(&fnBody, 0x0B); // end opcode

    // 10. Code Section (id: 10)
    ByteBuffer codeSection;
    initBuffer(&codeSection);
    writeU32Leb128(&codeSection, 1); // 1 function body
    writeU32Leb128(&codeSection, (uint32_t)fnBody.size);
    writeBytes(&codeSection, fnBody.data, fnBody.size);
    emitSection(&wasm, 10, &codeSection);
    freeBuffer(&fnBody);
    freeBuffer(&codeSection);

    // 11. Data Section (id: 11)
    if (dataRaw.size > 0) {
        ByteBuffer dataSection;
        initBuffer(&dataSection);
        writeU32Leb128(&dataSection, 1); // 1 data segment
        writeByte(&dataSection, 0x00); // active segment, mem 0
        writeByte(&dataSection, 0x41); // i32.const 1024
        writeI32Leb128(&dataSection, 1024);
        writeByte(&dataSection, 0x0B); // end
        writeU32Leb128(&dataSection, (uint32_t)dataRaw.size);
        writeBytes(&dataSection, dataRaw.data, dataRaw.size);
        emitSection(&wasm, 11, &dataSection);
        freeBuffer(&dataSection);
    }
    freeBuffer(&dataRaw);

    // Write .wasm file
    FILE* f = fopen(outWasmPath, "wb");
    if (!f) {
        freeBuffer(&wasm);
        return false;
    }
    fwrite(wasm.data, sizeof(uint8_t), wasm.size, f);
    fclose(f);
    freeBuffer(&wasm);

    // Emit JS glue if requested
    if (options && options->emitJsGlue) {
        char jsPath[512];
        snprintf(jsPath, sizeof(jsPath), "%s.js", options->outputPrefix ? options->outputPrefix : "app");
        char wasmName[256];
        const char* baseName = strrchr(outWasmPath, '/');
        if (!baseName) baseName = strrchr(outWasmPath, '\\');
        snprintf(wasmName, sizeof(wasmName), "%s", baseName ? baseName + 1 : outWasmPath);
        emitWasmJsGlue(jsPath, wasmName);

        if (options->emitHtml) {
            char htmlPath[512];
            snprintf(htmlPath, sizeof(htmlPath), "%s.html", options->outputPrefix ? options->outputPrefix : "app");
            emitWasmHtmlRunner(htmlPath, jsPath, "ProXPL Wasm Application");
        }
    }

    return true;
}

bool emitWasmJsGlue(const char* outJsPath, const char* wasmFileName) {
    FILE* f = fopen(outJsPath, "w");
    if (!f) return false;

    fprintf(f, "// ProXPL v1.6.3 (Wasm World) Runtime Glue Code\n");
    fprintf(f, "export async function initProXPL(wasmUrl = './%s') {\n", wasmFileName);
    fprintf(f, "  let wasmMemory = null;\n");
    fprintf(f, "  const textDecoder = new TextDecoder('utf-8');\n\n");
    fprintf(f, "  function readString(ptr) {\n");
    fprintf(f, "    if (!wasmMemory) return '';\n");
    fprintf(f, "    const mem = new Uint8Array(wasmMemory.buffer);\n");
    fprintf(f, "    let end = ptr;\n");
    fprintf(f, "    while (mem[end] !== 0) end++;\n");
    fprintf(f, "    return textDecoder.decode(mem.subarray(ptr, end));\n");
    fprintf(f, "  }\n\n");
    fprintf(f, "  const imports = {\n");
    fprintf(f, "    env: {\n");
    fprintf(f, "      print_num: (num) => console.log('[ProXPL Wasm]', num),\n");
    fprintf(f, "      print_str: (ptr) => console.log('[ProXPL Wasm]', readString(ptr)),\n");
    fprintf(f, "      dom_get_element_by_id: (idPtr) => 0,\n");
    fprintf(f, "      dom_set_text: (idPtr, textPtr) => {\n");
    fprintf(f, "        const el = document.getElementById(readString(idPtr));\n");
    fprintf(f, "        if (el) el.innerText = readString(textPtr);\n");
    fprintf(f, "      },\n");
    fprintf(f, "      canvas_draw_rect: (ctxPtr, x, y, w, h, colorPtr) => {\n");
    fprintf(f, "        const canvas = document.getElementById(readString(ctxPtr));\n");
    fprintf(f, "        if (canvas) {\n");
    fprintf(f, "          const ctx = canvas.getContext('2d');\n");
    fprintf(f, "          ctx.fillStyle = readString(colorPtr);\n");
    fprintf(f, "          ctx.fillRect(x, y, w, h);\n");
    fprintf(f, "        }\n");
    fprintf(f, "      }\n");
    fprintf(f, "    },\n");
    fprintf(f, "    wasi_snapshot_preview1: {\n");
    fprintf(f, "      fd_write: (fd, iovs, iovs_len, nwritten) => 0,\n");
    fprintf(f, "      proc_exit: (code) => console.log('[ProXPL Exit]', code)\n");
    fprintf(f, "    }\n");
    fprintf(f, "  };\n\n");
    fprintf(f, "  let instance;\n");
    fprintf(f, "  if (typeof fetch === 'function' && typeof WebAssembly.instantiateStreaming === 'function') {\n");
    fprintf(f, "    const res = await WebAssembly.instantiateStreaming(fetch(wasmUrl), imports);\n");
    fprintf(f, "    instance = res.instance;\n");
    fprintf(f, "  } else {\n");
    fprintf(f, "    const bytes = (typeof window === 'undefined') ? await import('fs').then(fs => fs.readFileSync(wasmUrl)) : await fetch(wasmUrl).then(r => r.arrayBuffer());\n");
    fprintf(f, "    const res = await WebAssembly.instantiate(bytes, imports);\n");
    fprintf(f, "    instance = res.instance;\n");
    fprintf(f, "  }\n");
    fprintf(f, "  wasmMemory = instance.exports.memory;\n");
    fprintf(f, "  if (instance.exports._start) instance.exports._start();\n");
    fprintf(f, "  else if (instance.exports.main) instance.exports.main();\n");
    fprintf(f, "  return instance.exports;\n");
    fprintf(f, "}\n");

    fclose(f);
    return true;
}

bool emitWasmHtmlRunner(const char* outHtmlPath, const char* jsFileName, const char* title) {
    FILE* f = fopen(outHtmlPath, "w");
    if (!f) return false;

    fprintf(f, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    fprintf(f, "  <meta charset=\"UTF-8\">\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(f, "  <title>%s</title>\n", title ? title : "ProXPL Wasm App");
    fprintf(f, "  <style>\n");
    fprintf(f, "    body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: #0f172a; color: #f8fafc; padding: 2rem; }\n");
    fprintf(f, "    .card { background: #1e293b; border-radius: 12px; padding: 1.5rem; max-width: 600px; margin: 0 auto; box-shadow: 0 10px 25px rgba(0,0,0,0.5); }\n");
    fprintf(f, "    h1 { color: #38bdf8; margin-top: 0; }\n");
    fprintf(f, "    #console { background: #090d16; padding: 1rem; border-radius: 8px; font-family: monospace; min-height: 100px; white-space: pre-wrap; }\n");
    fprintf(f, "  </style>\n</head>\n<body>\n");
    fprintf(f, "  <div class=\"card\">\n");
    fprintf(f, "    <h1>ProXPL v1.6.3 Wasm World</h1>\n");
    fprintf(f, "    <p>Running WebAssembly compilation target in browser:</p>\n");
    fprintf(f, "    <div id=\"console\">Loading Wasm module...</div>\n");
    fprintf(f, "  </div>\n");
    fprintf(f, "  <script type=\"module\">\n");
    fprintf(f, "    import { initProXPL } from './%s';\n", jsFileName);
    fprintf(f, "    const logBox = document.getElementById('console');\n");
    fprintf(f, "    const oldLog = console.log;\n");
    fprintf(f, "    console.log = (...args) => {\n");
    fprintf(f, "      oldLog(...args);\n");
    fprintf(f, "      logBox.innerText += args.join(' ') + '\\n';\n");
    fprintf(f, "    };\n");
    fprintf(f, "    logBox.innerText = '';\n");
    fprintf(f, "    initProXPL().then(() => {\n");
    fprintf(f, "      console.log('ProXPL WebAssembly execution completed.');\n");
    fprintf(f, "    }).catch(err => {\n");
    fprintf(f, "      console.error(err);\n");
    fprintf(f, "      logBox.innerText += 'Error: ' + err.message;\n");
    fprintf(f, "    });\n");
    fprintf(f, "  </script>\n</body>\n</html>\n");

    fclose(f);
    return true;
}
