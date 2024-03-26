#include "cache.h"

#include <chrono>
#include <iostream>
#include <unordered_map>

#include "../messages/get_last_modified_time.h"

std::unordered_map<
    std::string,
    std::pair<std::pair<long long, long long>,
              std::pair<std::pair<uint32_t, uint32_t>, std::string> > >
    cache;

long long fInterval;

void cacheWrite(std::string filename, std::string filecontent,
                long long tmserver, uint32_t startIdx) {
    // if filename is not in cache
    if (cache.find(filename) == cache.end()) {
        std::pair<long long, long long> timestamp;
        std::pair<uint32_t, uint32_t> idx;

        // create timestamp pair of <tc, tmclient>
        timestamp.first =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
        timestamp.second = tmserver;

        // set startIdx and endIdx
        idx.first = startIdx;
        idx.second = startIdx + filecontent.size();

        // create idx pair to file content
        std::pair<std::pair<uint32_t, uint32_t>, std::string> idxContent;
        idxContent.first = idx;
        idxContent.second = filecontent;

        // create cache data
        std::pair<std::pair<long long, long long>,
                  std::pair<std::pair<uint32_t, uint32_t>, std::string> >
            cacheData;
        cacheData.first = timestamp;
        cacheData.second = idxContent;

        // write new entry to cache
        cache[filename] = cacheData;
    }
    // if file is in cache
    else {
        std::pair<uint32_t, uint32_t> idx;
        // get cache data
        std::pair<std::pair<long long, long long>,
                  std::pair<std::pair<uint32_t, uint32_t>, std::string> >
            cacheData = cache[filename];

        // get timestamps
        std::pair<long long, long long> timestamps = cacheData.first;
        // update client cache entry validation to current time
        timestamps.first =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
        // update server file content modified time to same as server
        timestamps.second = tmserver;
        // set startIdx and endIdx
        idx.first = startIdx;
        idx.second = startIdx + filecontent.size();
        // update cache data
        cacheData.second.first = idx;
        cacheData.second.second = filecontent;
        // write updates to cache
        cache[filename] = cacheData;
    }
}

std::string cacheRead(std::string filename, uint32_t offset,
                      uint32_t bytesToRead) {
    if (cache.find(filename) == cache.end()) {
        return "";
    } else {
        std::string data = cache[filename].second.second;
        return data.substr(offset, bytesToRead);
    }
}

bool fileInCache(std::string filename, uint32_t offset, uint32_t bytesToRead) {
    if (cache.find(filename) == cache.end()) {
        std::cout << "File data not found in cache, reading file from server"
                  << '\n';
        return false;
    }
    // check if requested data is within the cache
    std::pair<uint32_t, uint32_t> idx = cache[filename].second.first;
    uint32_t startIdx = idx.first;
    uint32_t endIdx = idx.second;
    if (startIdx > offset || endIdx < offset + bytesToRead) {
        std::cout << "Requested data out of range in cache, reading file from server"
                  << '\n';
        return false;
    }
    return true;
}

bool cacheExpired(std::string filename) {
    // get curr time t
    long long t = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
    // get client cache entry last validated
    long long tc = cache[filename].first.first;
    if (t - tc < fInterval) {
        return false;
    }
    std::cout << "Cache data has expired" << '\n';
    return true;
}

bool fileChangedAtServer(std::string filename, long long tmserver) {
    if (cache.find(filename) == cache.end()) {
        return true;
    }
    long long tmclient = cache[filename].first.second;
    if (tmclient == tmserver) {
        std::cout << "File has not changed in server, refreshing cache timing"
                  << '\n';
        long long t = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
        cache[filename].first.first = t;
        return false;
    }
    std::cout << "File has changed in server" << '\n';
    return true;
}