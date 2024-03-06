#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void error(const char *msg);
void readFileContent(char* buf, char *dataBytes);

int main(int argc, char *argv[]) {
    int sock, length, n;
    socklen_t fromlen;
    struct sockaddr_in server;
    struct sockaddr_in from;
    char buf[1024], dataBytes[1024];
    std::string message;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(0);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("Opening socket");
    length = sizeof(server);
    bzero(&server, length);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));
    if (bind(sock, (struct sockaddr *)&server, length) < 0) error("binding");
    fromlen = sizeof(struct sockaddr_in);
    while (1) {
        n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0) error("recvfrom");
        write(1, "Received a datagram: ", 21);
        write(1, buf, n);
        switch (buf[0]) {
            case '1':
                // read file content
                readFileContent(buf, dataBytes);
                break;
            case '2':
                // write to file

                break;
            case '3':
                // monitor updates
                printf("You have selected option 3");
                break;
        }
        n = sendto(sock, dataBytes, strlen(dataBytes), 0, (struct sockaddr *)&from,
                   fromlen);
        if (n < 0) error("sendto");
    }
    return 0;
}

void readFileContent(char* buf, char* dataBytes) {
    std::string message, s, filepath, readData;
    std::stringstream ss;
    std::vector<std::string> instructions;
    std::ifstream file;
    int offset, bytesToRead;
    char c;

    message = std::string(buf);
    ss = std::stringstream(message);
    while (getline(ss, s, ' ')) {
        instructions.push_back(s);
    }
    filepath = instructions[1];
    offset = stoi(instructions[2]);
    bytesToRead = stoi(instructions[3]);
    file.open(filepath);
    while (file.get(c)) {
        readData += c;
    }
    strcpy(dataBytes, readData.c_str());
}

void error(const char *msg) {
    perror(msg);
    exit(0);
}