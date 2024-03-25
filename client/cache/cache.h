#ifndef CACHE_H
#define CACHE_H

#include <chrono>
#include <iostream>
#include <unordered_map>

// filename : { {tc : tmclient} : filecontent }, all timestamps are in epoch
// time in milliseconds
extern std::unordered_map<
    std::string, std::pair<std::pair<long long, long long>, std::string> >
    cache;
extern long long fInterval;

void cacheWrite(std::string filename, std::string filecontent,
                long long tmserver);
std::string cacheRead(std::string filename);
bool isValidCacheEntry(std::string filename);

#endif