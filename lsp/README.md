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

Quick test
----------
Create a test file `example.prox` with the following content:

```
data = recv()
db.insert(data)
```

Start the server and connect your LSP-capable editor; the server will publish a warning diagnostic for `data` used in `db.insert` and offer a code action to wrap it with `sanitize_json(data)`.


Note: This is a minimal prototype for demonstration. Integrating with editors requires providing the appropriate semantic token legend and mapping token types on the client.
