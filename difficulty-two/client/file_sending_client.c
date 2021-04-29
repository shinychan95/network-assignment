#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <dirent.h> 


#define BUF_SIZE 32
void error_handling(char *message);
void send_file(FILE *fp, int sockfd);


int main(int argc, char *argv[])
{
    int sd;
    FILE *fp;
    char buf[BUF_SIZE];
    char mode[BUF_SIZE];
    struct sockaddr_in serv_adr;
    
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    
    sd = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));
    
    connect(sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    
    printf("=========== File List ===========\n");
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, ".")) continue;
            if (!strcmp(dir->d_name, "..")) continue;
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    printf("=========== File List ===========\n\n");

    while(1) {
        printf("Write file's name to be upload: ");
        char filename[BUF_SIZE];
        fgets(filename, BUF_SIZE, stdin);
        filename[strlen(filename)-1] = '\0';

        // If user's input was "exit", then get out this while loop.
        if (!strcmp(filename,"exit\n") || !strcmp(filename,"exit"))
            break;

        printf("File name to be sended: %s\n", filename);
        if (send(sd, filename, sizeof(filename), 0) == -1) {
            perror("[-]Error in sending filename.");
            exit(1);
        }

        // Opend the file and send data to the server
        fp = fopen(filename, "r");
        if (fp == NULL) {
            perror("[-]Error in reading file.");
            exit(1);
        }
        send_file(fp, sd);
    }

    close(sd);
    printf("End of Client\n");
    return 0;
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


void send_file(FILE *fp, int sockfd) {
    int n;
    char data[BUF_SIZE] = {0};

    while(fgets(data, BUF_SIZE, fp) != NULL) {
      if (send(sockfd, data, sizeof(data), 0) == -1) {
        perror("[-]Error in sending file.");
        exit(1);
      }
      bzero(data, BUF_SIZE);
    }
    send(sockfd, "\0", sizeof("\0"), 0);
}
