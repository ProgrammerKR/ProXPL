import {
	createConnection,
	TextDocuments,
	ProposedFeatures,
	InitializeParams,
	DidChangeConfigurationNotification,
	CompletionItem,
	CompletionItemKind,
	TextDocumentPositionParams,
	TextDocumentPositionParams,
	TextDocumentSyncKind,
	InitializeResult,
	Location
} from 'vscode-languageserver/node';
import {
	TextDocument
} from 'vscode-languageserver-textdocument';

// Create a connection for the server, using Node's IPC as a transport.
// Also include all preview / proposed LSP features.
const connection = createConnection(ProposedFeatures.all);

// Create a simple text document manager.
const documents: TextDocuments<TextDocument> = new TextDocuments(TextDocument);

let hasConfigurationCapability = false;
let hasWorkspaceFolderCapability = false;
let hasDiagnosticRelatedInformationCapability = false;

connection.onInitialize((params: InitializeParams) => {
	const capabilities = params.capabilities;

	// Does the client support the `workspace/configuration` request?
	// If not, we fall back using global settings.
	hasConfigurationCapability = !!(
		capabilities.workspace && !!capabilities.workspace.configuration
	);
	hasWorkspaceFolderCapability = !!(
		capabilities.workspace && !!capabilities.workspace.workspaceFolders
	);
	hasDiagnosticRelatedInformationCapability = !!(
		capabilities.textDocument &&
		capabilities.textDocument.publishDiagnostics &&
		capabilities.textDocument.publishDiagnostics.relatedInformation
	);

	const result: InitializeResult = {
		capabilities: {
			textDocumentSync: TextDocumentSyncKind.Incremental,
			// Tell the client that this server supports code completion.
			completionProvider: {
				resolveProvider: true
			},
			definitionProvider: true
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
		// Register for all configuration changes.
		connection.client.register(DidChangeConfigurationNotification.type, undefined);
	}
	if (hasWorkspaceFolderCapability) {
		connection.workspace.onDidChangeWorkspaceFolders(_event => {
			connection.console.log('Workspace folder change event received.');
		});
	}
});

// The content of a text document has changed. This event is emitted
// when the text document first opened or when its content has changed.
documents.onDidChangeContent(_change => {
	// validateTextDocument(change.document);
});

connection.onDidChangeWatchedFiles(_change => {
	// Monitored files have change in VSCode
	connection.console.log('We received an file change event');
});

// This handler provides the initial list of the completion items.
connection.onCompletion(
	(_textDocumentPosition: TextDocumentPositionParams): CompletionItem[] => {
		// The pass parameter contains the position of the text document in
		// which code complete got requested. For the example we ignore this
		// info and always provide the same completion items.

		const keywords = [
			'func', 'class', 'if', 'else', 'while', 'for', 'return', 'print',
			'var', 'let', 'const', 'true', 'false', 'null', 'use', 'export',
			'prox', 'loop', 'from', 'as', 'try', 'catch', 'throw', 'async', 'await'
		];
		const builtins = ['len', 'str', 'clock', 'input', 'type'];

		const items: CompletionItem[] = [];

		for (const word of keywords) {
			items.push({
				label: word,
				kind: CompletionItemKind.Keyword,
				data: 1
			});
		}

		for (const word of builtins) {
			items.push({
				label: word,
				kind: CompletionItemKind.Function,
				data: 2
			});
		}

		return items;
	}
);

// This handler resolves additional information for the item selected in
// the completion list.
connection.onCompletionResolve(
	(item: CompletionItem): CompletionItem => {
		if (item.data === 1) {
			item.detail = 'ProXPL Keyword';
			item.documentation = 'A reserved keyword in the ProXPL language.';
		} else if (item.data === 2) {
			item.detail = 'ProXPL Built-in';
			item.documentation = 'A built-in function or type.';
		}
		return item;
	}
);

connection.onDefinition(
	(params: TextDocumentPositionParams): Location | null => {
		const document = documents.get(params.textDocument.uri);
		if (!document) return null;

		const position = params.position;
		const offset = document.offsetAt(position);
		const text = document.getText();

		// Simple word extraction
		const wordRegex = /[a-zA-Z0-9_]+/g;
		let match;
		let word = "";

		while ((match = wordRegex.exec(text)) !== null) {
			if (offset >= match.index && offset <= match.index + match[0].length) {
				word = match[0];
				break;
			}
		}

		if (!word) return null;

		// Regex to find definition
		// func <word> or class <word>
		const funcRegex = new RegExp(`func\\s+${word}\\s*\\(`, 'g');
		const classRegex = new RegExp(`class\\s+${word}\\s*\\{`, 'g');

		let defMatch;
		// Check functions
		while ((defMatch = funcRegex.exec(text)) !== null) {
			return Location.create(
				params.textDocument.uri,
				{
					start: document.positionAt(defMatch.index),
					end: document.positionAt(defMatch.index + match[0].length + 5)
				}
			);
		}
		// Check classes
		while ((defMatch = classRegex.exec(text)) !== null) {
			return Location.create(
				params.textDocument.uri,
				{
					start: document.positionAt(defMatch.index),
					end: document.positionAt(defMatch.index + match[0].length + 6)
				}
			);
		}

		return null;
	}
);

// Make the text document manager listen on the connection
// for open, change and close text document events
documents.listen(connection);

// Listen on the connection
connection.listen();
