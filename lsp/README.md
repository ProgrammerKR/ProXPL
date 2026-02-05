ProXPL LSP Prototype
=====================

This folder contains a minimal LSP prototype that highlights identifiers prefixed with `taint_` as tainted tokens.

Setup
-----
Install dependencies in a virtualenv:

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
```

Run
---

```powershell
python proxpl_lsp_taint.py
```

Note: This is a minimal prototype for demonstration. Integrating with editors requires providing the appropriate semantic token legend and mapping token types on the client.
