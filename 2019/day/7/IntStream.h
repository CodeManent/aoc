#ifndef InttStream_H
#define InttStream_H

#include <queue>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <condition_variable>

class IntStream{
	std::queue<int> state;
	std::mutex m;
	std::condition_variable cv;

public:
	void Write(int value){
		{
			std::scoped_lock<std::mutex> lock{m};
			state.emplace(value);
		}
		cv.notify_one();
	}

	int Read() {
		std::unique_lock<std::mutex> lock{m};
		while(state.empty()){
			cv.wait(lock);
		}

		int result = state.front();
		state.pop();

		return result;
	}
};


#endif
