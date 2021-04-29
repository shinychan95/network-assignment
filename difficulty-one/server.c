#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
    int seqnum = 1;
	FILE *fp;
	
	char buf[BUF_SIZE];
    char out[BUF_SIZE];
    char message[BUF_SIZE];
	int read_cnt, recv_size, file_size, str_len;
	struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
	
	sock = socket(PF_INET, SOCK_DGRAM, 0);   

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = PF_INET;
	serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_adr.sin_port = htons(8080);
	
	if (bind(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	
    printf("[STARTING] UDP File Server is starting...\n");

    while (1) {
        // Receive request meesage from client 
        clnt_adr_sz = sizeof(clnt_adr);
        recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        if (buf[0] == 'f') {
            printf("File Transfer Request\n");

            fp = fopen("server.txt", "wb");

            recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
            file_size = atoi(buf);
            printf("File size=%d bytes\n", file_size);

            memset(buf, 0, BUF_SIZE);
            recv_size = 0;
            int count = 1;
            while (recv_size < file_size)
            {
                read_cnt = recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
                strcpy(out, buf);
                char *ptr = strtok(out, "\n");
                printf("[RECEIVING %d/%d] Data: %s\n", count++, file_size / BUF_SIZE + 1, ptr);
                fwrite((void*)buf, 1, read_cnt, fp);
                recv_size += read_cnt;
            }
            printf("[SUCCESS] Data transfer complete.\n");
            sendto(sock, "finished", 8, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
            fclose(fp);
        }
        else if (buf[0] == 'm') {
            clnt_adr_sz = sizeof(clnt_adr);
            str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
            message[str_len - 1] = 0;
            if (seqnum == 11 || strcmp(message, "Start Alarm") == 0) {
                sendto(sock, "close", 5, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
                printf("[ClOSING]: Close the server\n");
                break;
            }
            
            printf("[RECEIVING] Message: %s\n", message);
            
            for (int i = 0; i < (int)strlen(message); i++){
                if (message[i] >= 'a' && message[i] <= 'z'){
                    message[i] -= 32;
                }
            }

            sendto(sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
            printf("[SENDING] Message: %s\n", message);
            printf("[Sequence NUMBER]: %d\n", seqnum);
            seqnum++;
        }
        else {
            printf("Irrecognizable Request Comes\n");
        }        
    }

    close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}