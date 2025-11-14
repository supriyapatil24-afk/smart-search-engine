cmake_minimum_required(VERSION 3.10)
project(SmartSearchEngine)

set(CMAKE_CXX_STANDARD 11)

add_executable(search_engine 
    main.cpp 
    trie.cpp 
    graph.cpp 
    hashmap.cpp 
    heap.cpp 
    utils.cpp
    datapersistence.cpp
)