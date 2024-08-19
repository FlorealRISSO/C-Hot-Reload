# C Hot Reload Proof of Concept

This project demonstrates a proof of concept for hot reloading in C. It allows you to dynamically update a function in a running program without stopping or restarting the process.

## How It Works

1. The program starts by running a function `f1()` in a loop, printing its return value every second.

2. When a SIGINT signal (Ctrl+C) is received, the program:
   - Uses `objdump` to disassemble the `f1()` function from the object file.
   - Processes the disassembled output to extract the machine code.
   - Allocates a new executable memory page using `mmap`.
   - Copies the extracted machine code to this new memory page.
   - Updates the function pointer to point to this new memory location.

3. After the signal handler completes, the program continues running, now using the updated version of the function.

This approach allows you to modify the `f1()` function, recompile it, and have the running program pick up the changes without restarting.

## Usage

To try out this proof of concept:

1. Compile the project:
   ```
   make
   ```

2. Run the program:
   ```
   ./main
   ```
   You should see the current version of `f1()` being called repeatedly.

3. Send a SIGINT signal to the running program (press Ctrl+C in its terminal).
   You should see "Caught SIGINT!" printed, and then the output should change to reflect the new version of `f1()`.

4. To stop the program, send a SIGQUIT signal (Ctrl+\ on most systems).

## Notes

- This is a proof of concept and not intended for production use.
- The hot reloading mechanism only works for the specific `f1()` function.
- Ensure that the new version of the function has the same signature and doesn't introduce new dependencies.
- This approach bypasses normal safety checks and can lead to crashes if not used carefully.
- The `opcode.sh` file contains the shell commands used for disassembling the function. This script extracts the machine code (opcodes) from the compiled object file using `objdump` and processes the output with `sed` and `tr` commands. It provides a reliable method for obtaining the raw instruction bytes, which are crucial for the hot reloading process. You can examine this file to understand the exact disassembly process or modify it if you need to adjust the extraction method for different architectures or compilation settings.

## Requirements

- A Unix-like operating system (Linux, macOS, etc.)
- GCC or Clang compiler
- Make
- objdump utility
