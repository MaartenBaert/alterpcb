#include "Drawing.h"

Drawing::Drawing(stringtag_t name, DrawingType type){

	m_name = name;
	m_type = type;

}

void Drawing::HistoryClear() {
	m_history.clear();
	m_history.emplace_back(false);
}

void Drawing::HistoryRevert() {
	//TODO//
}

void Drawing::HistoryPush(bool soft) {
	UNUSED(soft);
	//TODO//
}

void Drawing::HistoryUndo() {
	//TODO//
}

void Drawing::HistoryRedo() {
	//TODO//
}

