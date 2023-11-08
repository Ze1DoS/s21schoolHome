#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {    
    FILE *fileName;
    char line[100];
    int number = 1;
    fileName = fopen(argv[argc-1], "r");
    if (strcmp(argv[argc-2], "-n") == 0) {
        while(fgets(line, 100, fileName) != NULL){
            printf("%d %s", number, line);
            number++;
        }
    } else if(strcmp(argv[argc-2], "-b") == 0) {
        while(fgets(line, 100, fileName) != NULL){
            int prevLine = '\n';
            if (line[0] != '\n' || prevLine != '\n'){
                printf("%d %s", number, line);
                number ++;
            }
        }
    } else if (strcmp(argv[argc-2], "-s") == 0) {
        int emptyLine = 0;
        while(fgets(line, 100, fileName) != NULL){
            if (line[0] != '\n') {
                printf("%s", line);

                emptyLine = 0;
            } else if (!emptyLine) {
                printf("%s", line);

                emptyLine = 1;
            }
        }
    }
    fclose(fileName);
}
