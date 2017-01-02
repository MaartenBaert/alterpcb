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

#include <memory>
#include <memdataset.h>
template<typename T>
class SafePointer;

template<typename T>
class SafeTarget {

private:
	SafePointer<T> *m_ptr;

public:
	inline SafeTarget() {
		m_ptr = NULL;
	}
	inline ~SafeTarget() {
		while(m_ptr != NULL)
			m_ptr->Reset();
	}

	inline SafePointer<T>* GetSafePointerInternal() const noexcept { return m_ptr; }
	inline void SetSafePointerInternal(SafePointer<T> *ptr) noexcept { m_ptr = ptr; }

	// noncopyable
	SafeTarget(const SafeTarget&) = delete;
	SafeTarget& operator=(const SafeTarget&) = delete;

};

template<typename T>
class SafePointer {

private:
	SafeTarget<T> *m_ptr;
	SafePointer *m_prev, *m_next;

public:
	inline SafePointer() noexcept {
		m_ptr = NULL;
		m_next = NULL;
	}
	inline SafePointer(T *ptr) noexcept {
		m_ptr = ptr;
		AddSelf();
	}
	inline SafePointer(const SafePointer &other) noexcept {
		m_ptr = other.m_ptr;
		AddSelf();
	}
	inline ~SafePointer() noexcept {
		RemoveSelf();
	}

	inline SafePointer& operator=(T *ptr) noexcept {
		RemoveSelf();
		m_ptr = ptr;
		AddSelf();
		return *this;
	}
	inline SafePointer& operator=(const SafePointer &other) noexcept {
		// self-assignment ok
		RemoveSelf();
		m_ptr = other.m_ptr;
		AddSelf();
		return *this;
	}

	inline T* Get() const noexcept {
		return static_cast<T*>(m_ptr);
	}

	inline operator bool() const noexcept {
		return m_ptr;
	}

	inline void Reset() noexcept {
		RemoveSelf();
		m_ptr = NULL;
	}

	inline void Swap(SafePointer& other) noexcept {
		if(std::addressof(other) == this)
			return;
		RemoveSelf();
		other.RemoveSelf();
		std::swap(m_ptr, other.m_ptr);
		AddSelf();
		other.AddSelf();
	}

	inline T& operator*() const {
		return *static_cast<T*>(m_ptr);
	}

	inline T* operator->() const {
		return static_cast<T*>(m_ptr);
	}

	friend inline bool operator==(const SafePointer<T> &a, const SafePointer<T> &b) noexcept { return a.m_ptr == b.m_ptr; }
	friend inline bool operator==(const SafePointer<T> &a, std::nullptr_t         ) noexcept { return a.m_ptr == nullptr; }
	friend inline bool operator==(std::nullptr_t         , const SafePointer<T> &b) noexcept { return nullptr == b.m_ptr; }
	friend inline bool operator!=(const SafePointer<T> &a, const SafePointer<T> &b) noexcept { return a.m_ptr != b.m_ptr; }
	friend inline bool operator!=(const SafePointer<T> &a, std::nullptr_t         ) noexcept { return a.m_ptr != nullptr; }
	friend inline bool operator!=(std::nullptr_t         , const SafePointer<T> &b) noexcept { return nullptr != b.m_ptr; }
	friend inline bool operator< (const SafePointer<T> &a, const SafePointer<T> &b) noexcept { return a.m_ptr <  b.m_ptr; }
	friend inline bool operator< (const SafePointer<T> &a, std::nullptr_t         ) noexcept { return a.m_ptr <  nullptr; }
	friend inline bool operator< (std::nullptr_t         , const SafePointer<T> &b) noexcept { return nullptr <  b.m_ptr; }
	friend inline bool operator<=(const SafePointer<T> &a, const SafePointer<T> &b) noexcept { return a.m_ptr <= b.m_ptr; }
	friend inline bool operator<=(const SafePointer<T> &a, std::nullptr_t         ) noexcept { return a.m_ptr <= nullptr; }
	friend inline bool operator<=(std::nullptr_t         , const SafePointer<T> &b) noexcept { return nullptr <= b.m_ptr; }
	friend inline bool operator> (const SafePointer<T> &a, const SafePointer<T> &b) noexcept { return a.m_ptr >  b.m_ptr; }
	friend inline bool operator> (const SafePointer<T> &a, std::nullptr_t         ) noexcept { return a.m_ptr >  nullptr; }
	friend inline bool operator> (std::nullptr_t         , const SafePointer<T> &b) noexcept { return nullptr >  b.m_ptr; }
	friend inline bool operator>=(const SafePointer<T> &a, const SafePointer<T> &b) noexcept { return a.m_ptr >= b.m_ptr; }
	friend inline bool operator>=(const SafePointer<T> &a, std::nullptr_t         ) noexcept { return a.m_ptr >= nullptr; }
	friend inline bool operator>=(std::nullptr_t         , const SafePointer<T> &b) noexcept { return nullptr >= b.m_ptr; }

private:
	inline void AddSelf() {
		if(m_ptr == NULL)
			return;
		SafePointer *head = m_ptr->GetSafePointerInternal();
		if(head == NULL) {
			m_prev = this;
			m_next = this;
			m_ptr->SetSafePointerInternal(this);
		} else {
			m_prev = head->m_prev;
			m_next = head;
			m_prev->m_next = this;
			m_next->m_prev = this;
		}
	}
	inline void RemoveSelf() {
		if(m_ptr == NULL)
			return;
		if(m_next == this) {
			m_ptr->SetSafePointerInternal(NULL);
		} else {
			m_prev->m_next = m_next;
			m_next->m_prev = m_prev;
			if(m_ptr->GetSafePointerInternal() == this)
				m_ptr->SetSafePointerInternal(m_next);
		}
		m_prev = NULL;
		m_next = NULL;
	}

};

namespace std {
template<typename T>
void swap(SafePointer<T> &a, SafePointer<T> &b) noexcept {
	a.Swap(b);
}
}
