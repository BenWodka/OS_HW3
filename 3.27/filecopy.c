#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{

	if (argc != 3){
		printf("ERROR! Need exactly two parameters. For example: input.txt copy.txt. Where input.txt is the file to be copied and copy.txt is the name of the new copied file.\n");
		return 1;
	}

    int filepipe[2];
    char buffer[BUFFER_SIZE];
    int count;

    // Create pipe
    if (pipe(filepipe) == -1){
        printf("Pipe Failed\n");
        return 1;
    }

    int pid = fork();
    
    if(pid<0) //Error
    {
        printf("ERROR!\n");
        exit(1);
    }

    if(pid > 0) //Parent Process
    {
        close(filepipe[0]); // Close read end

        int inputFile = open(argv[1], O_RDONLY); //Open in read only mode
        if(inputFile == -1){
            printf("Error opening input file");
            close(filepipe[1]);
            return 1;
        }

        while((count = read(inputFile, buffer, BUFFER_SIZE)) > 0){
                if (write(filepipe[1], buffer, count) != count) {
                    printf("Error writing to pipe");
                    close(inputFile);
                    close(filepipe[1]); //Close Write end
                    return 1;
            }
        }
        close(inputFile);
        close(filepipe[1]); // Close write end

        wait(NULL);

    }
    else  //Child Process
    {
        close(filepipe[1]); //Close write end


        //O_WRONLY: write only  O_CREAT: if file doesnt exist, create it.  O_TRUNC: if file exists, erase it before writing to it.  S_IRUSR: set read permission.  S_IWUSR: set write permission
        int destinationFile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if(destinationFile == -1){
            printf("Error opening destination file");
            close(filepipe[0]);
            return 1;
        }

        while((count = read(filepipe[0], buffer, BUFFER_SIZE)) > 0){
                if (write(destinationFile, buffer, count) != count) {
                    printf("Error writing to pipe");
                    close(destinationFile);
                    close(filepipe[0]); //Close read end
                    return 1;
            }
        }
        close(destinationFile);
        close(filepipe[0]); // Close read end
    }
    return 0;
}