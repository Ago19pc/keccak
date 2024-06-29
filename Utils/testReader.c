#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void readPiece(FILE* file, char* input, int* len, char* result){
        char word[1000];
        fscanf(file, "%s", word);
        if (strcmp(word, "Len") == 0){
            fscanf(file, "%s", word);
            fscanf(file, "%s", word);
            *len = atoi(word);
        } else if (strcmp(word, "Msg") == 0){
            fscanf(file, "%s", word);
            fscanf(file, "%s", input);
            
        } else if (strcmp(word, "MD") == 0){
            fscanf(file, "%s", word);
            fscanf(file, "%s", result);
        } else {
            printf("-%s\n", word);
            
            }

    return;
}

void longmsg(FILE* file, char* input, int* len, char* result){
    for (int i = 0; i < 3; i++){
        readPiece(file, input, len, result);
    }
    return;
}

// ../sha-3bytetestvectors/SHA3_256LongMsg.rsp

int main (int argc, char* argv[]){
    if (argc != 2){
        printf("INVALID NUMBER OF ARGUMENTS\n");
        return 0;
    }
    const char* fileName = argv[1];
    FILE* file = fopen(fileName, "r");
    if (file == NULL){
        printf("INVALID FILE\n");
        return 0;
    }

    char input[30000];
    int len = 0;
    char output[100];
    while (!feof(file)){
        longmsg(file, input, &len, output);
        printf("Input: %s\n", input);
        printf("Len: %d\n", len);
        printf("Output: %s\n", output);
    }

}