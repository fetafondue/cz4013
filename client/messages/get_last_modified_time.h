#ifndef GET_LAST_MODIFIED_TIME_H
#define GET_LAST_MODIFIED_TIME_H

#include <vector>
#include <string>

struct GetLastModifiedTimeRequest
{
    std::string pathname;
};

struct GetLastModifiedTimeResponse
{
    bool success;
    uint64_t lastModifiedUnixTime;
    std::string errorMessage;
};

std::vector<uint8_t> marshalGetLastModifiedTimeRequest(const GetLastModifiedTimeRequest &req);
GetLastModifiedTimeResponse unmarshalGetLastModifiedTimeResponse(const std::vector<uint8_t> &res);
void handleGetLastModifiedTimeRequest(GetLastModifiedTimeRequest *req);

#endif
