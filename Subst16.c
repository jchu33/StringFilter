/*
 File name: Subst16.c
 Created by: Jason Chu
 Subst16 is a pseudo string rewriting filter that reads lines from standard
 input and performs the given, viable substitutions and writes the modified
 input to standard output.
 Subst16 accepts three types of arguments in the order of [FROM TO FLAGS]:
 the FROM argument is the string to replace, the TO argument is the
 replacement string, and the FLAGS argument is the application of the
 substitution rule (either through quit, global, or rescan).
 The FLAGS argument also accepts Sm or Fn arguments, which applies the mth
 or nth number rule (if such a rule exists).
 Lastly, the FROM argument accepts a wildcard ., which will match any character.
 The TO argument accepts a char ^, which represents the replaced FROM string.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "getLine.h"

int main(int argc, char * argv[])
{
    // inputString is the standard input given by the user
    char *inputString;
    
    inputString = getLine(stdin);
    while (inputString != NULL)
    {
        
        // number of arguments is a multiple of three (besides calling ./Subst16
        // numRules is the number of rules given in command line
        long int numRules = (argc - 1)/3;
        // fromStrings is an array of all the strings that we want to replace
        char *fromStrings[numRules];
        // toStrings is an array of all the strings to replace with
        char *toStrings[numRules];
        // flags is an array of flags, NOT including the success/failure flags
        char *flags[numRules];
        // orderRules is an array of the success/failure flags
        char *orderRules[numRules];
        
        // processing of the command line arguments
        for (long int i = 0; i < numRules; i++)
        {
            // fromIndex is the index of the next "from" string
            long int fromIndex = (i * 3) + 1;
            // toIndex is the index of the next "to" string
            long int toIndex = (i * 3) + 2;
            // flagIndex is the index of the next flag
            long int flagIndex = (i * 3) + 3;
            
            // allocating fromStrings array and putting in the from string
            fromStrings[i] = calloc(strlen(argv[fromIndex])+1, sizeof(char));
            for (long int j = 0; j < strlen(argv[fromIndex]); j++)
            {
                fromStrings[i][j] = argv[fromIndex][j];
            }
            
            // allocating toStrings array and putting in the to string
            toStrings[i] = calloc(strlen(argv[toIndex])+1, sizeof(char));
            for (long int j = 0; j < strlen(argv[toIndex]); j++)
            {
                toStrings[i][j] = argv[toIndex][j];
            }
            
            long int index = 0;
            
            // flagLength is the length of the current flag element
            long int flagLength = 0;
            // orderRuleLength is the length of the current order rule element
            long int orderRuleLength = 0;
            
            // determining length of the flag and Sm/Fn strings
            while (argv[flagIndex][index] != 0)
            {
                char letter = argv[flagIndex][index];
                //
                if (letter == 'q' || letter == 'g' || letter == 'r')
                {
                    flagLength += 1;
                }
                else if (letter != '-' )
                {
                    orderRuleLength += 1;
                }
                index += 1;
            }
            
            flagLength += 1;
            orderRuleLength += 1;
            flags[i] = calloc(flagLength, sizeof(char));
            orderRules[i] = calloc(orderRuleLength, sizeof(char));
            
            long int findex = 0;
            long int oindex = 0;
            
            // putting the flag string in the flag array
            for (long int j = 0; j < index; j++)
            {
                char letter = argv[flagIndex][j];
                if (letter == 'q' || letter == 'g' || letter == 'r')
                {
                    flags[i][findex] = letter;
                    findex++;
                }
                else if (letter != '-')
                {
                    orderRules[i][oindex] = letter;
                    oindex++;
                }
            }
        }
        
        // iteration through the rules; skips possible my manipulating i
        for (long int i = 0; i < numRules; i++)
        {
            // bool success is true when there is a successful rule change, false when there is not
            bool success = false;
            
            // bool quitFlag is true when quitFlag is the rule change operator
            bool quitFlag = true;
            
            // bool globalFlag is true when quitFlag is the rule change operator
            bool globalFlag = false;
            
            // bool rescanFlag is true when quitFlag is the rule change operator
            bool rescanFlag = false;
            
            long int index = 0;
            
            // determining the last flag (quit, global, or rescan)
            while (flags[i][index] != 0)
            {
                
                // determining if the last flag is a quit, global, or rescan flag,
                if (flags[i][index] == 'q')
                {
                    quitFlag = true;
                    globalFlag = false;
                    rescanFlag = false;
                }
                else if (flags[i][index] == 'g')
                {
                    quitFlag = false;
                    globalFlag = true;
                    rescanFlag = false;
                }
                else if (flags[i][index] == 'r')
                {
                    quitFlag = false;
                    globalFlag = false;
                    rescanFlag = true;
                }
                index++;
            }
            
            //long int length = strlen(inputString);
            // iterate through the input string to find string match
            for (long int input = 0; input < strlen(inputString); input++)
            {
                // bool match is true when from string is in input string
                bool match = true;
                // offset is number of escaped characters
                int offset = 0;
                for (long int from = 0; from < strlen(fromStrings[i]); from++)
                {
                    // bool escaped is true when there is an escaped character (@. or @@)
                    bool escaped = false;
                    // if escaped @. or @@
                    if (strncmp(fromStrings[i] + from, "@", 1) == 0 && \
                        (strncmp(fromStrings[i] + from + 1, "@", 1) == 0 || \
                         strncmp(fromStrings[i] + from + 1, ".", 1) == 0))
                    {
                        escaped = true;
                        offset++;
                        from++;
                    }
                    // if wildcard and next char in input string is newline, break
                    if (!escaped && strncmp(fromStrings[i] + from, ".", 1) == 0 && \
                        strncmp(inputString + input + from - offset, "\n", 1) == 0)
                    {
                        match = false;
                        break;
                    }
                    // if wildcard, then just keep checking the from string
                    if (!escaped && strncmp(fromStrings[i] + from, ".", 1) == 0) {
                        continue;
                    }
                    // if character that was escaped in from is not the same as the next input char
                    if (escaped && \
                        strncmp(inputString + input + from - offset, fromStrings[i] + from, 1) != 0)
                    {
                        match = false;
                        escaped = false;
                        break;
                    }
                    // if not escaped and chars in input and from do not match, then break
                    else if (!escaped && \
                             strncmp(inputString + input + from - offset, fromStrings[i] + from, 1) != 0)
                    {
                        match = false;
                        break;
                    }
                }
                // if a match of from string in input string was found
                if (match)
                {
                    // carets is number of carets in the to string
                    int carets = 0;
                    // escapes is number of escaped @
                    int escapes = 0;
                    
                    char *newStr = calloc((input + 1), sizeof(char));
                    // add any previous elements
                    strncpy(newStr, inputString, input);
                    
                    // add the to string, accounting for escaped characters and the caret character
                    for (long int to = 0; to < strlen(toStrings[i]); to++)
                    {
                        // escapedTo is true when inside an escaped character @^ or @@
                        bool escapedTo = false;
                        // skip the first @ in escaped characters @. and @@
                        if (strncmp(toStrings[i] + to, "@", 1) == 0 && \
                            (strncmp(toStrings[i] + to + 1, "@", 1) == 0 || \
                             strncmp(toStrings[i] + to + 1, "^", 1) == 0))
                        {
                            escapedTo = true;
                            escapes++;
                            to++;
                        }
                        // if the caret symbol is seen (not escaped)
                        if (!escapedTo && strncmp(toStrings[i] + to, "^", 1) == 0)
                        {
                            carets++;
                            // allocate to adjust for size of the from string
                            char *temp = calloc((input + strlen(fromStrings[i]) - offset + 1), sizeof(char));
                            memcpy(temp, newStr, strlen(newStr));
                            free(newStr);
                            newStr = temp;
                            // add the input matched string
                            strncat(newStr, inputString + input, strlen(fromStrings[i]) - offset);
                        }
                        // if not a caret, then add the next char in to string to input string
                        else
                        {
                            char *temp = calloc((strlen(newStr) + 1 + 1 /* null terminator */), sizeof(char));
                            memcpy(temp, newStr, strlen(newStr));
                            free(newStr);
                            newStr = temp;
                            strncat(newStr, toStrings[i] + to, 1);
                        }
                    }
                    char *temp = calloc(strlen(newStr) + \
                                        strlen(inputString + input + strlen(fromStrings[i]) - offset) + 1, sizeof(char));
                    memcpy(temp, newStr, strlen(newStr));
                    free(newStr);
                    // add the rest of the input string
                    strcat(temp, inputString + input + strlen(fromStrings[i]) - offset);
                    free(inputString);
                    inputString = temp;
                    //length = length - strlen(fromStrings[i]) - offset + (carets * (strlen(fromStrings[i]) - offset)) + (strlen(toStrings[i]) - escapes - carets) + 1;
                    
                    // handle specific cases for quit (break), global (change input index), and rescan
                    success = true;
                    
                    // quit flag
                    if (quitFlag)
                    {
                        break;
                    }
                    
                    // global flag
                    else if (globalFlag)
                    {
                        // if there are carets, then the next char to check depends on length of from string
                        if (carets > 0)
                        {
                            input = input + (strlen(fromStrings[i]) - offset)*carets + \
                            (strlen(toStrings[i]) - escapes - carets) - 1;
                        }
                        else
                        {
                            input = input + strlen(toStrings[i]) - escapes - 1;
                        }
                    }
                    
                    // rescan flag
                    else if (rescanFlag)
                    {
                        input = input - 1;
                    }
                    
                }
                
            }
            
            //nextIndex holds the index of the next rule to skip to if there is a successful substitution or failure flag
            long int nextIndex = -1;
            // if a substitution rule was made, then find the S-rule
            if (success == true)
            {
                for (long int order = 0; order < strlen(orderRules[i]); order++)
                {
                    // if there is an occurrence of S
                    if (strstr(orderRules[i]+order, "S") != NULL)
                    {
                        char *ptr;
                        nextIndex = strtol(strstr(orderRules[i]+order, "S") + 1, &ptr, 10);
                    }
                }
            }
            
            // if a substitution rule was not made, then find the F-rule
            else
            {
                for (long int order = 0; order < strlen(orderRules[i]); order++)
                {
                    // if there is an occurrence of F
                    if (strstr(orderRules[i]+order, "F") != NULL)
                    {
                        char *ptr;
                        nextIndex = strtol(strstr(orderRules[i]+order, "F") + 1, &ptr, 10);
                    }
                }
                
            }
            
            // skip to the next index if nextIndex is a viable index
            if (nextIndex != -1 && nextIndex < numRules)
            {
                i = nextIndex-1;
            }
            // if the nextIndex is not viable, break out of the l   oop
            else if (nextIndex != -1 && nextIndex >= numRules)
            {
                break;
            }
        }
        
        
        // output the input string
        printf("%s", inputString);
        // free inputstring
        free(inputString);
        
        // free the from, to flags, and orderrules arrays of strings
        for (long int i = 0; i < numRules; i++)
        {
            free(fromStrings[i]);
            free(toStrings[i]);
            free(flags[i]);
            free(orderRules[i]);
        }
        inputString = getLine(stdin);
    }
    if (inputString != NULL)
    {
        free(inputString);
    }
 return 0;
}
