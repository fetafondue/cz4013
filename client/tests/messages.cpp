#include <iostream>
#include "../messages/delete.h"
#include "../messages/read.h"
#include "../messages/write.h"

// example
void deleteMessageRequestTest()
{
    DeleteRequest req;
    req.pathname = "/path/to/some/file.pdf";
    req.offset = 4294967295;
    req.numBytes = 123;

    std::vector<uint8_t> marshalledReq = marshalDeleteRequest(req);
    std::cout << "Marshalled Request: ";
    for (uint8_t b : marshalledReq)
        printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req unmarshals to the original one
}

void readMessageRequestTest()
{
    ReadRequest req;
    req.pathname = "file.txt";
    req.offset = 1;
    req.numBytes = 2;

    std::vector<uint8_t> marshalledReq = marshalReadRequest(req);
    std::cout << "Marshalled Request: ";
    for (uint8_t b : marshalledReq)
        printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req unmarshals to the original one
}

void writeMessageRequestTest()
{
    WriteRequest req;
    req.pathname = "file.txt";
    req.offset = 1;
    req.content = "eee";

    std::vector<uint8_t> marshalledReq = marshalWriteRequest(req);
    std::cout << "Marshalled Request: ";
    for (uint8_t b : marshalledReq)
        printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req unmarshals to the original one
}

// example
void deleteMessageResponseTest()
{
    std::vector<uint8_t> marshalledResponse;
    // paste in the (marshalled) byte values from server side test
    uint8_t byteValues[] = {0, 0, 0, 0, 24, 115, 111, 109, 101, 32, 101, 114, 114, 111, 114, 32, 111, 99, 99, 117, 114, 114, 101, 100, 32, 62, 58, 40, 41};
    for (uint8_t b : byteValues)
        marshalledResponse.push_back(b);
    DeleteResponse res = unmarshalDeleteResponse(marshalledResponse);
    std::cout << "Unmarshalled Response: " << res.success << "\n"
              << res.errorMessage << std::endl;
    // check if the printed response values are the same as the original on server side
}

int main()
{
    writeMessageRequestTest();

    return 0;
}
