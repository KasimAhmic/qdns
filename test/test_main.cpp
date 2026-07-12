#define BOOST_TEST_MODULE qdns_tests

#include <boost/mpl/list.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(dns_message_tests)

BOOST_AUTO_TEST_CASE(basic_test) { BOOST_TEST(true); }

BOOST_DATA_TEST_CASE(parameterized_test, boost::unit_test::data::make({0, 1, 2, 3, 4}), value) {
    BOOST_TEST(value < 5);
}

using test_types = boost::mpl::list<int, float, double>;

BOOST_AUTO_TEST_CASE_TEMPLATE(type_parameterized_test, T, test_types) {
    T value{};
    BOOST_TEST(value == T{});
}

BOOST_AUTO_TEST_SUITE_END()
