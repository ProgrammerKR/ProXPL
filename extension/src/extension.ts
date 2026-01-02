import * as vscode from 'vscode';
import * as cp from 'child_process';
import * as path from 'path';
import {
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
    TransportKind
} from 'vscode-languageclient/node';

let client: LanguageClient;

export function activate(context: vscode.ExtensionContext) {
    const diagnosticCollection = vscode.languages.createDiagnosticCollection('proxpl');
    context.subscriptions.push(diagnosticCollection);

    vscode.window.showInformationMessage('ProX Studio Alpha started.');

    // --- LSP Client Setup ---
    const serverModule = context.asAbsolutePath(
        path.join('server', 'out', 'server.js')
    );
    // If the extension is launched in debug mode then the debug server options are used
    // Otherwise the run options are used
    const debugOptions = { execArgv: ['--nolazy', '--inspect=6009'] };

    const serverOptions: ServerOptions = {
        run: { module: serverModule, transport: TransportKind.ipc },
        debug: {
            module: serverModule,
            transport: TransportKind.ipc,
            options: debugOptions
        }
    };

    const clientOptions: LanguageClientOptions = {
        documentSelector: [{ scheme: 'file', language: 'proxpl' }],
        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher('**/.clientrc')
        }
    };

    client = new LanguageClient(
        'proxplLanguageServer',
        'ProXPL Language Server',
        serverOptions,
        clientOptions
    );

    client.start();


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

class ProXDebugAdapterDescriptorFactory implements vscode.DebugAdapterDescriptorFactory {
    createDebugAdapterDescriptor(_session: vscode.DebugSession): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
        // For now, use a simple inline implementation or rely on an executable
        // This is a placeholder for the future DAP implementation
        // return new vscode.DebugAdapterExecutable('proxpl', ['--debug-adapter']);
        return new vscode.DebugAdapterInlineImplementation(new ProXDebugAdapter());
    }
}

class ProXDebugAdapter implements vscode.DebugAdapter {
    private _sendMessage = new vscode.EventEmitter<vscode.DebugProtocolMessage>();
    readonly onDidSendMessage: vscode.Event<vscode.DebugProtocolMessage> = this._sendMessage.event;

    handleMessage(message: vscode.DebugProtocolMessage): void {
        // Minimal Mock Handler
        if (message.type === 'request') {
            const request = message as any;
            if (request.command === 'initialize') {
                this._sendMessage.fire({
                    type: 'response',
                    request_seq: request.seq,
                    success: true,
                    command: request.command,
                    body: {
                        supportsConfigurationDoneRequest: true
                    }
                } as any);
            } else {
                this._sendMessage.fire({
                    type: 'response',
                    request_seq: request.seq,
                    success: true,
                    command: request.command
                } as any);
            }
        }
    }

    dispose() {

    }
}

function mapLineNumber(lineStr: string): number {
    const num = parseInt(lineStr);
    return isNaN(num) ? 0 : num - 1;
}

export function deactivate(): Thenable<void> | undefined {
    if (!client) {
        return undefined;
    }
    return client.stop();
}
