//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <ctype.h>
//
//// Prototypes for functions
//int frobcmp(char const *a, char const *b);
//int mainCmp(const void *a, const void *b);
//
//// Global Variables
//int f_flag = 0;
//int isRegFile = 0;
//
//int main(int argc, const char* argv[]) {
//    // Checks the number of arguments present. If more than 2, then too many
//    // arguments. If == 2 check if the first is the -f flag option.
//    if (argc > 2) {
//        char* error = "Error: Too many arguments\n";
//        write(2, error, strlen(error));
//        exit(1);
//    } else if (argc == 2) {
//        // If the first argument is NOT "-f", then output a message and exit
//        if (strcmp(argv[1], "-f") != 0) {
//            char* error = "Error: Invalid option\n";
//            write(2, error, strlen(error));
//            exit(1);
//        }
//
//        // If you get here, then the first argument is, in fact, "-f". Set the
//        // global variable so we know.
//        f_flag = 1;
//    }
//
//    // Set up fstat and check for any potential errors.
//    struct stat fileData;
//    if (fstat(0, &fileData) < 0) {
//        char* error = "Error: fstat execution has failed\n";
//        write(2, error, strlen(error));
//        exit(1);
//    }
//
//    // Now check if the file is regular. If it is, set the correct
//    // variable
//    if (S_ISREG(fileData.st_mode)) {
//        isRegFile = 1;
//    }
//
//    off_t fileSize = fileData.st_size + 1;
//
//    // mainPointer is throughout the rest of the logic to handle movement of
//    // words.
//    char* mainPointer;
//    // char* currWord = (char*) malloc(sizeof(char));
//    char** allWords;
//    int currWordPos = 0;
//    int allWordsSize = 0;
//
//    // Handles the logic for a regular file
//    if (isRegFile) {
//        // Allocate enough space for the entire file
//        mainPointer = (char*) malloc(fileSize*sizeof(char));
//
//        // Check that allocation was successful.
//        if (mainPointer == NULL) {
//            char* error= "Error: Could not allocate memory\n";
//            write(2, error, strlen(error));
//            exit(1);
//        }
//
//        // Read everything from the file into mainPointer and store the
//        // amount read. Then check for errors.
//        ssize_t amtRead = read(0, mainPointer, fileSize);
//        if (amtRead < 0) {
//            char* error= "Error: Could not read file\n";
//            write(2, error, strlen(error));
//            exit(1);
//        }
//
//        // Loop through the file and increment allWordsSize. This will
//        // be used to allocate allWords after. We are essentially just finding
//        // the number of words in this file.
//        for (int i = 0; i < amtRead; i++) {
//            // While spaces are read, just keep incrementing i. This helps
//            // combat leading spaces as well
//            while (mainPointer[i] == ' ' && i < amtRead) {
//                i++;
//            }
//
//            // At this point, mainPointer will point at a word OR i >= amtRead.
//            // If mainPointer is NOT equal to a space and i < amtRead
//            if (mainPointer[i] != ' ' && i < amtRead) {
//                // Increment allWordsSize. Now increase i until we get out of the
//                // word (hit a space) or i >= amtRead.
//                allWordsSize++;
//                while (mainPointer[i] != ' ' && i < amtRead) {
//                    i++;
//                }
//            }
//        }
//
//        // Allocate the allWords array. Check for errors. If they exist, free
//        // everything and leave.
//        allWords = (char**) malloc(allWordsSize*sizeof(char*));
//        if (allWords == NULL) {
//            char* error= "Error: Could not allocate memory\n";
//            write(2, error, strlen(error));
//            free(mainPointer);
//            exit(1);
//        }
//
//        // Now add the words to allWords. wordPresent acts as a boolean
//        // to tell the loop whether or not a word should be added.
//        int wordPresent = 0;
//        for (int i = 0; i < amtRead; i++) {
//            // If wordPresent is 0 and the mainPointer is NOT a space, add
//            // the position of the pointer to allWords and increment as needed.
//            if (wordPresent == 0 && mainPointer[i] != ' ') {
//                allWords[currWordPos] = &mainPointer[i];
//                currWordPos++;
//                wordPresent = 1;
//
//            // If wordPresent is 1 and mainPointer points to a space, reset
//            // wordPresent. This means that the loop has traveled through
//            // the entirety of a word and is ready for a new word.
//            } else if (wordPresent == 1 && mainPointer[i] == ' ') {
//                wordPresent = 0;
//            }
//        }
//    }
//
//    // Handles irregular files
//    else {
//        // Allocate memory for arrays/pointers and check to make sure
//        // everything is okay.
//        allWords = (char**) malloc(sizeof(char*));
//        mainPointer = (char*) malloc(sizeof(char));
//        if (allWords == NULL || mainPointer == NULL) {
//             char* error= "Error: Could not allocate memory\n";
//             write(2, error, strlen(error));
//             exit(1);
//         }
//
//        // Set up currChar and nextChar for use in the loop. Check for errors.
//        // [1] is given to currChar and nextChar to remove errors, though [0]
//        // is what is mainly to be used in the loop.
//        char currChar[1];
//        ssize_t amtCurr = read(0, currChar, 1);
//        char nextChar[1];
//        ssize_t amtNext = read(0, nextChar, 1);
//        if (amtNext < 0 || amtCurr < 0) {
//             char* error= "Error: Could not read\n";
//             write(2, error, strlen(error));
//             exit(1);
//         }
//
//        // While loop that continues until EOF (AKA when amtCurr = 0)
//        int wordSize = 0;
//        while (amtCurr > 0 ) {
//            // Reallocate more space and add that current character to mainPointer.
//            // Check for errors and free if needed.
//            char* newWord = (char*) realloc(mainPointer, (wordSize+2) * sizeof(char));
//            if (newWord == NULL) {
//                char* error= "Error: Could not allocate memory\n";
//                write(2, error, strlen(error));
//                free(mainPointer);
//                exit(1);
//            }
//
//            mainPointer = newWord;
//            mainPointer[wordSize] = currChar[0];
//            wordSize++;
//
//            // If the current word is a space, then that means we have a new word.
//            // Add it to the array via reallocation. Check for errors.
//            if (currChar[0] == ' ') {
//                char** newAllWords = (char**) realloc(allWords, (currWordPos+2) * sizeof(char*));
//                if (newAllWords == NULL) {
//                    char* error= "Error: Could not allocate memory\n";
//                    write(2, error, strlen(error));
//                    for (int i = 0; i < allWordsSize; i++){
//                        free(allWords[i]);
//                    }
//                    free(allWords);
//                    free(mainPointer);
//                    exit(1);
//                }
//
//                // Set allWords to newAllWords. Add the mainPointer to the end.
//                // Increment as needed. allWordsSize is incremented because it's
//                // used for freeing memory at the end.
//                allWords = newAllWords;
//                allWords[currWordPos] = mainPointer;
//                currWordPos++;
//                allWordsSize++;
//
//                // Reset currWord to prepare for the next word (if it exists)
//                mainPointer = (char*) malloc(sizeof(char));
//                wordSize = 0;
//
//            }
//
//            // If the current character and the next character are spaces
//            // Then keep looping until the current character is not a space.
//            if (currChar[0] == ' ' && nextChar[0] == ' ') {
//                while (currChar[0] == ' ') {
//                    amtCurr = read(0, currChar, 1);
//                    if (amtCurr < 0) {
//                        char* error= "Error: Could not read file\n";
//                        write(2, error, strlen(error));
//                        exit(1);
//                    }
//
//                    // After the current character is no longer a space. Get
//                    // nextChar up to speed. Increment word size.
//                    amtNext = read(0, nextChar, 1);
//                    if (amtNext < 0) {
//                        char* error= "Error: Could not read file\n";
//                        write(2, error, strlen(error));
//                        exit(1);
//                    }
//                    wordSize++;
//                    continue;
//                }
//            }
//
//            // If amtNext = 0, then we've reached the end of the file.
//            if (amtNext == 0 )
//                break;
//
//            // Set the current word to the next word.
//            currChar[0] = nextChar[0];
//            amtNext = read(0, nextChar, 1);
//        }
//
//        // Add a space to the end if necessary. This should only run if we've reached
//        // the end of the file but a space was not properly added.
////        if (wordSize > 0 && allWords[allWordsSize][wordSize-1] != ' ') {
////            allWords[allWordsSize][wordSize] = ' ';
////        }
//    }
//
//
//
//    // Call qsort on allWords. Use mainCmp which contains the
//    // appropriate casting and utilizes frobcmp.
//    qsort(allWords, allWordsSize, sizeof(char*), mainCmp);
//
//    // Now that everything has been sorted, print all the words out
//    // Use a buffer read (output whole words at once)
//    ssize_t fileStatus;
//    for (int i = 0; i < allWordsSize; i++ ) {
//        int j = 0;
//        int num = 0;
//
//        // While the current row of allWords doesn't have a space
//        // increment j and num. j is used for the array and num is used
//        // for writing.
//        while (allWords[i][j] != ' ') {
//            j++;
//            num++;
//        }
//        num++;
//
//        fileStatus = write(1, allWords[i], num);
//        if (fileStatus < 0) {
//            char* error = "Error: Could not write output\n";
//            write(2, error, strlen(error));
//            exit(1);
//        }
//    }
//
//    // Everything is finished! Free everything and exit successfully :)
//    free(mainPointer);
//    for (int i = 0; i < allWordsSize; i++){
//        free(allWords[i]);
//    }
//    free(allWords);
//    exit(0);
//}
//
//// // // // // // // // // // // //
////  HELPER FUNCTION DEFINITIONS  //
//// // // // // // // // // // // //
//
//// Negative, zero, or positive depending on whether
//// a is less than, equal to, or greater than b respectively
//int frobcmp(char const *a, char const *b) {
//    // Loop through the entire array of words
//    for (;; a++, b++) {
//        // If both *a and *b are spaces, they are the same. Return 0
//        if (*a == ' ' && *b == ' ' ) {
//            return 0;
//
//        }
//
//        char orgA = *a ^ 42;
//        char orgB = *b ^ 42;
//
//        // If -f option was used, change them to uppercase
//        if (f_flag) {
//            orgA = toupper(orgA);
//            orgB = toupper(orgB);
//        }
//
//        // If non-obfuscated a < non-obfuscated b OR *a is a space, return -1
//        if (orgA < orgB || *a == ' ' ) {
//            return -1;
//
//        }
//
//        // If non-obfuscated a > non-obfuscated b OR *b is a space, return 1
//        if (orgA > orgB || *b == ' ' ) {
//            return 1;
//
//        }
//    }
//}
//
//// This is the compare function used inside qsort
//// Cast to pointers to pointers since that is what allWords holds
//int mainCmp(const void *a, const void *b) {
//    const char* newA = *(const char**)a;
//    const char* newB = *(const char**)b;
//    return frobcmp(newA, newB);
//}
//
//

// START OF NEW BACK-UP 25 out of 40 //
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
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
    int wordSize = 100;
    int allWordsPos = 0;
    int wordsPos = 0;
    
    // Now check if the file is regular. If it is, set the correct
    // variable. If it's a regular file OVERRIDE allWordsSize so the
    // proper amount is malloc'd.
    if (S_ISREG(fileData.st_mode)) {
        allWordsSize = (fileData.st_size+1)*sizeof(char*);
    }
    
    
    // allWords holds all the words. currWord holds the current word.
    // Allocate and check for errors.
    char** allWords = (char**) malloc(allWordsSize);
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
            if (allWordsPos >= 0.5*allWordsSize) {
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
            if (wordsPos >= 0.5*wordSize) {
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
    // was not added yet.
    // Additionally, increase allWordsPos by 1 afterwards.
    if (wordsPos > 0 ) {
        currWord[wordsPos] = ' ';
        allWords[allWordsPos] = currWord;
        allWordsPos++;
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
            if (orgA >= 'a' && orgA <= 'z')
                orgA = toupper(orgA);
            if (orgB >= 'a' && orgB <= 'z')
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


