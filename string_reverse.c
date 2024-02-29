#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    char userInput[256];

    //Index 0 is reading end, Index 1 is writing end
    int pipe1[2], pipe2[2];

    
    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        printf("Pipe Failed\n");
        return 1;
    }

    printf("Please type in a message to reverse its case:\n");
    fgets(userInput, sizeof(userInput), stdin); // Read user input including spaces
    userInput[strcspn(userInput, "\n")] = 0; // Remove newline character at the end

    int pid = fork();
    
    if(pid<0) //Error
    {
        printf("ERROR!\n");
        exit(1);
    }

    if(pid > 0) //Parent Process
    {
        char modifiedString[256];

        close(pipe1[0]); //Closes reading end
        write(pipe1[1], userInput, strlen(userInput) + 1); //Write user input to pipe
        close(pipe1[1]); //Closes writing end oif pipe

        wait(NULL); //Wait for response from child

        close(pipe2[1]); //Closes writing end
        read(pipe2[1], userInput, strlen(userInput) + 1); //Read string from pipe
        printf("Parent read >%s<\n", modifiedString);
        close(pipe2[0]); //Close reading end

    }
    else //Child Process
    {
        char receivedString[256];

        close(pipe1[1]); //Closes writing end
        read(pipe1[0], receivedString, sizeof(receivedString)); // Read string from pipe
        printf("Child read >%s<\n", receivedString);

        // Reverse case
        for (int i = 0; receivedString[i]; i++) {
            if (isupper(receivedString[i]))
                receivedString[i] = tolower(receivedString[i]);
            else if (islower(receivedString[i]))
                receivedString[i] = toupper(receivedString[i]);
        }

        //Close reading ends
        close(pipe1[0]);
        close(pipe2[0]);

        write(pipe2[1], receivedString, strlen(receivedString) + 1); // Send modified string back to parent
        printf("Child write >%s<\n", receivedString);
        close(pipe2[1]); // Close writing end of second pipe
        exit(0);

    }
    return 0;
}
