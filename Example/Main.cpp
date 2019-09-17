#include <iostream>

#include <fstream>
#include <unordered_map>
#include <Serialized/Document.hpp>
#include <Serialized/Json/Json.hpp>

using namespace acid;

namespace test {
class Example1 {
public:
	class XML {
	public:
		std::vector<std::vector<std::string>> data = {{"clunky"}, {"uses more words than necessary"}};
		std::optional<float> optional0;
		std::optional<std::string> optional1 = "Hello optional string!";

		friend const Document &operator>>(const Document &document, XML &xml) {
			document["data"].Get(xml.data);
			document["optional0"].Get(xml.optional0);
			document["optional1"].Get(xml.optional1);
			return document;
		}

		friend Document &operator<<(Document &document, const XML &xml) {
			document["data"].Set(xml.data);
			document["optional0"].Set(xml.optional0);
			document["optional1"].Set(xml.optional1);
			return document;
		}
	} xml;

	class Objects {
	public:
		std::string url = R"(<a href=\"http://twitter.com/download/iphone\" rel=\"nofollow\">Twitter for iPhone</a>)";
		std::string key = "value";
		std::vector<float> values = {190.0f, 11.0f, -0.001f};

		friend const Document &operator>>(const Document &document, Objects &objects) {
			document["url"].Get(objects.url);
			document["key"].Get(objects.key, "failed to get key");
			document["values"].Get(objects.values);
			return document;
		}

		friend Document &operator<<(Document &document, const Objects &objects) {
			document["url"].Set(objects.url);
			document["key"].Set(objects.key);
			document["values"].Set(objects.values);
			return document;
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

	friend const Document &operator>>(const Document &document, Example1 &example1) {
		document["currentPath"].Get(example1.currentPath);
		document["paragraph"].Get(example1.paragraph);
		//document["google"].Get(example1.google);
		document["content"].Get(example1.content);
		document["xml"].Get(example1.xml);
		document["json"].Get(example1.json);
		document["yaml"].Get(example1.yaml);
		document["map"].Get(example1.map);
		document["vectorMap"].Get(example1.vectorMap);
		document["objects"].Get(example1.objects);
		return document;
	}

	friend Document &operator<<(Document &document, const Example1 &example1) {
		document["currentPath"].Set(example1.currentPath);
		document["paragraph"].Set(example1.paragraph);
		//document["google"].Set(example1.google);
		document["content"].Set(example1.content);
		document["xml"].Set(example1.xml);
		document["json"].Set(example1.json);
		document["yaml"].Set(example1.yaml);
		document["map"].Set(example1.map);
		document["vectorMap"].Set(example1.vectorMap);
		document["objects"].Set(example1.objects);
		return document;
	}
};

class User {
public:
	std::string username;
	std::string fullname;
	std::string description;
	bool employed = false;
	std::string birthday;

	friend const Document &operator>>(const Document &document, User &user) {
		document["username"].Get(user.username);
		document["fullname"].Get(user.fullname);
		document["description"].Get(user.description);
		document["employed"].Get(user.employed);
		document["birthday"].Get(user.birthday);
		return document;
	}

	friend Document &operator<<(Document &document, const User &user) {
		document["username"].Set(user.username);
		document["fullname"].Set(user.fullname);
		document["description"].Set(user.description);
		document["employed"].Set(user.employed);
		document["birthday"].Set(user.birthday);
		return document;
	}
};
}

int main(int argc, char **argv) {
	test::Example1 example1;
	Document document;
	document = example1;

	// Appends different types into a array.
	document["array1"]->Append("Hello", nullptr, 10, 4.8924f);

	// Creates a array, then appends values to the back of the array.
	document["array2"] = std::vector{1.0f, 10.0f, -5.55f, 9.3456f};
	document["array2"]->Append(64, 32.1f, -2.0);
	//document["array2"].SetName("array2_renamed");
	//auto array2Name{document["array2"].GetName()};
	//auto array2{document["array2"].Get<std::vector<float>>()};

	auto timeNow = document["timeNow"].Get<int64_t>(123456); // 123456
	document.RemoveProperty("timeNow");

	auto data00 = document["xml"]["data"][0][0].Get<std::string>(); // "clunky"
	auto data10 = document["xml"]["data"][1][0].Get<std::string>(); // "uses more words than necessary"

	auto mapN2 = document["map"]["-2"].Get<std::string>(); // TODO: Can names be numbers without searching with keys?
	auto map400 = document["map"]["400"].Get<std::string>(); // TODO: Can names be numbers without searching with keys?

	if (auto mapN2_ = document["map"]["-2"]) {
		auto value = mapN2_.Get<std::string>();
	}

	document["users"][0] = test::User{"user1", "User One", "C++ developer", false, "10/07/2000"};
	document["users"][1] = test::User{"user2", "User Two", "University student", true, "11/05/1999"};
	document["users"][3] = test::User{"user3", "User Three", "Website developer", false, "2/03/1996"};
	document["users"][6] = test::User{"user4", "User Four", "High school student", true, "30/04/2002"};
	//document["users"][7] = test::User{"köln", "'Etat de São Paulo", R"(\"Hello World\")", true, "01/00/2000"};
	auto users = document["users"].Get<std::vector<std::optional<test::User>>>();

	std::ofstream testStream("Test.json");
	Json<char>().WriteStream(document, testStream, Document::Format::Beautified);
	
	/*Yaml yaml2(document);
	//yaml2.Write(std::cout, Document::Format::Beautified);
	yaml2.Write(std::ofstream("Test.yml"));*/

	//std::cout << "\nPress enter to continue...";
	//std::cin.get();
	return 0;
}
