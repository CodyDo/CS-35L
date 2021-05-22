#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prototypes for functions
void checkErrors(int n);
int checkRepeat(const char* string, size_t fromLength);

int main(int argc, const char* argv[]) {

    // Checks to make sure there is an appropriate number of arguments
    if (argc > 3) {
        fprintf(stderr, "Error: Too many arguments\n");
        exit(1);
    }

    // Pulls the 'from' and 'to' arguments and set them as variables
    // Create length variables for them as well
    const char* from = argv[1];
    const char* to = argv[2];
    size_t fromLength=strlen(from);
    size_t toLength=strlen(to);

    // Checks to make sure they're the same size
    if (fromLength != toLength) {
        fprintf(stderr, "Error: 'from' and 'to' must be the same length\n");
        exit(1);
    }

    // Makes sure that 'from' does not have any repeating value
    if (checkRepeat(from, fromLength)) {
        fprintf(stderr, "Error: 'from' cannot have any duplicates\n");
        exit(1);
    }

    // At this point, the 'from' and 'to' strings are okay so start taking
    // things in from standard input. Check for any errors.
    char currChar = getchar();
    checkErrors(1);

    // While we are NOT at the end of file while reading from stdin
    while (!feof(stdin)) {
        // Loop through 'from' string to check for matches.
        for (int i = 0; i < fromLength; i++) {
            // If currChar exists inside 'from', change it
            if (from[i] == currChar) {
                currChar = to[i];
                break;
            }
        }

        // Output the character & reset for next iteration (if it exists)
        putchar(currChar);
        checkErrors(2);
        currChar = getchar();
        checkErrors(1);
    }

    exit(0);
}

// Checks for errors depending on input.
// n = 1 -- stdin (input)
// n = 2 -- stdout (output)
void checkErrors(int n) {
    if (n == 1 && ferror(stdin) != 0) {
        fprintf(stderr, "Error: Can't read input");
        exit(1);
    }
    else if (n == 2 && ferror(stdout) != 0) {
        fprintf(stderr, "Error: Can't write output");
        exit(1);
    }
}

// Checks for any repeats. Takes in a pointer to the 'from' string and the
// size of the string. Loops through the string, checking each character with
// all characters occurring AFTER the letter for repeats.
// Returns 1 (True) for repeat and 0 (False) for no repeat.
int checkRepeat(const char* string, size_t fromLength) {
    for (int i = 0; i < fromLength; i++) {
        for (int j = i+1; j < fromLength; j++) {
            if (string[i] == string[j]) {
                return 1;
            }
        }
    }
    return 0;
}
