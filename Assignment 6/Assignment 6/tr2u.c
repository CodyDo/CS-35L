#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Prototype
int checkRepeat(const char* string, size_t fromLength);

int main(int argc, const char* argv[]) {

    // Checks to make sure there is an appropriate number of arguments
    if (argc > 3) {
        char* error = "Error: Too many arguments\n";
        write(2, error, strlen(error));
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
        char* error = "Error: 'from' and 'to' must be the same length\n";
        write(2, error, strlen(error));
        exit(1);
    }

    // Makes sure that 'from' does not have any repeating value
    if (checkRepeat(from, fromLength)) {
        char* error = "Error: 'from' cannot have any duplicates\n";
        write(2, error, strlen(error));
        exit(1);
    }

    // At this point, the 'from' and 'to' strings are okay. Prepare for loop.
    // noChange is used inside the loop as a way to check if the current character
    // has already been written in its altered form.
    char currChar;
    int noChange = 1;

    // While loop runs ONLY if read returns a value greater than 0. This only
    // happens if there is still characters being read.
    while (read(0, &currChar, 1) > 0) {

        // Loop through 'from' string to check for matches.
        for (int i = 0; i < fromLength; i++) {
            // If currChar exists inside 'from', output changed version.
            if (from[i] == currChar) {
                noChange = 0;
                write(1, &to[i], 1);
                break;
            }
        }

        // If noChange is 0 (false) then it was already output in the above
        // for loop. If it's 1 (true) then it hasn't and must be output.
        if (noChange) {
            write(1, &currChar, 1);
        }

        // Reset noChange for next iteration.
        noChange = 1;
    }

    exit(0);
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
