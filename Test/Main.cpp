#include <codecvt>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include "MemoryData.hpp"

MemoryData::MemoryData() {
	std::wifstream canadaStream("Data/canada.json");
	canadaStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	canadaString = std::string(std::istreambuf_iterator<wchar_t>(canadaStream), {});

	std::wifstream catalogStream("Data/citm_catalog.json");
	catalogStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	catalogString = std::string(std::istreambuf_iterator<wchar_t>(catalogStream), {});

	std::wifstream twitterStream("Data/twitter.json");
	twitterStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	twitterString = std::string(std::istreambuf_iterator<wchar_t>(twitterStream), {});

	std::filesystem::remove_all("Tests");
	std::filesystem::create_directory("Tests");
	//std::filesystem::copy_file("Data/canada.json", "Tests/canada.json");
	//std::filesystem::copy_file("Data/citm_catalog.json", "Tests/citm_catalog.json");
	//std::filesystem::copy_file("Data/twitter.json", "Tests/twitter.json");
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}