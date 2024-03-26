#include <iostream>

#include "../messages/delete.h"
#include "../messages/get_last_modified_time.h"
#include "../messages/read.h"
#include "../messages/replace.h"
#include "../messages/subscribe.h"
#include "../messages/write.h"

// example
void deleteMessageRequestTest() {
    DeleteRequest req;
    req.pathname = "/path/to/some/file.pdf";
    req.offset = 4294967295;
    req.numBytes = 123;

    std::vector<uint8_t> marshalledReq = marshalDeleteRequest(1, req);
    std::cout << "Delete test marshalled request: ";
    for (uint8_t b : marshalledReq) printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req
    // unmarshals to the original one
}

void getlastmodtimeMessageRequestTest()
{
    GetLastModifiedTimeRequest req;
    req.pathname = "file.txt";

    std::vector<uint8_t> marshalledReq = marshalGetLastModifiedTimeRequest(2, req);
    std::cout << "Get_last_mod_time test marshalled request: ";
    for (uint8_t b : marshalledReq)
        printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req
    // unmarshals to the original one
}

void readMessageRequestTest() {
    ReadRequest req;
    req.pathname = "file.txt";
    req.offset = 1;
    req.numBytes = 2;

    std::vector<uint8_t> marshalledReq = marshalReadRequest(3, req);
    std::cout << "Read test marshalled request: ";
    for (uint8_t b : marshalledReq) printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req
    // unmarshals to the original one
}

void writeMessageRequestTest() {
    WriteRequest req;
    req.pathname = "file.txt";
    req.offset = 1;
    req.content = "eee";

    std::vector<uint8_t> marshalledReq = marshalWriteRequest(4, req);
    std::cout << "Write test marshalled request: ";
    for (uint8_t b : marshalledReq) printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req
    // unmarshals to the original one
}

void replaceMessageRequestTest() {
    ReplaceRequest req;
    req.pathname = "file.txt";
    req.offset = 1;
    req.content = "eee";

    std::vector<uint8_t> marshalledReq = marshalReplaceRequest(5, req);
    std::cout << "Replace test marshalled request: ";
    for (uint8_t b : marshalledReq) printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req
    // unmarshals to the original one
}

void subscribeMessageRequestTest() {
    SubscribeRequest req;
    req.pathname = "file.txt";
    req.monitorIntervalSeconds = 1;

    std::vector<uint8_t> marshalledReq = marshalSubscribeRequest(6, req);
    std::cout << "Replace test marshalled request: ";
    for (uint8_t b : marshalledReq) printf("\\x%.2x", b);
    std::cout << std::endl;
    // paste the printed value into server side test and see if the req
    // unmarshals to the original one
}

// example
void deleteMessageResponseTest() {
    std::vector<uint8_t> marshalledResponse;
    // paste in the (marshalled) byte values from server side test
    uint8_t byteValues[] = {0,   0,   0,   0,   24,  115, 111, 109, 101, 32,
                            101, 114, 114, 111, 114, 32,  111, 99,  99,  117,
                            114, 114, 101, 100, 32,  62,  58,  40,  41};
    for (uint8_t b : byteValues) marshalledResponse.push_back(b);
    DeleteResponse res = unmarshalDeleteResponse(marshalledResponse);
    std::cout << "Delete test unmarshalled response: " << res.success << "\n"
              << res.errorMessage << std::endl;
    // check if the printed response values are the same as the original on
    // server side
}

void getlastmodtimeMessageResponseTest()
{
    std::vector<uint8_t> marshalledResponse;
    // paste in the (marshalled) byte values from server side test
    uint8_t byteValues[] = {0, 0, 0, 0, 18, 116, 104, 105, 115, 32, 105, 115, 32, 97, 110, 32, 101, 114, 114, 111, 114, 33, 33};
    for (uint8_t b : byteValues)
        marshalledResponse.push_back(b);
    GetLastModifiedTimeResponse res = unmarshalGetLastModifiedTimeResponse(marshalledResponse);

    // format unix time to datetime
    std::time_t unix_time = res.lastModifiedUnixTime;
    std::tm *ptm = std::gmtime(&unix_time);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);

    std::cout << "Getlastmodtime test unmarshalled response: " << res.success << "\n"
              << "UnixTime:" << res.lastModifiedUnixTime << "\n"
              << "DateTime: " << buffer << "\n"
              << res.errorMessage << std::endl;

    // check if the printed response values are the same as the original on
    // server side
}

int main() {
    deleteMessageRequestTest();
    getlastmodtimeMessageRequestTest();
    readMessageRequestTest();
    writeMessageRequestTest();
    replaceMessageRequestTest();
    subscribeMessageRequestTest();
    deleteMessageResponseTest();
    getlastmodtimeMessageResponseTest();

    return 0;
}
