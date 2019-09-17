#pragma once

#include "Document.hpp"

namespace acid {
template<typename T>
T DocumentReturn::Get() {
	if (!has_value()) {
		return {};
	}

	return m_value->Get<T>();
}

template<typename T>
T DocumentReturn::Get(const T &fallback) {
	if (!has_value()) {
		return {};
	}

	return m_value->Get<T>(fallback);
}

template<typename T>
void DocumentReturn::Get(T &dest) {
	if (!has_value()) {
		return;
	}

	return m_value->Get<T>(dest);
}

template<typename T, typename K>
void DocumentReturn::Get(T &dest, const K &fallback) {
	if (!has_value()) {
		return;
	}

	return m_value->Get<T>(dest, fallback);
}

template<typename T>
void DocumentReturn::Set(const T &value) {
	return get()->Set<T>(value);
}

template<typename T>
Document &DocumentReturn::operator=(const T &rhs) {
	return *get() = rhs;
}
}
