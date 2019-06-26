#include <iostream>

#include "Serialized/Node.hpp"

using namespace acid;

namespace test
{
class Example1
{
public:
	class XML
	{
	public:
		std::vector<std::vector<std::string>> data{{"clunky"}, {"uses more words than necessary"}};
		std::optional<float> optional0{};
		std::optional<std::string> optional1{"Hello optional string!"};

		friend const Node &operator>>(const Node &node, XML &xml)
		{
			xml.data = node["data"].As<decltype(data)>();
			xml.optional0 = node["optional0"].As<decltype(optional0)>(std::nullopt);
			xml.optional1 = node["optional1"].As<decltype(optional1)>(std::nullopt);
			return node;
		}

		friend Node &operator<<(Node &node, const XML &xml)
		{
			node["data"] = xml.data;
			node["optional0"] = xml.optional0;
			node["optional1"] = xml.optional1;
			return node;
		}
	} xml;

	class Objects
	{
	public:
		std::string key{"value"};
		std::vector<float> values{190.0f, 11.0f, -0.001f};

		friend const Node &operator>>(const Node &node, Objects &objects)
		{
			objects.key = node["key"].As<decltype(key)>("key not found");
			objects.values = node["values"].As<decltype(values)>();
			return node;
		}

		friend Node &operator<<(Node &node, const Objects &objects)
		{
			node["key"] = objects.key;
			node["values"] = objects.values;
			return node;
		}
	} objects;

	std::string paragraph{"Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"};
	std::unique_ptr<std::string> content{std::make_unique<std::string>("Ut enim ad minim veniam,\nquis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.")};

	std::filesystem::path currentPath{std::filesystem::current_path()};
	std::vector<std::string> json{"rigid", "better for data interchange"};
	std::vector<std::string> yaml{"slim and flexible", "better for configuration", "supports comments"};
	std::map<int32_t, std::string> map{{10, "Hello World"}, {-2, "Negative Keys"}, {400, "Larger Key"}};
	std::map<int32_t, std::vector<std::string>> vectorMap{{-1, {"A", "B", "C"}}, {8, {"1", "2.00", "3.00"}}, {700, {"%", "$", "#", "&", "#"}}};

	friend const Node &operator>>(const Node &node, Example1 &example1)
	{
		example1.currentPath = node["currentPath"].As<decltype(currentPath)>();
		example1.paragraph = node["paragraph"].As<decltype(paragraph)>();
		example1.content = node["content"].As<decltype(content)>();
		example1.xml = node["xml"].As<decltype(xml)>();
		example1.json = node["json"].As<decltype(json)>();
		example1.yaml = node["yaml"].As<decltype(yaml)>();
		example1.map = node["map"].As<decltype(map)>();
		example1.vectorMap = node["vectorMap"].As<decltype(vectorMap)>();
		example1.objects = node["objects"].As<decltype(objects)>();
		return node;
	}

	friend Node &operator<<(Node &node, const Example1 &example1)
	{
		node["currentPath"] = example1.currentPath;
		node["paragraph"] = example1.paragraph;
		node["content"] = example1.content;
		node["xml"] = example1.xml;
		node["json"] = example1.json;
		node["yaml"] = example1.yaml;
		node["map"] = example1.map;
		node["vectorMap"] = example1.vectorMap;
		node["objects"] = example1.objects;
		return node;
	}
};
}

int main(int argc, char **argv)
{
	test::Example1 example1;
	Node node;
	node << example1;

	node["array1"].Append("Hello", nullptr, 10, 4.8924f);

	// Creates a array, then appends values to the back of the array.
	node["array2"] = std::vector{1.0f, 10.0f, -5.55f, 9.3456f};
	node["array2"].Append(64, 32.1f, -2.0);
	//node["array2"].SetName("array2_renamed");
	//auto array2Name{node["array2"].GetName()};
	//auto array2{node["array2"].As<std::vector<float>>()};

	auto timeNow2{node["timeNow2"].As<int64_t>(123456)}; // 123456
	node.RemoveProperty("timeNow2");

	auto timeNow{node["timeNow"].As<int64_t>()}; // 195998
	auto data00{node["xml"]["data"][0][0].As<std::string>()}; // "clunky"
	auto data10{node["xml"]["data"][1][0].As<std::string>()}; // "uses more words than necessary"

	auto mapN2{node["map"]["-2"].As<std::string>()}; // TODO: Can names be numbers without searching with keys?
	auto map400{node["map"]["400"].As<std::string>()}; // TODO: Can names be numbers without searching with keys?


	std::cout << "Press enter to continue...";
	std::cin.get();
	return 0;
}
