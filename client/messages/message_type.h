#ifndef MESSAGE_TYPE_H
#define MESSAGE_TYPE_H

#include <cstdint>

enum MessageType : uint8_t
{
    READ = 0b000001,
    WRITE = 0b000010,
    SUBSCRIBE = 0b000100,
    REPLACE = 0b001000,
    DELETE = 0b010000,
    GET_LAST_MODIFIED_TIME = 0b100000
};

#endif
