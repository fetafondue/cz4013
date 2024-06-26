#ifndef REPLACE_H
#define REPLACE_H

#include <vector>
#include <string>

struct ReplaceRequest
{
    std::string pathname;
    uint32_t offset;
    std::string content;
};

struct ReplaceResponse
{
    bool success;
    std::string errorMessage;
};

std::vector<uint8_t> marshalReplaceRequest(const uint32_t &reqID, const ReplaceRequest &req);
ReplaceResponse unmarshalReplaceResponse(const std::vector<uint8_t> &res);
void prepareReplaceRequest(ReplaceRequest *req);

#endif
