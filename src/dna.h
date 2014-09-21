#pragma once

#include "color.h"
#include "polygon.h"
#include <vector>


struct PolyDNA {
	Color color;
	float alpha;
	std::vector<Polygon::Point> v;
};


class DNA {
public:
	DNA() = default;
	DNA(const DNA&) = default;
	DNA(const std::size_t pc, const std::size_t vc)
	: polyCount(pc), vertCount(vc) {
		data.reserve(polyCount);
	}
	~DNA() = default;

	void add(const Polygon& p, const Color& c, const float a) {
		PolyDNA pd;
		pd.color = c;
		pd.alpha = a;
		pd.v.reserve(p.size());
		auto& vert = p.vertices();
		for (auto it = vert.cbegin(); it != vert.cend(); ++it)
			pd.v.push_back(*it);
		data.push_back(pd);
	}

	bool empty() const {
		return data.empty();
	}

	std::size_t polyCount = 0;
	std::size_t vertCount = 0;
	std::vector<PolyDNA> data;
};
