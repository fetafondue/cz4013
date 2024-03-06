#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

void error(const char *msg);

int main(int argc, char *argv[]) {
    int sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    char buffer[1024];
    char message[1024];

    if (argc != 3) {
        printf("Usage: server port\n");
        exit(1);
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("socket");

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == 0) error("Unknown host");

    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length = sizeof(struct sockaddr_in);
    printf(
        "What would you like to do?\n"
        "1. Read file content\n"
        "2. Write to file\n"
        "3. Monitor updates\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    switch (buffer[0]) {
        case '1':
            // read file content
            message[0] = '1';
            message[1] = ' ';
            printf(
                "You have selected option 1, please specify the <pathname> "
                "<offset> <number of bytes to read from the file>\n"
                "Please note that each entry is separated by a space\n");
            bzero(buffer, 1024);
            fgets(buffer, 1023, stdin);
            for (int i = 0; i < strlen(buffer); i++) {
                message[i + 2] = buffer[i];
            }
            break;
        case '2':
            // write to file
            message[0] = '2';
            message[1] = ' ';
            printf(
                "You have selected option 2, please specify the <pathname> "
                "<offset> <number of bytes to read from the file>\n"
                "Please note that each entry is separated by a space\n");
            bzero(buffer, 1024);
            fgets(buffer, 1023, stdin);
            for (int i = 0; i < strlen(buffer); i++) {
                message[i + 2] = buffer[i];
            }
            break;
        case '3':
            // monitor updates
            printf("You have selected option 3");
            break;
    }
    n = sendto(sock, message, strlen(message), 0,
               (const struct sockaddr *)&server, length);
    if (n < 0) error("Sendto");
    n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from, &length);
    if (n < 0) error("recvfrom");
    write(1, "Got an ack: ", 12);
    write(1, buffer, n);
    close(sock);
    return 0;
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}