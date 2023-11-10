#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {    
    FILE *fileName;
    char line[100];
    int number = 1;
    fileName = fopen(argv[argc-1], "r");
    if (strcmp(argv[argc-2], "-n") == 0) {
        while(fgets(line, sizeof(line), fileName) != NULL){
            printf("%d %s", number, line);
            number++;
        }
    } else if(strcmp(argv[argc-2], "-b") == 0) {
        while(fgets(line, sizeof(line), fileName) != NULL){
            if (line[0] != '\n'){
                printf("%d ", number);
                number++;
            }
            printf("%s", line);
        }
    } else if (strcmp(argv[argc-2], "-s") == 0) {
        int prevLine = '\n';
        while(fgets(line, sizeof(line), fileName) != NULL) {
            if (line[0] != '\n' || prevLine != '\n') {
                printf("%s", line);
            }
            prevLine =line[0];
        }
    } else if(strcmp(argv[argc-2], "-e") == 0) {
        while (fgets(line, sizeof(line), fileName) != NULL) {
            int len = strlen(line);
            if (line[len - 1] == '\n') {
                line[len - 1] = '$';
                printf("%s\n", line);
            } else {
                printf("%s$", line);
            }
        }
    } else if (strcmp(argv[argc - 2], "-t") == 0) {
        while (fgets(line, sizeof(line), fileName) != NULL) {
            for (int i = 0; i < strlen(line); i++) {
                if (line[i] == '\t') {
                    printf("^I");
                } else {
                    printf("%c", line[i]);
                }
            }
        }
    } else if(strcmp(argv[argc-2], "--number-nonblank") == 0) {
        while(fgets(line, sizeof(line), fileName) != NULL){
            if (line[0] != '\n'){
                printf("%d ", number);
                number++;
            }
            printf("%s", line);
        }
    } else if (strcmp(argv[argc-2], "--number") == 0) {
        while(fgets(line, sizeof(line), fileName) != NULL){
            printf("%d %s", number, line);
            number++;
        }
    } else if (strcmp(argv[argc-2], "--squeeze-blank") == 0) {
        int prevLine = '\n';
        while(fgets(line, sizeof(line), fileName) != NULL) {
            if (line[0] != '\n' || prevLine != '\n') {
                printf("%s", line);
            }
            prevLine =line[0];
        }
    } 
    fclose(fileName);
}
