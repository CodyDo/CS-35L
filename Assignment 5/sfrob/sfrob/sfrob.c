#include <stdio.h>
#include <stdlib.h>

// Prototypes for functions
int frobcmp(char const *a, char const *b);
int mainCmp(const void *a, const void *b);
void checkErrors(int n);

int main() {
    // currWord holds the current word while allWords holds all the words
    // currWord are pointers to chars while allWords contains pointers to these pointers.
    // currWordPos and allWordsSize are the positions/size, respectively.
    char *currWord = (char*) malloc(sizeof(char));
    char **allWords = (char**) malloc(sizeof(char*));
    int currWordPos = 0;
    int allWordsSize = 0;
    
    // Check that allocation was successful
    if (currWord == NULL || allWords == NULL) {
        fprintf(stderr, "Error: Could not allocate memory");
        exit(1);
    }
    
    // Get the first character and check for input errors
    char currChar = getchar();
    checkErrors(1);
    
    // While we are NOT at the end of file while reading from stdin
    while (!feof(stdin)) {
        // Check for input errors at the beginning of each loop
        checkErrors(1);
        
        // If currChar is NOT a space, create a new word array based off
        // currWord array. Basically add the character and allocate larger
        // array
        if (currChar != ' ') {
            char* newWord = (char*) realloc(currWord, (currWordPos+2) * sizeof(char));
            
            // Make sure newWord allocated successfully. If not, free everything
            // and exit
            if (newWord == NULL) {
                for (int i = 0; i < allWordsSize; i++){
                    free(allWords[i]);
                }
                free(allWords);
                fprintf(stderr, "Error: Could not allocate memory");
                exit(1);
            }
            
            // Set currWord to this new word array and add currChar
            // to the appropriate location.
            currWord = newWord;
            currWord[currWordPos] = currChar;
            currWordPos++;
        }
        
        // If the current character is a space and the word size is not 0,
        // then we have reached the end of an input word. Do same approach as
        // above if-statement, but apply it to allWords as well.
        if (currChar == ' ' && currWordPos != 0) {
            char* newWord = (char*) realloc(currWord, (currWordPos+2) * sizeof(char));
            char** newAllWords = (char**) realloc(allWords, (allWordsSize+2) * sizeof(char*));
            
            // Make sure both newWord and newAllWords have been allocated properly.
            // If not, free everything and exit
            if (newWord == NULL || newAllWords == NULL) {
                for (int i = 0; i < allWordsSize; i++){
                    free(allWords[i]);
                }
                free(allWords);
                fprintf(stderr, "Error: Could not allocate memory");
                exit(1);
            }
            
            // Set currWord to newWord. Add the character to the end.
            currWord = newWord;
            currWord[currWordPos] = currChar;
            
            // set allWords to newAllWords. Add the word to the appropriate spot
            // and increment the position. currWord is now held by allWords.
            allWords = newAllWords;
            allWords[allWordsSize] = currWord;
            allWordsSize++;
            
            // Reset currWord to prepare for the next word (if it exists)
            currWord = (char*) malloc(sizeof(char));
            currWordPos = 0;
        }
        
        // Get the next character and continue looping. This will also be
        // instantly run if the current character is a space and there has
        // been no words input yet (essentially a bunch of leading spaces).
        currChar = getchar();
    }
    
    // This if statement only runs if we have reached the end of the file
    // BUT there was not a space at the end. Thus, anyything that was added
    // to currWords must be added to allWords.
    if (currWordPos > 0) {
        char* newWord = (char*) realloc(currWord, (currWordPos+2) * sizeof(char));
        char** newAllWords = (char**) realloc(allWords, (allWordsSize+2) * sizeof(char*));
        
        // Make sure both newWord and newAllWords have been allocated properly.
        // If not, free everything and exit
        if (newWord == NULL || newAllWords == NULL) {
            free(currWord);
            free(allWords);
            fprintf(stderr, "Error: Could not allocate memory");
            exit(1);
        }
        
        // Set currWord to newWord. Add a space to the end.
        currWord = newWord;
        currWord[currWordPos] = ' ';
        
        // set allWords to newAllWords. Add the word to the appropriate spot
        // and increment the position. currWord is now held by allWords.
        allWords = newAllWords;
        allWords[allWordsSize] = currWord;
        allWordsSize++;
    }
    
    // Call qsort on allWords. Use mainCmp which contains the
    // appropriate casting and utilizes frobcmp.
    qsort(allWords, allWordsSize, sizeof(char*), mainCmp);
    
    // Now that everything has been sorted, print all the words out
    for (int i = 0; i < allWordsSize; i++ ) {
        for (int j = 0 ;; j++) {
            // Output each character and check for output errors
            putchar(allWords[i][j]);
            checkErrors(2);
            
            // If the character output was a space, the word is finished.
            // Break out of inner for loop and begin next word.
            if (allWords[i][j] == ' ') {
                break;
            }
        }
    }
    
    // Everything is finished! Free everything and exit successfully :)
    for (int i = 0; i < allWordsSize; i++){
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
        
        char orgA = *a ^ 42;
        char orgB = *b ^ 42;
        
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


// Checks for errors depending on input
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
