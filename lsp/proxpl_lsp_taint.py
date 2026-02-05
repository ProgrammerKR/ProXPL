"""
Minimal ProXPL LSP prototype that highlights tokens prefixed with 'taint_' as `tainted`.
This uses `pygls` as a demonstration. Install with `pip install -r requirements.txt`.
"""
from pygls.server import LanguageServer
from pygls.lsp.types import (DidOpenTextDocumentParams, DidChangeTextDocumentParams,
                             InitializeParams, TextDocumentItem, Position,
                             Range, SemanticTokens, SemanticTokensLegend,
                             SemanticTokensParams)
from pygls.lsp.types import SemanticTokens, SemanticTokensParams

import re

SERVER = LanguageServer('proxpl-lsp', 'v0.1')

TAINT_TOKEN_TYPE = 100  # custom type index (client must map)

@SERVER.feature('textDocument/didOpen')
def did_open(ls: LanguageServer, params: DidOpenTextDocumentParams):
    doc = params.text_document
    publish_taint_tokens(ls, doc.text, doc.uri)

@SERVER.feature('textDocument/didChange')
def did_change(ls: LanguageServer, params: DidChangeTextDocumentParams):
    doc = ls.workspace.get_document(params.text_document.uri)
    publish_taint_tokens(ls, doc.source, doc.uri)

def publish_taint_tokens(ls: LanguageServer, text: str, uri: str):
    tokens = []  # as list of integers per LSP: [line, startChar, length, tokenType, tokenModifiers]
    for lineno, line in enumerate(text.splitlines()):
        for m in re.finditer(r"\btaint_[A-Za-z0-9_]+\b", line):
            start = m.start()
            length = m.end() - m.start()
            tokens.extend([lineno, start, length, TAINT_TOKEN_TYPE, 0])
    st = SemanticTokens(data=tokens)
    # Semantic token publishing uses a custom notification for simplicity
    ls.show_message_log(f"Publishing {len(tokens)//5} taint tokens for {uri}")
    ls.send_notification('textDocument/semanticTokens', {'uri': uri, 'tokens': tokens})

def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--tcp', action='store_true', help='Run LSP server over stdio (default)')
    args = parser.parse_args()
    SERVER.start_io()

if __name__ == '__main__':
    main()
