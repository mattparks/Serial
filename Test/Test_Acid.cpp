#include <gtest/gtest.h>

#include <Serialized/Node.hpp>
#include <Serialized/Json/Json.hpp>

#include "MemoryData.hpp"

static class AcidData {
public:
	acid::Json canada;
	acid::Json catalog;
	acid::Json twitter;
} acidData;

TEST(acid, parseInMemory) {
	acidData.canada.Load(memoryData.canadaString);
	acidData.catalog.Load(memoryData.catalogString);
	acidData.twitter.Load(memoryData.twitterString);
}

TEST(acid, stringify) {
	auto canadaString = acidData.canada.Write();
	auto catalogString = acidData.catalog.Write();
	auto twitterString = acidData.twitter.Write();
}

TEST(acid, prettify) {
	auto canadaString = acidData.canada.Write(acid::Node::Format::Beautified);
	auto catalogString = acidData.catalog.Write(acid::Node::Format::Beautified);
	auto twitterString = acidData.twitter.Write(acid::Node::Format::Beautified);
}

TEST(acid, writeToFiles) {
	std::ofstream canadaFile("Tests/canada.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.canada.Write(canadaFile);
	std::ofstream catalogFile("Tests/citm_catalog.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.catalog.Write(catalogFile, acid::Node::Format::Beautified);
	std::ofstream twitterFile("Tests/twitter.acid.json", std::ios_base::binary | std::ios_base::out);
	acidData.twitter.Write(twitterFile, acid::Node::Format::Beautified);
}
