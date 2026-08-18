#ifndef PROXPL_WASM_GEN_H
#define PROXPL_WASM_GEN_H

#include "ast.h"
#include <stdbool.h>

typedef struct {
    bool emitJsGlue;
    bool emitHtml;
    bool enableWasi;
    const char* outputPrefix;
} WasmOptions;

// Compiles AST / statements into WebAssembly binary (.wasm) and JS glue code (.js)
bool compileToWasm(StmtList* program, const char* outWasmPath, const WasmOptions* options);

// Emits browser JS runtime glue code connecting Wasm imports to Web APIs (DOM, Events, Fetch, Canvas, WASI)
bool emitWasmJsGlue(const char* outJsPath, const char* wasmFileName);

// Emits HTML runner template
bool emitWasmHtmlRunner(const char* outHtmlPath, const char* jsFileName, const char* title);

#endif // PROXPL_WASM_GEN_H
