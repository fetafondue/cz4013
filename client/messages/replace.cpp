#include <vector>

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

// Marshals a ReplaceRequest struct into a byte vector
std::vector<uint8_t> marshalReplaceRequest(const ReplaceRequest &req)
{
    std::vector<uint8_t> pathnameBytes(req.pathname.begin(), req.pathname.end());
    uint32_t pathnameLen = pathnameBytes.size();
    std::vector<uint8_t> contentBytes(req.content.begin(), req.content.end());
    uint32_t contentLen = contentBytes.size();

    // buffer consists of: pathnameLen (uint32), pathname (str), offset (uint32), contentLen (uint32), content (str)
    size_t bufSize = pathnameLen + contentLen + 3 * sizeof(uint32_t);
    std::vector<uint8_t> buf(bufSize);

    // insert pathname length and itself into buf
    uint32_t pathnameLenBE = htonl(pathnameLen); // big endian format
    std::memcpy(buf.data(), &pathnameLenBE, sizeof(uint32_t));
    std::memcpy(buf.data() + sizeof(uint32_t), pathnameBytes.data(), pathnameLen);
    // insert offset into buf
    uint32_t offsetBE = htonl(req.offset); // big endian format
    std::memcpy(buf.data() + sizeof(uint32_t) + pathnameLen, &offsetBE, sizeof(uint32_t));
    // insert content length and itself into buf
    uint32_t contentLenBE = htonl(contentLen); // big endian format
    std::memcpy(buf.data() + 2 * sizeof(uint32_t) + pathnameLen, &contentLenBE, sizeof(uint32_t));
    std::memcpy(buf.data() + 3 * sizeof(uint32_t) + pathnameLen, contentBytes.data(), contentLen);

    return buf;
}

bool validateMarshalledReplaceResponse(const std::vector<uint8_t> &res)
{
    // ensure that the byte vector is at least its minimum length (single bool)
    if (res.size() == 0)
        throw std::invalid_argument("invalid response: byte vector length is too short");

    bool success = res[0] == 1;

    // case of only bool present
    if (res.size() == sizeof(bool))
    {
        // should not have success = false
        if (!success)
            throw std::invalid_argument("invalid response: unsuccessful without error message");

        return true;
    }

    // should not have success = true
    if (success)
        throw std::invalid_argument("invalid response: successful with additional unknown bytes");

    size_t errMsgLen = 0;
    for (size_t i = sizeof(bool); i < sizeof(bool) + sizeof(uint32_t); i++)
        errMsgLen = (errMsgLen << 8) | res[i];

    // Ensure that the byte slice has the correct length if it includes errorMessage
    if (res.size() != sizeof(bool) + sizeof(uint32_t) + errMsgLen)
        throw std::invalid_argument("invalid response: incorrect byte array length");

    return success;
}

// Unmarshals a byte vector into a ReplaceResponse struct
ReplaceResponse unmarshalReplaceResponse(const std::vector<uint8_t> &res)
{
    bool success = validateMarshalledReplaceResponse(res);

    // successful
    if (success)
        return {true, ""};

    // extract errorMessage if unsuccessful
    std::string errMsg(res.begin() + sizeof(bool) + sizeof(uint32_t), res.end());

    return {false, errMsg};
}
