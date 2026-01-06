#!/usr/bin/env node

import { Command } from 'commander';
import { runCommand } from './commands/run.js';
import { buildCommand } from './commands/build.js';
import { replCommand } from './commands/repl.js';
import { logger } from './utils/logger.js';
import fs from 'fs-extra';
import path from 'path';
import { fileURLToPath } from 'url';

import { loadConfig, mergeOptions } from './utils/config.js';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const packageJson = fs.readJsonSync(path.join(__dirname, 'package.json'));
const config = await loadConfig();

const program = new Command();

program
    .name('prox')
    .description('ProXPL CLI - Professional Command-Line Interface for ProX Programming Language')
    .version(packageJson.version, '-v, --version');

program
    .command('run')
    .alias('r')
    .description('Execute a .prox script')
    .argument('<file>', 'Path to the .prox script')
    .option('-w, --watch', 'Watch mode: automatically rerun on file changes')
    .option('-d, --debug', 'Show detailed debug information')
    .option('-v, --verbose', 'Show verbose execution info')
    .action((file, options) => {
        const merged = mergeOptions(options, config);
        runCommand(file, merged);
    });

program
    .command('build')
    .alias('b')
    .description('Compile a .prox script to bytecode')
    .argument('<file>', 'Path to the .prox script')
    .option('-o, --output <output>', 'Specified output file')
    .action((file, options) => {
        const merged = mergeOptions(options, config);
        buildCommand(file, merged);
    });

program
    .command('repl')
    .description('Start interactive ProXPL shell')
    .action(() => {
        replCommand();
    });

// Handle unknown commands
program.on('command:*', () => {
    logger.error('Invalid command: ' + program.args.join(' '));
    logger.info('See "prox --help" for available commands.');
    process.exit(1);
});

// Default behavior: run repl if no args, or run file if first arg is a file
if (process.argv.length === 2) {
    replCommand();
} else {
    program.parse(process.argv);
}
