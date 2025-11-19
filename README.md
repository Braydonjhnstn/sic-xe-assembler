##
## Name: Braydon Johnston REDid: 131049942 Class Acc: cssc2115
## Name: Reinaldo Roldan  REDid: 825760540 Class Acc: cssc2131
##


# SIC/XE Assembler (asx)

A two-pass assembler for the SIC/XE (Simplified Instructional Computer / Extended Edition) architecture.

yes

## Building

[cssc@edoras ~/a2] make

This will create the `asx` executable.

## Usage


[cssc@edoras ~/a2] ./asx <input_file1.sic> [input_file2.sic ...]

The assembler accepts one or more XE assembler source files as command-line arguments.
If no arguments are provided, the program will print an error message and terminate.

## Output

For each input file, the assembler generates:
- Listing file (`.l` extension) - Contains the assembled listing with addresses and object code
- Symbol table file (`.st` extension) - Contains all symbols and their addresses

**Note:** The assembler does NOT generate object code files (`.obj`).

## Project Structure

- `include/` - Header files
- `src/` - Source files
- `tests/` - Test SIC assembly files

## Features

- Two-pass assembly process
- Symbol table management
- Operation code table (OPTAB) with SIC/XE instructions
- XE assembler directives (WORD, BYTE, RESW, RESB, START, END)
- Error detection and reporting
- Listing file generation
- Symbol table file generation

## Testing

Run all test files:

[cssc@edoras ~/a2] make test
