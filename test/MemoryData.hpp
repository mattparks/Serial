#pragma once

#include <string>

struct MemoryData {
    static std::string _canadaString;
    static std::string _catalogString;
    static std::string _twitterString;
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
