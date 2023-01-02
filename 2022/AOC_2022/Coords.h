#ifndef Coords_h_
#define Coords_h_

#include "PointOps.h"
#include "Point2.h"

using Coords = Point2<int>;
extern const Coords Origin;
extern const Coords Up;
extern const Coords Down;
extern const Coords Left;
extern const Coords Right;

class CoordsHasher {
public:
	size_t operator()(const Coords&) const;
};

#include <functional>

template<>
struct std::hash<Coords> {
	size_t operator()(const Coords& c) const {
		return CoordsHasher()(c);
	}
};

#endif
