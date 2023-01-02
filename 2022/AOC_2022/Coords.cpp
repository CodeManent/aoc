#include "Coords.h"
#include <cstdint>

extern const Coords Origin{ 0, 0 };
extern const Coords Up{ 0, 1 };
extern const Coords Down{ 0, -1 };
extern const Coords Left{ -1, 0 };
extern const Coords Right{ 1, 0 };

size_t CoordsHasher::operator() (const Coords& c) const {
	return static_cast<size_t>(static_cast<uint64_t>(static_cast<uint32_t>(c.x)) << 32 | static_cast<uint32_t>(c.y));
}
