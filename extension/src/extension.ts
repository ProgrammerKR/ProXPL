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
                const terminal = vscode.window.activeTerminal || vscode.window.createTerminal('ProXPL');
                terminal.show();
                terminal.sendText(`proxpl run "${fileName}"`);

                // Background execution for diagnostics
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
            });
        });
    });
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
        } else {
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
        provideDocumentFormattingEdits(document: vscode.TextDocument): vscode.TextEdit[] {
            const edits: vscode.TextEdit[] = [];
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
                } else {
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
        provideHover(document: vscode.TextDocument, position: vscode.Position) {
            const range = document.getWordRangeAtPosition(position);
            if (!range) return null;
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

function mapLineNumber(lineStr: string): number {
    const num = parseInt(lineStr);
    return isNaN(num) ? 0 : num - 1;
}

export function deactivate() { }
