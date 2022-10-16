#pragma once

#include <codecvt>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <string>

struct MemoryData {
    MemoryData() {
        {
            std::wifstream canadaStream("data/canada.json");
            canadaStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
            _canada = std::string(std::istreambuf_iterator<wchar_t>(canadaStream), {});
        }

        {
            std::wifstream catalogStream("data/citm_catalog.json");
            catalogStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
            _catalog = std::string(std::istreambuf_iterator<wchar_t>(catalogStream), {});
        }

        {
            std::wifstream twitterStream("data/twitter.json");
            twitterStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
            _twitter = std::string(std::istreambuf_iterator<wchar_t>(twitterStream), {});
        }

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

    std::string _canada;
    std::string _catalog;
    std::string _twitter;
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
