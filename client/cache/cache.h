#ifndef CACHE_H
#define CACHE_H

#include <chrono>
#include <iostream>
#include <unordered_map>

// filename : { {tc : tmclient} : filecontent }
extern std::unordered_map<
    std::string,
    std::pair<std::pair<std::chrono::milliseconds, std::chrono::milliseconds>,
              std::string> >
    cache;

extern int fInterval;

void cacheWrite(std::string filename, std::string filecontent,
                std::chrono::milliseconds tmserver);
std::string cacheRead(std::string filename);

#endif