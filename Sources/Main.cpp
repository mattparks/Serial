#include <iostream>

#include <fstream>
#include <unordered_map>
#include "Serialized/Node.hpp"
#include "Serialized/Json/Json.hpp"
#include "Serialized/Yaml/Yaml.hpp"

using namespace acid;

namespace test {
class Example1 {
public:
	class XML {
	public:
		std::vector<std::vector<std::string>> data = {{"clunky"}, {"uses more words than necessary"}};
		std::optional<float> optional0;
		std::optional<std::string> optional1 = "Hello optional string!";

		friend const Node &operator>>(const Node &node, XML &xml) {
			node["data"].Get(xml.data);
			node["optional0"].Get(xml.optional0);
			node["optional1"].Get(xml.optional1);
			return node;
		}

		friend Node &operator<<(Node &node, const XML &xml) {
			node["data"].Set(xml.data);
			node["optional0"].Set(xml.optional0);
			node["optional1"].Set(xml.optional1);
			return node;
		}
	} xml;

	class Objects {
	public:
		std::string url = "https://equilibrium.games/";
		std::string key = "value";
		std::vector<float> values = {190.0f, 11.0f, -0.001f};

		friend const Node &operator>>(const Node &node, Objects &objects) {
			node["url"].Get(objects.url);
			node["key"].Get(objects.key, "failed to get key");
			node["values"].Get(objects.values);
			return node;
		}

		friend Node &operator<<(Node &node, const Objects &objects) {
			node["url"].Set(objects.url);
			node["key"].Set(objects.key);
			node["values"].Set(objects.values);
			return node;
		}
	} objects;

	std::string paragraph = "Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n";
	std::unique_ptr<std::string> content = std::make_unique<std::string>("Ut enim ad minim veniam,\nquis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");
	//std::string google = "\"https://google.com\"";

	std::filesystem::path currentPath = std::filesystem::current_path();
	std::vector<std::string> json = {"rigid", "better for data interchange"};
	std::vector<std::string> yaml = {"slim and flexible", "better for configuration", "supports comments"};
	std::map<int32_t, std::string> map = {{10, "Hello World"}, {-2, "Negative Keys"}, {400, "Larger Key"}};
	std::map<int32_t, std::vector<std::string>> vectorMap = {{-1, {"A", "B", "C"}}, {8, {"1", "2.00", "3.00"}}, {700, {"%", "$", "#", "&", "#"}}};

	friend const Node &operator>>(const Node &node, Example1 &example1) {
		node["currentPath"].Get(example1.currentPath);
		node["paragraph"].Get(example1.paragraph);
		//node["google"].Get(example1.google);
		node["content"].Get(example1.content);
		node["xml"].Get(example1.xml);
		node["json"].Get(example1.json);
		node["yaml"].Get(example1.yaml);
		node["map"].Get(example1.map);
		node["vectorMap"].Get(example1.vectorMap);
		node["objects"].Get(example1.objects);
		return node;
	}

	friend Node &operator<<(Node &node, const Example1 &example1) {
		node["currentPath"].Set(example1.currentPath);
		node["paragraph"].Set(example1.paragraph);
		//node["google"].Set(example1.google);
		node["content"].Set(example1.content);
		node["xml"].Set(example1.xml);
		node["json"].Set(example1.json);
		node["yaml"].Set(example1.yaml);
		node["map"].Set(example1.map);
		node["vectorMap"].Set(example1.vectorMap);
		node["objects"].Set(example1.objects);
		return node;
	}
};

class User {
public:
	std::string username;
	std::string fullname;
	std::string description;
	bool employed = false;
	std::string birthday;

	friend const Node &operator>>(const Node &node, User &user) {
		node["username"].Get(user.username);
		node["fullname"].Get(user.fullname);
		node["description"].Get(user.description);
		node["employed"].Get(user.employed);
		node["birthday"].Get(user.birthday);
		return node;
	}

	friend Node &operator<<(Node &node, const User &user) {
		node["username"].Set(user.username);
		node["fullname"].Set(user.fullname);
		node["description"].Set(user.description);
		node["employed"].Set(user.employed);
		node["birthday"].Set(user.birthday);
		return node;
	}
};
}

#ifndef _DEBUG
#define TEST_RUN
//#define TEST_STRINGIFY_FILE
#endif

int main(int argc, char **argv) {
#ifdef TEST_RUN
	// https://github.com/miloyip/nativejson-benchmark/tree/master/data
	Json canada;
	Json catalog;
	Json twitter;

#ifdef TEST_STRINGIFY_FILE
	std::filesystem::remove("canada.1.json");
	std::filesystem::remove("citm_catalog.1.json");
	std::filesystem::remove("twitter.1.json");
#endif

	{
		std::wifstream canadaStream("canada.json");
		canadaStream.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char>));
		std::string canadaString(std::istreambuf_iterator<wchar_t>(canadaStream), {});

		std::wifstream catalogStream("citm_catalog.json");
		catalogStream.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char>));
		std::string catalogString(std::istreambuf_iterator<wchar_t>(catalogStream), {});

		std::wifstream twitterStream("twitter.json");
		twitterStream.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char>));
		std::string twitterString(std::istreambuf_iterator<wchar_t>(twitterStream), {});
		
		auto start = std::chrono::high_resolution_clock::now();

		canada.Load(canadaString);
		catalog.Load(catalogString);
		twitter.Load(twitterString);

		auto length = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		std::cout << "Parse: " << length.count() << "ms\n"; // Parse in-memory JSON into DOM (tree structure).

		std::size_t size = 0;
		canada.AddSize(size);
		catalog.AddSize(size);
		twitter.AddSize(size);
		std::cout << "Memory: " << size << " bytes\n";
	}
	{
#ifdef TEST_STRINGIFY_FILE
		auto start = std::chrono::high_resolution_clock::now();

		std::ofstream canadaFile("canada.1.json", std::ios_base::binary | std::ios_base::out);
		canada.Write(canadaFile);
		std::ofstream catalogFile("citm_catalog.1.json", std::ios_base::binary | std::ios_base::out);
		catalog.Write(catalogFile, Node::Format::Beautified);
		std::ofstream twitterFile("twitter.1.json", std::ios_base::binary | std::ios_base::out);
		twitter.Write(twitterFile, Node::Format::Beautified);

		auto length = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		std::cout << "Write: " << length.count() << "ms\n";
#endif
	}
	{
		auto start = std::chrono::high_resolution_clock::now();

		auto canadaString = canada.Write();
		auto catalogString = catalog.Write();
		auto twitterString = twitter.Write();

		auto length = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		std::cout << "Stringify: " << length.count() << "ms\n";
	}
	{
		auto start = std::chrono::high_resolution_clock::now();

		auto canadaString = canada.Write(Node::Format::Beautified);
		auto catalogString = catalog.Write(Node::Format::Beautified);
		auto twitterString = twitter.Write(Node::Format::Beautified);

		auto length = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		std::cout << "Prettify: " << length.count() << "ms\n";
	}
	std::cout << "Code size: 967 lines\n";
#endif

	test::Example1 example1;
	Node node;
	node = example1;

	// Appends different types into a array.
	node["array1"]->Append("Hello", nullptr, 10, 4.8924f);

	// Creates a array, then appends values to the back of the array.
	node["array2"] = std::vector{1.0f, 10.0f, -5.55f, 9.3456f};
	node["array2"]->Append(64, 32.1f, -2.0);
	//node["array2"].SetName("array2_renamed");
	//auto array2Name{node["array2"].GetName()};
	//auto array2{node["array2"].Get<std::vector<float>>()};

	auto timeNow = node["timeNow"].Get<int64_t>(123456); // 123456
	node.RemoveProperty("timeNow");

	auto data00 = node["xml"]["data"][0][0].Get<std::string>(); // "clunky"
	auto data10 = node["xml"]["data"][1][0].Get<std::string>(); // "uses more words than necessary"

	auto mapN2 = node["map"]["-2"].Get<std::string>(); // TODO: Can names be numbers without searching with keys?
	auto map400 = node["map"]["400"].Get<std::string>(); // TODO: Can names be numbers without searching with keys?

	if (auto mapN2_ = node["map"]["-2"]) {
		auto value = mapN2_.Get<std::string>();
	}

	node["users"][0] = test::User{"user1", "User One", "C++ developer", false, "10/07/2000"};
	node["users"][1] = test::User{"user2", "User Two", "University student", true, "11/05/1999"};
	node["users"][3] = test::User{"user3", "User Three", "Website developer", false, "2/03/1996"};
	node["users"][6] = test::User{"user4", "User Four", "High school student", true, "30/04/2002"};
	//node["users"][7] = test::User{"köln", "'Etat de São Paulo", R"(\"Hello World\")", true, "01/00/2000"};
	auto users = node["users"].Get<std::vector<std::optional<test::User>>>();

	Json json1(std::move(node));

	Json json2;
	json2.Load(json1.Write(Node::Format::Minified));
	//json2.Write(std::cout, Node::Format::Beautified);
	{
		std::ofstream testStream("Test.json");
		json2.Write(testStream);
	}

	/*Yaml yaml2(node);
	//yaml2.Write(std::cout, Node::Format::Beautified);
	yaml2.Write(std::ofstream("Test.yml"));*/

	//std::cout << "\nPress enter to continue...";
	//std::cin.get();
	return 0;
}
