#include <fstream>

#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "MemoryData.hpp"

static struct NlohmannData {
    nlohmann::json _canada;
    nlohmann::json _catalog;
    nlohmann::json _twitter;
} NlohmannData;

TEST(Nlohmann, ParseInMemory) {
    NlohmannData._canada = nlohmann::json::parse(MemoryData::_canadaString);
    NlohmannData._catalog = nlohmann::json::parse(MemoryData::_catalogString);
    NlohmannData._twitter = nlohmann::json::parse(MemoryData::_twitterString);
}

TEST(Nlohmann, Stringify) {
    auto canadaString = NlohmannData._canada.dump();
    auto catalogString = NlohmannData._catalog.dump();
    auto twitterString = NlohmannData._twitter.dump();
}

TEST(Nlohmann, Prettify) {
    auto canadaString = NlohmannData._canada.dump(2);
    auto catalogString = NlohmannData._catalog.dump(2);
    auto twitterString = NlohmannData._twitter.dump(2);
}

TEST(Nlohmann, WriteToFiles) {
    std::ofstream canadaStream("Tests/canada.nlohmann.json", std::ios_base::binary | std::ios_base::out);
    canadaStream << NlohmannData._canada.dump();
    std::ofstream catalogStream("Tests/citm_catalog.nlohmann.json", std::ios_base::binary | std::ios_base::out);
    catalogStream << NlohmannData._catalog.dump(2);
    std::ofstream twitterStream("Tests/twitter.nlohmann.json", std::ios_base::binary | std::ios_base::out);
    twitterStream << NlohmannData._twitter.dump(2);
}
