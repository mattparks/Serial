#include <fstream>

#include "gtest/gtest.h"
#include "Serial/Json.hpp"
#include "MemoryData.hpp"

static struct SerialData {
    serial::Node _canada;
    serial::Node _catalog;
    serial::Node _twitter;
} SerialData;

TEST(Serial, ParseInMemory) {
    SerialData._canada.parseString<serial::Json>(MemoryData::_canadaString);
    SerialData._catalog.parseString<serial::Json>(MemoryData::_catalogString);
    SerialData._twitter.parseString<serial::Json>(MemoryData::_twitterString);
}

inline const serial::Node &operator>>(const serial::Node &node, Status &status) {
    node["id"].get(status._id);
    node["id_str"].get(status._idStr);
    node["user"].get(status._user);
    return node;
}

inline const serial::Node &operator>>(const serial::Node &node, Status::User &user) {
    node["id"].get(user._id);
    node["id_str"].get(user._idStr);
    
    node["name"].get(user._name);
    node["screen_name"].get(user._screenName);
    node["location"].get(user._description);
    node["followers_count"].get(user._followersCount);
    node["friends_count"].get(user._friendsCount);
    node["listed_count"].get(user._listedCount);
    node["favourites_count"].get(user._favouritesCount);
    node["statuses_count"].get(user._statusesCount);

    return node;
}

TEST(Serial, GetTwitterData) {
    auto statuses = SerialData._twitter["statuses"].get<std::vector<Status>>();
}

TEST(Serial, Stringify) {
    auto canadaString = SerialData._canada.writeString<serial::Json>();
    auto catalogString = SerialData._catalog.writeString<serial::Json>();
    auto twitterString = SerialData._twitter.writeString<serial::Json>();
}

TEST(Serial, Prettify) {
    auto canadaString = SerialData._canada.writeString<serial::Json>(serial::NodeFormat::Beautified);
    auto catalogString = SerialData._catalog.writeString<serial::Json>(serial::NodeFormat::Beautified);
    auto twitterString = SerialData._twitter.writeString<serial::Json>(serial::NodeFormat::Beautified);
}

TEST(Serial, WriteToFiles) {
    std::ofstream canadaFile("Tests/canada.serial.json", std::ios_base::binary | std::ios_base::out);
    SerialData._canada.writeStream<serial::Json>(canadaFile);
    std::ofstream catalogFile("Tests/citm_catalog.serial.json", std::ios_base::binary | std::ios_base::out);
    SerialData._catalog.writeStream<serial::Json>(catalogFile, serial::NodeFormat::Beautified);
    std::ofstream twitterFile("Tests/twitter.serial.json", std::ios_base::binary | std::ios_base::out);
    SerialData._twitter.writeStream<serial::Json>(twitterFile, serial::NodeFormat::Beautified);
}
