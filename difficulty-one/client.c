#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void print_select_message(void);

int main(int argc, char *argv[])
{
	int sock;
    int select;
    int str_len;

    char fname[100];
	FILE *fp;
	
	char buf[BUF_SIZE];
    char out[BUF_SIZE];
    char message[BUF_SIZE];

	int read_cnt, file_size;
	struct sockaddr_in serv_adr;

    clock_t start, end;
	float rtt;
    
    while (1) {
        print_select_message();

        printf(":");
        scanf("%d", &select);
        while (getchar() != '\n');

        switch (select) {   
            case 1: // File Transfer
                printf("Input the file name\n");
                printf(":");
                fgets(fname, 100, stdin);
                printf("\n");   
                fname[strlen(fname) - 1] = '\0';
                
                fp = fopen(fname, "rb");
                if (fp == NULL) break;

                sock = socket(PF_INET, SOCK_DGRAM, 0);   
                
                memset(&serv_adr, 0, sizeof(serv_adr));
                serv_adr.sin_family = PF_INET;
                serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
                serv_adr.sin_port = htons(8080);

                connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

                // Send the request type
                write(sock, "f", 1);

                // Send file size to server 
                fseek(fp, 0, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                sprintf(buf, "%d", file_size);
                write(sock, buf, BUF_SIZE);


                // Send file data to server
                int count = 1;
                while(1)
                {   
                    read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
                    strcpy(out, buf);
                    char *ptr = strtok(out, "\n");
                    printf("[SENDING %d/%d] Data: %s...\n", count++, file_size / BUF_SIZE + 1, ptr);
                    if (read_cnt < BUF_SIZE)
                    {
                        write(sock, buf, read_cnt);
                        break;
                    }
                    write(sock, buf, read_cnt);
                    
                }
                printf("[SUCCESS] Data transfer complete.\n");
                
                read(sock, buf, BUF_SIZE);

                fclose(fp);
                close(sock);

                break;

            case 2: // Send Message
                sock = socket(PF_INET, SOCK_DGRAM, 0);   
                if (sock == -1)
                    error_handling("socket() error");
                
                memset(&serv_adr, 0, sizeof(serv_adr));
                serv_adr.sin_family = PF_INET;
                serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
                serv_adr.sin_port = htons(8080);
                
                connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

                

                while (1)
                {
                    printf("Type the message for server (STOP to quit)\n");
                    printf(":");
                    fgets(message, sizeof(message), stdin);
                    message[strlen(message) - 1] = 0;
                    printf("\n");    
                    if (!strcmp(message,"STOP") || !strcmp(message,"stop"))	
                        break;
                    
                    // Send the request type
                    write(sock, "m", 1);

                    start = clock();
                    write(sock, message, strlen(message));
                    str_len = read(sock, message, sizeof(message)-1);
                    end = clock();
                    message[str_len] = 0;

                    if (strcmp(message, "close") == 0) {
                        start = clock();
                        float count = 1;
                        printf("Count :: 1\n");
                        while(1) {
                            end = clock();
                            if ((float)(end - start)/CLOCKS_PER_SEC > count) {
                                count++;
                                if (count == 6) {
                                    break;
                                }
                                printf("Count :: %d\n", (int)count);
                            }
                        }

                        printf("Time Out\n");
                        return 0;
                    }
                    
                    rtt = (float)(end - start)/CLOCKS_PER_SEC;
                    
                    printf("[SERVER RECEIVED]: %s\n", message);
                    printf("[RTT of Message] %f\n", rtt);
                }	

                close(sock);

                break;

            case 3:
                return 0;

            default:
                printf("\nWrong. Again.\n");

        }
    }
    
    return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void print_select_message(void)
{
	printf("---------------------------\n");
    printf("1. File Transfer\n");
    printf("2. Send Message\n");
    printf("3. Finish\n");
    printf("\n");
}