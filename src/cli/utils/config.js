import fs from 'fs-extra';
import path from 'path';
import { logger } from './logger.js';

const CONFIG_FILENAMES = ['.proxrc', 'prox.json', '.proxrc.json'];

export async function loadConfig(cwd = process.cwd()) {
    for (const filename of CONFIG_FILENAMES) {
        const configPath = path.join(cwd, filename);
        if (await fs.pathExists(configPath)) {
            try {
                const config = await fs.readJson(configPath);
                logger.debug(`Loaded config from ${filename}`);
                return config;
            } catch (err) {
                logger.warn(`Failed to parse config file ${filename}: ${err.message}`);
            }
        }
    }
    return {};
}

export function mergeOptions(cmdOptions, config) {
    return {
        ...config,
        ...cmdOptions
    };
}
