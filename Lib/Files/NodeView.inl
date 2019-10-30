#pragma once

#include "Node.hpp"

namespace acid {
template<typename T>
T NodeView::Get() {
	if (!has_value()) {
		return {};
	}

	return m_value->Get<T>();
}

template<typename T>
T NodeView::Get(const T &fallback) {
	if (!has_value()) {
		return fallback;
	}

	return m_value->Get<T>(fallback);
}

template<typename T>
void NodeView::Get(T &dest) {
	if (!has_value()) {
		return;
	}

	return m_value->Get<T>(dest);
}

template<typename T, typename K>
void NodeView::Get(T &dest, const K &fallback) {
	if (!has_value()) {
		dest = fallback;
		return;
	}

	return m_value->Get<T>(dest, fallback);
}

template<typename T>
void NodeView::Set(const T &value) {
	return get()->Set<T>(value);
}

template<typename T>
Node &NodeView::operator=(const T &rhs) {
	return *get() = rhs;
}
}
