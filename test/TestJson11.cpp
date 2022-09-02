#include <fstream>

#include "gtest/gtest.h"
#include "json11/json11.hpp"
#include "MemoryData.hpp"

static struct Json11Data {
    json11::Json _canada;
    json11::Json _catalog;
    json11::Json _twitter;
} Json11Data;

TEST(Json11, ParseInMemory) {
    std::string err;
    Json11Data._canada = json11::Json::parse(MemoryData::_canadaString, err);
    Json11Data._catalog = json11::Json::parse(MemoryData::_catalogString, err);
    Json11Data._twitter = json11::Json::parse(MemoryData::_twitterString, err);
}

TEST(Json11, Stringify) {
    std::string canadaString = Json11Data._canada.dump();
    std::string catalogString = Json11Data._catalog.dump();
    std::string twitterString = Json11Data._twitter.dump();
}

TEST(Json11, WriteToFiles) {
    std::ofstream canadaStream("Tests/canada.json11.json", std::ios_base::binary | std::ios_base::out);
    canadaStream << Json11Data._canada.dump();
    std::ofstream catalogStream("Tests/citm_catalog.json11.json", std::ios_base::binary | std::ios_base::out);
    catalogStream << Json11Data._catalog.dump();
    std::ofstream twitterStream("Tests/twitter.json11.json", std::ios_base::binary | std::ios_base::out);
    twitterStream << Json11Data._twitter.dump();
}
