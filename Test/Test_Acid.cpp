#include <gtest/gtest.h>

#include <Files/Json/Json.hpp>

#include "MemoryData.hpp"

static class AcidData {
public:
	acid::Json canada;
	acid::Json catalog;
	acid::Json twitter;
} acidData;

TEST(acid, parseInMemory) {
	acidData.canada.LoadString(memoryData.canadaString);
	acidData.catalog.LoadString(memoryData.catalogString);
	acidData.twitter.LoadString(memoryData.twitterString);
}

TEST(acid, stringify) {
	auto canadaString = acidData.canada.WriteString();
	auto catalogString = acidData.catalog.WriteString();
	auto twitterString = acidData.twitter.WriteString();
}

TEST(acid, prettify) {
	auto canadaString = acidData.canada.WriteString(acid::Json::Format::Beautified);
	auto catalogString = acidData.catalog.WriteString(acid::Json::Format::Beautified);
	auto twitterString = acidData.twitter.WriteString(acid::Json::Format::Beautified);
}

TEST(acid, writeToFiles) {
	std::ofstream canadaFile("Tests/canada.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.canada.WriteStream(canadaFile);
	std::ofstream catalogFile("Tests/citm_catalog.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.catalog.WriteStream(catalogFile, acid::Json::Format::Beautified);
	std::ofstream twitterFile("Tests/twitter.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.twitter.WriteStream(twitterFile, acid::Json::Format::Beautified);
}
