#ifndef CACHE_H
#define CACHE_H

#include <chrono>
#include <iostream>
#include <unordered_map>

// filename : <<tc, tmclient>, <<startIdx, endIdx>, filecontent>>, all
// timestamps are in epoch time in milliseconds
extern std::unordered_map<
    std::string,
    std::pair<std::pair<long long, long long>,
              std::pair<std::pair<uint32_t, uint32_t>, std::string> > >
    cache;
extern long long fInterval;

void cacheWrite(std::string filename, std::string filecontent,
                long long tmserver, uint32_t startIdx);
std::string cacheRead(std::string filename, uint32_t offset,
                      uint32_t bytesToRead);
bool fileChangedAtServer(std::string filename, long long tmserver);
bool fileInCache(std::string filename, uint32_t offset, uint32_t bytesToRead);
bool cacheExpired(std::string filename);

#endif