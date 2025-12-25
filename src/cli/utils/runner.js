import { spawn } from 'child_process';
import path from 'path';
import { fileURLToPath } from 'url';
import { logger } from './logger.js';
import fs from 'fs-extra';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const ROOT_DIR = path.resolve(__dirname, '../../../');

/**
 * Finds the ProXPL executable based on OS and common build locations
 */
export async function findExecutable() {
    const possiblePaths = [
        path.join(ROOT_DIR, 'proxpl.exe'),
        path.join(ROOT_DIR, 'proxpl'),
        path.join(ROOT_DIR, 'build', 'proxpl.exe'),
        path.join(ROOT_DIR, 'build', 'proxpl'),
        path.join(ROOT_DIR, 'bin', 'proxpl.exe'),
        path.join(ROOT_DIR, 'bin', 'proxpl'),
    ];

    for (const p of possiblePaths) {
        if (await fs.pathExists(p)) {
            return p;
        }
    }

    return null;
}

/**
 * Runs the ProXPL executable with given arguments
 */
export async function runProXPL(args, options = {}) {
    const executable = await findExecutable();

    if (!executable) {
        logger.error('ProXPL executable not found.', 'Please build the project first using CMake.');
        process.exit(1);
    }

    logger.debug(`Executing: ${executable} ${args.join(' ')}`);

    return new Promise((resolve, reject) => {
        const child = spawn(executable, args, {
            stdio: 'inherit',
            ...options
        });

        child.on('close', (code) => {
            if (code === 0) {
                resolve();
            } else {
                reject(new Error(`ProXPL exited with code ${code}`));
            }
        });

        child.on('error', (err) => {
            reject(err);
        });
    });
}
