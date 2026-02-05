"""
Minimal ProXPL LSP prototype that highlights tokens prefixed with 'taint_' as `tainted`.
This uses `pygls` as a demonstration. Install with `pip install -r requirements.txt`.
"""
from pygls.server import LanguageServer
from pygls.lsp.types import (
    DidOpenTextDocumentParams,
    DidChangeTextDocumentParams,
    TextDocumentItem,
    Position,
    Range,
    Diagnostic,
    DiagnosticSeverity,
    CodeAction,
    CodeActionKind,
    TextEdit,
    WorkspaceEdit,
    VersionedTextDocumentIdentifier,
)
from pygls.lsp.methods import TEXT_DOCUMENT_DID_OPEN, TEXT_DOCUMENT_DID_CHANGE

import re

SERVER = LanguageServer('proxpl-lsp', 'v0.1')

TAINT_TOKEN_TYPE = 100  # custom type index (client must map)

SOURCES = [r"\b(recv|read|input|http_get)\s*\("]
SANITIZERS = [r"\b(sanitize|sanitize_json|json_schema)\s*\("]
SINKS = [r"\b(db\.insert|exec|eval|os\.system|print_unchecked|write_unchecked)\b"]


def analyze_taint(text: str):
    """Very small heuristic taint analysis.

    - Marks variables assigned from source calls as tainted.
    - Propagates taint through simple assignments: `b = a`.
    - Recognizes sanitizer calls to mark variables as sanitized.
    Returns: (taint_map: dict var->'tainted'|'sanitized', diagnostics: list)
    """
    taint = {}
    diagnostics = []
    lines = text.splitlines()

    # simple pass to detect direct sources and sanitizers
    for lineno, line in enumerate(lines):
        # assignment pattern: var = expr
        m = re.match(r"\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.+)$", line)
        if m:
            lhs = m.group(1)
            rhs = m.group(2)
            # source
            if any(re.search(p, rhs) for p in SOURCES):
                taint[lhs] = 'tainted'
                continue
            # sanitizer
            if any(re.search(p, rhs) for p in SANITIZERS):
                taint[lhs] = 'sanitized'
                continue
            # rhs contains other var
            for var in re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b", rhs):
                if var in taint and taint[var] == 'tainted':
                    taint[lhs] = 'tainted'
                    break

    # second pass: find tainted usages in sinks
    for lineno, line in enumerate(lines):
        for sink_pat in SINKS:
            if re.search(sink_pat, line):
                # find variables used in the call
                for var in re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b", line):
                    if var in taint and taint[var] == 'tainted':
                        # produce a diagnostic recommending sanitization
                        start = line.find(var)
                        diag = Diagnostic(
                            range=Range(start=Position(line=lineno, character=start),
                                        end=Position(line=lineno, character=start + len(var))),
                            message=f"Tainted variable '{var}' used in sensitive sink; consider sanitizing",
                            severity=DiagnosticSeverity.Warning,
                            source='proxpl-taint',
                        )
                        diagnostics.append(diag)
    return taint, diagnostics


def publish_taint(ls: LanguageServer, uri: str, text: str):
    taint_map, diagnostics = analyze_taint(text)
    # publish diagnostics
    ls.publish_diagnostics(uri, diagnostics)

    # publish semantic tokens via a custom notification for demo clients
    tokens = []
    for lineno, line in enumerate(text.splitlines()):
        for var in re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b", line):
            if var in taint_map and taint_map[var] == 'tainted':
                start = line.find(var)
                length = len(var)
                tokens.extend([lineno, start, length, TAINT_TOKEN_TYPE, 0])
    ls.send_notification('proxpl/taintTokens', {'uri': uri, 'tokens': tokens})


@SERVER.feature(TEXT_DOCUMENT_DID_OPEN)
def did_open(ls: LanguageServer, params: DidOpenTextDocumentParams):
    doc = params.text_document
    publish_taint(ls, doc.uri, doc.text)


@SERVER.feature(TEXT_DOCUMENT_DID_CHANGE)
def did_change(ls: LanguageServer, params: DidChangeTextDocumentParams):
    # Use the workspace document if available
    try:
        doc = ls.workspace.get_document(params.text_document.uri)
        text = doc.source
    except Exception:
        # Fallback: use the incremental content from params
        text = params.content_changes[0].text if params.content_changes else ''
    publish_taint(ls, params.text_document.uri, text)


@SERVER.feature('textDocument/codeAction')
def code_action(ls: LanguageServer, params):
    # Look for diagnostics at the requested range and provide a quick fix
    uri = params.text_document.uri
    actions = []
    diagnostics = params.context.diagnostics
    for diag in diagnostics:
        if diag.source == 'proxpl-taint':
            # extract variable name from message
            m = re.search(r"'([A-Za-z_][A-Za-z0-9_]*)'", diag.message)
            if not m:
                continue
            var = m.group(1)
            # create a TextEdit to wrap the variable with sanitize_json(var)
            start = diag.range.start
            end = diag.range.end
            edit = TextEdit(range=Range(start=start, end=end), new_text=f"sanitize_json({var})")
            wedit = WorkspaceEdit(changes={uri: [edit]})
            ca = CodeAction(title=f"Sanitize '{var}' with sanitize_json()",
                            kind=CodeActionKind.QuickFix,
                            edit=wedit)
            actions.append(ca)
    return actions


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--stdio', action='store_true', help='Run LSP server over stdio (default True)')
    args = parser.parse_args()
    SERVER.start_io()


if __name__ == '__main__':
    main()
