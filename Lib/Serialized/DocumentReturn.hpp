#pragma once

#include "StdAfx.hpp"

namespace acid {
class Document;

/**
 * @brief Class that is returned from a {@link Document} when getting properties. This represents a key tree from a parent,
 * this allows reads of large trees with broken nodes to not need to generate new content.
 */
class ACID_EXPORT DocumentReturn {
public:
	DocumentReturn() = default;
	DocumentReturn(Document const *parent, std::variant<std::string, int32_t> key, Document const *value);
	DocumentReturn(DocumentReturn *parent, std::variant<std::string, int32_t> key);

	bool has_value() const noexcept;
	Document *get();

	explicit operator bool() const noexcept { return has_value(); }
	operator Document &() { return *get(); }

	Document &operator*() { return *get(); }
	Document *operator->() { return get(); }

	template<typename T>
	T Get();
	template<typename T>
	T Get(const T &fallback);
	template<typename T>
	void Get(T &dest);
	template<typename T, typename K>
	void Get(T &dest, const K &fallback);
	template<typename T>
	void Set(const T &value);

	DocumentReturn operator[](const std::string &key);
	DocumentReturn operator[](uint32_t index);

	template<typename T>
	Document &operator=(const T &rhs);

	std::string GetName() const;
	void SetName(const std::string &name);

private:
	Document *m_parent = nullptr;
	std::vector<std::variant<std::string, int32_t>> m_keys;
	Document *m_value = nullptr;
};
}
