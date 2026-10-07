#include <stdio.h>
#include <unistd.h>

void open_file(char *file){
    printf("UID: %d, EUID: %d\n", getuid(), geteuid());
    FILE *fin = fopen(file, "r");
    if (fin == NULL){
        perror("Error with opening this file");
    }
    else{
        printf("File opened\n");
        fclose(fin);
    }
}

int main(int argc, char **argv){
    char *file;
    if (argc > 1){
        file = argv[1];
    }
    else{
        file = "data.txt";
    }

    open_file(file);

    setuid(getuid());
    open_file(file);

    return 0;
}
