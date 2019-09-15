#include "Json.hpp"

#include "Helpers/String.hpp"

// TODO: Fix loading and writing chars like /n, /", ... \"http://google.com\"

namespace acid {
Json::Json(const Node &node) :
	Node(node) {
	SetType(Type::Object);
}

Json::Json(Node &&node) :
	Node(std::move(node)) {
	SetType(Type::Object);
}

void Json::AddToken(std::vector<std::pair<Type, std::string>> &tokens, std::string &current) {
	if (!current.empty()) {
		// Finds the node value type of the string and adds it to the tokens vector.
		if (String::IsNumber(current)) {
			tokens.emplace_back(Type::Number, current);
		} else if (current == "null") {
			tokens.emplace_back(Type::Null, current);
		} else if (current == "true" || current == "false") {
			tokens.emplace_back(Type::Boolean, current);
		} else {
			// if (current.front() == current.back() == '\"')
			tokens.emplace_back(Type::String, current.substr(1, current.size() - 2));
		}
	}

	// Clears the current summation stream.
	current.clear();
}

void Json::Convert(Node &current, const std::vector<std::pair<Type, std::string>> &v, int32_t i, int32_t &r) {
	if (v[i].second == "{") {
		auto k{i + 1};

		while (v[k].second != "}") {
			auto key{v[k].second};
			k += 2; // k + 1 should be ':'
			Convert(current.AddProperty(key, {}), v, k, k);

			if (v[k].second == ",") {
				k++;
			}
		}

		current.SetType(Type::Object);
		r = k + 1;
	} else if (v[i].second == "[") {
		auto k{i + 1};

		while (v[k].second != "]") {
			Convert(current.AddProperty(), v, k, k);

			if (v[k].second == ",") {
				k++;
			}
		}

		current.SetType(Type::Array);
		r = k + 1;
	} else {
		current.SetValue(String::UnfixReturnTokens(v[i].second));
		current.SetType(v[i].first);
		r = i + 1;
	}
}
}
