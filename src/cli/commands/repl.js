import { runProXPL } from '../utils/runner.js';
import { logger } from '../utils/logger.js';

export async function replCommand() {
    logger.info('Starting ProXPL REPL...');
    try {
        await runProXPL([]);
    } catch (err) {
        process.exit(1);
    }
}
