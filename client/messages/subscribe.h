#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H

#include <vector>
#include <string>

struct SubscribeRequest
{
    std::string pathname;
    uint32_t monitorIntervalSeconds;
};

struct SubscribeResponse
{
    bool success;
    std::string errorMessage;
};

std::vector<uint8_t> marshalSubscribeRequest(const SubscribeRequest &req);
SubscribeResponse unmarshalSubscribeResponse(const std::vector<uint8_t> &res);
void handleSubscribeRequest(SubscribeRequest *req);

#endif
