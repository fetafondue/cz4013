#include <vector>
#include "message_type.h"
#include "read.h"

// Marshals a READ request into a byte vector
std::vector<uint8_t> marshalReadRequest(const ReadRequest &req)
{
    std::vector<uint8_t> pathnameBytes(req.pathname.begin(), req.pathname.end());
    uint32_t pathnameLen = pathnameBytes.size();

    // buffer consists of: MessageType (byte), pathnameLen (uint32), pathname (str), offset (uint32), numBytes (uint32)
    size_t bufSize = sizeof(MessageType) + pathnameLen + 3 * sizeof(uint32_t);
    std::vector<uint8_t> buf(bufSize);

    // insert MessageType into buf
    buf[0] = READ;

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

bool validateMarshalledReadResponse(const std::vector<uint8_t> &res)
{
    // ensure that the byte vector is at least its minimum length (1 bool + 1 uint32)
    if (res.size() < sizeof(bool) + sizeof(uint32_t))
        throw std::invalid_argument("invalid response: byte vector length is too short");

    bool success = res[0] == 1;

    size_t strLen = 0;
    for (size_t i = sizeof(bool); i < sizeof(bool) + sizeof(uint32_t); i++)
        strLen = (strLen << 8) | res[i];

    // ensure that the byte vector has the correct length
    if (res.size() != sizeof(bool) + sizeof(uint32_t) + strLen)
        throw std::invalid_argument("invalid response: incorrect byte vector length");

    return success;
}

// Unmarshals a byte vector into a ReadResponse struct
ReadResponse unmarshalReadResponse(const std::vector<uint8_t> &res)
{
    bool success = validateMarshalledReadResponse(res);

    // extract str
    std::string str(res.begin() + sizeof(bool) + sizeof(uint32_t), res.end());

    if (success)
        return {true, str, ""};
    return {false, "", str};
}

void prepareReadRequest(ReadRequest *req) {
    char buffer[1024];
    std::string pathname, offset, numBytes;

    printf(
        "You have selected option 1, please enter the pathname of "
        "the file\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    for (int i = 0; i < strlen(buffer); i++) {
        pathname += buffer[i];
    }
    pathname.erase(std::remove_if(pathname.begin(), pathname.end(),::isspace),
        pathname.end());
    req->pathname = pathname;

    printf("Please enter the offset\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    for (int i = 0; i < strlen(buffer); i++) {
        offset += buffer[i];
    }
    req->offset = stoi(offset);

    printf("Please enter the number of bytes to read from the file\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    for (int i = 0; i < strlen(buffer); i++) {
        numBytes += buffer[i];
    }
    req->numBytes = stoi(numBytes);
}