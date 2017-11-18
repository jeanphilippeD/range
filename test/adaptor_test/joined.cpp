// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#include <boost/range/adaptor/joined.hpp>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/assign.hpp>
#include <boost/range/algorithm_ext.hpp>

#include <algorithm>
#include <list>
#include <set>
#include <vector>

namespace boost
{
    namespace
    {
        template< class Container >
        void joined_test_impl( Container& c )
        {
            using namespace boost::adaptors;

            std::vector< int > test_result1;
            boost::push_back(test_result1, c | joined);

            std::vector< int > test_result2;
            boost::push_back(test_result2, adaptors::join(c));

            std::vector< int > reference;
            typedef typename Container::const_iterator iter_t;
            for (iter_t it = c.begin(); it != c.end(); ++it)
            {
                boost::push_back(reference, *it);
            }

            BOOST_CHECK_EQUAL_COLLECTIONS( reference.begin(), reference.end(),
                                           test_result1.begin(), test_result1.end() );
            BOOST_CHECK_EQUAL_COLLECTIONS( reference.begin(), reference.end(),
                                           test_result2.begin(), test_result2.end() );
        }

        template< class Container >
        void joined_test_impl()
        {
            using namespace boost::assign;
            typedef typename Container::value_type value_t;

            value_t sub0;
            value_t sub1;
            sub1 += 1;

            value_t sub2;
            sub2 += 1,1,1,2,2,2;

            value_t sub3;
            sub3 += 3,3,3,3,3,4,5,6,6,6,7,8,9;

            // Test empty
            {
                Container c;
                joined_test_impl(c);
            }

            // Test one empty
            {
                Container c;
                c += sub0;
                joined_test_impl(c);
            }

            // Test one
            {
                Container c;
                c += sub1;
                joined_test_impl(c);
            }

            // Test many
            {
                Container c;
                c += sub1,sub2,sub3;
                joined_test_impl(c);
            }

            // Test many with empty
            {
                Container c;
                c += sub0,sub1,sub2,sub0,sub0,sub3,sub0;
                joined_test_impl(c);
            }
        }

        void joined_test()
        {
            typedef std::vector< int > vec_t;
            typedef std::list< int > lst_t;
            typedef std::set< int > set_t;
            typedef std::multiset< int > mset_t;

            joined_test_impl< std::vector< vec_t > >();
            joined_test_impl< std::list< lst_t > >();
            joined_test_impl< std::set< set_t > >();
            joined_test_impl< std::multiset< mset_t > >();

            joined_test_impl< std::vector< lst_t > >();
            joined_test_impl< std::vector< set_t > >();
            joined_test_impl< std::vector< mset_t > >();
        }
    }
}

boost::unit_test::test_suite*
init_unit_test_suite(int argc, char* argv[])
{
    boost::unit_test::test_suite* test
        = BOOST_TEST_SUITE( "RangeTestSuite.adaptor.joined" );

    test->add( BOOST_TEST_CASE( &boost::joined_test ) );

    return test;
}
