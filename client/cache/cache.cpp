#include "cache.h"
#include "../messages/get_last_modified_time.h"

#include <chrono>
#include <iostream>
#include <unordered_map>

std::unordered_map<std::string,
                   std::pair<std::pair<long long, long long>, std::string> >
    cache;

long long fInterval;

void cacheWrite(std::string filename, std::string filecontent,
                long long tmserver) {
    // if filename is not in cache
    if (cache.find(filename) == cache.end()) {
        std::pair<long long, long long> timestamp;
        // set client cache entry validation to current time
        timestamp.first =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
        // set server file content modified time to same as server
        timestamp.second = tmserver;

        // create pair of timestamps to file data
        std::pair<std::pair<long long, long long>, std::string> cacheData;
        cacheData.first = timestamp;
        cacheData.second = filecontent;

        // write new entry to cache
        cache[filename] = cacheData;
    }
    // if file is in cache
    else {
        // get cache data
        std::pair<std::pair<long long, long long>, std::string> cacheData =
            cache[filename];

        // get timestamps
        std::pair<long long, long long> timestamps = cacheData.first;
        // update client cache entry validation to current time
        timestamps.first =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
        // update server file content modified time to same as server
        timestamps.second = tmserver;
        // update filecontent
        cacheData.second = filecontent;
    }
}

std::string cacheRead(std::string filename) {
    if (cache.find(filename) == cache.end()) {
        return "";
    } else {
        return cache[filename].second;
    }
}

bool isValidCacheEntry(std::string filename) {
    if (cache.find(filename) == cache.end()) {
        return false;
    }
    // get curr time t
    long long t = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
    // get client cache entry last validated
    long long tc = cache[filename].first.first;
    if (t - tc < fInterval) {
        return true;
    }
    return false;
}