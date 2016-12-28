#pragma once

#include "Basics.h"
#include "VData.h"

#include <string>
#include <vector>

struct ShapeTransform {
	real_t m_x, m_y, m_angle;
	bool m_mirror, m_flip;
};

class Shape {

private:
	stringtag_t m_name;
	ShapeTransform m_transform;
	Cow<VData::Dict> m_params;

public:
	inline Shape(stringtag_t name, const ShapeTransform& transform, const Cow<VData::Dict> &params)
		: m_name(name), m_transform(transform), m_params(params) {}
	inline Shape(stringtag_t name, const ShapeTransform& transform, Cow<VData::Dict> &&params)
		: m_name(name), m_transform(transform), m_params(std::move(params)) {}
	inline Shape(stringtag_t name, const ShapeTransform& transform, const VData::Dict &params)
		: m_name(name), m_transform(transform), m_params(std::make_shared<VData::Dict>(params)) {}
	inline Shape(stringtag_t name, const ShapeTransform& transform, VData::Dict &&params)
		: m_name(name), m_transform(transform), m_params(std::make_shared<VData::Dict>(std::move(params))) {}

	inline stringtag_t GetName() const { return m_name; }
	inline       ShapeTransform& GetTransform()       { return m_transform; }
	inline const ShapeTransform& GetTransform() const { return m_transform; }
	inline       Cow<VData::Dict>& GetParams()       { return m_params; }
	inline const Cow<VData::Dict>& GetParams() const { return m_params; }

};
