#pragma once
#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <list>

typedef int int32;
typedef unsigned int uint32;

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
