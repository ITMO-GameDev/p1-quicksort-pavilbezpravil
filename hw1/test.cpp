#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <algorithm>

#include "catch.hpp"
#include "sort.h"

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


/// я написал тесты для проверки основных случаев: 0 значений, 1, 2 в правильном и не правильном порядке
/// массив из одинаковых элементов и также массив со случайными числами, сортировка которых сравнивается с сортировкой std::sort
TEST_CASE( "sort test", "[basic]" ) {
    auto comp = std::less<int>();

    SECTION("empty") {
        std::vector<int> v;
        mysort(v.begin(), v.end(), comp);
        REQUIRE(VectorEqual(v, {}));
    }

    SECTION("one value") {
        std::vector<int> v = {1};
        mysort(v.begin(), v.end(), comp);
        REQUIRE(VectorEqual(v, {1}));
    }

    SECTION("two value(right order)") {
        std::vector<int> v = {1, 2};
        mysort(v.begin(), v.end(), comp);
        REQUIRE(VectorEqual(v, {1, 2}));
    }

    SECTION("two value(wrong order)") {
        std::vector<int> v = {2, 1};
        mysort(v.begin(), v.end(), comp);
        REQUIRE(VectorEqual(v, {1, 2}));
    }

    SECTION("many equal value") {
        std::vector<int> v = {1, 1, 1, 1, 1, 1, 1, 1, 1};
        mysort(v.begin(), v.end(), comp);
        REQUIRE(VectorEqual(v, {1, 1, 1, 1, 1, 1, 1, 1, 1}));
    }

    SECTION("reversed order") {
        std::vector<int> v = {5, 4, 3, 2, 1};
        mysort(v.begin(), v.end(), comp);
        REQUIRE(VectorEqual(v, {1, 2, 3, 4, 5}));
    }

    SECTION("gen random vector and compare with std::sort") {
        for (int i = 0; i < 100; ++i) {
            std::vector<int> v = MakeRandomVector(i, 0, 100);

            auto vForMySort = v;
            mysort(vForMySort.begin(), vForMySort.end(), comp);

            auto vForStdSort = v;
            mysort(vForStdSort.begin(), vForStdSort.end(), comp);

            REQUIRE(VectorEqual(vForMySort, vForStdSort));
        }
    }
}
