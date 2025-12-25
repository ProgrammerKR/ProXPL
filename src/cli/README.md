# ProXPL CLI

Professional Command-Line Interface for the ProX Programming Language.

## Features

- **Run Script**: Execute `.prox` files with `prox run <file>`.
- **REPL**: Interactive shell with `prox repl` or by running `prox` without arguments.
- **Build**: Compile scripts to bytecode (currently a placeholder for future VM support).
- **Watch Mode**: Automatically rerun scripts on change with `--watch`.
- **Debug/Verbose**: Get detailed execution info with `--debug` and `--verbose`.
- **Config**: Support for `.proxrc` or `prox.json` configuration files.

## Installation

```bash
cd src/cli
npm install
npm link
```

## Usage

### Running a script
```bash
prox run examples/hello.prox
```

### Watch mode
```bash
prox run main.prox --watch
```

### REPL
```bash
prox repl
```

### Help
```bash
prox --help
```

## Configuration

You can create a `.proxrc` or `prox.json` in your project root to set default options:

```json
{
  "debug": true,
  "verbose": false
}
```

## Development

The CLI is built with Node.js and uses the `commander` package. It interacts with the C-based ProXPL core by spawning it as a child process.

### Directory Structure
- `index.js`: Main entry point.
- `commands/`: Individual command implementations.
- `utils/`: Helper utilities (logger, runner, config).
