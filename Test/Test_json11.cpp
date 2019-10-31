#include <fstream>

#include <gtest/gtest.h>
#include <json11/json11.hpp>

#include "MemoryData.hpp"

static class Json11Data {
public:
	json11::Json canada;
	json11::Json catalog;
	json11::Json twitter;
} json11Data;

TEST(json11, parseInMemory) {
	std::string err;
	json11Data.canada = json11::Json::parse(MemoryData::canadaString, err);
	json11Data.catalog = json11::Json::parse(MemoryData::catalogString, err);
	json11Data.twitter = json11::Json::parse(MemoryData::twitterString, err);
}

TEST(json11, stringify) {
	std::string canadaString = json11Data.canada.dump();
	std::string catalogString = json11Data.catalog.dump();
	std::string twitterString = json11Data.twitter.dump();
}

TEST(json11, writeToFiles) {
	std::ofstream canadaStream("Tests/canada.json11.json", std::ios_base::binary | std::ios_base::out);
	canadaStream << json11Data.canada.dump();
	std::ofstream catalogStream("Tests/citm_catalog.json11.json", std::ios_base::binary | std::ios_base::out);
	catalogStream << json11Data.catalog.dump();
	std::ofstream twitterStream("Tests/twitter.json11.json", std::ios_base::binary | std::ios_base::out);
	twitterStream << json11Data.twitter.dump();
}
