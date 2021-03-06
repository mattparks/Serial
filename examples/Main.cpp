#include <iostream>

#include <fstream>
#include <unordered_map>
#include <Node.hpp>
#include <Json/Json.hpp>
#include <Xml/Xml.hpp>

using namespace serial;

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
		std::string url = R"(<a href=\"http://twitter.com/download/iphone\" rel=\"nofollow\">Twitter for iPhone</a>)";
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

	std::string paragraph = "\tLorem ipsum dolor sit amet,\nconsectetur adipiscing elit,\nsed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n";
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

int main(int argc, char **argv) {
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
	Node user4;
	user4 = test::User{"user4", "User Four", "High school student", true, "30/04/2002"};
	node["users"][6] = std::move(user4);
	//node["users"][7] = test::User{"k�ln", "'Etat de S�o Paulo", R"(\"Hello World\")", true, "01/00/2000"};

	// TODO: Resizing a vector changes the internal addressed of each element. 
	// NodeView holds a pointer and will not be aware that the value it holds has been reallocated.
	// In this instance the left side of the assignment will invalidate the right without a resize before assignment.
	//node["users"]->AddProperty(8); 
	//node["users"][8] = node["users"][6];

	Node numbers;
	numbers["a"] = std::vector{1, 2, 3, 4};
	numbers["b"] = std::vector{5, 6, 7, 8};
	numbers["c"] = std::vector{9, 10, 11, 12};
	//numbers["d"] = numbers["a"];
	node["numbers"] = std::move(numbers);
	
	auto users = node["users"].Get<std::vector<std::optional<test::User>>>();

	std::filesystem::create_directory("Example");
	
	{
		// Make a copy of the node.
		Node json1 = node;
		//Json::WriteStream(json1, std::cout);
	
		// Test Json writer.
		std::ofstream outStream1("Example/Test1.json");
		json1.WriteStream<Json>(outStream1, NodeFormat::Beautified);
		outStream1.close();

		// Test Json reader.
		std::ifstream inStream1("Example/Test1.json");
		Node json2;
		json2.ParseStream<Json>(inStream1);
		inStream1.close();
		
		// Ensure Test1.json and Test2.json values are the same (ignore order changes).
		std::ofstream outStream2("Example/Test2.json");
		json2.WriteStream<Json>(outStream2, NodeFormat::Beautified);
		outStream2.close();
	}
	{
		// Make a copy of the node.
		Node xml1("node", node);

		// Test Xml writer.
		std::ofstream outStream1("Example/Xml1.xml");
		xml1.WriteStream<Xml>(outStream1, NodeFormat::Beautified);
		outStream1.close();
	}

	/*node.WriteStream<Yaml>(std::cout, NodeFormat::Beautified);
	node.WriteStream<Yaml>(std::ofstream("Test.yml"));*/

	//std::cout << "\nPress enter to continue...";
	//std::cin.get();
	return 0;
}
