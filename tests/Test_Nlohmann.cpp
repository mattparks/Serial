#include <fstream>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "MemoryData.hpp"

static class NlohmannData {
public:
	nlohmann::json canada;
	nlohmann::json catalog;
	nlohmann::json twitter;
} nlohmannData;

TEST(nlohmann, parseInMemory) {
	nlohmannData.canada = nlohmann::json::parse(MemoryData::canadaString);
	nlohmannData.catalog = nlohmann::json::parse(MemoryData::catalogString);
	nlohmannData.twitter = nlohmann::json::parse(MemoryData::twitterString);
}

TEST(nlohmann, stringify) {
	auto canadaString = nlohmannData.canada.dump();
	auto catalogString = nlohmannData.catalog.dump();
	auto twitterString = nlohmannData.twitter.dump();
}

TEST(nlohmann, prettify) {
	auto canadaString = nlohmannData.canada.dump(2);
	auto catalogString = nlohmannData.catalog.dump(2);
	auto twitterString = nlohmannData.twitter.dump(2);
}

TEST(nlohmann, writeToFiles) {
	std::ofstream canadaStream("Tests/canada.nlohmann.json", std::ios_base::binary | std::ios_base::out);
	canadaStream << nlohmannData.canada.dump();
	std::ofstream catalogStream("Tests/citm_catalog.nlohmann.json", std::ios_base::binary | std::ios_base::out);
	catalogStream << nlohmannData.catalog.dump(2);
	std::ofstream twitterStream("Tests/twitter.nlohmann.json", std::ios_base::binary | std::ios_base::out);
	twitterStream << nlohmannData.twitter.dump(2);
}
