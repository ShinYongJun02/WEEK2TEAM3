#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <list>

typedef char int8;
typedef unsigned char uint8;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned long long uint64;

template<typename T>
using TArray = std::vector<T>;

template<typename T>
using TSet = std::unordered_set<T>;

template<typename K, typename V>
using TMap = std::unordered_map<K, V>;

template<typename T>
using TQueue = std::queue<T>;

template<typename T>
using TLinkedList = std::list<T>;

template <typename A, typename B>
using TPair = std::pair<A, B>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

using FString = std::string;

template <typename TKey>
using TDeque = std::deque<TKey>;

template <size_t N>
using FBitSet = std::bitset<N>;

using FDateTime = std::chrono::system_clock::time_point;