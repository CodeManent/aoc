#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <stdexcept>
#include <stack>

typedef std::string NodeId;

struct NodeInfo{
	NodeId parent;
	int totalDistance;
};

typedef std::map<NodeId, NodeInfo> OrbitMap;


OrbitMap ReadMap(std::istream &is) {
	OrbitMap result;

	std::string line;

	for (int lineNo = 1; is && std::getline(is, line); ++lineNo){
		const auto& from = std::cbegin(line);
		const auto& to = std::cend(line);

		const auto& splitPos = std::find(from, to, ')');
		if (splitPos == std::cend(line)){
			std::cerr << lineNo << ": No ')' separator found."<< std::endl;
			return {};
		}

		std::string name1{from, splitPos};
		std::string name2{splitPos + 1, to};

		if(result.find(name2) != result.cend()){
			std::cerr << lineNo << ": \"" <<  name2 << "\" already contained in the map." << std::endl;
			return {};
		}

		result[name2] = {name1, 0};
	}


	return result;
}

std::ostream& operator<<(std::ostream &os, const NodeInfo &ni) {
	os << "NodeInfo {" << ni.parent << ", " << ni.totalDistance << "}";

	return os;
}

template <typename TKey, typename TValue>
std::ostream& operator<< (std::ostream& os, const std::map<TKey, TValue> &m){
	os << '{';
	const auto begin = std::begin(m);
	const auto end = std::end(m);

	for(auto i = begin; i != end; ++i) {
		if(i != begin) {
			os << ", ";
		}
		os << i->first;
		os << ": ";
		os << i->second;
	}
	os << '}';

	return os;
}

// Assumes no loops in the graph
int SourceDistance(OrbitMap &m, const NodeId &current){
	if(current == "COM"){
		return 0;
	}

	auto &info = m[current];
	if(!info.totalDistance){
		info.totalDistance = SourceDistance(m, info.parent) + 1;
	}

	return info.totalDistance;
}

int SumOrbits(OrbitMap &m) {
	return std::accumulate(
		m.begin(),
		m.end(),
		0,
		[&](int partialSum, auto& entry) {
			return partialSum + SourceDistance(m, entry.first);
		});
}

std::stack<NodeId> RootPath(const OrbitMap &m, const NodeId& root, const NodeId &node){
	std::stack<NodeId> result;
	for(NodeId current = node; current != root; current = m.at(current).parent){
		result.push(current);
	}

	result.push(root);

	return result;
}

int HopCount(const OrbitMap &m, const NodeId &from, const NodeId &to)
{
	NodeId root = "COM";
	auto fromPath = RootPath(m, root, from);
	auto toPath = RootPath(m, root, to);

	for (; !fromPath.empty() and !toPath.empty() && fromPath.top() == toPath.top(); fromPath.pop(), toPath.pop()){
		root = fromPath.top();
	}

	return (fromPath.size() -1) + (toPath.size() -1);
}

bool Test() {
	const char *const testInput = R"(COM)B
B)C
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L)";
	std::istringstream ss{testInput};
	
	auto testMap = ReadMap(ss);
	//std::cout << "TestMap: " << testMap << std::endl;
	int orbits = SumOrbits(testMap);

	if(orbits != 42) {
		std::cout << "Bad number of orbits " << orbits << ". Was expecting " << 42 << "." << std::endl;
		return false;
	}

	const char* const hopTestInput = R"(COM)B
B)C
C)D
D)E
E)F
B)G
G)H
D)I
E)J
J)K
K)L
K)YOU
I)SAN)";
	std::istringstream hopSS{hopTestInput};
	int hops = HopCount(ReadMap(hopSS), "YOU", "SAN");

	if(hops != 4) {
		std::cout << "Wrong number of hops " << hops << ". Was expecting " << 4 << "." << std::endl;
		return false;
	}

	return true;
}

int main(int, const char**){
	std::cout << "Day 6" << std::endl;

	if(!Test()){
		std::cerr << "Tests failed." << std::endl;
		return EXIT_FAILURE;
	}

	auto inputMap = ReadMap(std::cin);
	const int orbits = SumOrbits(inputMap);
	std::cout << "Orbits: " << orbits << std::endl;
	const int hops = HopCount(inputMap, "YOU", "SAN");
	std::cout << "Hops: " << hops << std::endl;

	return EXIT_SUCCESS;
}