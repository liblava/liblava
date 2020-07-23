// file      : tests/driver.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/lava.hpp>

/* ---

LAVA_TEST(ID, DESCRIPTION)
{
    // command line arguments
    // argh::parser argh;

    ...

    return 0; // return int
}

copy and paste:

LAVA_TEST(1, "first test")
{
    return argh.size();
}

*/

namespace lava {

    enum test_result {

        no_tests = -100,
        not_found = -101,
        wrong_arguments = -102
    };

    struct test {
        using map = std::map<index, test*>;
        using func = std::function<i32(argh::parser)>;

        explicit test(ui32 id, name descr, func func);

        index id = 0;
        string descr;
        func on_func;
    };

    struct driver {
        static driver& instance() {
            static driver singleton;
            return singleton;
        }

        void add_test(test* test) {
            tests.emplace(test->id, test);
        }

        test::map const& get() const {
            return tests;
        }

    private:
        driver() = default;

        test::map tests;
    };

} // namespace lava

#define OBJ test_
#define FUNC _test

#define STR_(n, m) n##m
#define STR(n, m) STR_(n, m)

#define LAVA_TEST(ID, NAME) \
    i32 STR(FUNC, ID)(argh::parser argh); \
    lava::test STR(OBJ, ID)(ID, NAME, ::STR(FUNC, ID)); \
    i32 STR(FUNC, ID)(argh::parser argh)
