#ifndef Rope_h_
#define Rope_h_

#include "Coords.h"
#include <vector>

class Rope {
public:
	std::vector<Coords> knots;


	Rope(const size_t knotCount, const Coords& initialPos);

	void move(const Coords& c);
	void move(size_t i, const Coords& newPos);

	Coords first();
	Coords first() const;

	Coords second();
	Coords second() const;

	Coords last();
	Coords last() const;
};


#endif
