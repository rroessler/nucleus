const fs = require('fs');
const path = require('path');
const readline = require('readline');

// Nucleus Native Binding
const { binding_file, binding_eval } = require('./build/Release/binding.node');

/**
 * Runs a Nucleus File. Makes sure the file exists, is a ".nuc" file and runs a resolved
 * full path to the file.
 * @param {string} filename             Filename of Nucleus file to run.
 */
const run = filename => {
    // ensure file exists
    if (!fs.existsSync(filename)) throw new TypeError('File does not exist.');

    // and has the correct file extension
    if (path.extname(filename) !== '.nuc') throw new TypeError('File does not have a ".nuc" file extension.');

    binding_file(path.resolve(filename)); // make sure to resolve to full path
};

/**
 * Runs a given Nucleus source code input. This method expects a string of Nucleus code
 * instead of a file to run.
 * @param {string} input                Input to run.
 */
const eval = input => {
    binding_eval(input);
};

/** Coordinates an instance of a Nucleus REPL. */
const repl = async () => {
    // set up a readline interface for allowing a question/response input
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    // and coordinate the repl
    while (true) {
        // await the response before continuing
        const input = await new Promise(resolve =>
            rl.question("\x1b[36mnucleus\x1b[0m > ", resolve));

        if (input == "exit") break;
        binding_eval(input); // eval the input
    }

    // and clean up the readline interface
    rl.close();
    process.exit(0);
}

// and export under Nucleus object for stricter naming
module.exports = { nucleus: Object.freeze({ run, eval, repl }) };