#include "VData.h"

#include "StringRegistry.h"

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
			stream << data.AsFloat();
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
				for(size_t i = 1; i < ref.GetSize(); ++i) {
					stream << ", \"" << StringRegistry::GetString(ref[i].Key()) << "\": " << ref[i].Value();
				}
			}
			stream << "}";
			break;
		}
	}
	return stream;
}
