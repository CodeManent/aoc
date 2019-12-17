#ifndef IntStream_H
#define IntStream_H

#include <queue>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <condition_variable>

template <typename Tint>
class IntStream {
	std::queue<Tint> state;
	std::mutex m;
	std::condition_variable cv;

public:
	void Write(const Tint &value) {
		{
			std::scoped_lock<std::mutex> lock{ m };
			state.emplace(value);
		}
		cv.notify_one();
	}

	Tint Read() {
		std::unique_lock<std::mutex> lock{ m };
		while (state.empty()) {
			cv.wait(lock);
		}

		int result = state.front();
		state.pop();

		return result;
	}

	auto empty() const {
		return state.empty();
	}
};


#endif
