#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 32
void error_handling(char *buf);
void write_file(int sockfd, char *filename);


int main(int argc, char *argv[]) {
    // Declare variables.
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;

    // Declare fd_set variables.
    fd_set reads, cpy_reads;

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    // If there was no argument when executed, exit the program.
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // Make socket for TCP connection.
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // Bind the port.
    if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    // Listen the connection setup request from clients.
    if (listen(serv_sock, 10) == -1)
        error_handling("listen() error");


    ///////////////////////////////////////////////////////////////////////////////
    // TODO: Initialize the fd_set reads to have zero bits for all file descriptors

    // TODO: Add serv_sock into reads fd_set variable.
    // Question: Why do we need this?

    ///////////////////////////////////////////////////////////////////////////////


    fd_max = serv_sock;
    printf("fd_max: %d\n", fd_max);

    while(1) {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // The return values of select()
        //            -1: Exception occurred
        //             0: Timeout
        // Larger than 1: The number of file descriptors where an event occurred
        if ((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1) {
            printf("An exception occurred!\n");
            break;
        }

        if (fd_num == 0) {
            printf("Time out!\n");
            break;
        }

        // Question: Why fd_max+1?
        for (i = 0; i < fd_max+1; i++) {
            // Check whether i th fd had an event
            if (FD_ISSET(i, &cpy_reads)) {

                // When an event occurred within serv_sock
                // Connection request from a client!
                if (i == serv_sock) {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock= accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);


                    ////////////////////////////////////////////////////////////////////////
                    // TODO: Add clnt_sock to the proper fd_set variable

                    // TODO: if clnt_sock is greater than fd_max, make fd_max as clnt_sock

                    ////////////////////////////////////////////////////////////////////////


                    printf("Connected Client: %d \n", clnt_sock);
                }

                // When an event occured within a socket connected to a client.
                else {
                    str_len = read(i, buf, BUF_SIZE);

                    // Close Request
                    if (str_len == 0) {

                        ////////////////////////////////////////////////////////////////////////
                        // TODO: remove file descriptor i from fd_set

                        // TODO: close the i th file descriptor

                        ////////////////////////////////////////////////////////////////////////


                        printf("Closed Client: %d\n", i);
                    }
                    // Get a file from the client.
                    else {
                        printf("File name to be saved: %s\n", buf);

                        // Get file data from the client, and save the file
                        write_file(i, buf);
                        printf("%s is received and saved\n", buf);
                    }
                }
            }
        }
    }

    close(serv_sock);
    return 0;
}


void error_handling(char *buf) {
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}


void write_file(int sockfd, char *filename){
    int n;
    FILE *fp;
    char buffer[BUF_SIZE];

    fp = fopen(filename, "w");
    while (1) {
      n = recv(sockfd, buffer, BUF_SIZE, 0);
      if (n <= 0){
        break;
      }
      if (!strcmp(buffer, "\0"))
        break;
      fprintf(fp, "%s", buffer);
      bzero(buffer, BUF_SIZE);
    }
    fclose(fp);
    return;
}
