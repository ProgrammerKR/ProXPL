# VS Code Extension Quickstart for ProXPL

Follow these steps to compile and test the extension features (Run Code, Formatter, and Hover Support).

## Prerequisites

Ensure you have [Node.js](https://nodejs.org/) installed, which includes `npm`.

## 1. Install Dependencies

Open a terminal in the `extension/` directory and run:

```bash
npm install
```

This will install the VS Code API types and the TypeScript compiler.

## 2. Compile the TypeScript Code

To translate the `.ts` files into executable `.js` files in the `out/` directory:

```bash
npm run compile
```

If you want to keep the compiler running and watching for changes:

```bash
npm run watch
```

## 3. Launch and Test the Extension

1.  In the current VS Code window (where the `extension/` folder is open), press **F5**.
2.  A new window titled **[Extension Development Host]** will open.
3.  In that new window, open or create a file with the `.prox` extension (e.g., `hello.prox`).
4.  **Test Run Code**: Click the **Play button** in the top-right corner of the editor.
5.  **Test Hover**: Hover over keywords like `func`, `var`, or `print`.
6.  **Test Formatter**: Right-click in the editor and select **Format Document** (or use `Alt+Shift+F`).

## 4. Troubleshooting

- If the **Play button** doesn't appear, ensure the file ends in `.prox`.
- If the terminal says `proxpl not found`, ensure the ProXPL compiler is installed and added to your system PATH.
- If changes aren't reflected, ensure you've run `npm run compile` and restarted the host window via the debug toolbar.
