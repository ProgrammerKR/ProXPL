"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const node_1 = require("vscode-languageserver/node");
const vscode_languageserver_textdocument_1 = require("vscode-languageserver-textdocument");
// Create a connection for the server, using Node's IPC as a transport.
const connection = (0, node_1.createConnection)(node_1.ProposedFeatures.all);
// Create a simple text document manager.
const documents = new node_1.TextDocuments(vscode_languageserver_textdocument_1.TextDocument);
let hasConfigurationCapability = false;
let hasWorkspaceFolderCapability = false;
let hasDiagnosticRelatedInformationCapability = false;
connection.onInitialize((params) => {
    const capabilities = params.capabilities;
    hasConfigurationCapability = !!(capabilities.workspace && !!capabilities.workspace.configuration);
    hasWorkspaceFolderCapability = !!(capabilities.workspace && !!capabilities.workspace.workspaceFolders);
    hasDiagnosticRelatedInformationCapability = !!(capabilities.textDocument &&
        capabilities.textDocument.publishDiagnostics &&
        capabilities.textDocument.publishDiagnostics.relatedInformation);
    const result = {
        capabilities: {
            textDocumentSync: node_1.TextDocumentSyncKind.Incremental,
            completionProvider: {
                resolveProvider: true,
                triggerCharacters: ['.', ':', '>', '@']
            },
            definitionProvider: true,
            hoverProvider: true,
            documentSymbolProvider: true
        }
    };
    if (hasWorkspaceFolderCapability) {
        result.capabilities.workspace = {
            workspaceFolders: {
                supported: true
            }
        };
    }
    return result;
});
connection.onInitialized(() => {
    if (hasConfigurationCapability) {
        connection.client.register(node_1.DidChangeConfigurationNotification.type, undefined);
    }
    if (hasWorkspaceFolderCapability) {
        connection.workspace.onDidChangeWorkspaceFolders(_event => {
            connection.console.log('Workspace folder change event received.');
        });
    }
});
const LANGUAGE_DOCS = {
    // --- Keywords & Paradigms ---
    'func': {
        detail: 'Function Declaration',
        signature: 'func name(param1, param2, ...) { ... }',
        doc: 'Declares a function in ProXPL. Functions support closures, recursion, default parameters, and first-class passing.',
        example: 'func add(a, b) {\n    return a + b;\n}',
        category: 'keyword'
    },
    'async': {
        detail: 'Asynchronous Function Modifier',
        signature: 'async func name(params) { ... }',
        doc: 'Marks a function as asynchronous, enabling non-blocking execution and coroutine suspension with `await`.',
        example: 'async func fetchData(url) {\n    return await http.get(url);\n}',
        category: 'keyword'
    },
    'await': {
        detail: 'Await Expression',
        signature: 'let result = await promiseExpr;',
        doc: 'Suspends the execution of an `async` function until the asynchronous task/promise resolves.',
        category: 'keyword'
    },
    'let': {
        detail: 'Variable Declaration (Mutable)',
        signature: 'let identifier = value;',
        doc: 'Declares a mutable block-scoped variable with automatic type inference or optional explicit type annotation.',
        example: 'let count = 42;\nlet name: string = "Alice";',
        category: 'keyword'
    },
    'const': {
        detail: 'Constant Declaration (Immutable)',
        signature: 'const identifier = value;',
        doc: 'Declares an immutable block-scoped constant value.',
        category: 'keyword'
    },
    'class': {
        detail: 'Class Declaration (OOP)',
        signature: 'class Name [extends SuperName] { ... }',
        doc: 'Defines an object-oriented class supporting fields, methods, inheritance, and constructors.',
        example: 'class Person {\n    func init(name) {\n        this.name = name;\n    }\n    func greet() {\n        print("Hello, " + this.name);\n    }\n}',
        category: 'keyword'
    },
    'intent': {
        detail: 'Intent-Oriented Goal (Pillar 1)',
        signature: 'intent GoalName(param1, param2, ...);',
        doc: 'Declares an abstract intention/goal in Intent-Oriented Programming (IOP). The runtime automatically resolves this intent via registered `resolver` blocks.',
        example: 'intent SendMessage(recipient, message);\n\nresolver EmailSender matches SendMessage {\n    print("Sending email to " + recipient);\n}',
        category: 'pillar'
    },
    'resolver': {
        detail: 'Intent Resolver Block (Pillar 1)',
        signature: 'resolver ResolverName matches IntentName { ... }',
        doc: 'Provides a concrete fulfillment strategy for a declared `intent`. The ProXPL VM dynamically selects and executes the appropriate resolver.',
        category: 'pillar'
    },
    'context': {
        detail: 'Context-Aware Polymorphism Group (Pillar 2)',
        signature: 'context ContextName { layer Layer1 { ... } }',
        doc: 'Defines an execution context in Context-Oriented Programming (COP) containing dynamic behavioral layers.',
        example: 'context SecurityContext {\n    layer Authenticated {\n        // Override methods\n    }\n}\n\nactivate SecurityContext {\n    processSensitiveData();\n}',
        category: 'pillar'
    },
    'layer': {
        detail: 'Context Behavioral Layer (Pillar 2)',
        signature: 'layer LayerName { ... }',
        doc: 'Defines a partial behavioral overlay inside a `context` that activates dynamically.',
        category: 'pillar'
    },
    'activate': {
        detail: 'Context Activation Block (Pillar 2)',
        signature: 'activate ContextName { ... }',
        doc: 'Activates a context dynamically for the duration of the enclosed lexical block, pushing it onto the runtime context stack.',
        category: 'pillar'
    },
    'resilient': {
        detail: 'Autonomic Self-Healing Block (ASR - Pillar 3)',
        signature: 'resilient { ... } recovery { ... }',
        doc: 'Marks a critical code block as fault-tolerant. If any runtime exception or hardware error occurs, execution automatically transfers safely to the `recovery` block.',
        example: 'resilient {\n    let result = riskyNetworkCall();\n} recovery {\n    print("Recovered from network failure. Using cached fallback.");\n}',
        category: 'pillar'
    },
    'recovery': {
        detail: 'ASR Recovery Fallback (Pillar 3)',
        signature: 'recovery { ... }',
        doc: 'Fallback execution block attached to a `resilient` statement.',
        category: 'pillar'
    },
    'actor': {
        detail: 'Lightweight Actor Model (Concurrency)',
        signature: 'actor ActorName { receive(msg) { ... } }',
        doc: 'Defines a lightweight isolated actor with its own mailbox, processing messages asynchronously via `!` and `?`.',
        example: 'actor Worker {\n    receive(msg) {\n        print("Worker received: " + to_string(msg));\n    }\n}\n\nlet w = Worker();\nw ! "process_job";',
        category: 'keyword'
    },
    'comptime': {
        detail: 'Compile-Time Evaluation',
        signature: 'let val = comptime { ... };',
        doc: 'Executes expressions at compile time and embeds the resulting constant directly into bytecode.',
        category: 'keyword'
    },
    // --- Standard Library Native Functions ---
    'print': {
        detail: 'Native I/O Output',
        signature: 'print(value1, value2, ...)',
        doc: 'Prints the given values followed by a newline to standard output.',
        category: 'builtin'
    },
    'input': {
        detail: 'Native I/O Input',
        signature: 'input(prompt?: string) -> string',
        doc: 'Reads a line of text from standard input, optionally displaying a prompt string.',
        example: 'let name = input("Enter your name: ");',
        category: 'builtin'
    },
    'to_string': {
        detail: 'Type Conversion to String',
        signature: 'to_string(value) -> string',
        doc: 'Converts any ProXPL value (number, list, dictionary, object) into its string representation.',
        category: 'builtin'
    },
    'to_int': {
        detail: 'Type Conversion to Integer',
        signature: 'to_int(value) -> int',
        doc: 'Parses a string or converts a float into an integer.',
        category: 'builtin'
    },
    'to_float': {
        detail: 'Type Conversion to Float',
        signature: 'to_float(value) -> float',
        doc: 'Parses a string or converts an integer into a floating-point number.',
        category: 'builtin'
    },
    'len': {
        detail: 'Collection / String Length',
        signature: 'len(collectionOrString) -> int',
        doc: 'Returns the number of elements in a list, entries in a dictionary, or characters in a string.',
        example: 'let size = len([1, 2, 3]); // 3',
        category: 'builtin'
    },
    'type': {
        detail: 'Value Type Introspection',
        signature: 'type(value) -> string',
        doc: 'Returns the runtime type of a value ("number", "string", "bool", "list", "dict", "function", "instance", "null").',
        category: 'builtin'
    },
    'clock': {
        detail: 'High-Resolution Execution Clock',
        signature: 'clock() -> float',
        doc: 'Returns the elapsed execution time in seconds with microsecond precision. Useful for benchmarking.',
        category: 'builtin'
    },
    'sqrt': {
        detail: 'Math: Square Root',
        signature: 'sqrt(x: number) -> number',
        doc: 'Calculates the square root of `x`.',
        category: 'builtin'
    },
    'pow': {
        detail: 'Math: Exponentiation',
        signature: 'pow(base: number, exp: number) -> number',
        doc: 'Raises `base` to the power `exp`.',
        category: 'builtin'
    },
    'abs': {
        detail: 'Math: Absolute Value',
        signature: 'abs(x: number) -> number',
        doc: 'Returns the absolute value of `x`.',
        category: 'builtin'
    },
    'random': {
        detail: 'Math: Random Number',
        signature: 'random(min?: number, max?: number) -> number',
        doc: 'Generates a random float between 0.0 and 1.0, or a random integer between `min` and `max` when arguments are supplied.',
        example: 'let roll = random(1, 6);',
        category: 'builtin'
    },
    'read_file': {
        detail: 'Filesystem: Read File',
        signature: 'read_file(path: string) -> string',
        doc: 'Reads the entire contents of a file as a string. Returns `null` or raises an error if the file cannot be opened.',
        example: 'let content = read_file("data.json");',
        category: 'builtin'
    },
    'write_file': {
        detail: 'Filesystem: Write File',
        signature: 'write_file(path: string, content: string) -> bool',
        doc: 'Writes text to the specified file path, overwriting existing contents.',
        example: 'write_file("output.txt", "Hello ProXPL!");',
        category: 'builtin'
    },
    'append_file': {
        detail: 'Filesystem: Append File',
        signature: 'append_file(path: string, content: string) -> bool',
        doc: 'Appends text to the end of the specified file path.',
        category: 'builtin'
    },
    'file_exists': {
        detail: 'Filesystem: Check Existence',
        signature: 'file_exists(path: string) -> bool',
        doc: 'Checks whether a file or directory exists at the given path.',
        category: 'builtin'
    },
    'push': {
        detail: 'List: Push Element',
        signature: 'push(list: list, item: any) -> void',
        doc: 'Appends an element to the end of a list in-place.',
        category: 'builtin'
    },
    'pop': {
        detail: 'List: Pop Element',
        signature: 'pop(list: list) -> any',
        doc: 'Removes and returns the last element of a list.',
        category: 'builtin'
    },
    'split': {
        detail: 'String: Split by Delimiter',
        signature: 'split(str: string, delimiter: string) -> list',
        doc: 'Splits a string into a list of substrings based on the specified delimiter.',
        example: 'let parts = split("a,b,c", ","); // ["a", "b", "c"]',
        category: 'builtin'
    },
    'join': {
        detail: 'String: Join List',
        signature: 'join(list: list, delimiter: string) -> string',
        doc: 'Joins elements of a list into a single string separated by the delimiter.',
        category: 'builtin'
    },
    'to_upper': {
        detail: 'String: To Uppercase',
        signature: 'to_upper(str: string) -> string',
        doc: 'Converts all lowercase characters in a string to uppercase.',
        category: 'builtin'
    },
    'to_lower': {
        detail: 'String: To Lowercase',
        signature: 'to_lower(str: string) -> string',
        doc: 'Converts all uppercase characters in a string to lowercase.',
        category: 'builtin'
    },
    'trim': {
        detail: 'String: Trim Whitespace',
        signature: 'trim(str: string) -> string',
        doc: 'Strips leading and trailing whitespace from a string.',
        category: 'builtin'
    },
    'json_parse': {
        detail: 'JSON: Parse String',
        signature: 'json_parse(jsonText: string) -> any',
        doc: 'Parses a JSON string into ProXPL native dictionaries, lists, strings, and numbers.',
        category: 'builtin'
    },
    'json_stringify': {
        detail: 'JSON: Serialize to String',
        signature: 'json_stringify(value: any) -> string',
        doc: 'Serializes a ProXPL data structure into a JSON formatted string.',
        category: 'builtin'
    },
    'base64_encode': {
        detail: 'Encoding: Base64 Encode',
        signature: 'base64_encode(text: string) -> string',
        doc: 'Encodes a string into Base64 format.',
        category: 'builtin'
    },
    'base64_decode': {
        detail: 'Encoding: Base64 Decode',
        signature: 'base64_decode(encoded: string) -> string',
        doc: 'Decodes a Base64 encoded string back to plaintext.',
        category: 'builtin'
    },
    'time': {
        detail: 'System: Unix Timestamp',
        signature: 'time() -> int',
        doc: 'Returns the current Unix epoch timestamp in seconds.',
        category: 'builtin'
    },
    'env': {
        detail: 'System: Environment Variable',
        signature: 'env(name: string) -> string',
        doc: 'Retrieves the value of the specified system environment variable.',
        example: 'let path = env("PATH");',
        category: 'builtin'
    }
};
// ==============================================================================
// 2. Syntax Diagnostics & Validator
// ==============================================================================
function validateTextDocument(textDocument) {
    const text = textDocument.getText();
    const diagnostics = [];
    const lines = text.split(/\r?\n/);
    const stack = [];
    for (let lineIndex = 0; lineIndex < lines.length; lineIndex++) {
        const line = lines[lineIndex];
        // Check for unclosed string literal on this line
        let inString = false;
        let stringChar = '';
        let stringStartCol = 0;
        let inTemplate = false;
        let templateStartCol = 0;
        for (let col = 0; col < line.length; col++) {
            const char = line[col];
            const prevChar = col > 0 ? line[col - 1] : '';
            // Skip single-line comments
            if (!inString && !inTemplate && char === '/' && line[col + 1] === '/') {
                break;
            }
            // Strings
            if ((char === '"' || char === "'") && prevChar !== '\\' && !inTemplate) {
                if (!inString) {
                    inString = true;
                    stringChar = char;
                    stringStartCol = col;
                }
                else if (stringChar === char) {
                    inString = false;
                }
            }
            // Template Literals `...`
            if (char === '`' && prevChar !== '\\' && !inString) {
                if (!inTemplate) {
                    inTemplate = true;
                    templateStartCol = col;
                }
                else {
                    inTemplate = false;
                }
            }
            if (inString || inTemplate) {
                continue;
            }
            // Bracket matching
            if (char === '{' || char === '(' || char === '[') {
                stack.push({ char, line: lineIndex, col });
            }
            else if (char === '}' || char === ')' || char === ']') {
                const expected = char === '}' ? '{' : char === ')' ? '(' : '[';
                if (stack.length === 0) {
                    diagnostics.push({
                        severity: node_1.DiagnosticSeverity.Error,
                        range: {
                            start: { line: lineIndex, character: col },
                            end: { line: lineIndex, character: col + 1 }
                        },
                        message: `Unmatched closing bracket '${char}'`,
                        source: 'proxpl-lsp'
                    });
                }
                else {
                    const top = stack.pop();
                    if (top.char !== expected) {
                        diagnostics.push({
                            severity: node_1.DiagnosticSeverity.Error,
                            range: {
                                start: { line: lineIndex, character: col },
                                end: { line: lineIndex, character: col + 1 }
                            },
                            message: `Mismatched bracket: expected closing for '${top.char}' from line ${top.line + 1}, found '${char}'`,
                            source: 'proxpl-lsp'
                        });
                    }
                }
            }
        }
        if (inString) {
            diagnostics.push({
                severity: node_1.DiagnosticSeverity.Error,
                range: {
                    start: { line: lineIndex, character: stringStartCol },
                    end: { line: lineIndex, character: line.length }
                },
                message: `Unterminated string literal (missing closing ${stringChar})`,
                source: 'proxpl-lsp'
            });
        }
        // Check for common syntax patterns like `let = value` (missing variable name)
        const missingVarMatch = line.match(/\b(let|var|const)\s*(=|;)/);
        if (missingVarMatch && missingVarMatch.index !== undefined) {
            diagnostics.push({
                severity: node_1.DiagnosticSeverity.Error,
                range: {
                    start: { line: lineIndex, character: missingVarMatch.index },
                    end: { line: lineIndex, character: missingVarMatch.index + missingVarMatch[0].length }
                },
                message: `Missing identifier name after '${missingVarMatch[1]}'`,
                source: 'proxpl-lsp'
            });
        }
    }
    // Check for any unclosed open brackets
    while (stack.length > 0) {
        const open = stack.pop();
        diagnostics.push({
            severity: node_1.DiagnosticSeverity.Error,
            range: {
                start: { line: open.line, character: open.col },
                end: { line: open.line, character: open.col + 1 }
            },
            message: `Unclosed opening bracket '${open.char}'`,
            source: 'proxpl-lsp'
        });
    }
    connection.sendDiagnostics({ uri: textDocument.uri, diagnostics });
}
documents.onDidChangeContent(change => {
    validateTextDocument(change.document);
});
// ==============================================================================
// 3. Hover Provider
// ==============================================================================
connection.onHover((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return null;
    const position = params.position;
    const text = document.getText();
    const offset = document.offsetAt(position);
    const wordRegex = /[a-zA-Z0-9_]+/g;
    let match;
    let word = "";
    while ((match = wordRegex.exec(text)) !== null) {
        if (offset >= match.index && offset <= match.index + match[0].length) {
            word = match[0];
            break;
        }
    }
    if (!word)
        return null;
    const entry = LANGUAGE_DOCS[word];
    if (entry) {
        let markdown = `**${entry.detail}**\n\n`;
        if (entry.signature) {
            markdown += `\`\`\`proxpl\n${entry.signature}\n\`\`\`\n\n`;
        }
        markdown += `${entry.doc}\n`;
        if (entry.example) {
            markdown += `\n*Example:*\n\`\`\`proxpl\n${entry.example}\n\`\`\``;
        }
        return {
            contents: {
                kind: node_1.MarkupKind.Markdown,
                value: markdown
            }
        };
    }
    return null;
});
// ==============================================================================
// 4. Enhanced Definition Provider (Go to Definition)
// ==============================================================================
connection.onDefinition((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return null;
    const position = params.position;
    const offset = document.offsetAt(position);
    const text = document.getText();
    const wordRegex = /[a-zA-Z0-9_]+/g;
    let match;
    let word = "";
    while ((match = wordRegex.exec(text)) !== null) {
        if (offset >= match.index && offset <= match.index + match[0].length) {
            word = match[0];
            break;
        }
    }
    if (!word)
        return null;
    // Regex patterns for symbols
    const patterns = [
        new RegExp(`\\bfunc\\s+(${word})\\s*\\(`, 'g'),
        new RegExp(`\\bclass\\s+(${word})\\b`, 'g'),
        new RegExp(`\\bactor\\s+(${word})\\b`, 'g'),
        new RegExp(`\\bintent\\s+(${word})\\b`, 'g'),
        new RegExp(`\\bresolver\\s+(${word})\\b`, 'g'),
        new RegExp(`\\bcontext\\s+(${word})\\b`, 'g'),
        new RegExp(`\\blayer\\s+(${word})\\b`, 'g'),
        new RegExp(`\\b(let|var|const)\\s+(${word})\\b`, 'g')
    ];
    for (const pattern of patterns) {
        let defMatch;
        while ((defMatch = pattern.exec(text)) !== null) {
            const startPos = document.positionAt(defMatch.index);
            const endPos = document.positionAt(defMatch.index + defMatch[0].length);
            return node_1.Location.create(params.textDocument.uri, { start: startPos, end: endPos });
        }
    }
    return null;
});
// ==============================================================================
// 5. Document Symbols Provider (Outline Tree)
// ==============================================================================
connection.onDocumentSymbol((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return [];
    const text = document.getText();
    const symbols = [];
    const lines = text.split(/\r?\n/);
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        // Function definition
        const funcMatch = line.match(/\bfunc\s+([a-zA-Z0-9_]+)\s*\((.*?)\)/);
        if (funcMatch && funcMatch.index !== undefined) {
            const name = funcMatch[1];
            const range = {
                start: { line: i, character: funcMatch.index },
                end: { line: i, character: line.length }
            };
            symbols.push({
                name: name,
                detail: `(${funcMatch[2]})`,
                kind: node_1.SymbolKind.Function,
                range: range,
                selectionRange: {
                    start: { line: i, character: funcMatch.index + 5 },
                    end: { line: i, character: funcMatch.index + 5 + name.length }
                }
            });
        }
        // Class definition
        const classMatch = line.match(/\bclass\s+([a-zA-Z0-9_]+)/);
        if (classMatch && classMatch.index !== undefined) {
            const name = classMatch[1];
            const range = {
                start: { line: i, character: classMatch.index },
                end: { line: i, character: line.length }
            };
            symbols.push({
                name: name,
                detail: 'class',
                kind: node_1.SymbolKind.Class,
                range: range,
                selectionRange: {
                    start: { line: i, character: classMatch.index + 6 },
                    end: { line: i, character: classMatch.index + 6 + name.length }
                }
            });
        }
        // Actor definition
        const actorMatch = line.match(/\bactor\s+([a-zA-Z0-9_]+)/);
        if (actorMatch && actorMatch.index !== undefined) {
            const name = actorMatch[1];
            symbols.push({
                name: name,
                detail: 'actor',
                kind: node_1.SymbolKind.Class,
                range: { start: { line: i, character: 0 }, end: { line: i, character: line.length } },
                selectionRange: { start: { line: i, character: 0 }, end: { line: i, character: line.length } }
            });
        }
        // Intent definition
        const intentMatch = line.match(/\bintent\s+([a-zA-Z0-9_]+)/);
        if (intentMatch && intentMatch.index !== undefined) {
            const name = intentMatch[1];
            symbols.push({
                name: name,
                detail: 'intent',
                kind: node_1.SymbolKind.Interface,
                range: { start: { line: i, character: 0 }, end: { line: i, character: line.length } },
                selectionRange: { start: { line: i, character: 0 }, end: { line: i, character: line.length } }
            });
        }
        // Resolver definition
        const resolverMatch = line.match(/\bresolver\s+([a-zA-Z0-9_]+)\s+matches\s+([a-zA-Z0-9_]+)/);
        if (resolverMatch && resolverMatch.index !== undefined) {
            const name = resolverMatch[1];
            symbols.push({
                name: name,
                detail: `matches ${resolverMatch[2]}`,
                kind: node_1.SymbolKind.Event,
                range: { start: { line: i, character: 0 }, end: { line: i, character: line.length } },
                selectionRange: { start: { line: i, character: 0 }, end: { line: i, character: line.length } }
            });
        }
    }
    return symbols;
});
// ==============================================================================
// 6. Context-Aware Code Completion
// ==============================================================================
connection.onCompletion((_textDocumentPosition) => {
    const items = [];
    // Keywords & Builtins from Knowledge Base
    for (const [key, entry] of Object.entries(LANGUAGE_DOCS)) {
        let kind = node_1.CompletionItemKind.Keyword;
        if (entry.category === 'builtin')
            kind = node_1.CompletionItemKind.Function;
        if (entry.category === 'pillar')
            kind = node_1.CompletionItemKind.Event;
        if (entry.category === 'type')
            kind = node_1.CompletionItemKind.TypeParameter;
        items.push({
            label: key,
            kind: kind,
            detail: entry.detail,
            documentation: {
                kind: node_1.MarkupKind.Markdown,
                value: entry.doc + (entry.signature ? `\n\n\`${entry.signature}\`` : '')
            }
        });
    }
    // Snippet completions for rapid development
    items.push({
        label: 'func (snippet)',
        kind: node_1.CompletionItemKind.Snippet,
        insertTextFormat: node_1.InsertTextFormat.Snippet,
        insertText: 'func ${1:name}(${2:params}) {\n    ${0}\n}',
        detail: 'Function snippet'
    });
    items.push({
        label: 'resilient (snippet)',
        kind: node_1.CompletionItemKind.Snippet,
        insertTextFormat: node_1.InsertTextFormat.Snippet,
        insertText: 'resilient {\n    ${1:// resilient logic}\n} recovery {\n    ${2:// recovery fallback}\n}',
        detail: 'ASR Self-Healing snippet'
    });
    items.push({
        label: 'intent (snippet)',
        kind: node_1.CompletionItemKind.Snippet,
        insertTextFormat: node_1.InsertTextFormat.Snippet,
        insertText: 'intent ${1:GoalName}(${2:args});\n\nresolver ${3:Handler} matches ${1:GoalName} {\n    ${0}\n}',
        detail: 'Intent & Resolver snippet'
    });
    items.push({
        label: 'class (snippet)',
        kind: node_1.CompletionItemKind.Snippet,
        insertTextFormat: node_1.InsertTextFormat.Snippet,
        insertText: 'class ${1:ClassName} {\n    func init(${2:args}) {\n        ${0}\n    }\n}',
        detail: 'Class declaration snippet'
    });
    return items;
});
connection.onCompletionResolve((item) => {
    return item;
});
// Make the text document manager listen on the connection
documents.listen(connection);
// Listen on the connection
connection.listen();
//# sourceMappingURL=server.js.map