#!/usr/bin/env node

// node modules required
const { nucleus } = require('./index.js');
const { Command } = require('commander');

// if given NO arguments, then set up the repl
if (process.argv.length <= 2) return nucleus.repl();

// build the CLI program
const program = new Command();

// set up the program version
program.version("0.2.0", '-v, --version', 'Outputs the Nucleus CLI version.');

// add program commands
program // 'empty' action
    .command('* [filename]')
    .description('If given a Nucleus filename, then runs a file. Otherwise coordinates a Nucleus REPL instance.')
    .action(filename => {
        if (filename) nucleus.run(filename);
        else nucleus.repl();
    });

program // 'run' action
    .command('run <filename>')
    .description('Runs a Nucleus source file.')
    .action(nucleus.run);

program // 'eval' action
    .command('eval <input>')
    .description('Evaluates a given Nucleus source code input.')
    .action(nucleus.eval);

// and parse the program
program.parse(process.argv);