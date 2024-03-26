#ifndef WRITE_H
#define WRITE_H

#include <string>
#include <vector>

struct WriteRequest {
    std::string pathname;
    uint32_t offset;
    std::string content;
};

struct WriteResponse {
    bool success;
    std::string errorMessage;
};

std::vector<uint8_t> marshalWriteRequest(const uint32_t &reqID,
                                         const WriteRequest &req);
WriteResponse unmarshalWriteResponse(const std::vector<uint8_t> &res);
void prepareWriteRequest(WriteRequest *req);

#endif
