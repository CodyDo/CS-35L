#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> // This is the version with a dedicated reg file part
#include <sys/stat.h>
#include <ctype.h>

// Prototypes for functions
int frobcmp(char const *a, char const *b);
int mainCmp(const void *a, const void *b);

// Global Variable for -f flag
int f_flag = 0;

int main(int argc, const char* argv[]) {
    // Checks the number of arguments present. If more than 2, then too many
    // arguments. If == 2 check if the first is the -f flag option.
    if (argc > 2) {
        char* error = "Error: Too many arguments\n";
        write(2, error, strlen(error));
        exit(1);
    } else if (argc == 2) {
        // If the first argument is NOT "-f", then output a message and exit
        if (strcmp(argv[1], "-f") != 0) {
            char* error = "Error: Invalid option\n";
            write(2, error, strlen(error));
            exit(1);
        }
        
        // If you get here, then the first argument is, in fact, "-f". Set the
        // global variable so we know.
        f_flag = 1;
    }
    
    // Set up fstat and check for any potential errors.
    struct stat fileData;
    if (fstat(0, &fileData) < 0) {
        char* error = "Error: fstat execution has failed\n";
        write(2, error, strlen(error));
        exit(1);
    }
    
    // Set up default variables. allWordsSize is used to malloc allWords later.
    // Since each pointer is 8 bytes and we want a default of 8kb, allWordsSize
    // is 1000. wordSize is set to an arbitrary 100
    // allWordsPos and wordPos keep track of positional information for each array
    // or string.
    long allWordsSize = 8192;
    int wordSize = 10;
    int allWordsPos = 0;
    int wordsPos = 0;
    char** allWords;
    
    // Now check if the file is regular. If it is, set the correct
    // variable. If it's a regular file OVERRIDE allWordsSize so the
    // proper amount is malloc'd.
    if (S_ISREG(fileData.st_mode)) {
        // Allocate proper size and do error checking.
        allWordsSize = fileData.st_size + 1;
        allWords = (char**) malloc(allWordsSize);
        if (allWords == NULL) {
            char* error= "Error: Could not allocate memory\n";
            write(2, error, strlen(error));
            exit(1);
        }
        
        // Set things up for the while loop. Check for reading errors.
        char c;
        long bytesRead = read(0, &c, 1);
        if (bytesRead < 0) {
            char* error= "Error: Could not read file\n";
            write(2, error, strlen(error));
            exit(1);
        }
        
        // While there is still information being read from stdin (aka bytesRead
        // doesn't return 0) loop through.
        while (bytesRead > 0) {
            // Add the character to the end.
            allWords[allWordsPos][wordsPos] = c;
            
            // If it's the end of a word (aka there's a space), increment allWordsPos.
            if (c == ' ') {
                allWordsPos++;
                
                // If buffer is more than half full, reallocate to twice the size
                // Check for errors and exit if needed/free everything if needed.
                // (From Hints Slides)
                if (allWordsPos >= 0.5*allWordsSize) {
                    allWordsSize *= 2;
                    char** newAllWords = (char**) realloc(allWords, allWordsSize);
                    if (newAllWords == NULL) {
                        char* error= "Error: Could not allocate memory\n";
                        write(2, error, strlen(error));
                        for (int i = 0; i < allWordsPos; i++){
                            free(allWords[i]);
                        }
                        exit(1);
                    }
                    allWords = newAllWords;
                }
                
                // Reset wordPos for the next word
                wordsPos = 0;
            }
            
            // If it's not a space, increment wordPos and do sizing checks.
            // Reallocate and check for errors if necessary.
            if (c != ' ') {
                wordsPos++;
            }
            
            // Increment bytesRead for the next loop. Check for errors (lower than 0)
            bytesRead = read(0, &c, 1);
            if (bytesRead < 0) {
                char* error= "Error: Could not read file\n";
                write(2, error, strlen(error));
                exit(1);
            }
        }

        // After the while loop, if wordPos is > zero, then there is a word that
        // was not added yet. Additionally, increase allWordsPos by 1 afterwards.
        if (wordsPos > 0 ) {
            allWords[allWordsPos][wordsPos] = ' ';
            allWordsPos++;
        }
        
    }
    
    else {
    // allWords holds all the words. currWord holds the current word.
    // Allocate and check for errors.
    allWords = (char**) malloc(allWordsSize);
    char* currWord = (char*) malloc(wordSize*sizeof(char));
    if (allWords == NULL || currWord == NULL ) {
        char* error= "Error: Could not allocate memory\n";
        write(2, error, strlen(error));
        exit(1);
    }

    // Set things up for the while loop. Check for reading errors.
    char c;
    long bytesRead = read(0, &c, 1);
    if (bytesRead < 0) {
        char* error= "Error: Could not read file\n";
        write(2, error, strlen(error));
        exit(1);
    }
    
    // While there is still information being read from stdin (aka bytesRead
    // doesn't return 0) loop through.
    while (bytesRead > 0) {
        // Add the character to currWord.
        currWord[wordsPos] = c;
        
        // If it's the end of a word (aka there's a space), add currWord to
        // allWords. Increment allWordsPos.
        if (c == ' ') {
            allWords[allWordsPos] = currWord;
            allWordsPos++;
            
            // If buffer is more than half full, reallocate to twice the size
            // Check for errors and exit if needed/free everything if needed.
            // (From Hints Slides)
            if (allWordsPos >= allWordsSize) {
                allWordsSize *= 2;
                char** newAllWords = (char**) realloc(allWords, allWordsSize*sizeof(char*));
                if (newAllWords == NULL) {
                    char* error= "Error: Could not allocate memory\n";
                    write(2, error, strlen(error));
                    for (int i = 0; i < allWordsPos; i++){
                        free(allWords[i]);
                    }
                    free(currWord);
                    exit(1);
                }
                allWords = newAllWords;
            }
            
            // Reset currWord and wordPos for the next word. Check for errors.
            wordsPos = 0;
            currWord = (char*) malloc(wordSize*sizeof(char));
            if (currWord == NULL) {
                char* error= "Error: Could not allocate memory\n";
                write(2, error, strlen(error));
                for (int i = 0; i < allWordsPos; i++){
                    free(allWords[i]);
                }
                exit(1);
            }
        }
        
        // If it's not a space, increment wordPos and do sizing checks.
        // Reallocate and check for errors if necessary.
        if (c != ' ') {
            wordsPos++;
            if (wordsPos >= wordSize) {
                wordSize *= 2;
                char* newCurrWord = (char*) realloc(currWord, wordSize*sizeof(char));
                if (newCurrWord == NULL) {
                    char* error= "Error: Could not allocate memory\n";
                    write(2, error, strlen(error));
                    for (int i = 0; i < allWordsPos; i++){
                        free(allWords[i]);
                    }
                    free(currWord);
                    exit(1);
                }
                currWord = newCurrWord;
            }
        }
        
        // Increment bytesRead for the next loop. Check for errors (lower than 0)
        bytesRead = read(0, &c, 1);
        if (bytesRead < 0) {
            char* error= "Error: Could not read file\n";
            write(2, error, strlen(error));
            exit(1);
        }
    }

    // After the while loop, if wordPos is > zero, then there is a word that
    // was not added yet. Additionally, increase allWordsPos by 1 afterwards.
    if (wordsPos > 0 ) {
        currWord[wordsPos] = ' ';
        allWords[allWordsPos] = currWord;
        allWordsPos++;
    }
}
    
    // Call qsort on allWords. Use mainCmp which contains the
    // appropriate casting and utilizes frobcmp.
    qsort(allWords, allWordsPos, sizeof(char*), mainCmp);
    
    // Now that everything has been sorted, print all the words out
    // Use a buffer read (output whole words at once)
    ssize_t fileStatus;
    for (int i = 0; i < allWordsPos; i++ ) {
        int j = 0;
        int num = 0;

        // While the current row of allWords doesn't have a space
        // increment j and num. j is used for the array and num is used
        // for writing.
        while (allWords[i][j] != ' ') {
            j++;
            num++;
        }
        num++;

        fileStatus = write(1, allWords[i], num);
        if (fileStatus < 0) {
            char* error = "Error: Could not write output\n";
            write(2, error, strlen(error));
            exit(1);
        }
    }
 
    // Everything is finished! Free everything and exit successfully :)
    for (int i = 0; i < allWordsPos; i++){
        free(allWords[i]);
    }
    free(allWords);
    exit(0);
}

// // // // // // // // // // // //
//  HELPER FUNCTION DEFINITIONS  //
// // // // // // // // // // // //

// Negative, zero, or positive depending on whether
// a is less than, equal to, or greater than b respectively
int frobcmp(char const *a, char const *b) {
    // Loop through the entire array of words
    for (;; a++, b++) {
        // If both *a and *b are spaces, they are the same. Return 0
        if (*a == ' ' && *b == ' ' ) {
            return 0;

        }

        unsigned char orgA = *a ^ 42;
        unsigned char orgB = *b ^ 42;
        
        // If -f option was used, change them to uppercase only if they
        // are between 'a' and 'z'
        if (f_flag == 1) {
            orgA = toupper(orgA);
            orgB = toupper(orgB);
        }

        // If non-obfuscated a < non-obfuscated b OR *a is a space, return -1
        if (orgA < orgB || *a == ' ' ) {
            return -1;

        }

        // If non-obfuscated a > non-obfuscated b OR *b is a space, return 1
        if (orgA > orgB || *b == ' ' ) {
            return 1;

        }
    }
}

// This is the compare function used inside qsort
// Cast to pointers to pointers since that is what allWords holds
int mainCmp(const void *a, const void *b) {
    const char* newA = *(const char**)a;
    const char* newB = *(const char**)b;
    return frobcmp(newA, newB);
}



