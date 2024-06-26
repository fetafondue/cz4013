#ifndef READ_H
#define READ_H

#include <string>
#include <vector>

struct ReadRequest {
    std::string pathname;
    uint32_t offset;
    uint32_t numBytes;
};

struct ReadResponse {
    bool success;
    std::string content;
    std::string errorMessage;
};

std::vector<uint8_t> marshalReadRequest(const uint32_t &reqID, const ReadRequest &req);
ReadResponse unmarshalReadResponse(const std::vector<uint8_t> &res);
void prepareReadRequest(ReadRequest *req);

#endif
