#ifndef MESSAGE_TYPE_H
#define MESSAGE_TYPE_H

#include <cstdint>

enum MessageType : uint8_t
{
    READ = 0b00001,
    WRITE = 0b00010,
    SUBSCRIBE = 0b00100,
    REPLACE = 0b01000,
    DELETE = 0b10000
};

#endif
