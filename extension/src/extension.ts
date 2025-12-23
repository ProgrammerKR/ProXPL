import * as vscode from 'vscode';
import * as cp from 'child_process';

export function activate(context: vscode.ExtensionContext) {
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
        if (!fileName.endsWith('.prox')) {
            vscode.window.showErrorMessage('Not a ProXPL (.prox) file.');
            return;
        }

        // Save file before running
        editor.document.save().then(() => {
            const terminal = vscode.window.activeTerminal || vscode.window.createTerminal('ProXPL');
            terminal.show();
            terminal.sendText(`proxpl run "${fileName}"`);

            // Optional: Background execution for diagnostics
            // We run another process to capture output specifically for the "linter"
            cp.exec(`proxpl check "${fileName}"`, (error, stdout, stderr) => {
                diagnosticCollection.clear();
                const diagnostics: vscode.Diagnostic[] = [];

                // Simple pattern matching for "Error at line X: message"
                const errorLog = stderr || stdout;
                const errorLines = errorLog.split('\n');

                errorLines.forEach(line => {
                    const match = line.match(/Error at line (\d+): (.*)/);
                    if (match) {
                        const lineNum = parseInt(match[1]) - 1;
                        const message = match[2];
                        const range = new vscode.Range(lineNum, 0, lineNum, 100);
                        diagnostics.push(new vscode.Diagnostic(range, message, vscode.DiagnosticSeverity.Error));
                    }
                });

                diagnosticCollection.set(editor.document.uri, diagnostics);
            });
        });
    });
    context.subscriptions.push(runCommand);

    // 2. Formatter Provider
    const formattingProvider = vscode.languages.registerDocumentFormattingEditProvider('proxpl', {
        provideDocumentFormattingEdits(document: vscode.TextDocument): vscode.TextEdit[] {
            const edits: vscode.TextEdit[] = [];

            for (let i = 0; i < document.lineCount; i++) {
                const line = document.lineAt(i);
                const text = line.text;

                // Remove trailing whitespace
                if (text.endsWith(' ') || text.endsWith('\t')) {
                    edits.push(vscode.TextEdit.delete(new vscode.Range(i, text.trimEnd().length, i, text.length)));
                }

                // Basic Indentation (simplified example - fixes existing whitespace to 4 spaces)
                // This is a naive implementation; complex ones use AST
                const indentMatch = text.match(/^(\s+)/);
                if (indentMatch) {
                    const oldIndent = indentMatch[1];
                    // Example: convert tabs to 4 spaces or ensure 4-space blocks
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
    const hoverProvider = vscode.languages.registerHoverProvider('proxpl', {
        provideHover(document, position) {
            const range = document.getWordRangeAtPosition(position);
            const word = document.getText(range);

            const descriptions: { [key: string]: string } = {
                'func': 'Defines a new function in ProXPL. Syntax: `func name(params) { ... }`',
                'var': 'Declares a new variable. ProXPL is dynamically typed but variables must be declared.',
                'if': 'Conditional statement. Executes a block if the condition is true.',
                'else': 'Defines an alternative block for an `if` statement.',
                'while': 'Loop that continues as long as a condition is true.',
                'return': 'Exits a function and optionally returns a value.',
                'print': 'Built-in function to output values to the terminal.',
                'import': 'Incorporates external modules into the current script.'
            };

            if (descriptions[word]) {
                return new vscode.Hover(new vscode.MarkdownString(descriptions[word]));
            }
            return null;
        }
    });
    context.subscriptions.push(hoverProvider);
}

export function deactivate() { }
