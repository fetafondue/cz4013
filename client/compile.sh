#!/bin/bash

# Compile the client
g++ -o client_udp client_udp.cpp messages/delete.cpp messages/read.cpp messages/replace.cpp messages/subscribe.cpp messages/write.cpp messages/get_last_modified_time.cpp messages/error.cpp cache/cache.cpp -lnetwork

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi
