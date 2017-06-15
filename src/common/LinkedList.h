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

/*
A basic intrusive double-linked list with minimal overhead, based on sentinels.
Elements which are not part of any list are linked to themselves.
*/

template<typename T>
struct LinkedListHook {
	LinkedListHook *m_prev, *m_next;
};

template<typename T>
class LinkedListElement;

template<typename T>
class LinkedList : private LinkedListHook<T> {

	friend class LinkedListElement<T>;

public:
	inline LinkedList() noexcept {
		LinkedListHook<T>::m_prev = this;
		LinkedListHook<T>::m_next = this;
	}
	inline ~LinkedList() noexcept {
		assert(LinkedListHook<T>::m_prev == this);
		assert(LinkedListHook<T>::m_next == this);
	}

	// noncopyable (move only)
	LinkedList(const LinkedList&) = delete;
	LinkedList& operator=(const LinkedList&) = delete;

	// move
	inline LinkedList(LinkedList &&other) noexcept {
		if(&other == other.LinkedListHook<T>::m_prev) {
			LinkedListHook<T>::m_prev = this;
			LinkedListHook<T>::m_next = this;
		} else {
			LinkedListHook<T>::m_prev = other.LinkedListHook<T>::m_prev;
			LinkedListHook<T>::m_next = other.LinkedListHook<T>::m_next;
			LinkedListHook<T>::m_prev->m_next = this;
			LinkedListHook<T>::m_next->m_prev = this;
			other.LinkedListHook<T>::m_prev = &other;
			other.LinkedListHook<T>::m_next = &other;
		}
	}
	LinkedList& operator=(LinkedList &&other) noexcept {
		if(this != &other) {
			assert(LinkedListHook<T>::m_prev == this);
			assert(LinkedListHook<T>::m_next == this);
			if(&other == other.LinkedListHook<T>::m_prev) {
				LinkedListHook<T>::m_prev = this;
				LinkedListHook<T>::m_next = this;
			} else {
				LinkedListHook<T>::m_prev = other.LinkedListHook<T>::m_prev;
				LinkedListHook<T>::m_next = other.LinkedListHook<T>::m_next;
				LinkedListHook<T>::m_prev->m_next = this;
				LinkedListHook<T>::m_next->m_prev = this;
				other.LinkedListHook<T>::m_prev = &other;
				other.LinkedListHook<T>::m_next = &other;
			}
		}
	}

	inline LinkedListHook<T>* GetSentinel() noexcept { return this; }
	inline T* GetFront() noexcept { return static_cast<T*>(LinkedListHook<T>::m_next); }
	inline T* GetBack() noexcept { return static_cast<T*>(LinkedListHook<T>::m_prev); }

};

template<typename T>
class LinkedListElement : public LinkedListHook<T> {

	friend class LinkedList<T>;

public:
	inline LinkedListElement() {
		LinkedListHook<T>::m_prev = this;
		LinkedListHook<T>::m_next = this;
	}
	inline ~LinkedListElement() {
		LinkedListHook<T>::m_prev->m_next = LinkedListHook<T>::m_next;
		LinkedListHook<T>::m_next->m_prev = LinkedListHook<T>::m_prev;
	}

	// noncopyable (move only)
	LinkedListElement(const LinkedListElement&) = delete;
	LinkedListElement& operator=(const LinkedListElement&) = delete;

	// move
	inline LinkedListElement(LinkedListElement &&other) noexcept {
		if(&other == other.LinkedListHook<T>::m_prev) {
			LinkedListHook<T>::m_prev = this;
			LinkedListHook<T>::m_next = this;
		} else {
			LinkedListHook<T>::m_prev = other.LinkedListHook<T>::m_prev;
			LinkedListHook<T>::m_next = other.LinkedListHook<T>::m_next;
			LinkedListHook<T>::m_prev->m_next = this;
			LinkedListHook<T>::m_next->m_prev = this;
			other.LinkedListHook<T>::m_prev = &other;
			other.LinkedListHook<T>::m_next = &other;
		}
	}
	LinkedListElement& operator=(LinkedListElement &&other) noexcept {
		if(this != &other) {
			LinkedListHook<T>::m_prev->m_next = LinkedListHook<T>::m_next;
			LinkedListHook<T>::m_next->m_prev = LinkedListHook<T>::m_prev;
			if(&other == other.LinkedListHook<T>::m_prev) {
				LinkedListHook<T>::m_prev = this;
				LinkedListHook<T>::m_next = this;
			} else {
				LinkedListHook<T>::m_prev = other.LinkedListHook<T>::m_prev;
				LinkedListHook<T>::m_next = other.LinkedListHook<T>::m_next;
				LinkedListHook<T>::m_prev->m_next = this;
				LinkedListHook<T>::m_next->m_prev = this;
				other.LinkedListHook<T>::m_prev = &other;
				other.LinkedListHook<T>::m_next = &other;
			}
		}
	}

	inline void InsertFront(LinkedList<T> *list) noexcept {
		InsertBetween(list, list->LinkedListHook<T>::m_next);
	}
	inline void InsertBack(LinkedList<T> *list) noexcept {
		InsertBetween(list->LinkedListHook<T>::m_prev, list);
	}
	inline void InsertBefore(LinkedListElement *other) noexcept {
		InsertBetween(other->m_prev, other);
	}
	inline void InsertAfter(LinkedListElement *other) noexcept {
		InsertBetween(other, other->m_next);
	}
	inline void Remove() noexcept {
		LinkedListHook<T>::m_prev->m_next = LinkedListHook<T>::m_next;
		LinkedListHook<T>::m_next->m_prev = LinkedListHook<T>::m_prev;
		LinkedListHook<T>::m_prev = this;
		LinkedListHook<T>::m_next = this;
	}

	//inline T* GetObject() noexcept { return static_cast<T*>(this); }
	inline T* GetPrev() noexcept { return static_cast<T*>(LinkedListHook<T>::m_prev); }
	inline T* GetNext() noexcept { return static_cast<T*>(LinkedListHook<T>::m_next); }

private:
	inline void InsertBetween(LinkedListHook<T> *a, LinkedListHook<T> *b) noexcept {
		LinkedListHook<T>::m_prev->m_next = LinkedListHook<T>::m_next;
		LinkedListHook<T>::m_next->m_prev = LinkedListHook<T>::m_prev;
		a->m_next = this;
		b->m_prev = this;
		LinkedListHook<T>::m_prev = a;
		LinkedListHook<T>::m_next = b;
	}

};
