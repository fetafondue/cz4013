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
#include "messages/get_last_modified_time.h"
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
    std::vector<uint8_t> msg;

    if (argc != 4) {
        std::cout << "Missing arguments" << '\n';
        exit(1);
    }

    fInterval = atoi(argv[3]) * 1000;

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
        std::cout << "What would you like to do?\n"
                  << "1. Read file content\n"
                  << "2. Write to file\n"
                  << "3. Monitor updates\n"
                  << "4. Replace file content\n"
                  << "5. Delete file content\n"
                  << "6. End the program\n";
        // clear the vector and assign memory for it
        msg.clear();
        msg.resize(5000);
        bzero(buffer, 1024);
        fgets(buffer, 1023, stdin);  // ignore newline char
        switch (buffer[0]) {
            case '1': {
                ReadRequest readReq;
                ReadResponse readResp;
                std::vector<uint8_t> marshalledReq;
                bool fileInCache = false;

                handleReadRequest(&readReq);
                if (isValidCacheEntry(readReq.pathname)) {
                    std::cout
                        << "Valid cache entry, retrieving data from cache..."
                        << '\n';
                    std::string data = cacheRead(
                        readReq.pathname, readReq.offset, readReq.numBytes);
                    std::cout << "Data: {" << data << "}" << '\n';
                } else {
                    std::cout << "Marshalling read request..." << '\n';
                    marshalledReq = marshalReadRequest(readReq);
                    std::cout << "Sending read request to server for file {"
                              << readReq.pathname << "} ..." << '\n';
                    n = sendto(sock, marshalledReq.data(), marshalledReq.size(),
                               0, (const struct sockaddr *)&server, length);
                    if (n < 0) error("Sendto");
                    n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                 (struct sockaddr *)&from, &length);
                    if (n < 0) error("recvfrom");
                    // resize buffer for message unmarshalling validation
                    msg.resize(n);
                    readResp = unmarshalReadResponse(msg);
                    std::cout << "Got an ack, content is: {" << readResp.content
                              << '}' << '\n';

                    // clear the vector and assign memory for it
                    msg.clear();
                    msg.resize(5000);

                    // get last modified time at server
                    GetLastModifiedTimeRequest timeReq;
                    GetLastModifiedTimeResponse timeResp;
                    std::vector<uint8_t> marshalledTimeReq;
                    // build request
                    timeReq.pathname = readReq.pathname;
                    std::cout << "Marshalling last modified time request..."
                              << '\n';
                    marshalledTimeReq =
                        marshalGetLastModifiedTimeRequest(timeReq);
                    std::cout << "Sending request for last modified time..."
                              << '\n';
                    n = sendto(sock, marshalledTimeReq.data(),
                               marshalledTimeReq.size(), 0,
                               (const struct sockaddr *)&server, length);
                    if (n < 0) error("Sendto");
                    n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                 (struct sockaddr *)&from, &length);
                    if (n < 0) error("recvfrom");
                    // resize buffer for message unmarshalling validation
                    msg.resize(n);
                    timeResp = unmarshalGetLastModifiedTimeResponse(msg);
                    std::cout
                        << "Got an ack, last modified time at server is: {"
                        << timeResp.lastModifiedUnixTime << '}' << '\n';
                    std::cout << "Writing to cache..." << '\n';
                    cacheWrite(timeReq.pathname, readResp.content,
                               timeResp.lastModifiedUnixTime);
                    std::cout << "Successfully cached response" << '\n';
                }
                break;
            }
            case '2': {
                WriteRequest req;
                WriteResponse resp;
                std::vector<uint8_t> marshalledReq;

                handleWriteRequest(&req);
                std::cout << "Marshalling write request..." << '\n';
                marshalledReq = marshalWriteRequest(req);

                std::cout << "Sending write request to server..." << '\n';
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                             (struct sockaddr *)&from, &length);
                if (n < 0) error("recvfrom");
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalWriteResponse(msg);
                std::cout << "Got an ack, write success is " << std::boolalpha
                          << resp.success << '\n';

                break;
            }
            case '3': {
                SubscribeRequest req;
                SubscribeResponse resp;
                std::vector<uint8_t> marshalledReq;

                handleSubscribeRequest(&req);
                std::cout << "Marshalling subscribe request..." << '\n';
                marshalledReq = marshalSubscribeRequest(req);

                std::cout << "Sending subscribe request to server..." << '\n';
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                             (struct sockaddr *)&from, &length);
                if (n < 0) error("recvfrom");
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalSubscribeResponse(msg);
                std::cout << "Got an ack, subscribe success is "
                          << std::boolalpha << resp.success << '\n';

                std::cout
                    << "Subscribe request success, listening to server for "
                    << req.monitorIntervalSeconds << " seconds..." << '\n';
                // TODO: whats the best way to listen to the server changes

                break;
            }
            case '4': {
                ReplaceRequest req;
                ReplaceResponse resp;
                std::vector<uint8_t> marshalledReq;

                handleReplaceRequest(&req);
                std::cout << "Marshalling replace request..." << '\n';
                marshalledReq = marshalReplaceRequest(req);

                std::cout << "Sending replace request to server..." << '\n';
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                             (struct sockaddr *)&from, &length);
                if (n < 0) error("recvfrom");
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalReplaceResponse(msg);
                std::cout << "Got an ack, replace success is " << std::boolalpha
                          << resp.success << '\n';

                break;
            }
            case '5': {
                DeleteRequest req;
                DeleteResponse resp;
                std::vector<uint8_t> marshalledReq;

                handleDeleteRequest(&req);
                std::cout << "Marshalling delete request..." << '\n';
                marshalledReq = marshalDeleteRequest(req);

                std::cout << "Sending delete request to server..." << '\n';
                n = sendto(sock, marshalledReq.data(), marshalledReq.size(), 0,
                           (const struct sockaddr *)&server, length);
                if (n < 0) error("Sendto");
                n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                             (struct sockaddr *)&from, &length);
                if (n < 0) error("recvfrom");
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalDeleteResponse(msg);
                std::cout << "Got an ack, delete success is " << std::boolalpha
                          << resp.success << '\n';

                break;
            }
            case '6':
                std::cout << "Terminating program" << '\n';
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