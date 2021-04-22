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
	FILE *fp;
	
	char buf[BUF_SIZE];
	int read_cnt, recv_size, file_size;
	struct sockaddr_in serv_adr, from_adr;
	if (argc != 3) {
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	fp = fopen("ufile_server.c", "wb");
	sock = socket(PF_INET, SOCK_DGRAM, 0);   

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	
	// Send start message
	strcpy(buf, "Let's start!");
	write(sock, buf, strlen(buf));
	printf("%s\n", buf);

	// Receive file size
	read(sock, buf, 4);
	file_size = atoi(buf);
	printf("File size=%d bytes\n", file_size);

	/// 
	memset(buf, 0, BUF_SIZE);
	recv_size = 0;
	while (recv_size < file_size)
	{
		read_cnt = read(sock, buf, BUF_SIZE);
		fwrite((void*)buf, 1, read_cnt, fp);
		recv_size += read_cnt;
	}
	
	puts("Received file data");
	write(sock, "Thank you", 10);
	fclose(fp);
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}