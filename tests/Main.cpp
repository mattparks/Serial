#include <codecvt>
#include <fstream>
#include <filesystem>
#include <thread>

#include <gtest/gtest.h>

#include "MemoryData.hpp"

std::string MemoryData::canadaString;
std::string MemoryData::catalogString;
std::string MemoryData::twitterString;

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);

	auto debugStart = std::chrono::high_resolution_clock::now();

	std::wifstream canadaStream("data/canada.json");
	canadaStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	MemoryData::canadaString = std::string(std::istreambuf_iterator<wchar_t>(canadaStream), {});

	std::wifstream catalogStream("data/citm_catalog.json");
	catalogStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	MemoryData::catalogString = std::string(std::istreambuf_iterator<wchar_t>(catalogStream), {});

	std::wifstream twitterStream("data/twitter.json");
	twitterStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	MemoryData::twitterString = std::string(std::istreambuf_iterator<wchar_t>(twitterStream), {});

	std::filesystem::remove_all("tests");
	std::filesystem::create_directory("tests");
	//std::filesystem::copy_file("data/canada.json", "tests/canada.json");
	//std::filesystem::copy_file("data/citm_catalog.json", "tests/citm_catalog.json");
	//std::filesystem::copy_file("data/twitter.json", "tests/twitter.json");

	std::cout << "Memory data loaded in " << 0.001 * std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - debugStart).count() << "ms\n";

	return RUN_ALL_TESTS();
}