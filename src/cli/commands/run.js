import { runProXPL } from '../utils/runner.js';
import { logger } from '../utils/logger.js';
import chokidar from 'chokidar';
import path from 'path';
import fs from 'fs-extra';

export async function runCommand(file, options) {
    if (!file) {
        logger.error('No file specified.', 'Usage: prox run <file>');
        process.exit(1);
    }

    const filePath = path.resolve(file);

    if (!await fs.pathExists(filePath)) {
        logger.error(`File not found: ${file}`);
        process.exit(1);
    }

    if (options.watch) {
        logger.info(`Watching for changes in ${path.basename(file)}...`);

        const watcher = chokidar.watch(filePath);

        let isRunning = false;

        const execute = async () => {
            if (isRunning) return;
            isRunning = true;

            console.clear();
            logger.info(`Rerunning ${path.basename(file)}...`);

            try {
                await runProXPL(['run', filePath]);
                logger.success('Execution finished.');
            } catch (err) {
                // Error already logged by runner stdio:inherit
            } finally {
                isRunning = false;
                logger.dim('\nWaiting for changes...');
            }
        };

        watcher.on('change', execute);
        execute(); // Initial run
    } else {
        try {
            await runProXPL(['run', filePath]);
        } catch (err) {
            process.exit(1);
        }
    }
}
