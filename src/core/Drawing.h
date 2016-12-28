#pragma once

#include "BackPointer.h"
#include "StringRegistry.h"
#include "CoreBasics.h"
#include "Cow.h"

#include <string>
#include <vector>

class Shape;

class DrawingHistory {

private:
	bool m_soft;
	std::vector<Cow<Shape>> m_shapes;

public:
	inline DrawingHistory(bool soft) : m_soft(soft) {}

	// default copy and assignment
	DrawingHistory(const DrawingHistory&) = default;
	DrawingHistory(DrawingHistory&&) = default;
	DrawingHistory& operator=(const DrawingHistory&) = default;
	DrawingHistory& operator=(DrawingHistory&&) = default;

	inline bool IsSoft() const { return m_soft; }

	inline       std::vector<Cow<Shape>>& GetShapes()       { return m_shapes; }
	inline const std::vector<Cow<Shape>>& GetShapes() const { return m_shapes; }

};

class Drawing : public BackPointer<Drawing> {

private:
	stringtag_t m_name;
	DrawingType m_type;
	std::vector<DrawingHistory> m_history;
	size_t m_history_position;

public:
	Drawing(stringtag_t name, DrawingType type);

	// noncopyable
	Drawing(const Drawing&) = delete;
	Drawing& operator=(const Drawing&) = delete;

	void HistoryClear();
	void HistoryRevert();
	void HistoryPush(bool soft = false);
	void HistoryUndo();
	void HistoryRedo();

	inline stringtag_t GetName() const { return m_name; }
	inline DrawingType GetType() const { return m_type; }

	inline       std::vector<Cow<Shape>>& GetShapes()       { assert(!m_history.empty()); return m_history[m_history_position].GetShapes(); }
	inline const std::vector<Cow<Shape>>& GetShapes() const { assert(!m_history.empty()); return m_history[m_history_position].GetShapes(); }

};

struct DrawingCompare {
	inline bool operator()(const ForwardPointer<Drawing> &a, const ForwardPointer<Drawing> &b) const {
		return StringRegistry::GetString(a->GetName()) < StringRegistry::GetString(b->GetName());
	}
};
