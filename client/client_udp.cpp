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
#include "messages/error.h"
#include "messages/get_last_modified_time.h"
#include "messages/read.h"
#include "messages/replace.h"
#include "messages/subscribe.h"
#include "messages/write.h"

int main(int argc, char *argv[]) {
    int sock, n;
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    char buffer[1024];
    std::vector<uint8_t> msg;
    struct timeval tv;
    uint32_t id = 0;

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
        std::cout << "\n===========================\n"
                  << "What would you like to do?\n"
                  << "1. Read file content\n"
                  << "2. Write to file\n"
                  << "3. Monitor updates\n"
                  << "4. Replace file content\n"
                  << "5. Delete file content\n"
                  << "6. End the program\n"
                  << "===========================\n";
        // clear the vector and assign memory for it
        msg.clear();
        msg.resize(5000);
        bzero(buffer, 1024);
        fgets(buffer, 1023, stdin);  // ignore newline char
        // set default timeout for recv
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));

        switch (buffer[0]) {
            case '1': {
                ReadRequest readReq;
                ReadResponse readResp;
                std::vector<uint8_t> marshalledReq;

                prepareReadRequest(&readReq);
                // if the file does not exist in the cache or is outside the
                // cached index, fire to server
                if (!fileInCache(readReq.pathname, readReq.offset,
                                 readReq.numBytes)) {
                    // get last modified time at server
                    GetLastModifiedTimeRequest timeReq;
                    GetLastModifiedTimeResponse timeResp;
                    std::vector<uint8_t> marshalledTimeReq;
                    // build request
                    timeReq.pathname = readReq.pathname;
                    std::cout << "Marshalling last modified time request..."
                              << '\n';
                    marshalledTimeReq =
                        marshalGetLastModifiedTimeRequest(++id, timeReq);
                    std::cout << "Sending request for last modified time..."
                              << '\n';
                    while (true) {
                        // retry if packet loss
                        try {
                            n = sendto(sock, marshalledTimeReq.data(),
                                       marshalledTimeReq.size(), 0,
                                       (const struct sockaddr *)&server,
                                       length);
                            if (n < 0)
                                throw std::invalid_argument("Lost packet");
                            n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                         (struct sockaddr *)&from, &length);
                            if (n < 0)
                                throw std::invalid_argument("Lost packet");
                            break;
                        } catch (const std::exception &e) {
                            std::cout << "Request timeout\n";
                        }
                    }
                    // resize buffer for message unmarshalling validation
                    msg.resize(n);
                    timeResp = unmarshalGetLastModifiedTimeResponse(msg);
                    std::cout
                        << "Got an ack, last modified time at server is: {"
                        << timeResp.lastModifiedUnixTime << '}' << '\n';

                    // clear the vector and assign memory for it
                    msg.clear();
                    msg.resize(5000);

                    std::cout << "Marshalling read request..." << '\n';
                    marshalledReq = marshalReadRequest(id++, readReq);
                    std::cout << "Sending read request to server for file {"
                              << readReq.pathname << "} ..." << '\n';
                    while (true) {
                        // retry if packet loss
                        try {
                            n = sendto(sock, marshalledReq.data(),
                                       marshalledReq.size(), 0,
                                       (const struct sockaddr *)&server,
                                       length);
                            if (n < 0)
                                throw std::invalid_argument("Lost packet");
                            n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                         (struct sockaddr *)&from, &length);
                            if (n < 0)
                                throw std::invalid_argument("Lost packet");
                            break;
                        } catch (const std::exception &e) {
                            std::cout << "Request timeout\n";
                        }
                    }
                    // resize buffer for message unmarshalling validation
                    msg.resize(n);
                    readResp = unmarshalReadResponse(msg);
                    if (!readResp.success) {
                        std::cout << "Error message: " << readResp.errorMessage
                                  << '\n';
                        break;
                    }
                    std::cout << "Got an ack, content is:"
                              << "\n------------------------------\n"
                              << readResp.content
                              << "\n------------------------------\n";
                    std::cout << "Writing to cache..." << '\n';
                    cacheWrite(timeReq.pathname, readResp.content,
                               timeResp.lastModifiedUnixTime, readReq.offset);
                    std::cout << "Successfully cached response" << '\n';
                }
                // if cache expired
                else if (cacheExpired(readReq.pathname)) {
                    // check server for tmserver and check if file has been
                    // changed
                    GetLastModifiedTimeRequest timeReq;
                    GetLastModifiedTimeResponse timeResp;
                    std::vector<uint8_t> marshalledTimeReq;
                    // build request
                    timeReq.pathname = readReq.pathname;
                    std::cout << "Marshalling last modified time request..."
                              << '\n';
                    marshalledTimeReq =
                        marshalGetLastModifiedTimeRequest(id++, timeReq);
                    std::cout << "Sending request for last modified time..."
                              << '\n';
                    while (true) {
                        // retry if packet loss
                        try {
                            n = sendto(sock, marshalledTimeReq.data(),
                                       marshalledTimeReq.size(), 0,
                                       (const struct sockaddr *)&server,
                                       length);
                            if (n < 0)
                                throw std::invalid_argument("Lost packet");
                            n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                         (struct sockaddr *)&from, &length);
                            if (n < 0)
                                throw std::invalid_argument("Lost packet");
                            break;
                        } catch (const std::exception &e) {
                            std::cout << "Request timeout\n";
                        }
                    }
                    // resize buffer for message unmarshalling validation
                    msg.resize(n);
                    timeResp = unmarshalGetLastModifiedTimeResponse(msg);
                    std::cout
                        << "Got an ack, last modified time at server is: {"
                        << timeResp.lastModifiedUnixTime << '}' << '\n';
                    // if it has been changed then fire to server get last
                    // modified time at server
                    if (fileChangedAtServer(readReq.pathname,
                                            timeResp.lastModifiedUnixTime)) {
                        // clear the vector and assign memory for it
                        msg.clear();
                        msg.resize(5000);

                        std::cout << "Marshalling read request..." << '\n';
                        marshalledReq = marshalReadRequest(id++, readReq);
                        std::cout << "Sending read request to server for file {"
                                  << readReq.pathname << "} ..." << '\n';
                        while (true) {
                            // retry if packet loss
                            try {
                                n = sendto(sock, marshalledReq.data(),
                                           marshalledReq.size(), 0,
                                           (const struct sockaddr *)&server,
                                           length);
                                if (n < 0)
                                    throw std::invalid_argument("Lost packet");
                                n = recvfrom(sock, msg.data(), msg.capacity(),
                                             0, (struct sockaddr *)&from,
                                             &length);
                                if (n < 0)
                                    throw std::invalid_argument("Lost packet");
                                break;
                            } catch (const std::exception &e) {
                                std::cout << "Request timeout\n";
                            }
                        }
                        // resize buffer for message unmarshalling validation
                        msg.resize(n);
                        readResp = unmarshalReadResponse(msg);
                        if (!readResp.success) {
                            std::cout
                                << "Error message: " << readResp.errorMessage
                                << '\n';
                            break;
                        }
                        std::cout << "Got an ack, content is:"
                                  << "\n------------------------------\n"
                                  << readResp.content
                                  << "\n------------------------------\n";
                        std::cout << "Writing to cache..." << '\n';
                        cacheWrite(timeReq.pathname, readResp.content,
                                   timeResp.lastModifiedUnixTime,
                                   readReq.offset);
                        std::cout << "Successfully cached response" << '\n';
                    }
                    // else just read from cache
                    else {
                        std::cout << "Valid cache entry, retrieving data from "
                                     "cache..."
                                  << '\n';
                        std::string data = cacheRead(
                            readReq.pathname, readReq.offset, readReq.numBytes);
                        std::cout << "Data:\n------------------------------\n"
                                  << data
                                  << "\n------------------------------\n";
                    }
                }
                // else, just read from cache
                else {
                    std::cout
                        << "Valid cache entry, retrieving data from cache..."
                        << '\n';
                    std::string data = cacheRead(
                        readReq.pathname, readReq.offset, readReq.numBytes);
                    std::cout << "Data:\n------------------------------\n"
                              << data << "\n------------------------------\n";
                }
                break;
            }
            case '2': {
                WriteRequest req;
                WriteResponse resp;
                std::vector<uint8_t> marshalledReq;

                prepareWriteRequest(&req);
                std::cout << "Marshalling write request..." << '\n';
                marshalledReq = marshalWriteRequest(id++, req);

                std::cout << "Sending write request to server..." << '\n';
                while (true) {
                    // retry if packet loss
                    try {
                        n = sendto(sock, marshalledReq.data(),
                                   marshalledReq.size(), 0,
                                   (const struct sockaddr *)&server, length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                     (struct sockaddr *)&from, &length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        break;
                    } catch (const std::exception &e) {
                        std::cout << "Request timeout\n";
                    }
                }
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalWriteResponse(msg);
                std::cout << "Got an ack, write success is " << std::boolalpha
                          << resp.success << '\n';
                if (!resp.success) {
                    std::cout << "Error message: " << resp.errorMessage << '\n';
                    break;
                }

                break;
            }
            case '3': {
                SubscribeRequest req;
                SubscribeResponse resp;
                std::vector<uint8_t> marshalledReq;

                prepareSubscribeRequest(&req);
                std::cout << "Marshalling subscribe request..." << '\n';
                marshalledReq = marshalSubscribeRequest(id++, req);

                std::cout << "Sending subscribe request to server..." << '\n';
                while (true) {
                    // retry if packet loss
                    try {
                        n = sendto(sock, marshalledReq.data(),
                                   marshalledReq.size(), 0,
                                   (const struct sockaddr *)&server, length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                     (struct sockaddr *)&from, &length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        break;
                    } catch (const std::exception &e) {
                        std::cout << "Request timeout\n";
                    }
                }
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalSubscribeResponse(msg);
                std::cout << "Got an ack, subscribe success is "
                          << std::boolalpha << resp.success << '\n';
                if (!resp.success) {
                    std::cout << "Error message: " << resp.errorMessage << '\n';
                    break;
                }

                std::cout
                    << "Subscribe request success, listening to server for "
                    << req.monitorIntervalSeconds << " seconds..." << '\n';
                // TODO: whats the best way to listen to the server changes
                long long startTime =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();
                // set timeout for recv
                tv.tv_sec = req.monitorIntervalSeconds;
                tv.tv_usec = 0;
                while (std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                               .count() -
                           startTime <
                       (tv.tv_sec * 1000)) {
                    bzero(buffer, 1024);
                    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,
                               sizeof(tv));

                    n = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&from,
                                 &length);
                    write(1, "Got an ack: \n", 13);
                    write(1, buffer, n);
                    write(1, "\n", 1);
                }

                break;
            }
            case '4': {
                ReplaceRequest req;
                ReplaceResponse resp;
                std::vector<uint8_t> marshalledReq;

                prepareReplaceRequest(&req);
                std::cout << "Marshalling replace request..." << '\n';
                marshalledReq = marshalReplaceRequest(id++, req);

                std::cout << "Sending replace request to server..." << '\n';
                while (true) {
                    // retry if packet loss
                    try {
                        n = sendto(sock, marshalledReq.data(),
                                   marshalledReq.size(), 0,
                                   (const struct sockaddr *)&server, length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                     (struct sockaddr *)&from, &length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        break;
                    } catch (const std::exception &e) {
                        std::cout << "Request timeout\n";
                    }
                }
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalReplaceResponse(msg);
                std::cout << "Got an ack, replace success is " << std::boolalpha
                          << resp.success << '\n';
                if (!resp.success) {
                    std::cout << "Error message: " << resp.errorMessage << '\n';
                    break;
                }

                break;
            }
            case '5': {
                DeleteRequest req;
                DeleteResponse resp;
                std::vector<uint8_t> marshalledReq;

                prepareDeleteRequest(&req);
                std::cout << "Marshalling delete request..." << '\n';
                marshalledReq = marshalDeleteRequest(id++, req);

                std::cout << "Sending delete request to server..." << '\n';
                while (true) {
                    // retry if packet loss
                    try {
                        n = sendto(sock, marshalledReq.data(),
                                   marshalledReq.size(), 0,
                                   (const struct sockaddr *)&server, length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        n = recvfrom(sock, msg.data(), msg.capacity(), 0,
                                     (struct sockaddr *)&from, &length);
                        if (n < 0) throw std::invalid_argument("Lost packet");
                        break;
                    } catch (const std::exception &e) {
                        std::cout << "Request timeout\n";
                    }
                }
                // resize buffer for message unmarshalling validation
                msg.resize(n);
                resp = unmarshalDeleteResponse(msg);
                std::cout << "Got an ack, delete success is " << std::boolalpha
                          << resp.success << '\n';
                if (!resp.success) {
                    std::cout << "Error message: " << resp.errorMessage << '\n';
                    break;
                }

                break;
            }
            case '6':
                std::cout << "Terminating program" << '\n';
                close(sock);
                return 0;
            default:
                std::cout << "you have entered an invalid option, please enter "
                             "1 to 6";
        }
    }
    close(sock);
    return 0;
}