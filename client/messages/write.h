#ifndef WRITE_H
#define WRITE_H

#include <vector>

struct WriteRequest
{
    std::string pathname;
    uint32_t offset;
    std::string content;
};

struct WriteResponse
{
    bool success;
    std::string errorMessage;
};

std::vector<uint8_t> marshalWriteRequest(const WriteRequest &req);
WriteResponse unmarshalWriteResponse(const std::vector<uint8_t> &res);

#endif
