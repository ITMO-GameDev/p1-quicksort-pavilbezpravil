#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <algorithm>

#include "../catch.hpp"
#include "List.h"

template< typename T>
bool ListEqualVector(const List<T> &a, const std::vector<T> &v) {
    if (a.size() != v.size()) {
        return false;
    }

    size_t count = 0;
    for (auto iter = a.iterator(); iter.hasNext(); iter.next()) {
        if (iter.get() != v[count]) {
            return false;
        }
        ++count;
    }

    return count == v.size();
}

TEST_CASE( "List test", "[basic]" ) {
    SECTION("empty") {
        List<int> a;
        REQUIRE(ListEqualVector(a, {}));
    }

    SECTION("insert head") {
        List<int> a;
        a.insert_head(0);
        REQUIRE(ListEqualVector(a, {0}));
    }

    SECTION("insert head 2") {
        List<int> a;
        a.insert_head(1);
        a.insert_head(0);
        REQUIRE(ListEqualVector(a, {0, 1}));
    }

    SECTION("insert tail") {
        List<int> a;
        a.insert_tail(0);
        REQUIRE(ListEqualVector(a, {0}));
    }

    SECTION("insert tail 2") {
        List<int> a;
        a.insert_tail(0);
        a.insert_tail(1);
        REQUIRE(ListEqualVector(a, {0, 1}));
    }

    SECTION("insert head tail") {
        List<int> a;
        a.insert_head(0);
        a.insert_tail(1);
        REQUIRE(ListEqualVector(a, {0, 1}));

        a.insert_head(-1);
        a.insert_tail(2);
        REQUIRE(ListEqualVector(a, {-1, 0, 1, 2}));
    }

    SECTION("remove head tail") {
        List<int> a;
        a.insert_head(0);
        a.insert_tail(1);
        a.insert_head(-1);
        a.insert_tail(2);
        REQUIRE(ListEqualVector(a, {-1, 0, 1, 2}));

        a.remove_tail();
        REQUIRE(ListEqualVector(a, {-1, 0, 1}));

        a.remove_head();
        REQUIRE(ListEqualVector(a, {0, 1}));

        a.remove_tail();
        REQUIRE(ListEqualVector(a, {0}));

        a.remove_head();
        REQUIRE(ListEqualVector(a, {}));
    }

    SECTION("iterator test") {
        List<int> a;

        auto iter = a.iterator();
        iter.insert(0);
        REQUIRE(ListEqualVector(a, {0}));
        iter.insert(1);
        REQUIRE(ListEqualVector(a, {1, 0}));
        iter.insert(2);
        REQUIRE(ListEqualVector(a, {2, 1, 0}));

        iter.next();

        iter.insert(7);
        REQUIRE(ListEqualVector(a, {2, 7, 1, 0}));
        iter.remove();
        REQUIRE(ListEqualVector(a, {2, 1, 0}));
        REQUIRE(iter.get() == 1);

        iter.prev();
        iter.remove();
        REQUIRE(ListEqualVector(a, {1, 0}));
        iter.remove();
        iter.remove();
        REQUIRE(ListEqualVector(a, {}));
    }
}
