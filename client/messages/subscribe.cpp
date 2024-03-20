#include "subscribe.h"

#include <vector>

#include "message_type.h"

// Marshals a SUBSCRIBE request into a byte vector
std::vector<uint8_t> marshalSubscribeRequest(const SubscribeRequest &req) {
    std::vector<uint8_t> pathnameBytes(req.pathname.begin(),
                                       req.pathname.end());
    uint32_t pathnameLen = pathnameBytes.size();

    // buffer consists of: MessageType (byte), pathnameLen (uint32), pathname
    // (str), monitorIntervalSeconds (uint32)
    size_t bufSize = sizeof(MessageType) + pathnameLen + 2 * sizeof(uint32_t);
    std::vector<uint8_t> buf(bufSize);

    // insert MessageType into buf
    buf[0] = SUBSCRIBE;

    // insert pathname length and itself into buf
    uint32_t pathnameLenBE = htonl(pathnameLen);  // big endian format
    std::memcpy(buf.data() + sizeof(MessageType), &pathnameLenBE,
                sizeof(uint32_t));
    std::memcpy(buf.data() + sizeof(MessageType) + sizeof(uint32_t),
                pathnameBytes.data(), pathnameLen);
    // insert monitorIntervalSeconds into buf
    uint32_t monitorIntervalSecondsBE =
        htonl(req.monitorIntervalSeconds);  // big endian format
    std::memcpy(
        buf.data() + sizeof(MessageType) + sizeof(uint32_t) + pathnameLen,
        &monitorIntervalSecondsBE, sizeof(uint32_t));

    return buf;
}

bool validateMarshalledSubscribeResponse(const std::vector<uint8_t> &res) {
    // ensure that the byte vector is at least its minimum length (single bool)
    if (res.size() == 0)
        throw std::invalid_argument(
            "invalid response: byte vector length is too short");

    bool success = res[0] == 1;

    // case of only bool present
    if (res.size() == sizeof(bool)) {
        // should not have success = false
        if (!success)
            throw std::invalid_argument(
                "invalid response: unsuccessful without error message");

        return true;
    }

    // should not have success = true
    if (success)
        throw std::invalid_argument(
            "invalid response: successful with additional unknown bytes");

    size_t errMsgLen = 0;
    for (size_t i = sizeof(bool); i < sizeof(bool) + sizeof(uint32_t); i++)
        errMsgLen = (errMsgLen << 8) | res[i];

    // ensure that the byte slice has the correct length if it includes
    // errorMessage
    if (res.size() != sizeof(bool) + sizeof(uint32_t) + errMsgLen)
        throw std::invalid_argument(
            "invalid response: incorrect byte array length");

    return success;
}

// Unmarshals a byte vector into a SubscribeResponse struct
SubscribeResponse unmarshalSubscribeResponse(const std::vector<uint8_t> &res) {
    bool success = validateMarshalledSubscribeResponse(res);

    // successful
    if (success) return {true, ""};

    // extract errorMessage if unsuccessful
    std::string errMsg(res.begin() + sizeof(bool) + sizeof(uint32_t),
                       res.end());

    return {false, errMsg};
}

void handleSubscribeRequest(SubscribeRequest *req) {
    char buffer[1024];
    std::string pathname, offset, content;

    printf(
        "You have selected option 3, please enter the pathname of "
        "the file\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    for (int i = 0; i < strlen(buffer); i++) {
        pathname += buffer[i];
    }
    pathname.erase(std::remove_if(pathname.begin(), pathname.end(), ::isspace),
                   pathname.end());
    req->pathname = pathname;

    printf("Please enter the monitoring interval\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    for (int i = 0; i < strlen(buffer); i++) {
        offset += buffer[i];
    }
    req->monitorIntervalSeconds = stoi(offset);
}

