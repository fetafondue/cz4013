#include <vector>
#include "message_type.h"
#include "delete.h"

// Marshals a DELETE request into a byte vector
std::vector<uint8_t> marshalDeleteRequest(const DeleteRequest &req)
{
    std::vector<uint8_t> pathnameBytes(req.pathname.begin(), req.pathname.end());
    uint32_t pathnameLen = pathnameBytes.size();

    // buffer consists of: MessageType (byte), pathnameLen (uint32), pathname (str), offset (uint32), numBytes (uint32)
    size_t bufSize = sizeof(MessageType) + pathnameLen + 3 * sizeof(uint32_t);
    std::vector<uint8_t> buf(bufSize);

    // insert MessageType into buf
    buf[0] = DELETE;
    
    // insert pathname length and itself into buf
    uint32_t pathnameLenBE = htonl(pathnameLen); // big endian format
    std::memcpy(buf.data() + sizeof(MessageType), &pathnameLenBE, sizeof(uint32_t));
    std::memcpy(buf.data() + sizeof(MessageType) + sizeof(uint32_t), pathnameBytes.data(), pathnameLen);
    // insert offset into buf
    uint32_t offsetBE = htonl(req.offset); // big endian format
    std::memcpy(buf.data() + sizeof(MessageType) + sizeof(uint32_t) + pathnameLen, &offsetBE, sizeof(uint32_t));
    // insert numBytes into buf
    uint32_t numBytesBE = htonl(req.numBytes); // big endian format
    std::memcpy(buf.data() + sizeof(MessageType) + 2 * sizeof(uint32_t) + pathnameLen, &numBytesBE, sizeof(uint32_t));

    return buf;
}

bool validateMarshalledDeleteResponse(const std::vector<uint8_t> &res)
{
    // ensure that the byte vector is at least its minimum length (single bool)
    if (res.size() == 0)
        throw std::invalid_argument("invalid response: byte vector length is too short");

    bool success = res[0] == 1;

    // case of only bool present
    if (res.size() == sizeof(bool))
    {
        // should not have Success = false
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

// Unmarshals a byte vector into a DeleteResponse struct
DeleteResponse unmarshalDeleteResponse(const std::vector<uint8_t> &res)
{
    bool success = validateMarshalledDeleteResponse(res);

    // successful
    if (success)
        return {true, ""};

    // extract errorMessage if unsuccessful
    std::string errMsg(res.begin() + sizeof(bool) + sizeof(uint32_t), res.end());

    return {false, errMsg};
}
