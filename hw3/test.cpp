#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <algorithm>

#include "catch.hpp"
#include "dict.h"

template< typename T>
std::vector<T> MakeRandomVector(size_t n, T min, T max) {
    std::vector<T> rnds;
    rnds.resize(n);
    for (auto &val : rnds) {
        val = min +  rand() % (max - min);
    }
    return rnds;
}

template< typename T>
bool VectorEqual(const std::vector<T>& v1, const std::vector<T>& v2) {
    return v1.size() == v2.size() && std::equal(v1.begin(), v1.end(), v2.begin());
}

TEST_CASE( "dict test", "[basic]" ) {
    SECTION("empty") {
        Dict<int, int> d;
        REQUIRE(d.size() == 0);
    }

    SECTION("one") {
        Dict<int, int> d;
        d.put(1, 11);
        REQUIRE(d.size() == 1);
        REQUIRE(d[1] == 11);
    }

    SECTION("two") {
        Dict<int, int> d;
        d.put(1, 11);
        d.put(2, 22);
        REQUIRE(d.size() == 2);
        REQUIRE(d[1] == 11);
        REQUIRE(d[2] == 22);
    }

    SECTION("three") {
        Dict<int, int> d;
        d.put(1, 11);
        d.put(2, 22);
        d.put(3, 33);
        REQUIRE(d[1] == 11);
        REQUIRE(d[2] == 22);
        REQUIRE(d[3] == 33);
    }

    SECTION("contains") {
        Dict<int, int> d;
        REQUIRE(!d.contains(1));
        d.put(1, 11);
        REQUIRE(d.contains(1));
        REQUIRE(!d.contains(2));
        d.put(2, 22);
        REQUIRE(d.contains(2));
        REQUIRE(!d.contains(3));
        d.put(3, 33);
        REQUIRE(d.contains(3));
    }

    SECTION("iterator") {
        Dict<int, int> d;
        d.put(1, 11);
        d.put(2, 22);
        d.put(3, 33);

        std::set<int> nums;
        auto it = d.iterator();
        REQUIRE(!it.hasPrev());
        REQUIRE(it.hasNext());
        nums.insert(it.get());
        it.next();
        REQUIRE(it.hasPrev());
        REQUIRE(it.hasNext());
        nums.insert(it.get());
        it.next();
        REQUIRE(it.hasPrev());
        REQUIRE(!it.hasNext());
        nums.insert(it.get());

        REQUIRE(nums.count(11));
        REQUIRE(nums.count(22));
        REQUIRE(nums.count(33));
    }
}
