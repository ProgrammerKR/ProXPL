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
function activate(context) {
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('proxpl');
    context.subscriptions.push(diagnosticCollection);
    // 1. Code Runner Command
    let runCommand = vscode.commands.registerCommand('proxpl.run', () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            vscode.window.showErrorMessage('No active editor found.');
            return;
        }
        const fileName = editor.document.fileName;
        if (!fileName.endsWith('.prox') && !fileName.endsWith('.pxpl')) {
            vscode.window.showErrorMessage('Not a ProXPL (.prox or .pxpl) file.');
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
                let terminal = vscode.window.terminals.find(t => t.name === 'ProXPL');
                if (!terminal) {
                    terminal = vscode.window.createTerminal('ProXPL');
                }
                terminal.show();
                terminal.sendText(`proxpl run "${fileName}"`);
                // Background execution for diagnostics
                // ERROR: 'proxpl check' is not yet implemented in the CLI. 
                // Disabling to prevent errors.
                /*
                cp.exec(`proxpl check "${fileName}"`, (error: Error | null, stdout: string, stderr: string) => {
                    diagnosticCollection.clear();
                    const diagnostics: vscode.Diagnostic[] = [];
                    const errorLog = stderr || stdout;
                    const errorLines = errorLog.split('\n');

                    errorLines.forEach((line: string) => {
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
                */
            });
        });
    });
}
exports.activate = activate;
;
context.subscriptions.push(runCommand);
// 2. Watch Mode
let isWatchMode = false;
let watchStatusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
watchStatusBarItem.command = 'proxpl.startWatchMode';
context.subscriptions.push(watchStatusBarItem);
const updateStatusBar = () => {
    if (isWatchMode) {
        watchStatusBarItem.text = '$(eye) ProXPL Watch: ON';
        watchStatusBarItem.tooltip = 'Stop Watching ProXPL Files';
        watchStatusBarItem.backgroundColor = new vscode.ThemeColor('statusBarItem.warningBackground');
    }
    else {
        watchStatusBarItem.text = '$(eye-closed) ProXPL Watch: OFF';
        watchStatusBarItem.tooltip = 'Start Watching ProXPL Files';
        watchStatusBarItem.backgroundColor = undefined;
    }
    watchStatusBarItem.show();
};
updateStatusBar();
let watchCommand = vscode.commands.registerCommand('proxpl.startWatchMode', () => {
    isWatchMode = !isWatchMode;
    updateStatusBar();
    vscode.window.showInformationMessage(isWatchMode ? 'ProXPL Watch Mode Started' : 'ProXPL Watch Mode Stopped');
});
context.subscriptions.push(watchCommand);
vscode.workspace.onDidSaveTextDocument((document) => {
    if (isWatchMode && (document.fileName.endsWith('.prox') || document.fileName.endsWith('.pxpl'))) {
        const terminalName = 'ProXPL Debugger';
        let terminal = vscode.window.terminals.find(t => t.name === terminalName);
        if (!terminal) {
            terminal = vscode.window.createTerminal(terminalName);
        }
        terminal.show(); // Focus the terminal as requested
        // Clear previous output
        vscode.commands.executeCommand('workbench.action.terminal.clear');
        terminal.sendText(`proxpl "${document.fileName}"`);
    }
});
// 3. Formatter Provider
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
// 4. Hover Support
const hoverProvider = vscode.languages.registerHoverProvider('proxpl', {
    provideHover(document, position) {
        const range = document.getWordRangeAtPosition(position);
        if (!range)
            return null;
        const word = document.getText(range);
        const descriptions = {
            'func': 'Defines a new function in ProXPL. Syntax: `func name(params) { ... }`',
            'var': 'Declares a new variable. ProXPL is dynamically typed but variables must be declared (using `let` or `const` preferred).',
            'let': 'Declares a mutable variable.',
            'const': 'Declares an immutable constant.',
            'if': 'Conditional statement. Executes a block if the condition is true.',
            'else': 'Defines an alternative block for an `if` statement.',
            'while': 'Loop that continues as long as a condition is true.',
            'for': 'Loop with initializer, condition, and increment.',
            'return': 'Exits a function and optionally returns a value.',
            'print': 'Built-in statement/function to output values to the terminal.',
            'import': 'Incorporates external modules into the current script.',
            'class': 'Defines a new class with methods and properties.',
            'this': 'Refers to the current instance of the class.',
            'super': 'Refers to the superclass.',
            'true': 'Boolean true literal.',
            'false': 'Boolean false literal.',
            'null': 'Represents the absence of value.',
            'len': 'Built-in function. Returns the length of a string or list.',
            'str': 'Built-in function. Converts a value to a string.',
            'clock': 'Built-in function. Returns the current time in seconds.',
            'input': 'Built-in function. Reads a line of input from stdin.',
            'type': 'Built-in function. Returns the type of a value.',
            'try': 'Starts a block of code to test for errors.',
            'catch': 'Handles errors thrown in the try block.',
            'throw': 'Throws an error/exception.'
        };
        if (descriptions[word]) {
            return new vscode.Hover(new vscode.MarkdownString(descriptions[word]));
        }
        return null;
    }
});
context.subscriptions.push(hoverProvider);
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
// 6. Completion Item Provider
const completionProvider = vscode.languages.registerCompletionItemProvider('proxpl', {
    provideCompletionItems(document, position) {
        const keywords = [
            'func', 'class', 'if', 'else', 'while', 'for', 'return', 'print',
            'var', 'let', 'const', 'true', 'false', 'null', 'import', 'export',
            'from', 'as', 'try', 'catch', 'throw', 'async', 'await'
        ];
        const builtins = ['len', 'str', 'clock', 'input', 'type'];
        const items = [];
        keywords.forEach(word => {
            items.push(new vscode.CompletionItem(word, vscode.CompletionItemKind.Keyword));
        });
        builtins.forEach(word => {
            items.push(new vscode.CompletionItem(word, vscode.CompletionItemKind.Function));
        });
        return items;
    }
});
context.subscriptions.push(completionProvider);
function mapLineNumber(lineStr) {
    const num = parseInt(lineStr);
    return isNaN(num) ? 0 : num - 1;
}
function deactivate() { }
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map