#include <fstream>

#include "gtest/gtest.h"
#include "SimpleJSON/JSON.h"
#include "MemoryData.hpp"

static class SimpleJSONData {
public:
    JSONValue* canada;
    JSONValue* catalog;
    JSONValue* twitter;
} simplejsonData;

TEST(simplejson, parseInMemory) {
    simplejsonData.canada = JSON::Parse(MemoryData::_canadaString.c_str());
    simplejsonData.catalog = JSON::Parse(MemoryData::_catalogString.c_str());
    simplejsonData.twitter = JSON::Parse(MemoryData::_twitterString.c_str());
}

TEST(simplejson, stringify) {
    auto canadaString = JSON::Stringify(simplejsonData.canada);
    auto catalogString = JSON::Stringify(simplejsonData.catalog);
    auto twitterString = JSON::Stringify(simplejsonData.twitter);
}

TEST(simplejson, writeToFiles) {
    std::wofstream canadaStream("Tests/canada.simplejson.json", std::ios_base::binary | std::ios_base::out);
    canadaStream << JSON::Stringify(simplejsonData.canada);
    std::wofstream catalogStream("Tests/citm_catalog.simplejson.json", std::ios_base::binary | std::ios_base::out);
    catalogStream << JSON::Stringify(simplejsonData.catalog);
    std::wofstream twitterStream("Tests/twitter.simplejson.json", std::ios_base::binary | std::ios_base::out);
    twitterStream << JSON::Stringify(simplejsonData.twitter);
}
