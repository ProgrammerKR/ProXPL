import { runProXPL } from '../utils/runner.js';
import { logger } from '../utils/logger.js';
import path from 'path';
import fs from 'fs-extra';

export async function buildCommand(file, options) {
    if (!file) {
        logger.error('No file specified.', 'Usage: prox build <file>');
        process.exit(1);
    }

    const filePath = path.resolve(file);

    if (!await fs.pathExists(filePath)) {
        logger.error(`File not found: ${file}`);
        process.exit(1);
    }

    logger.info(`Compiling ${path.basename(file)}...`);

    try {
        // Current C implementation has a stub for build
        await runProXPL(['build', filePath]);
        logger.success('Build successful.');
    } catch (err) {
        process.exit(1);
    }
}
