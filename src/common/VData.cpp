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

#include "VData.h"

#include "StringRegistry.h"

const real_t VDATA_DECIMAL_SHIFT_SCALE = exp10(-VDATA_DECIMAL_SHIFT);

std::ostream& operator<<(std::ostream &stream, const VData &data) {
	switch(data.GetType()) {
		case VDATA_NULL: {
			stream << "null";
			break;
		}
		case VDATA_BOOL: {
			stream << ((data.AsBool())? "true" : "false");
			break;
		}
		case VDATA_INT: {
			stream << data.AsInt();
			break;
		}
		case VDATA_FLOAT: {
			stream << (data.AsFloat() * VDATA_DECIMAL_SHIFT_SCALE);
			break;
		}
		case VDATA_STRING: {
			stream << '"' << data.AsString() << '"';
			break;
		}
		case VDATA_LIST: {
			stream << "[";
			const VData::List &ref = data.AsList();
			if(ref.size() != 0) {
				stream << ref[0];
				for(size_t i = 1; i < ref.size(); ++i) {
					stream << ", " << ref[i];
				}
			}
			stream << "]";
			break;
		}
		case VDATA_DICT: {
			stream << "{";
			const VData::Dict &ref = data.AsDict();
			if(ref.GetSize() != 0) {
				stream << '"' << StringRegistry::GetString(ref[0].Key()) << "\": " << ref[0].Value();
				for(index_t i = 1; i < ref.GetSize(); ++i) {
					stream << ", \"" << StringRegistry::GetString(ref[i].Key()) << "\": " << ref[i].Value();
				}
			}
			stream << "}";
			break;
		}
	}
	return stream;
}

bool operator==(const VData &a, const VData &b) {
	if(a.GetType() != b.GetType())
		return false;
	switch(a.GetType()) {
		case VDATA_NULL: return true;
		case VDATA_BOOL: return (a.AsBool() == b.AsBool());
		case VDATA_INT: return (a.AsInt() == b.AsInt());
		case VDATA_FLOAT: return (a.AsFloat() == b.AsFloat());
		case VDATA_STRING: return (a.AsString() == b.AsString());
		case VDATA_LIST: {
			const VData::List &ref1 = a.AsList(), &ref2 = b.AsList();
			if(ref1.size() != ref2.size())
				return false;
			for(size_t i = 0; i < ref1.size(); ++i) {
				if(ref1[i] != ref2[i])
					return false;
			}
			return true;
		}
		case VDATA_DICT: {
			const VData::Dict &ref1 = a.AsDict(), &ref2 = b.AsDict();
			if(ref1.GetSize() != ref2.GetSize())
				return false;
			for(size_t i = 0; i < ref1.GetSize(); ++i) {
				if(ref1[i].Key() != ref2[i].Key())
					return false;
				if(ref1[i].Value() != ref2[i].Value())
					return false;
			}
			return true;
		}
	}
	// this should never be reached
	assert(false);
	return false;
}
