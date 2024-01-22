#include "utils.h"
#include <array>
#include <set>
#include <memory>

using namespace std;

const string test_raw_input = R"(.|...\....
|.-.\.....
.....|-...
........|.
..........
.........\
..../.\\..
.-.-/..|..
.|....-|.\
..//.|....)";

enum class Tile: char{
    Empty = '.',
    UpMirror = '/',
    DownMirror = '\\',
    VSplitter = '|',
    HSplitter = '-',
    Energized = '#'
};

ostream& operator<< (ostream &os, const Tile &tile) {
    return os << static_cast<char>(tile);
}

Tile parseTile(const char ch) {
    using enum Tile;

    switch(ch) {
        case static_cast<char>(Empty): return Empty;
        case static_cast<char>(UpMirror): return UpMirror;
        case static_cast<char>(DownMirror): return DownMirror;
        case static_cast<char>(VSplitter): return VSplitter;
        case static_cast<char>(HSplitter): return HSplitter;
        default:
            throw invalid_argument("Unexpected tile character: "s + ch);
    }
}

using Contraption = utils::Grid<Tile>;

Contraption parseInput(const vector<string>& input) {
    Contraption result;
    result.data.reserve(input.size());

    for(const auto& line: input) {
        auto tiles = line | views::transform(parseTile);

        result.data.emplace_back(begin(tiles), end(tiles));
    }

    return result;
}

enum class Direction{
    Up,
    Down,
    Left,
    Right,
    Vertical,
    Horizontal
};

Direction next(const Direction going, const Tile& tile) {
    using enum Direction;
    using enum Tile;

    if(!(going == Up
        || going == Down
        || going == Left
        || going == Right)) {
        throw invalid_argument("Bad direction");
    }

    switch(tile) {
    case Empty:
        return going;

    case UpMirror:
        switch (going) {
        case Up: return Right;
        case Down: return Left;
        case Left: return Down;
        case Right: return Up;
        default: throw logic_error("Should not reach this");
        }

    case DownMirror:
        switch(going) {
        case Up: return Left;
        case Down: return Right;
        case Left: return Up;
        case Right: return Down;
        default: throw logic_error("Should not reach this");
        }
    
    case HSplitter:
        switch(going) {
        case Up:
        case Down: return Horizontal;
        case Left: return Left;
        case Right: return Right;
        default: throw logic_error("Should not reach this");
        }

    case VSplitter:
        switch(going) {
        case Up: return Up;
        case Down: return Down;
        case Left:
        case Right: return Vertical;
        default: throw logic_error("Should not reach this");
        }

    default: throw logic_error("Unhandled tile on next");
    }
}


struct Beam{
    struct Node{
        Point pos;
        Direction dir;

        bool operator==(const Node& other) const {
            return pos == other.pos && dir == other.dir;
        }
    };

    vector<Node> nodes;
    vector<shared_ptr<Beam>> continuesTo;

    Beam(const Node& node)
        : nodes{{node}}
        , continuesTo{}
    {
    }

    Beam(const Beam& other)
        : nodes{other.nodes.begin(), other.nodes.end()}
        ,continuesTo{other.continuesTo.begin(), other.continuesTo.end()}
    {
    }

    Beam(Beam&& other)
        : nodes{move(other.nodes)}
        , continuesTo{move(other.continuesTo)}
    {
    }
};

void getAllSubbeams(const Beam& beam, set<shared_ptr<Beam>>& accumulator) {
    auto toAddView = beam.continuesTo | views::filter([&accumulator](const shared_ptr<Beam>& toCheck){
        return !accumulator.contains(toCheck);
    });
    vector<shared_ptr<Beam>> toAddCache{toAddView.begin(), toAddView.end()};
    accumulator.insert(toAddCache.begin(), toAddCache.end());

    for(auto& toAdd: toAddCache){
        getAllSubbeams(*toAdd, accumulator);
    }
}

set<Point> getUniquePoints(const Beam& beam) {
    set<shared_ptr<Beam>> allSubBeams;
    getAllSubbeams(beam, allSubBeams);

    auto beamPoints = beam.nodes | views::transform([](const Beam::Node& node) {
        return node.pos;
    });

    set<Point> allPoints{beamPoints.begin(), beamPoints.end()};

    auto allSubPointsV = allSubBeams
        | views::transform([](const shared_ptr<Beam>& beamPtr) {
            return beamPtr->nodes;
        })
        | views::join
        | views::transform([](const Beam::Node& node) {
            return node.pos;
        })
        | views::common;
    
    allPoints.insert(allSubPointsV.begin(), allSubPointsV.end());

    return allPoints;
}

Point applyDirection(const Point& point, const Direction direction) {
    using enum Direction;

    switch(direction) {
    case Up: return point.up();
    case Down: return point.down();
    case Left: return point.left();
    case Right: return point.right();
    default:
        throw logic_error("Unhandled direction case to apply to point");
    }
}

void drawBeam(const Contraption& contraption, const Beam& beam) {
    Contraption display;
    display.grow(contraption.columns().size(), contraption.rows().size(), Tile::Empty);
    utils::BoundingBox boundingBox{{0, 0}, {static_cast<long>(display.columns().size()) -1, static_cast<long>(display.rows().size()) - 1}};
    
    const auto uniquePoints = getUniquePoints(beam);

    for_each(uniquePoints.cbegin(), uniquePoints.cend(), [&display, &boundingBox](const Point& p) {
        if(boundingBox.hit(p)){
            display.at(p) = Tile::Energized;
        }
    });
    cout << endl << display << endl;
}

void probagate(Beam& beam, const Contraption& contraption, set<shared_ptr<Beam>>& beamContext, set<shared_ptr<Beam>>& pool) {
    // expand beam until a fork point
    using enum Direction;

    utils::BoundingBox boundingBox{
        {0l, 0l},
        {static_cast<long>(contraption.columns().size()) - 1l, static_cast<long>(contraption.rows().size()) - 1l}};
    
    auto continueNonLooping = [&beam, &contraption, &beamContext, &pool] (const Beam::Node& newHead){
        auto startsWithNewHead = [&newHead](const shared_ptr<Beam>& beamPtr) -> bool {
            return beamPtr->nodes.front() == newHead;
        };

        if(const auto ctxIt = find_if(beamContext.cbegin(), beamContext.cend(), startsWithNewHead); ctxIt == beamContext.cend()){
            const auto poolIt = find_if(pool.cbegin(), pool.cend(), startsWithNewHead);

            bool found = poolIt != pool.end();
            if(found) {
                const shared_ptr<Beam>& foundBeam = *poolIt;
                beam.continuesTo.push_back(foundBeam);
                auto insertAllToBeamContext = [&beamContext](const auto &self, const shared_ptr<Beam>& toInsert) -> void {
                    beamContext.insert(toInsert);
                    for(const auto& next: toInsert->continuesTo){
                        if(!beamContext.contains(next)){
                            self(self, next);
                        }
                    }
                };
                insertAllToBeamContext(insertAllToBeamContext, foundBeam);
            }
            else{
                auto newBeam =  make_shared<Beam>(newHead);
                beam.continuesTo.push_back(newBeam);
                beamContext.insert(newBeam);
                pool.insert(newBeam);

                probagate(*newBeam, contraption, beamContext, pool);
            }
        }
        else{
            // since it is found, it is already on the beam context, which implies it is alreay in the global context/pool
            // Here we create a loop
            beam.continuesTo.push_back(*ctxIt);
        }
    };

    // probagate until the split point (where recurcive call will handle the sub-beams)
    // or until the beam exits the contraption bounding box
    while(beam.continuesTo.empty()){
        auto node = beam.nodes.back();
        const auto nextPos = applyDirection(node.pos, node.dir);
        if(!boundingBox.hit(nextPos)){
            break;
        }
        const auto& tile = contraption.at(nextPos);

        const auto nextDir = next(node.dir, tile);
        switch(nextDir) {
        case Up:
            if(tile == Tile::VSplitter){
                continueNonLooping({nextPos, Up});
                return;
            }
            // fallthrough
        case Down:
            if(tile == Tile::VSplitter){
                continueNonLooping({nextPos, Down});
                return;
            }
            // fallthrough
        case Left:
            if(tile == Tile::HSplitter){
                continueNonLooping({nextPos, Left});
                return;
            }
            // fallthrough
        case Right:
            if(tile == Tile::HSplitter){
                continueNonLooping({nextPos, Right});
                return;
            }

            beam.nodes.emplace_back(nextPos, nextDir);
            break;

        case Horizontal: {
            for(auto& nextBeamDir : {Left, Right}){
                continueNonLooping({nextPos, nextBeamDir});
            }
            return;
        }

        case Vertical: {
            for(auto& nextBeamDir : {Up, Down}){
                continueNonLooping({nextPos, nextBeamDir});
            }
            return;
        }

        default: throw logic_error("Should not reach this");
        }
    }
}

// Point comparator
constexpr bool operator<(const Point& lhs, const Point& rhs) {
    if(lhs.x != rhs.x) {
        return lhs.x < rhs.x;
    }

    if(lhs.y != rhs.y) {
        return lhs.y < rhs.y;
    }

    return false;
}

void disconnect(Beam& beam) {
    vector<shared_ptr<Beam>> ctnV;

    beam.continuesTo.swap(ctnV);
    for(auto& cnt: ctnV) {
        disconnect(*cnt);
    }
}

long first(const vector<string>& input) {
    auto contraption = parseInput(input);
    // cout << contraption << endl;

    Beam beam{{{-1, 0}, Direction::Right}};
    set<shared_ptr<Beam>> pool;
    set<shared_ptr<Beam>> beamContext;

    probagate(beam, contraption, beamContext, pool);

    const auto uniquePoints = getUniquePoints(beam);

    for_each(pool.begin(), pool.end(), [](auto& sptr) { disconnect(*sptr); });

    // drawBeam(contraption, beam);

    return uniquePoints.size() - 1; // remove the "virtual" entry beam node at {-1, 0}
}

long second(const vector<string>& input) {
    auto contraption = parseInput(input);

    size_t maxUniquePoints = 0;
    set<shared_ptr<Beam>> pool;

    const long width = contraption.columns().size();
    const long height = contraption.rows().size();
    for(long x = 0; x < width; ++x) {
        // if(x % 5 == 0){
        //     cout << "x == " << x << endl;
        // }
        {
            Beam beam{{{x, -1}, Direction::Down}};
            set<shared_ptr<Beam>> beamContext;
            probagate(beam, contraption, beamContext, pool);
            const auto uniquePoints = getUniquePoints(beam);
            maxUniquePoints = max(uniquePoints.size(), maxUniquePoints);
        }
        {
            Beam beam{{{x, height}, Direction::Up}};
            set<shared_ptr<Beam>> beamContext;
            probagate(beam, contraption, beamContext, pool);
            const auto uniquePoints = getUniquePoints(beam);
            maxUniquePoints = max(uniquePoints.size(), maxUniquePoints);
        }
    }
    for(long y = 0; y < height; ++y) {
        // if(y % 5 == 0) {
        //     cout << "y == " << y << endl;
        // }
        {
            Beam beam{{{-1, y}, Direction::Right}};
            set<shared_ptr<Beam>> beamContext;
            probagate(beam, contraption, beamContext, pool);
            const auto uniquePoints = getUniquePoints(beam);
            maxUniquePoints = max(uniquePoints.size(), maxUniquePoints);
        }
        {
            Beam beam{{{width, y}, Direction::Left}};
            set<shared_ptr<Beam>> beamContext;
            probagate(beam, contraption, beamContext, pool);
            const auto uniquePoints = getUniquePoints(beam);
            maxUniquePoints = max(uniquePoints.size(), maxUniquePoints);
        }
    }

    // disconnect everything in order to break cycles and free the allocated memory
    for_each(pool.begin(), pool.end(), [](auto& sptr) { disconnect(*sptr); });

    return maxUniquePoints - 1;
}

int main(const int, const char* []) {
    cout << "Day 16" << endl;

    const auto first_test_result = first(utils::split(test_raw_input));
    assertEquals(46, first_test_result);
    cout << "Test ok" << endl;

    const auto input = utils::readFile("input/day16");
    const auto first_result = first(input);
    assertEquals(7632l, first_result);
    cout << "First: " << first_result << endl;

    const auto second_test_result = second(utils::split(test_raw_input));
    assertEquals(51, second_test_result);
    cout << "Test 2 ok" << endl;

    const auto second_result = second(input);
    assertEquals(8023, second_result);
    cout << "Second: " << second_result << endl;

    return EXIT_SUCCESS;
}
