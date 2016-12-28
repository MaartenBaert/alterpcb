#pragma once

#include <cassert>

#include <algorithm>
#include <string>
#include <vector>

template<typename A, typename B>
class EnumTranslator {

private:
	static const EnumTranslator SINGLETON;

private:
	std::vector<std::pair<A, B>> m_to_second, m_to_first;

private:
	inline static bool CompareFirst(const std::pair<A, B> &a, const std::pair<A, B> &b) { return (a.first < b.first); }
	inline static bool CompareSecond(const std::pair<A, B> &a, const std::pair<A, B> &b) { return (a.second < b.second); }

public:
	EnumTranslator(std::initializer_list<std::pair<A, B>> list) {
		m_to_second = m_to_first = list;
		std::stable_sort(m_to_second.begin(), m_to_second.end(), CompareFirst);
		std::stable_sort(m_to_first.begin(), m_to_first.end(), CompareSecond);
	}
	inline static bool ToFirst(A &first, const B &second) {
		auto it = std::lower_bound(SINGLETON.m_to_first.begin(), SINGLETON.m_to_first.end(), std::make_pair(A(), second), CompareSecond);
		if(it == SINGLETON.m_to_first.end() || it->second != second)
			return false;
		first = it->first;
		return true;
	}
	inline static bool ToSecond(const A &first, B &second) {
		auto it = std::lower_bound(SINGLETON.m_to_second.begin(), SINGLETON.m_to_second.end(), std::make_pair(first, B()), CompareFirst);
		if(it == SINGLETON.m_to_second.end() || it->first != first)
			return false;
		second = it->second;
		return true;
	}

};

// convenience functions without full error checking
template<typename E>
inline std::string EnumToString(E value) {
	std::string string;
	bool success = EnumTranslator<E, std::string>::ToSecond(value, string);
	assert(success);
	return string;
}
template<typename E>
inline E StringToEnum(const std::string &string, E fallback) {
	E value = fallback;
	EnumTranslator<E, std::string>::ToFirst(value, string);
	return value;
}

#define ENUMTRANSLATOR(A, B) template<> const EnumTranslator<A, B> EnumTranslator<A, B>::SINGLETON
#define ENUMSTRINGS(E) ENUMTRANSLATOR(E, std::string)
