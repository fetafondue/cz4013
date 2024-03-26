#include "write.h"

#include <iostream>
#include <vector>

#include "message_type.h"

// Marshals a WRITE request into a byte vector
std::vector<uint8_t> marshalWriteRequest(const uint32_t &reqID, WriteRequest &req)
{
    std::vector<uint8_t> pathnameBytes(req.pathname.begin(),
                                       req.pathname.end());
    uint32_t pathnameLen = pathnameBytes.size();
    std::vector<uint8_t> contentBytes(req.content.begin(), req.content.end());
    uint32_t contentLen = contentBytes.size();

    // buffer consists of: MessageType (byte), pathnameLen (uint32), pathname
    // (str), offset (uint32), contentLen (uint32), content (str)
    size_t bufSize =
        sizeof(MessageType) + pathnameLen + contentLen + 3 * sizeof(uint32_t);
    std::vector<uint8_t> buf(bufSize);

    // insert MessageType into buf
    buf[0] = WRITE;
    // insert message ID into buf
    std::memcpy(buf.data() + sizeof(MessageType), &reqID, sizeof(uint32_t));

    // insert pathname length and itself into buf
    uint32_t pathnameLenBE = htonl(pathnameLen); // big endian format
    std::memcpy(buf.data() + sizeof(MessageType) + sizeof(uint32_t), &pathnameLenBE,
                sizeof(uint32_t));
    std::memcpy(buf.data() + sizeof(MessageType) + 2 * sizeof(uint32_t),
                pathnameBytes.data(), pathnameLen);
    // insert offset into buf
    uint32_t offsetBE = htonl(req.offset); // big endian format
    std::memcpy(
        buf.data() + sizeof(MessageType) + 2 * sizeof(uint32_t) + pathnameLen,
        &offsetBE, sizeof(uint32_t));
    // insert content length and itself into buf
    uint32_t contentLenBE = htonl(contentLen); // big endian format
    std::memcpy(
        buf.data() + sizeof(MessageType) + 3 * sizeof(uint32_t) + pathnameLen,
        &contentLenBE, sizeof(uint32_t));
    std::memcpy(
        buf.data() + sizeof(MessageType) + 4 * sizeof(uint32_t) + pathnameLen,
        contentBytes.data(), contentLen);

    return buf;
}

bool validateMarshalledWriteResponse(const std::vector<uint8_t> &res)
{
    // ensure that the byte vector is at least its minimum length (single bool)
    if (res.size() == 0)
        throw std::invalid_argument(
            "invalid response: byte vector length is too short");

    bool success = res[0] == 1;

    // case of only bool present
    if (res.size() == sizeof(bool))
    {
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

    // ensure that the byte vector has the correct length if it includes
    // errorMessage
    if (res.size() != sizeof(bool) + sizeof(uint32_t) + errMsgLen)
        throw std::invalid_argument(
            "invalid response: incorrect byte vector length");

    return success;
}

// Unmarshals a byte vector into a WriteResponse struct
WriteResponse unmarshalWriteResponse(const std::vector<uint8_t> &res)
{
    bool success = validateMarshalledWriteResponse(res);

    // successful
    if (success)
        return {true, ""};

    // extract errorMessage if unsuccessful
    std::string errMsg(res.begin() + sizeof(bool) + sizeof(uint32_t),
                       res.end());

    return {false, errMsg};
}

void prepareWriteRequest(WriteRequest *req)
{
    char buffer[1024];
    std::string pathname, content;

    printf(
        "You have selected option 2, please enter the pathname of "
        "the file\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    for (int i = 0; i < strlen(buffer); i++)
    {
        pathname += buffer[i];
    }
    pathname.erase(std::remove_if(pathname.begin(), pathname.end(), ::isspace),
                   pathname.end());
    req->pathname = pathname;

    while (true)
    {
        std::string offset;
        printf("Please enter the offset\n");
        bzero(buffer, 1024);
        fgets(buffer, 1023, stdin);
        for (int i = 0; i < strlen(buffer); i++)
        {
            offset += buffer[i];
        }
        try
        {
            if (stoi(offset) < 0)
            {
                throw std::invalid_argument("Negative input");
            }
            req->offset = stoi(offset);
            break;
        }
        catch (const std::exception &e)
        {
            std::cout << "Invalid input, please enter a positive integer\n";
        }
    }

    printf(
        "Please enter the content that you would like to write to the file\n");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    for (int i = 0; i < strlen(buffer); i++)
    {
        content += buffer[i];
    }
    content.erase(std::remove_if(content.begin(), content.end(), ::isspace),
                  content.end());
    req->content = content;
}