#include "get_last_modified_time.h"

#include <vector>

#include "message_type.h"

// Marshals a GET_LAST_MODIFIED_TIME request into a byte vector
std::vector<uint8_t> marshalGetLastModifiedTimeRequest(
    const GetLastModifiedTimeRequest &req) {
    std::vector<uint8_t> pathnameBytes(req.pathname.begin(),
                                       req.pathname.end());
    uint32_t pathnameLen = pathnameBytes.size();

    // buffer consists of: MessageType (byte), pathnameLen (uint32), pathname
    // (str)
    size_t bufSize = sizeof(MessageType) + sizeof(uint32_t) + pathnameLen;
    std::vector<uint8_t> buf(bufSize);

    // insert MessageType into buf
    buf[0] = GET_LAST_MODIFIED_TIME;

    // insert pathname length and itself into buf
    uint32_t pathnameLenBE = htonl(pathnameLen);  // big endian format
    std::memcpy(buf.data() + sizeof(MessageType), &pathnameLenBE,
                sizeof(uint32_t));
    std::memcpy(buf.data() + sizeof(MessageType) + sizeof(uint32_t),
                pathnameBytes.data(), pathnameLen);

    return buf;
}

bool validateMarshalledGetLastModifiedTimeResponse(
    const std::vector<uint8_t> &res) {
    // ensure that the byte vector is at least a single bool long
    if (res.size() < sizeof(bool))
        throw std::invalid_argument(
            "invalid response: byte vector length is too short");

    bool success = res[0] == 1;

    // success: should contain lastModifiedUnixTime (uint64)
    if (success && res.size() != sizeof(bool) + sizeof(uint64_t))
        throw std::invalid_argument(
            "invalid response: incorrect byte vector length (success)");

    // fail: should contain a string (and uint32 for the string's length)
    if (!success) {
        size_t errMsgLen = 0;
        for (size_t i = sizeof(bool); i < sizeof(bool) + sizeof(uint32_t); i++)
            errMsgLen = (errMsgLen << 8) | res[i];

        // Ensure that the byte vector has the correct length if it includes
        // errorMessage
        if (res.size() != sizeof(bool) + sizeof(uint32_t) + errMsgLen)
            throw std::invalid_argument(
                "invalid response: incorrect byte vector length "
                "(unsuccessful)");
    }

    return success;
}

// Unmarshals a byte vector into a GetLastModifiedTimeResponse struct
GetLastModifiedTimeResponse unmarshalGetLastModifiedTimeResponse(
    const std::vector<uint8_t> &res) {
    bool success = validateMarshalledGetLastModifiedTimeResponse(res);

    // extract lastModifiedUnixTime if successful
    if (success) {
        uint64_t lastModifiedUnixTime = 0;
        for (size_t i = sizeof(bool); i < sizeof(bool) + sizeof(uint64_t); i++)
            lastModifiedUnixTime = (lastModifiedUnixTime << 8) | res[i];

        return {true, lastModifiedUnixTime, ""};
    }

    // extract errorMessage if unsuccessful
    std::string errMsg(res.begin() + sizeof(bool) + sizeof(uint32_t),
                       res.end());

    return {false, 0, errMsg};
}