#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <unordered_map>

#include "cache/cache.h"
#include "messages/delete.h"
#include "messages/read.h"
#include "messages/replace.h"
#include "messages/subscribe.h"
#include "messages/write.h"

void error(const char *msg);

int main(int argc, char *argv[]) {
    int sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    char buffer[1024];

    if (argc != 4) {
        printf("Missing arguments\n");
        exit(1);
    }

    fInterval = atoi(argv[3]);

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

                // TODO: add cache logic here (checking for time, then cache
                // read or continue down then cache write)

                printf("Sending read request to server...\n");
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                             &length);
                if (n < 0) error("recvfrom");
                write(1, "Got an ack: ", 12);
                write(1, buffer, n);
                write(1, "\n", n);

                break;
            }
            case '2': {
                WriteRequest req;
                std::vector<uint8_t> marshalledReq;

                handleWriteRequest(&req);
                printf("Marshalling request...\n");
                marshalledReq = marshalWriteRequest(req);

                printf("Sending write request to server...\n");
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                             &length);
                if (n < 0) error("recvfrom");
                write(1, "Got an ack: ", 12);
                write(1, buffer, n);
                write(1, "\n", n);

                break;
            }
            case '3': {
                SubscribeRequest req;
                std::vector<uint8_t> marshalledReq;

                handleSubscribeRequest(&req);
                printf("Marshalling request...\n");
                marshalledReq = marshalSubscribeRequest(req);

                printf("Sending subscribe request to server...\n");
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                             &length);
                if (n < 0) error("recvfrom");
                write(1, "Got an ack: ", 12);
                write(1, buffer, n);
                write(1, "\n", n);

                printf(
                    "Subscribe request success, listening to server for %d "
                    "seconds...\n",
                    req.monitorIntervalSeconds);
                // TODO: whats the best way to listen to the server changes

                break;
            }
            case '4': {
                ReplaceRequest req;
                std::vector<uint8_t> marshalledReq;

                handleReplaceRequest(&req);
                printf("Marshalling request...\n");
                marshalledReq = marshalReplaceRequest(req);

                printf("Sending replace request to server...\n");
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                             &length);
                if (n < 0) error("recvfrom");
                write(1, "Got an ack: ", 12);
                write(1, buffer, n);
                write(1, "\n", n);

                break;
            }
            case '5': {
                DeleteRequest req;
                std::vector<uint8_t> marshalledReq;

                handleDeleteRequest(&req);
                printf("Marshalling request...\n");
                marshalledReq = marshalDeleteRequest(req);

                printf("Sending delete request to server...\n");
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                             &length);
                if (n < 0) error("recvfrom");
                write(1, "Got an ack: ", 12);
                write(1, buffer, n);
                write(1, "\n", n);

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