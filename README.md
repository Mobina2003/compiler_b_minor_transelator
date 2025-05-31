# Bminor to C Translator

## Overview

Bminor to C is a simple compiler/translator written in C that converts Bminor source code into equivalent C source code. Bminor is a custom, simplified programming language designed for educational purposes, demonstrating fundamental language features like variables, functions, control flow (if/else, for loops), arrays, and basic I/O. The Bminor2C translator processes Bminor code line by line from standard input and outputs the translated C code to standard output.

## Features

The Bminor to C translator supports the following Bminor features:

* **Global and Local Variable Declarations:** Translates `integer`, `string`, and `boolean` variable declarations with and without initial assignments.
* **Function Definitions:** Correctly translates Bminor function signatures (name, parameters, return type) into C function definitions. Includes special handling for the `main` function.
* **Basic Data Types:** Supports `integer` (maps to `int`), `string` (maps to `char*`), and `boolean` (maps to `bool`).
* **Arithmetic Operations:** Translates standard arithmetic operators (`+`, `-`, `*`, `/`, `%`).
* **Power Operator (`^`):** Converts the Bminor power operator (`^`) into the C `pow()` function (from `math.h`).
* **Logical Operators:** Converts Bminor's `and`, `or`, and `not` into C's `&&`, `||`, and `!` respectively.
* **Conditional Statements (`if`/`else`):** Translates Bminor's `if (...) { ... } else { ... }` blocks into C.
* **Loops (`for`):** Translates Bminor `for` loops, including declarations of loop variables within the initialization part.
* **Array Handling:**
    * Supports fixed-size `static_array` declarations (e.g., `static_array: array [5] integer;`).
    * Handles dynamic array allocation using `new array[size]` (e.g., `dynamic_array: array [10] integer = new array[10];`).
    * Translates array element access and assignment (e.g., `my_array[index] = value;`).
* **Print Statements:** Converts Bminor `print` statements into C `printf()` calls, automatically determining format specifiers (`%d` for integers, `%s` for strings) for arguments.
* **Comments:** Preserves both single-line (`//`) and multi-line (`/* ... */`) comments.
* **Indentation:** Maintains proper C-style indentation for blocks and statements.

## How to Compile and Run

### Prerequisites

* A C compiler (e.g., GCC)

### Compilation

Navigate to the directory containing `bminor2c.c` and compile it using a C compiler:

```bash
gcc bminor2c.c -o bminor2c -lm
