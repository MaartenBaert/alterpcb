#pragma once

#include "HashTable.h"

#include <string>

/*
AlterPCB uses short strings in lots of places, most importantly for parameters. String lookup is slow, so instead all
these strings are replaced with a string tag (stringtag_t). This is really just a number that identifies, nothing more.
These tags are tracked by StringRegistry.

Currently this is not thread-safe.
*/

class StringRegistryEntry {

private:
	std::string m_string;

public:
	inline StringRegistryEntry(const std::string &str) : m_string(str) {}
	inline StringRegistryEntry(std::string &&str) : m_string(std::move(str)) {}

	// default copy and assignment
	StringRegistryEntry(const StringRegistryEntry&) = default;
	StringRegistryEntry(StringRegistryEntry&&) = default;
	StringRegistryEntry& operator=(const StringRegistryEntry&) = default;
	StringRegistryEntry& operator=(StringRegistryEntry&&) = default;

	inline const std::string& GetString() const {
		return m_string;
	}

	inline bool operator==(const StringRegistryEntry &other) const {
		return m_string == other.m_string;
	}
	inline bool operator==(const std::string &other) const {
		return m_string == other;
	}
	inline bool operator==(const char *other) const {
		return m_string == other;
	}

};

struct StringRegistryHasher {
	inline hash_t operator()(hash_t hash, const StringRegistryEntry &value) const {
		return MurmurHash::HashData(hash, value.GetString().data(), value.GetString().size());
	}
	inline hash_t operator()(hash_t hash, const std::string &value) const {
		return MurmurHash::HashData(hash, value.data(), value.size());
	}
	inline hash_t operator()(hash_t hash, const char *value) const {
		return MurmurHash::HashData(hash, value, strlen(value));
	}
};

class StringRegistry {

private:
	static StringRegistry *s_instance;

private:
	HashTable<StringRegistryEntry, StringRegistryHasher> m_entries;

public:
	inline StringRegistry() : m_entries(1024) {
		s_instance = this;
	}
	inline ~StringRegistry() {
		s_instance = NULL;
	}

	// noncopyable
	StringRegistry(const StringRegistry&) = delete;
	StringRegistry& operator=(const StringRegistry&) = delete;

	// Converts a string to a tag, adding the string to the registry if necessary.
	inline static stringtag_t NewTag(const std::string &str) {
		return s_instance->m_entries.TryEmplaceBack(str, str).first;
	}
	inline static stringtag_t NewTag(std::string &&str) {
		return s_instance->m_entries.TryEmplaceBack(str, std::move(str)).first;
	}
	inline static stringtag_t NewTag(const char *str) {
		return s_instance->m_entries.TryEmplaceBack(str, str).first;
	}

	// Converts a string to a tag, but if the string is not already registered, the string is not added and instead the
	// function returns STRINGTAG_NONE. Useful for things like key lookup.
	inline static stringtag_t FindTag(const std::string &str) {
		return s_instance->m_entries.Find(str);
	}
	inline static stringtag_t FindTag(const char *str) {
		return s_instance->m_entries.Find(str);
	}

	// Converts a tag back to a string. the tag *must* be valid, there is no error checking.
	inline static const std::string& GetString(stringtag_t tag) {
		return s_instance->m_entries[tag].GetString();
	}

};
