/* 
  Program: Regular Expressions (RegEx) Calculator
  Creator: Jessica Nguyen
  Date: 2023-10-20
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define LINE_MAX 1000
#define STR_MAX 10

/* declaring structure */
typedef struct data{
    int index;
    bool isSlash;
    char prevChar;
} optionalData;

/* declaring all functions */
char * fileReader(char fileName[], int line);
int match(char *regexp, char *text);
int matchDuplicate(char *regexp, char *text);
int matchHere(char *regexp, char *text, optionalData extraParameters);
int matchSlash(int c, char *regexp, char *text, optionalData extraParameters);
int matchStar(int c, char *regexp, char *text, optionalData extraParameters);
int matchPlus(int c, char *regexp, char *text, optionalData extraParameters);
int matchQuestion(int c, char *regexp, char *text, optionalData extraParameters);
    
int main (int argc, char *argv[]){   
    /* checks if an input was obtained from the command line */
    if (argc == 2){
        /* declaring variables */
        char *regexp, *text;
        int i, result;

        /* initializing variables */
        regexp = NULL;
        text = NULL;

        /* checks if multiple inputs were given at the command line (ex: ./a1 "ex1 ex2") */
        for (i = 0; i < strlen(argv[1]); i++){
            if (argv[1][i] == ' '){
                printf("Too many files were given in the command line.\n");
                return 0;
            }
        }

        /* reads the file and splits the 2 strings (in the file) into their correlated arrays */
        regexp = fileReader(argv[1], 0);
        regexp[strlen(regexp)-1] = '\0';
        text = fileReader(argv[1], 1);

        /* prints out all of the matches found */
        printf("match ");
        result = match(regexp, text);
        if (result == 0){
            printf("none");
        }
        printf("\n");

        /* free malloced variables */
        free(regexp);
        free(text);

    }else if (argc > 2){
        /* if more than 1 file was given, output this message */
        printf("Too many files were given in the command line.\n");
        
    }else{
        /* if no files were given, output this message */
        printf ("No files were given in the command line.\n");
    }
    return 0;
}

/* fileReader: this function reads an entire file and returns one string */
char * fileReader(char fileName[], int line){
    /* declaring variables */
    FILE *file;
    char *fileData, fileLine[LINE_MAX];
    
    /* initializing variables */
    file = NULL;

    /* opens the file by using the inputted command line argument */
    file = fopen(fileName, "r");

    /* dummy proof: checks if the file exists */
    if (file == NULL){
        printf ("%s does not exist.\n", fileName);

        /* exits the program, automatically closes the file */
        exit(0);
    }

    /* finds the file size */
    fseek(file, 0L, SEEK_END);
    fileData = (char*)malloc((ftell(file)+1)*sizeof(char));

    /* moves to the start of the file */
    rewind (file);

    /* puts one row of file text into fileLine (0 = first row, 1 = second row) */
    if (line == 0){
        /* obtain a line of file text */
        fgets(fileLine, LINE_MAX, file);
        
        /* removes the newline at the end of each fgets() with an end of line */
        if (fileLine[strlen(fileLine)-1] == '\n'){
            fileLine[strlen(fileLine)-1] = '\0';
        }

        /* puts the fileline into the returned string */
        strcpy(fileData, fileLine);
    }else{
        /* obtain a line of file text */
        fgets(fileLine, LINE_MAX, file);
        fgets(fileLine, LINE_MAX, file);

        /* removes the newline at the end of each fgets() with an end of line */
        if (fileLine[strlen(fileLine)-1] == '\n'){
            fileLine[strlen(fileLine)-1] = '\0';
        }

        /* puts the fileline into the returned string */
        strcpy(fileData, fileLine);
    }
        
    /* close the file */
    fclose(file);

    /* returns the file contents */
    return fileData;
}

/* match: search for regexp anywhere in text */
int match(char *regexp, char *text){
    /* declaring variables */
    optionalData extraParameters;
    bool match, sameChar;
    int i;
    char fullText[strlen(text)+1];

    /* initializing variables */
    match = false;
    sameChar = true;
    strcpy(fullText, text);
    extraParameters.index = 0;
    extraParameters.isSlash = false;
    extraParameters.prevChar = '0';

    /* if '^' exists, string must be at the start of the line. */ 
    if (regexp[0] == '^'){
        /* return true(match) or false(no match) from matchhere()*/
        if (matchHere(regexp+1, text, extraParameters)){
            /* print any matches found */
            printf("%d ", extraParameters.index);
            /* return true early */
            return 1;
        }
    }

    /* checks if the regexp is one long (one char) string */
    for (i = 0; i < strlen(regexp)-1 ; i++){
        if (regexp[i] != regexp[i+1]){
            sameChar = false;
        }
    }

    /* if it is one long (one char) string then match it differently */
    if (sameChar){
        return matchDuplicate(regexp, text);
    }

    /* must look even if the string is empty */
    do {
        /* set up some default values */
        if (extraParameters.index == 0){
            extraParameters.prevChar = '0';
        }

        /* if there is a match, print index and return true (match)!*/
        if (matchHere(regexp, text, extraParameters) && *text != '\0'){
            printf("%d ", extraParameters.index);
            match = true;
        }

        /* save the previous char and move onto the next char in text */
        extraParameters.prevChar = fullText[extraParameters.index];
        extraParameters.index++;

    /* keep searching the text until we reach the end */
    } while (*text++ != '\0');

    /* return the final match value */
    if (!match){
        return 0;
    }else{
        return 1;
    }
}

/* matchHere: search for regexp at beginning of text */
int matchHere(char *regexp, char *text, optionalData extraParameters){
    /* return true if we reach to the end of the regex string */
    if (regexp[0] == '\0'){
        return 1;
    }

    /* \ = the characters that follow the backslash are now a string literal */
    if (regexp[0] == '\\'){
        return matchSlash(regexp[0], regexp+1, text, extraParameters);
    }

    /* * = match for >=0 of the previous character */
    if (regexp[1] == '*' && !extraParameters.isSlash){
        return matchStar(regexp[0], regexp+2, text, extraParameters);
    }

    /* $ = match for end of the line */
    if (regexp[0] == '$' && regexp[1] == '\0'){
        return *text == '\0';
    }

    /* + = match for >0 of the previous character */
    if (regexp[1] == '+' && !extraParameters.isSlash){
        return matchPlus(regexp[0], regexp+2, text, extraParameters);
    }

    /* ? = the previous character can occur 0 or 1 times */
    if (regexp[1] == '?' && !extraParameters.isSlash){
        return matchQuestion(regexp[0], regexp+2, text, extraParameters);
    }

    /* [] = a set of characters within the brackets */
    if (regexp[0] == '['){
        /* INCOMPLETE! */
        return 0;
    }

    /* . = match for any single character or move onto the next part of the strings */
    if (*text!='\0' && (regexp[0]=='.' || regexp[0]==*text)){
        extraParameters.index++;
        return matchHere(regexp+1, text+1, extraParameters);
    }

    return 0;
}

/* matchDuplicate: deals with the string differently if regex is a 1 character only string */
int matchDuplicate(char *regexp, char *text){
    /* declaring variables */
    bool finalMatch, match;
    int i, j;

    /* initializing variables */
    finalMatch = false;
    match = false;

    /* checks for matches between the text and regexp */
    for(i = 0; i < strlen(text); i++){
        /* checks if text == regexp within the current index */
        for (j = 0; j < strlen(regexp); j++){
            if(text[i+j] != regexp[j]){
                match = false;
                j = strlen(regexp);
            }else{
                match = true;
            }
        }

        /* if a match is found, print out the index and move onto the next index space */
        if (match == true){
            finalMatch = true;
            printf("%d ", i);
            i = i + strlen(regexp)-1;
        }
    }

    /* return the final match value */
    if (!finalMatch){
        return 0;
    }else{
        return 1;
    }
}

/* matchStar: search for c*regexp */
int matchStar(int c, char *regexp, char *text, optionalData extraParameters){
    /* a * matches for zero or more instances */
    do {
        if (matchHere(regexp, text, extraParameters)){
            return 1;
        }
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}

/* matchSlash: the characters that follow the backslash are now a string literal (metacharacters have no specialties) */
int matchSlash(int c, char *regexp, char *text, optionalData extraParameters){
    do {
        /* Return to match here but isSlash is now true*/
        extraParameters.isSlash = true;
        if (matchHere(regexp, text, extraParameters)){
            return 1;
        }
    } while (*text != '\0' && (*text++ == c || c == '.'));

    return 0;
}

/* matchPlus: search for c+regexp */
int matchPlus(int c, char *regexp, char *text, optionalData extraParameters){
    /* a + matches for one or more instances */
    do {
        /* return true if the first/new char of the string is a match */
        if ((extraParameters.index == 0 && text[0] == c) || (extraParameters.prevChar != c && text[0] == c)){
            return matchHere(regexp, text, extraParameters);
        }

        /* save the previous char, increment the index, and move onto the next text index */
        extraParameters.prevChar = text[0];
        extraParameters.index++;
    } while (*text != '\0' && (*text++ == c || c == '.'));

    return 0;
}

/* matchQuestion: search for c?regexp */
int matchQuestion(int c, char *regexp, char *text, optionalData extraParameters){
    /* a ? matches for only zero or one instances */
    do {
        /* check if the text char after the first one isn't c*/
        if (text[0] != c || (text[0] == c && text[1] != c)){
            return matchHere(regexp, text, extraParameters);
        }

        /* increment to the new index of text */
        extraParameters.index++;
    } while (*text != '\0' && (*text++ == c || c == '.'));

    return 0;
}