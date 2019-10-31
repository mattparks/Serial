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

	std::wifstream canadaStream("Data/canada.json");
	canadaStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	MemoryData::canadaString = std::string(std::istreambuf_iterator<wchar_t>(canadaStream), {});

	std::wifstream catalogStream("Data/citm_catalog.json");
	catalogStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	MemoryData::catalogString = std::string(std::istreambuf_iterator<wchar_t>(catalogStream), {});

	std::wifstream twitterStream("Data/twitter.json");
	twitterStream.imbue(std::locale(std::locale(), new std::codecvt_utf8<char>));
	MemoryData::twitterString = std::string(std::istreambuf_iterator<wchar_t>(twitterStream), {});

	std::filesystem::remove_all("Tests");
	std::filesystem::create_directory("Tests");
	//std::filesystem::copy_file("Data/canada.json", "Tests/canada.json");
	//std::filesystem::copy_file("Data/citm_catalog.json", "Tests/citm_catalog.json");
	//std::filesystem::copy_file("Data/twitter.json", "Tests/twitter.json");

	std::cout << "Memory data loaded in " << 0.001 * std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - debugStart).count() << "ms\n";

	return RUN_ALL_TESTS();
}