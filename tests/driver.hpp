/**
 * @file tests/driver.hpp
 * @brief Test driver
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

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

/**
 * @brief Test result
 */
enum test_result {
    no_tests = -100,
    not_found = -101,
    wrong_arguments = -102
};

/**
 * @brief Test wrapper
 */
struct test {
    /// Map of tests
    using map = std::map<index, test*>;

    /// Test function
    using func = std::function<i32(argh::parser)>;

    /**
     * @brief Construct a new test
     * 
     * @param id Test id
     * @param descr Test description
     * @param func Test function
     */
    explicit test(ui32 id, name descr, func func);

    /// Test id
    index id = 0;

    /// Test description
    string descr;

    /// Called on test run
    func on_func;
};

/**
 * @brief Test driver
 */
struct driver {
    /**
     * @brief Get driver singleton
     * 
     * @return driver& Test driver
     */
    static driver& instance() {
        static driver singleton;
        return singleton;
    }

    /**
     * @brief Add a test
     * 
     * @param test Test to add
     */
    void add_test(test* test) {
        tests.emplace(test->id, test);
    }

    /**
     * @brief Get all tests
     * 
     * @return test::map const& Map of tests
     */
    test::map const& get() const {
        return tests;
    }

private:
    /**
     * @brief Construct a new driver
     */
    driver() = default;

    /// Map of tests
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
