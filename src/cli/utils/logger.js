import chalk from 'chalk';

export const logger = {
  info: (msg) => console.log(chalk.blue('ℹ ') + msg),
  success: (msg) => console.log(chalk.green('✔ ') + chalk.bold(msg)),
  warn: (msg) => console.log(chalk.yellow('⚠ ') + msg),
  error: (msg, detail) => {
    console.error(chalk.red('✘ ') + chalk.red.bold(msg));
    if (detail) console.error(chalk.gray(detail));
  },
  debug: (msg) => {
    if (process.env.DEBUG || process.argv.includes('--debug')) {
      console.log(chalk.magenta('DEBUG: ') + msg);
    }
  },
  verbose: (msg) => {
    if (process.env.VERBOSE || process.argv.includes('--verbose')) {
      console.log(chalk.cyan('VERBOSE: ') + msg);
    }
  },
  bold: (msg) => chalk.bold(msg),
  dim: (msg) => chalk.dim(msg),
};
