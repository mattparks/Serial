#include <gtest/gtest.h>

#include <Serialized/Document.hpp>
#include <Serialized/Json/Json.hpp>

#include "MemoryData.hpp"

static class AcidData {
public:
	acid::Document canada;
	acid::Document catalog;
	acid::Document twitter;
} acidData;

TEST(acid, parseInMemory) {
	acid::Json<char> parser;
	parser.LoadString(acidData.canada, memoryData.canadaString);
	parser.LoadString(acidData.catalog, memoryData.catalogString);
	parser.LoadString(acidData.twitter, memoryData.twitterString);
}

TEST(acid, stringify) {
	acid::Json<char> parser;
	auto canadaString = parser.WriteString(acidData.canada);
	auto catalogString = parser.WriteString(acidData.catalog);
	auto twitterString = parser.WriteString(acidData.twitter);
}

TEST(acid, prettify) {
	acid::Json<char> parser;
	auto canadaString = parser.WriteString(acidData.canada, acid::Document::Format::Beautified);
	auto catalogString = parser.WriteString(acidData.catalog, acid::Document::Format::Beautified);
	auto twitterString = parser.WriteString(acidData.twitter, acid::Document::Format::Beautified);
}

TEST(acid, writeToFiles) {
	acid::Json<char> parser;
	std::ofstream canadaFile("Tests/canada.acid.json", std::ios_base::binary | std::ios_base::out);
	parser.WriteStream(acidData.canada, canadaFile);
	std::ofstream catalogFile("Tests/citm_catalog.acid.json", std::ios_base::binary | std::ios_base::out);
	parser.WriteStream(acidData.catalog, catalogFile, acid::Document::Format::Beautified);
	std::ofstream twitterFile("Tests/twitter.acid.json", std::ios_base::binary | std::ios_base::out);
	parser.WriteStream(acidData.twitter, twitterFile, acid::Document::Format::Beautified);
}
