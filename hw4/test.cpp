#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <algorithm>

#include "catch.hpp"
#include "my_alloc.h"

#define HEADER_SIZE (32)
#define FOOTER_SIZE (8)
#define OVERHEAD_SIZE (HEADER_SIZE + FOOTER_SIZE)

// по большей степени тестирование проводилось глазами глядя на дамп
TEST_CASE( "free list", "[basic]" ) {
    SECTION("alloc one") {
        FreeList fl {1024};

        fl.init();
        fl.dumpStat();
        fl.dumpFreeList();

        void* a1 = fl.alloc(100);
        REQUIRE(a1 == fl.getDataPtr() + HEADER_SIZE);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a1);
        fl.dumpStat();
        fl.dumpFreeList();
        REQUIRE(a1 == fl.alloc(100));
        fl.free(a1);
        fl.dumpStat();
        fl.dumpFreeList();
    }

    SECTION("alloc multiple") {
        FreeList fl {1024};

        fl.init();
        fl.dumpStat();
        fl.dumpFreeList();

        void* a1 = fl.alloc(100);
        fl.dumpStat();
        fl.dumpFreeList();

        void* a2 = fl.alloc(100);
        void* a3 = fl.alloc(100);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a2);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a1);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a3);
        fl.dumpStat();
        fl.dumpFreeList();
    }

    SECTION("alloc multiple2") {
        FreeList fl {1024};

        fl.init();
        fl.dumpStat();
        fl.dumpFreeList();

        void* a1 = fl.alloc(100);
        fl.dumpStat();
        fl.dumpFreeList();

        void* a2 = fl.alloc(100);
        void* a3 = fl.alloc(100);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a2);
        fl.dumpStat();
        fl.dumpFreeList();

        void* a4 = fl.alloc(200);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a1);
        fl.dumpStat();
        fl.dumpFreeList();

        void* a5 = fl.alloc(100);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a3);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a4);
        fl.dumpStat();
        fl.dumpFreeList();

        fl.free(a5);
        fl.dumpStat();
        fl.dumpFreeList();
    }
}


TEST_CASE( "fma", "[basic]" ) {
    SECTION("alloc one") {
        FMA fl {256, 5};

        fl.init();
        fl.dumpStat();

        void* a1 = fl.alloc(100);
        fl.dumpStat();

        fl.free(a1);
        fl.dumpStat();
    }

    SECTION("alloc multiple") {
        FMA fl {256, 5};

        fl.init();
        fl.dumpStat();

        void* a1 = fl.alloc(100);
        fl.dumpStat();

        void* a2 = fl.alloc(100);
        void* a3 = fl.alloc(100);
        fl.dumpStat();

        fl.free(a2);
        fl.dumpStat();

        fl.free(a1);
        fl.dumpStat();

        fl.free(a3);
        fl.dumpStat();
    }

    SECTION("alloc multiple2") {
        FMA fl {256, 5};

        fl.init();
        fl.dumpStat();

        void* a1 = fl.alloc(100);
        fl.dumpStat();

        void* a2 = fl.alloc(100);
        void* a3 = fl.alloc(100);
        fl.dumpStat();

        fl.free(a2);
        fl.dumpStat();

        void* a4 = fl.alloc(200);
        fl.dumpStat();

        fl.free(a1);
        fl.dumpStat();

        void* a5 = fl.alloc(100);
        fl.dumpStat();

        fl.free(a3);
        fl.dumpStat();

        fl.free(a4);
        fl.dumpStat();

        fl.free(a5);
        fl.dumpStat();
    }
}