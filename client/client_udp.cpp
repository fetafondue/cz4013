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

#include "messages/read.h"
#include "messages/write.h"

void error(const char *msg);

int main(int argc, char *argv[]) {
    int sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    char buffer[1024];

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

    // keep accepting requests from the user
    while (1) {
        printf(
            "What would you like to do?\n"
            "1. Read file content\n"
            "2. Write to file\n"
            "3. Monitor updates\n"
            "4. Replace file content\n"
            "5. Delete file content\n"
            "6. End the program\n");
        bzero(buffer, 1024);
        fgets(buffer, 1023, stdin);  // ignore newline char
        switch (buffer[0]) {
            case '1': {
                ReadRequest req;
                std::vector<uint8_t> marshalledReq;

                handleReadRequest(&req);
                printf("Marshalling request...\n");
                marshalledReq = marshalReadRequest(req);

                printf("Sending read request to server...\n");
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                             &length);
                if (n < 0) error("recvfrom");
                write(1, "Got an ack: ", 12);
                write(1, buffer, n);

                break;
            }
            case '2':{
                WriteRequest req;
                std::vector<uint8_t> marshalledReq;

                handleWriteRequest(&req);
                printf("Marshalling request...\n");
                marshalledReq = marshalWriteRequest(req);

                printf("Sending read request to server...\n");
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                             &length);
                if (n < 0) error("recvfrom");
                write(1, "Got an ack: ", 12);
                write(1, buffer, n);

                break;
            }
            case '3':{
                printf("You have selected option 3");
                break;
            }
            case '4':{
                printf("You have selected option 4");
                break;
            }
            case '5': {
                printf("You have selected option 5");
                break;
            }
            case '6':
                printf("Terminating program\n");
                close(sock);
                return 0;
            default:
                error("you have entered an invalid option");
        }
    }
    close(sock);
    return 0;
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}