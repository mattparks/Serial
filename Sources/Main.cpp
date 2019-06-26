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
			node["data"]->Get(xml.data);
			node["optional0"]->Get(xml.optional0);
			node["optional1"]->Get(xml.optional1);
			return node;
		}

		friend Node &operator<<(Node &node, const XML &xml)
		{
			node["data"]->Set(xml.data);
			node["optional0"]->Set(xml.optional0);
			node["optional1"]->Set(xml.optional1);
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
			node["key"]->Get(objects.key, "failed to get key");
			node["values"]->Get(objects.values);
			return node;
		}

		friend Node &operator<<(Node &node, const Objects &objects)
		{
			node["key"]->Set(objects.key);
			node["values"]->Set(objects.values);
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
		node["currentPath"]->Get(example1.currentPath);
		node["paragraph"]->Get(example1.paragraph);
		node["content"]->Get(example1.content);
		node["xml"]->Get(example1.xml);
		node["json"]->Get(example1.json);
		node["yaml"]->Get(example1.yaml);
		node["map"]->Get(example1.map);
		node["vectorMap"]->Get(example1.vectorMap);
		node["objects"]->Get(example1.objects);
		return node;
	}

	friend Node &operator<<(Node &node, const Example1 &example1)
	{
		node["currentPath"]->Set(example1.currentPath);
		node["paragraph"]->Set(example1.paragraph);
		node["content"]->Set(example1.content);
		node["xml"]->Set(example1.xml);
		node["json"]->Set(example1.json);
		node["yaml"]->Set(example1.yaml);
		node["map"]->Set(example1.map);
		node["vectorMap"]->Set(example1.vectorMap);
		node["objects"]->Set(example1.objects);
		return node;
	}
};
}

int main(int argc, char **argv)
{
	test::Example1 example1;
	Node node;
	node << example1;

	if (auto map{node["map"]}; map)
	{
		auto mapN2{map["-2"]->Get<std::string>()};
	}

	auto tree0{node["map0"]["val1"]["username"]};

	node["array1"]->Append("Hello", nullptr, 10, 4.8924f);

	// Creates a array, then appends values to the back of the array.
	node["array2"] = std::vector{1.0f, 10.0f, -5.55f, 9.3456f};
	node["array2"]->Append(64, 32.1f, -2.0);
	//node["array2"].SetName("array2_renamed");
	//auto array2Name{node["array2"]->GetName()};
	//auto array2{node["array2"]->Get<std::vector<float>>()};

	auto timeNow{node["timeNow"]->Get<int64_t>(123456)}; // 123456
	node.RemoveProperty("timeNow");

	auto data00{node["xml"]["data"][0][0]->Get<std::string>()}; // "clunky"
	auto data10{node["xml"]["data"][1][0]->Get<std::string>()}; // "uses more words than necessary"

	auto mapN2{node["map"]["-2"]->Get<std::string>()}; // TODO: Can names be numbers without searching with keys?
	auto map400{node["map"]["400"]->Get<std::string>()}; // TODO: Can names be numbers without searching with keys?

	std::cout << "Press enter to continue...";
	std::cin.get();
	return 0;
}
