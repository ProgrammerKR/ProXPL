import * as vscode from 'vscode';
import * as cp from 'child_process';

export function activate(context: vscode.ExtensionContext) {
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('proxpl');
    context.subscriptions.push(diagnosticCollection);

    vscode.window.showInformationMessage('ProX Studio Alpha started.');

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
        cp.exec('proxpl --version', (err: Error | null) => {
            if (err) {
                vscode.window.showInformationMessage(
                    'ProXPL compiler not found in PATH. Please install it to run scripts.',
                    'View Installation Docs'
                ).then((selection: string | undefined) => {
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
            });
        });
    });

    context.subscriptions.push(runCommand);

    // ... (intermediate code skipped) ...

    // 4. Hover Support
    const hoverProvider = vscode.languages.registerHoverProvider('proxpl', {
        provideHover(document: vscode.TextDocument, position: vscode.Position) {
            const range = document.getWordRangeAtPosition(position);
            if (!range) return null;
            const word = document.getText(range);

            const descriptions: { [key: string]: string } = {
                'func': 'Defines a new function in ProXPL. Syntax: `func name(params) { ... }`',
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
        provideDefinition(document: vscode.TextDocument, position: vscode.Position) {
            const range = document.getWordRangeAtPosition(position);
            if (!range) return null;
            const word = document.getText(range);

            const text = document.getText();
            // Regex to find 'func word' or 'class word'
            const funcRegex = new RegExp(`func\\s+${word}\\s*\\(`, 'g');
            const classRegex = new RegExp(`class\\s+${word}\\s*\\{`, 'g');

            const results: vscode.Location[] = [];

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
        provideCompletionItems(document: vscode.TextDocument, position: vscode.Position) {
            const keywords = [
                'func', 'class', 'if', 'else', 'while', 'for', 'return', 'print',
                'var', 'let', 'const', 'true', 'false', 'null', 'use', 'export',
                'prox', 'loop', // User requested specific keywords
                'from', 'as', 'try', 'catch', 'throw', 'async', 'await'
            ];
            const builtins = ['len', 'str', 'clock', 'input', 'type'];

            const items: vscode.CompletionItem[] = [];

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
}

function mapLineNumber(lineStr: string): number {
    const num = parseInt(lineStr);
    return isNaN(num) ? 0 : num - 1;
}

export function deactivate() { }
