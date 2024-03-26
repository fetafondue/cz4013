#ifndef DELETE_H
#define DELETE_H

#include <vector>
#include <string>

struct DeleteRequest
{
    std::string pathname;
    uint32_t offset;
    uint32_t numBytes;
};

struct DeleteResponse
{
    bool success;
    std::string errorMessage;
};

std::vector<uint8_t> marshalDeleteRequest(const DeleteRequest &req);
DeleteResponse unmarshalDeleteResponse(const std::vector<uint8_t> &res);
void prepareDeleteRequest(DeleteRequest *req);

#endif
