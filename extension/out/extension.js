"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
const vscode = __importStar(require("vscode"));
const cp = __importStar(require("child_process"));
<<<<<<< HEAD
const path = __importStar(require("path"));
const node_1 = require("vscode-languageclient/node");
let client;
function activate(context) {
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('proxpl');
    context.subscriptions.push(diagnosticCollection);
    vscode.window.showInformationMessage('ProX Studio Alpha started.');
    // --- LSP Client Setup ---
    const serverModule = context.asAbsolutePath(path.join('server', 'out', 'server.js'));
    // If the extension is launched in debug mode then the debug server options are used
    // Otherwise the run options are used
    const debugOptions = { execArgv: ['--nolazy', '--inspect=6009'] };
    const serverOptions = {
        run: { module: serverModule, transport: node_1.TransportKind.ipc },
        debug: {
            module: serverModule,
            transport: node_1.TransportKind.ipc,
            options: debugOptions
        }
    };
    const clientOptions = {
        documentSelector: [{ scheme: 'file', language: 'proxpl' }],
        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher('**/.clientrc')
        }
    };
    client = new node_1.LanguageClient('proxplLanguageServer', 'ProXPL Language Server', serverOptions, clientOptions);
    client.start();
=======
function activate(context) {
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('proxpl');
    context.subscriptions.push(diagnosticCollection);
>>>>>>> fix-ci-build
    // 1. Code Runner Command
    let runCommand = vscode.commands.registerCommand('proxpl.run', () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            vscode.window.showErrorMessage('No active editor found.');
            return;
        }
        const fileName = editor.document.fileName;
<<<<<<< HEAD
        if (!fileName.endsWith('.prox') && !fileName.endsWith('.pxpl')) {
            vscode.window.showErrorMessage('Not a ProXPL (.prox or .pxpl) file.');
=======
        if (!fileName.endsWith('.prox')) {
            vscode.window.showErrorMessage('Not a ProXPL (.prox) file.');
>>>>>>> fix-ci-build
            return;
        }
        // Check if proxpl is in PATH
        cp.exec('proxpl --version', (err) => {
            if (err) {
                vscode.window.showInformationMessage('ProXPL compiler not found in PATH. Please install it to run scripts.', 'View Installation Docs').then((selection) => {
                    if (selection === 'View Installation Docs') {
                        vscode.env.openExternal(vscode.Uri.parse('https://github.com/ProgrammerKR/ProXPL#installation'));
                    }
                });
                return;
            }
            // Save file before running
            editor.document.save().then(() => {
<<<<<<< HEAD
                let terminal = vscode.window.terminals.find(t => t.name === 'ProXPL');
                if (!terminal) {
                    terminal = vscode.window.createTerminal('ProXPL');
                }
                terminal.show();
                terminal.sendText(`proxpl run "${fileName}"`);
=======
                const terminal = vscode.window.activeTerminal || vscode.window.createTerminal('ProXPL');
                terminal.show();
                terminal.sendText(`proxpl run "${fileName}"`);
                // Background execution for diagnostics
                cp.exec(`proxpl check "${fileName}"`, (error, stdout, stderr) => {
                    diagnosticCollection.clear();
                    const diagnostics = [];
                    const errorLog = stderr || stdout;
                    const errorLines = errorLog.split('\n');
                    errorLines.forEach((line) => {
                        const match = line.match(/Error at line (\d+): (.*)/);
                        if (match) {
                            const lineNum = mapLineNumber(match[1]);
                            const message = match[2];
                            const range = new vscode.Range(lineNum, 0, lineNum, 100);
                            diagnostics.push(new vscode.Diagnostic(range, message, vscode.DiagnosticSeverity.Error));
                        }
                    });
                    diagnosticCollection.set(editor.document.uri, diagnostics);
                });
>>>>>>> fix-ci-build
            });
        });
    });
    context.subscriptions.push(runCommand);
<<<<<<< HEAD
    // ... (intermediate code skipped) ...
    // 4. Hover Support
=======
    // 2. Formatter Provider
    const formattingProvider = vscode.languages.registerDocumentFormattingEditProvider('proxpl', {
        provideDocumentFormattingEdits(document) {
            const edits = [];
            let lastLineWasEmpty = false;
            for (let i = 0; i < document.lineCount; i++) {
                const line = document.lineAt(i);
                const text = line.text;
                // 1. Remove extra empty lines (consecutive empty lines)
                if (text.trim() === '') {
                    if (lastLineWasEmpty) {
                        // Delete this extra empty line
                        edits.push(vscode.TextEdit.delete(line.rangeIncludingLineBreak));
                        continue;
                    }
                    lastLineWasEmpty = true;
                }
                else {
                    lastLineWasEmpty = false;
                }
                // 2. Remove trailing whitespace
                if (text.endsWith(' ') || text.endsWith('\t')) {
                    edits.push(vscode.TextEdit.delete(new vscode.Range(i, text.trimEnd().length, i, text.length)));
                }
                // 3. Basic Indentation (Fix to 4 spaces)
                const indentMatch = text.match(/^(\s+)/);
                if (indentMatch) {
                    const oldIndent = indentMatch[1];
                    const newIndent = oldIndent.replace(/\t/g, '    ');
                    if (oldIndent !== newIndent) {
                        edits.push(vscode.TextEdit.replace(new vscode.Range(i, 0, i, oldIndent.length), newIndent));
                    }
                }
            }
            return edits;
        }
    });
    context.subscriptions.push(formattingProvider);
    // 3. Hover Support
>>>>>>> fix-ci-build
    const hoverProvider = vscode.languages.registerHoverProvider('proxpl', {
        provideHover(document, position) {
            const range = document.getWordRangeAtPosition(position);
            if (!range)
                return null;
            const word = document.getText(range);
            const descriptions = {
                'func': 'Defines a new function in ProXPL. Syntax: `func name(params) { ... }`',
<<<<<<< HEAD
                'var': 'Declares a new variable.',
                'let': 'Declares a mutable variable.',
                'const': 'Declares an immutable constant.',
                'if': 'Conditional statement.',
                'else': 'Defines an alternative block for an `if` statement.',
                'while': 'Loop that continues as long as a condition is true.',
                'for': 'Loop with initializer, condition, and increment.',
                'return': 'Exits a function and optionally returns a value.',
                'print': 'Output values to the terminal.',
                'use': 'Incorporates external modules.',
                'class': 'Defines a new class.',
                'interface': 'Defines an interface contract.',
                'implements': 'Declares that a class implements an interface.',
                'extends': 'Declares that a class inherits from another class.',
                'public': 'Access modifier: Member is accessible from anywhere.',
                'private': 'Access modifier: Member is accessible only within the class.',
                'protected': 'Access modifier: Member is accessible within class and subclasses.',
                'static': 'Defines a static member belonging to the class itself.',
                'abstract': 'Defines a method signature without implementation.',
                'this': 'Refers to the current instance.',
                'super': 'Refers to the superclass.',
                'async': 'Defines an asynchronous function.',
                'await': 'Pauses execution until a promise resolves.',
                'true': 'Boolean true literal.',
                'false': 'Boolean false literal.',
                'null': 'Represents the absence of value.',
                'len': 'Returns the length of a string or list.',
                'type': 'Returns the type of a value.',
                'try': 'Starts a block of code to test for errors.',
                'catch': 'Handles errors thrown in the try block.',
                'throw': 'Throws an error/exception.'
=======
                'var': 'Declares a new variable. ProXPL is dynamically typed but variables must be declared.',
                'if': 'Conditional statement. Executes a block if the condition is true.',
                'else': 'Defines an alternative block for an `if` statement.',
                'while': 'Loop that continues as long as a condition is true.',
                'return': 'Exits a function and optionally returns a value.',
                'print': 'Built-in function to output values to the terminal.',
                'import': 'Incorporates external modules into the current script.'
>>>>>>> fix-ci-build
            };
            if (descriptions[word]) {
                return new vscode.Hover(new vscode.MarkdownString(descriptions[word]));
            }
            return null;
        }
    });
    context.subscriptions.push(hoverProvider);
<<<<<<< HEAD
    // 5. Definition Provider (Basic "Go to Definition")
    const definitionProvider = vscode.languages.registerDefinitionProvider('proxpl', {
        provideDefinition(document, position) {
            const range = document.getWordRangeAtPosition(position);
            if (!range)
                return null;
            const word = document.getText(range);
            const text = document.getText();
            // Regex to find 'func word' or 'class word'
            const funcRegex = new RegExp(`func\\s+${word}\\s*\\(`, 'g');
            const classRegex = new RegExp(`class\\s+${word}\\s*\\{`, 'g');
            const results = [];
            let match;
            while ((match = funcRegex.exec(text)) !== null) {
                const pos = document.positionAt(match.index);
                results.push(new vscode.Location(document.uri, new vscode.Range(pos, pos)));
            }
            while ((match = classRegex.exec(text)) !== null) {
                const pos = document.positionAt(match.index);
                results.push(new vscode.Location(document.uri, new vscode.Range(pos, pos)));
            }
            return results;
        }
    });
    context.subscriptions.push(definitionProvider);
    // 6. Completion Item Provider (DEPRECATED: Now handled by LSP)
    /*
    const completionProvider = vscode.languages.registerCompletionItemProvider('proxpl', {
        provideCompletionItems(document: vscode.TextDocument, position: vscode.Position) {
            // ... (old logic) ...
            return [];
        }
    });
    context.subscriptions.push(completionProvider);
    */
    // --- DAP Setup ---
    context.subscriptions.push(vscode.debug.registerDebugAdapterDescriptorFactory('proxpl', new ProXDebugAdapterDescriptorFactory()));
}
exports.activate = activate;
class ProXDebugAdapterDescriptorFactory {
    createDebugAdapterDescriptor(_session) {
        // For now, use a simple inline implementation or rely on an executable
        // This is a placeholder for the future DAP implementation
        // return new vscode.DebugAdapterExecutable('proxpl', ['--debug-adapter']);
        return new vscode.DebugAdapterInlineImplementation(new ProXDebugAdapter());
    }
}
class ProXDebugAdapter {
    constructor() {
        this._sendMessage = new vscode.EventEmitter();
        this.onDidSendMessage = this._sendMessage.event;
    }
    handleMessage(message) {
        // Minimal Mock Handler
        const msg = message;
        if (msg.type === 'request') {
            const request = msg;
            if (request.command === 'initialize') {
                this._sendMessage.fire({
                    type: 'response',
                    request_seq: request.seq,
                    success: true,
                    command: request.command,
                    body: {
                        supportsConfigurationDoneRequest: true
                    }
                });
            }
            else {
                this._sendMessage.fire({
                    type: 'response',
                    request_seq: request.seq,
                    success: true,
                    command: request.command
                });
            }
        }
    }
    dispose() {
    }
}
=======
}
exports.activate = activate;
>>>>>>> fix-ci-build
function mapLineNumber(lineStr) {
    const num = parseInt(lineStr);
    return isNaN(num) ? 0 : num - 1;
}
<<<<<<< HEAD
function deactivate() {
    if (!client) {
        return undefined;
    }
    return client.stop();
}
=======
function deactivate() { }
>>>>>>> fix-ci-build
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map