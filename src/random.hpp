#pragma once

#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include <vector>
#include <algorithm>
#include <initializer_list>

namespace ext {

// Real (0, 1)
template<typename T = double>
T random_01() {
    return static_cast<double>(rand()) / RAND_MAX;
}

// Real (min, max)
template<typename T>
T random(T min, T max) {
    return random_01<>() * (max - min) + min;
}

template<typename T>
T random_pick(const std::vector<std::vector<T> > &ranges) throw(const char *) {
    int sum = 0;
    std::vector<int> segs;
    segs.reserve(ranges.size()+1);
    segs.push_back(0);
    for (int i = 0, size = ranges.size(); i < size; ++i) {
        if (ranges[i].size() != 2) {
            throw "range size should be 2";
        }
        int n = ranges[i][1] - ranges[i][0] + 1;
        if (n < 0) n = 0;
        segs.push_back(sum += n);
    }
    if (sum == 0) {
        throw "range empty";
    }
    int v =  random(0, sum);
    //std::cout << v << "/" << sum << " ";
    auto it = std::upper_bound(segs.begin(), segs.end(), v) - 1;
    int index = it - segs.begin();
    int offset = v - *it;
    return ranges[index][0] + offset;
}

template<typename T>
T random_pick(std::initializer_list<std::initializer_list<T> > ranges) {
    std::vector<std::vector<T> > _ranges;
    for (auto it = ranges.begin(); it != ranges.end(); ++it) {
        _ranges.push_back(std::vector<T>(*it));
    }
    return random_pick(_ranges);
}

}

