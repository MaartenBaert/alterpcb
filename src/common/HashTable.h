/*
Copyright (C) 2016  The AlterPCB team
Contact: Maarten Baert <maarten-baert@hotmail.com>

This file is part of AlterPCB.

AlterPCB is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AlterPCB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this AlterPCB.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Basics.h"
#include "MurmurHash.h"

#include <cstdint>

#include <utility>
#include <vector>

template<typename T, class Hasher>
class HashTable {

private:
	struct Bucket {
		index_t m_hashlink;
		index_t m_size;
		inline Bucket() {
			m_hashlink = INDEX_NONE;
			m_size = 0;
		}
	};
	struct Entry : public T {
		index_t m_hashlink;
		template<typename... Args>
		inline Entry(Args&&... args)
			: T(std::forward<Args>(args)...) {}
	};

private:
	constexpr static hash_t HASH_SEED1 = 0x5d02e5ec, HASH_SEED2 = 0xbfcb8a4e;

private:
	uint32_t m_bits;
	Hasher m_hasher;
	std::vector<Bucket> m_buckets;
	std::vector<Entry> m_data;

public:
	inline HashTable(index_t capacity = 16, const Hasher &hasher = Hasher())
		: m_bits(std::max<index_t>(4, CeilLog2(capacity))), m_hasher(hasher) {
		m_data.reserve(capacity);
		Rehash();
	}

	// default copy and assignment
	HashTable(const HashTable&) = default;
	HashTable(HashTable&&) = default;
	HashTable& operator=(const HashTable&) = default;
	HashTable& operator=(HashTable&&) = default;

	inline void Clear() {
		m_data.clear();
		Rehash();
	}

	void Reserve(index_t capacity) {
		m_data.reserve(capacity);
		if(capacity > (1 << m_bits)) {
			++m_bits;
			while(capacity > (1 << m_bits)) {
				++m_bits;
			}
			Rehash();
		}
	}

	inline std::pair<index_t, bool> TryPushBack(const T &value) {
		hash_t hash1 = Hash1(value);
		index_t res1 = FindAt(hash1, value);
		if(res1 != INDEX_NONE)
			return std::make_pair(res1, false);
		hash_t hash2 = Hash2(value);
		index_t res2 = FindAt(hash2, value);
		if(res2 != INDEX_NONE)
			return std::make_pair(res2, false);
		m_data.push_back(value);
		index_t res = m_data.size() - 1;
		AddHash(hash1, hash2, res);
		Resize();
		return std::make_pair(res, true);
	}
	inline std::pair<index_t, bool> TryPushBack(T &&value) {
		hash_t hash1 = Hash1(value);
		index_t res1 = FindAt(hash1, value);
		if(res1 != INDEX_NONE)
			return std::make_pair(res1, false);
		hash_t hash2 = Hash2(value);
		index_t res2 = FindAt(hash2, value);
		if(res2 != INDEX_NONE)
			return std::make_pair(res2, false);
		m_data.push_back(std::move(value));
		index_t res = m_data.size() - 1;
		AddHash(hash1, hash2, res);
		Resize();
		return std::make_pair(res, true);
	}

	template<typename K, typename... Args>
	inline std::pair<index_t, bool> TryEmplaceBack(const K &key, Args&&... args) {
		hash_t hash1 = Hash1(key);
		index_t res1 = FindAt(hash1, key);
		if(res1 != INDEX_NONE)
			return std::make_pair(res1, false);
		hash_t hash2 = Hash2(key);
		index_t res2 = FindAt(hash2, key);
		if(res2 != INDEX_NONE)
			return std::make_pair(res2, false);
		m_data.emplace_back(std::forward<Args>(args)...);
		index_t res = m_data.size() - 1;
		AddHash(hash1, hash2, res);
		Resize();
		return std::make_pair(res, true);
	}

	inline index_t PushBack(const T &value) {
		m_data.push_back(value);
		index_t res = m_data.size() - 1;
		AddHash(Hash1(m_data[res]), Hash2(m_data[res]), res);
		Resize();
		return res;
	}
	inline index_t PushBack(T &&value) {
		m_data.push_back(std::move(value));
		index_t res = m_data.size() - 1;
		AddHash(Hash1(m_data[res]), Hash2(m_data[res]), res);
		Resize();
		return res;
	}

	template<typename... Args>
	inline index_t EmplaceBack(Args&&... args) {
		m_data.emplace_back(std::forward<Args>(args)...);
		index_t res = m_data.size() - 1;
		AddHash(Hash1(m_data[res]), Hash2(m_data[res]), res);
		Resize();
		return res;
	}

	template<typename K>
	inline index_t Find(const K &key) const {
		index_t res1 = FindAt(Hash1(key), key);
		if(res1 != INDEX_NONE)
			return res1;
		index_t res2 = FindAt(Hash2(key), key);
		return res2;
	}

private:
	inline void AddHash(hash_t hash1, hash_t hash2, index_t index) {
		hash_t hash = (m_buckets[hash1].m_size <= m_buckets[hash2].m_size)? hash1 : hash2;
		m_data[index].m_hashlink = m_buckets[hash].m_hashlink;
		m_buckets[hash].m_hashlink = index;
		++m_buckets[hash].m_size;
	}

	template<typename K>
	inline index_t FindAt(hash_t hash, const K &key) const {
		for(index_t i = m_buckets[hash].m_hashlink; i != INDEX_NONE; i = m_data[i].m_hashlink) {
			if(m_data[i] == key)
				return i;
		}
		return INDEX_NONE;
	}

	inline void Resize() {
		if(m_data.size() > m_buckets.size()) {
			++m_bits;
			Rehash();
		}
	}

	inline void Rehash() {
		m_buckets.clear();
		m_buckets.resize(1 << m_bits);
		for(index_t i = 0; i < m_data.size(); ++i) {
			hash_t hash1 = Hash1(m_data[i]);
			hash_t hash2 = Hash2(m_data[i]);
			AddHash(hash1, hash2, i);
		}
	}

	template<typename K>
	inline hash_t Hash1(const K &key) const {
		hash_t hash = MurmurHash::HashFinish(m_hasher(HASH_SEED1, key));
		return MurmurHash::HashTruncate(hash, m_bits - 1);
	}

	template<typename K>
	inline hash_t Hash2(const K &key) const {
		hash_t hash = MurmurHash::HashFinish(m_hasher(HASH_SEED2, key));
		return MurmurHash::HashTruncate(hash, m_bits - 1) | (1 << (m_bits - 1));
	}

public:
	inline uint32_t GetBits() const {
		return m_bits;
	}
	inline index_t GetSize() const {
		return m_data.size();
	}
	inline T& operator[](index_t i) {
		assert(i < m_data.size());
		return m_data[i];
	}
	inline const T& operator[](index_t i) const {
		assert(i < m_data.size());
		return m_data[i];
	}

};
