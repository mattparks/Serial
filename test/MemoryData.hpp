#pragma once

#include <codecvt>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <string>

#include "mio.hpp"

struct MemoryData {
    MemoryData()
        : _canada("data/canada.json")
        , _catalog("data/citm_catalog.json")
        , _twitter("data/twitter.json")
     {
        static bool ClearedTests = false;
        if (!ClearedTests) {
            std::filesystem::remove_all("tests");
            std::filesystem::create_directory("tests");
            //std::filesystem::copy_file("data/canada.json", "tests/canada.json");
            //std::filesystem::copy_file("data/citm_catalog.json", "tests/citm_catalog.json");
            //std::filesystem::copy_file("data/twitter.json", "tests/twitter.json");
            ClearedTests = true;
        }
    }

    mio::mmap_source _canada;
    mio::mmap_source _catalog;
    mio::mmap_source _twitter;
};

struct Status {
    struct User {
        uint32_t _id = -1;
        std::string _idStr;
        
        std::string _name, _screenName;
        std::string _location, _description;
        uint32_t _followersCount, _friendsCount, _listedCount, _favouritesCount, _statusesCount;
    };
    
    User _user;
    uint64_t _id = -1;
    std::string _idStr;
};
