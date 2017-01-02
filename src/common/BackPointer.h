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

#include <cassert>

#include <memory>
#include <vector>
#include <iostream>

/*
ForwardPointer<T> behaves just like std::unique_ptr<T>, except the object being pointed to also points back to the smart
pointer. This makes certain operations easier, e.g. removing a std::unique_ptr<T> from a vector when you only know the
regular pointer.

The class being pointed to (T) must derive from BackPointer<T> to make this work.
*/

template<typename T>
class ForwardPointer;

template<typename T>
class BackPointer {

private:
	ForwardPointer<T> *m_ptr;

public:
	inline BackPointer() : m_ptr(NULL) {}

	inline ForwardPointer<T>* GetBackPointer() const noexcept { return m_ptr; }
	inline void SetBackPointerInternal(ForwardPointer<T> *ptr) noexcept { m_ptr = ptr; }

	// noncopyable
	BackPointer(const BackPointer&) = delete;
	BackPointer& operator=(const BackPointer&) = delete;

};

template<typename T>
class ForwardPointer {

public:
	std::unique_ptr<T> m_ptr;

public:
	inline ForwardPointer() noexcept {}
	inline ForwardPointer(std::nullptr_t) noexcept {}

	// only for construction
	inline explicit ForwardPointer(T *ptr) noexcept : m_ptr(ptr) {
		if(m_ptr)
			m_ptr->SetBackPointerInternal(this);
	}

	inline ForwardPointer(ForwardPointer &&other) noexcept {
		if(other.m_ptr)
			other.m_ptr->SetBackPointerInternal(NULL);
		m_ptr = std::move(other.m_ptr);
		if(m_ptr)
			m_ptr->SetBackPointerInternal(this);
	}

	// noncopyable (move only)
	ForwardPointer(const ForwardPointer&) = delete;
	ForwardPointer& operator=(const ForwardPointer&) = delete;

	inline ~ForwardPointer() {
		if(m_ptr)
			m_ptr->SetBackPointerInternal(NULL);
	}

	inline ForwardPointer& operator=(ForwardPointer &&ptr) noexcept {
		// self-assignment ok
		if(m_ptr)
			m_ptr->SetBackPointerInternal(NULL);
		m_ptr = std::move(ptr.m_ptr);
		if(m_ptr)
			m_ptr->SetBackPointerInternal(this);
		return *this;
	}

	inline ForwardPointer& operator=(std::nullptr_t) noexcept {
		if(m_ptr)
			m_ptr->SetBackPointerInternal(NULL);
		m_ptr = nullptr;
		return *this;
	}

	inline T* Get() const noexcept {
		return m_ptr.get();
	}

	inline operator bool() const noexcept {
		return m_ptr;
	}

	inline T* Release() noexcept {
		if(m_ptr)
			m_ptr->SetBackPointerInternal(NULL);
		return m_ptr.release();
	}

	inline void Reset(T* ptr = NULL) noexcept {
		if(m_ptr)
			m_ptr->SetBackPointerInternal(NULL);
		m_ptr.reset(ptr);
		if(m_ptr)
			m_ptr->SetBackPointerInternal(this);
	}

	inline void Swap(ForwardPointer& other) noexcept {
		// self-assignment ok
		if(m_ptr)
			m_ptr->SetBackPointerInternal(NULL);
		if(other.m_ptr)
			other.m_ptr->SetBackPointerInternal(NULL);
		m_ptr.swap(other.m_ptr);
		if(m_ptr)
			m_ptr->SetBackPointerInternal(this);
		if(other.m_ptr)
			other.m_ptr->SetBackPointerInternal(std::addressof(other));
	}

	inline T& operator*() const {
		return *m_ptr;
	}

	inline T* operator->() const {
		return m_ptr.get();
	}

	friend inline bool operator==(const ForwardPointer<T> &a, const ForwardPointer<T> &b) noexcept { return a.m_ptr == b.m_ptr; }
	friend inline bool operator==(const ForwardPointer<T> &a, std::nullptr_t            ) noexcept { return a.m_ptr == nullptr; }
	friend inline bool operator==(std::nullptr_t            , const ForwardPointer<T> &b) noexcept { return nullptr == b.m_ptr; }
	friend inline bool operator!=(const ForwardPointer<T> &a, const ForwardPointer<T> &b) noexcept { return a.m_ptr != b.m_ptr; }
	friend inline bool operator!=(const ForwardPointer<T> &a, std::nullptr_t            ) noexcept { return a.m_ptr != nullptr; }
	friend inline bool operator!=(std::nullptr_t            , const ForwardPointer<T> &b) noexcept { return nullptr != b.m_ptr; }
	friend inline bool operator< (const ForwardPointer<T> &a, const ForwardPointer<T> &b) noexcept { return a.m_ptr <  b.m_ptr; }
	friend inline bool operator< (const ForwardPointer<T> &a, std::nullptr_t            ) noexcept { return a.m_ptr <  nullptr; }
	friend inline bool operator< (std::nullptr_t            , const ForwardPointer<T> &b) noexcept { return nullptr <  b.m_ptr; }
	friend inline bool operator<=(const ForwardPointer<T> &a, const ForwardPointer<T> &b) noexcept { return a.m_ptr <= b.m_ptr; }
	friend inline bool operator<=(const ForwardPointer<T> &a, std::nullptr_t            ) noexcept { return a.m_ptr <= nullptr; }
	friend inline bool operator<=(std::nullptr_t            , const ForwardPointer<T> &b) noexcept { return nullptr <= b.m_ptr; }
	friend inline bool operator> (const ForwardPointer<T> &a, const ForwardPointer<T> &b) noexcept { return a.m_ptr >  b.m_ptr; }
	friend inline bool operator> (const ForwardPointer<T> &a, std::nullptr_t            ) noexcept { return a.m_ptr >  nullptr; }
	friend inline bool operator> (std::nullptr_t            , const ForwardPointer<T> &b) noexcept { return nullptr >  b.m_ptr; }
	friend inline bool operator>=(const ForwardPointer<T> &a, const ForwardPointer<T> &b) noexcept { return a.m_ptr >= b.m_ptr; }
	friend inline bool operator>=(const ForwardPointer<T> &a, std::nullptr_t            ) noexcept { return a.m_ptr >= nullptr; }
	friend inline bool operator>=(std::nullptr_t            , const ForwardPointer<T> &b) noexcept { return nullptr >= b.m_ptr; }

};

namespace std {
template<typename T>
void swap(ForwardPointer<T> &a, ForwardPointer<T> &b) noexcept {
	a.Swap(b);
}
}

template<typename T>
bool IsInVector(const std::vector<ForwardPointer<T>> &vec, T *ptr) {
	size_t index = ptr->GetBackPointer() - vec.data();
	return index < vec.size();
}

template<typename T>
size_t IndexInVector(const std::vector<ForwardPointer<T>> &vec, T *ptr) {
	size_t index = ptr->GetBackPointer() - vec.data();
	assert(index < vec.size()); // if this fails, it means that the object is not in this vector
	return index;
}

template<typename T>
void DeleteFromVector(std::vector<ForwardPointer<T>> &vec, size_t index) {
	if(index != vec.size() - 1)
		vec[index] = std::move(vec.back());
	vec.pop_back();
}

template<typename T>
size_t DeleteFromVector(std::vector<ForwardPointer<T>> &vec, T *ptr) {
	size_t index = IndexInVector(vec, ptr);
	DeleteFromVector(vec, index);
	return index;
}

template<typename T>
ForwardPointer<T> MoveFromVector(std::vector<ForwardPointer<T>> &vec, T *ptr) {
	size_t index = IndexInVector(vec, ptr);
	ForwardPointer<T> object(std::move(vec[index]));
	vec.erase(vec.begin() + index);
	return object;
}

template<typename T>
void MoveInVector(std::vector<ForwardPointer<T>> &vec, size_t current_index, size_t target_index) {
	assert(current_index < vec.size());
	assert(target_index < vec.size());
	while(current_index < target_index) {
		std::swap(vec[current_index], vec[current_index + 1]);
		++current_index;
	}
	while(current_index > target_index){
		std::swap(vec[current_index], vec[current_index - 1]);
		--current_index;
	}
}

template<typename T>
void BulkInsertIntoVector(std::vector<ForwardPointer<T>> &vec, std::vector<ForwardPointer<T>> &ins, size_t target_index) {
	size_t space_before = 0, space_after = 0;
	for(size_t i = 0; i < target_index; ++i) {
		if(vec[i] == nullptr) {
			++space_before;
		} else if(space_before != 0) {
			vec[i - space_before] = std::move(vec[i]);
		}
	}
	for(size_t i = vec.size(); i > target_index; ) {
		--i;
		if(vec[i] == nullptr) {
			++space_after;
		} else if(space_after != 0) {
			vec[i + space_after] = std::move(vec[i]);
		}
	}
	assert(space_before + space_after == ins.size());
	for(size_t i = 0; i < ins.size(); ++i) {
		vec[target_index - space_before + i] = std::move(ins[i]);
	}
}
