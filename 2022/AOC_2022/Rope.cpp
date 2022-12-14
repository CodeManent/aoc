#include "Rope.h"

Rope::Rope(const size_t knotCount, const Coords& initialPos)
	:knots(knotCount, initialPos)
{
}


void Rope::move(const Coords& c) {
	move(0, first() + c);
}

void Rope::move(size_t i, const Coords& newPos) {
	//const Coords initialPos = knots[i];
	knots[i] = newPos;

	// if there is a tail, process the tail head
	if (i + 1 < knots.size()) {
		const Coords tail = knots[i + 1];
		const auto d = newPos - tail;
		const auto distance = manhattan(newPos, tail);

		if (((d.x == 0 || d.y == 0) && distance == 2) || distance >= 3) {
			int tailX = d.x != 0;
			int tailY = d.y != 0;
			if (d.x < 0) {
				tailX *= -1;
			}
			if (d.y < 0) {
				tailY *= -1;
			}
			move(i + 1, tail + Coords{ tailX, tailY });
		}
	}
}

Coords Rope::first() {
	return knots.front();
}

Coords Rope::second() {
	return knots.at(1);
}

Coords Rope::last() {
	return knots.back();
}

Coords Rope::first() const {
	return knots.front();
}

Coords Rope::second() const {
	return knots.at(1);
}

Coords Rope::last() const {
	return knots.back();
}