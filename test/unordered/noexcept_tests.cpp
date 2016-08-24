
// Copyright 2013 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../helpers/prefix.hpp"
#if UNORDERED_TEST_STD
#include <unordered_map>
#include <unordered_set>
#else
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#endif
#include "../helpers/postfix.hpp"

#include "../helpers/test.hpp"
#include "../helpers/invariants.hpp"
#include <boost/type_traits/is_nothrow_move_assignable.hpp>
#include <boost/type_traits/is_nothrow_move_constructible.hpp>
#include <boost/functional/hash.hpp>
#include <boost/move/move.hpp>

namespace noexcept_tests
{
    struct test_exception {};

    bool throwing_test_exception = true;
    void test_throw(char const* name) {
        if (throwing_test_exception) {
            std::cerr << "Throw exception in: " << name << std::endl;
            throw test_exception();
        }
    }

    struct hash_possible_exception : boost::hash<int>
    {
        hash_possible_exception() {}
        hash_possible_exception(hash_possible_exception const&)
        { test_throw("Construct hash"); }
        hash_possible_exception& operator=(hash_possible_exception const&)
        { test_throw("Assign hash"); return *this; }
    };

    struct equal_to_possible_exception : std::equal_to<int>
    {
        equal_to_possible_exception() {}
        equal_to_possible_exception(equal_to_possible_exception const&)
        { test_throw("Construct equal_to"); }
        equal_to_possible_exception& operator=(equal_to_possible_exception const&)
        { test_throw("Assign equal_to"); return *this; }
    };

    class hash_nothrow_move : boost::hash<int>
    {
        BOOST_COPYABLE_AND_MOVABLE(hash_nothrow_move)

        typedef boost::hash<int> base;
    public:
        hash_nothrow_move(BOOST_RV_REF(hash_nothrow_move))
            BOOST_NOEXCEPT {}

        hash_nothrow_move() { test_throw("Constructor"); }
        hash_nothrow_move(hash_nothrow_move const&) { test_throw("Copy"); }
        hash_nothrow_move& operator=(BOOST_COPY_ASSIGN_REF(hash_nothrow_move))
        { test_throw("Assign"); return *this; }
        hash_nothrow_move& operator=(BOOST_RV_REF(hash_nothrow_move))
        { test_throw("Move Assign"); return *this; }
        std::size_t operator()(int x) const
        { test_throw("Operator"); return static_cast<base const&>(*this)(x); }
    };

    class equal_to_nothrow_move : std::equal_to<int>
    {
        BOOST_COPYABLE_AND_MOVABLE(equal_to_nothrow_move)

        typedef std::equal_to<int> base;
    public:
        equal_to_nothrow_move(BOOST_RV_REF(equal_to_nothrow_move))
            BOOST_NOEXCEPT {}
        equal_to_nothrow_move() { test_throw("Constructor"); }
        equal_to_nothrow_move(equal_to_nothrow_move const&)
        { test_throw("Copy"); }
        equal_to_nothrow_move& operator=(BOOST_COPY_ASSIGN_REF(equal_to_nothrow_move))
        { test_throw("Assign"); return *this; }
        equal_to_nothrow_move& operator=(BOOST_RV_REF(equal_to_nothrow_move))
        { test_throw("Move Assign"); return *this; }
        std::size_t operator()(int x, int y) const
        { test_throw("Operator"); return static_cast<base const&>(*this)(x, y); }
    };

    class hash_nothrow_move_assign : boost::hash<int>
    {
        BOOST_COPYABLE_AND_MOVABLE(hash_nothrow_move_assign)

        typedef boost::hash<int> base;
    public:
        hash_nothrow_move_assign() { test_throw("Constructor"); }
        hash_nothrow_move_assign(hash_nothrow_move_assign const&) { test_throw("Copy"); }
        hash_nothrow_move_assign(BOOST_RV_REF(hash_nothrow_move_assign))
        { test_throw("Move"); }
        hash_nothrow_move_assign& operator=(BOOST_COPY_ASSIGN_REF(hash_nothrow_move_assign))
        { test_throw("Assign"); return *this; }
        hash_nothrow_move_assign& operator=(BOOST_RV_REF(hash_nothrow_move_assign))
            BOOST_NOEXCEPT { return *this; }
        std::size_t operator()(int x) const
        { test_throw("Operator"); return static_cast<base const&>(*this)(x); }
    };

    class equal_to_nothrow_move_assign : std::equal_to<int>
    {
        BOOST_COPYABLE_AND_MOVABLE(equal_to_nothrow_move_assign)

        typedef std::equal_to<int> base;
    public:
        equal_to_nothrow_move_assign() { test_throw("Constructor"); }
        equal_to_nothrow_move_assign(equal_to_nothrow_move_assign const&)
        { test_throw("Copy"); }
        equal_to_nothrow_move_assign(BOOST_RV_REF(equal_to_nothrow_move_assign))
        { test_throw("Move"); }
        equal_to_nothrow_move_assign& operator=(BOOST_COPY_ASSIGN_REF(equal_to_nothrow_move_assign))
        { test_throw("Assign"); return *this; }
        equal_to_nothrow_move_assign& operator=(BOOST_RV_REF(equal_to_nothrow_move_assign))
            BOOST_NOEXCEPT { return *this; }
        std::size_t operator()(int x, int y) const
        { test_throw("Operator"); return static_cast<base const&>(*this)(x, y); }
    };

    bool have_is_nothrow_move = false;
    bool have_is_nothrow_move_assign = false;

    UNORDERED_AUTO_TEST(check_is_nothrow_move)
    {
        BOOST_TEST(!boost::is_nothrow_move_constructible<hash_possible_exception>::value);
        BOOST_TEST(!boost::is_nothrow_move_assignable<hash_possible_exception>::value);
        have_is_nothrow_move = boost::is_nothrow_move_constructible<hash_nothrow_move>::value;
        have_is_nothrow_move_assign = boost::is_nothrow_move_assignable<hash_nothrow_move_assign>::value;

        // Copied from type_traits implementation
        // to make sure this does actually detect it when expected.
#ifdef BOOST_IS_NOTHROW_MOVE_CONSTRUCT
        BOOST_TEST(have_is_nothrow_move);
        BOOST_TEST(have_is_nothrow_move_assign);
#elif !defined(BOOST_NO_CXX11_NOEXCEPT) && !defined(BOOST_NO_SFINAE_EXPR) && !BOOST_WORKAROUND(BOOST_GCC_VERSION, < 40800)
        BOOST_TEST(have_is_nothrow_move);
        BOOST_TEST(have_is_nothrow_move_assign);
#endif
    }

    UNORDERED_AUTO_TEST(test_noexcept)
    {
        if (have_is_nothrow_move) {
            BOOST_TEST((boost::is_nothrow_move_constructible<
                        UNORDERED_NAMESPACE::unordered_set<int> >::value));
            BOOST_TEST((boost::is_nothrow_move_constructible<
                        UNORDERED_NAMESPACE::unordered_multiset<int> >::value));
            BOOST_TEST((boost::is_nothrow_move_constructible<
                        UNORDERED_NAMESPACE::unordered_map<int, int> >::value));
            BOOST_TEST((boost::is_nothrow_move_constructible<
                        UNORDERED_NAMESPACE::unordered_multimap<int, int> >::value));

            /* TODO: Required is_always_equal support.
             * unordered_map& operator=(unordered_map&&)
             *  noexcept(allocator_traits<Allocator>::is_always_equal::value &&
             *  is_nothrow_move_assignable_v<Hash> &&
             * is_nothrow_move_assignable_v<Pred>);
            BOOST_TEST((boost::is_nothrow_move_assignable<
                        UNORDERED_NAMESPACE::unordered_set<int> >::value));
            BOOST_TEST((boost::is_nothrow_move_assignable<
                        UNORDERED_NAMESPACE::unordered_multiset<int> >::value));
            BOOST_TEST((boost::is_nothrow_move_assignable<
                        UNORDERED_NAMESPACE::unordered_map<int, int> >::value));
            BOOST_TEST((boost::is_nothrow_move_assignable<
                        UNORDERED_NAMESPACE::unordered_multimap<int, int> >::value));
            */
        }

        BOOST_TEST((!boost::is_nothrow_move_constructible<
                    UNORDERED_NAMESPACE::unordered_set<int, hash_possible_exception>
                >::value));
        BOOST_TEST((!boost::is_nothrow_move_constructible<
                    UNORDERED_NAMESPACE::unordered_multiset<int, boost::hash<int>,
                    equal_to_possible_exception>
                >::value));

        BOOST_TEST((!boost::is_nothrow_move_assignable<
                    UNORDERED_NAMESPACE::unordered_set<int, hash_possible_exception>
                >::value));
        BOOST_TEST((!boost::is_nothrow_move_assignable<
                    UNORDERED_NAMESPACE::unordered_multiset<int, boost::hash<int>,
                    equal_to_possible_exception>
                >::value));
    }

    UNORDERED_AUTO_TEST(test_no_throw_when_noexcept)
    {
        typedef UNORDERED_NAMESPACE::unordered_set<int,
                hash_nothrow_move, equal_to_nothrow_move> throwing_set;

        if (have_is_nothrow_move) {
            BOOST_TEST(boost::is_nothrow_move_constructible<throwing_set>::value);
        }

        throwing_test_exception = false;
        throwing_set x1;
        x1.insert(10);
        x1.insert(50);
        throwing_test_exception = true;

        try {
            throwing_set x2 = boost::move(x1);
            BOOST_TEST(x2.size() == 2);
            BOOST_TEST(*x2.begin() == 10 || *x2.begin() == 50);
        } catch(test_exception) {
#if defined(BOOST_UNORDERED_USE_MOVE) || !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
            BOOST_TEST(false);
#endif
        }
    }

    UNORDERED_AUTO_TEST(test_no_throw_assign_when_noexcept)
    {
        typedef UNORDERED_NAMESPACE::unordered_multiset<int,
                hash_nothrow_move_assign, equal_to_nothrow_move_assign>
                    throwing_set;

        /* TODO:
        if (have_is_nothrow_move_assign) {
            BOOST_TEST(boost::is_nothrow_move_assignable<throwing_set>::value);
        }
        */

        throwing_test_exception = false;
        throwing_set x1;
        throwing_set x2;
        x1.insert(42);
        x1.insert(93);
        x2.insert(12);
        x2.insert(53);
        throwing_test_exception = true;

        try {
            x2 = boost::move(x1);
            BOOST_TEST(x2.size() == 2);
            BOOST_TEST(*x2.begin() == 42 || *x2.begin() == 93);
        } catch(test_exception) {
#if defined(BOOST_UNORDERED_USE_MOVE) || !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
            BOOST_TEST(!have_is_nothrow_move_assign);
#endif
        }

        throwing_test_exception = false;
        test::check_equivalent_keys(x2);
        throwing_test_exception = true;
    }

    UNORDERED_AUTO_TEST(test_throw_in_hash)
    {
        typedef UNORDERED_NAMESPACE::unordered_multimap<int, int,
            hash_possible_exception, equal_to_nothrow_move> map;

        BOOST_TEST(!boost::is_nothrow_move_constructible<map>::value);

        throwing_test_exception = false;
        map x1;
        x1.emplace(25, 63);
        x1.emplace(17, 18);
        throwing_test_exception = true;

        try {
            map x2 = boost::move(x1);
            BOOST_TEST(false);
        } catch (test_exception) {
        }
    }

    UNORDERED_AUTO_TEST(test_throw_in_equal)
    {
        typedef UNORDERED_NAMESPACE::unordered_multimap<int, int,
            hash_nothrow_move, equal_to_possible_exception> map;

        BOOST_TEST(!boost::is_nothrow_move_constructible<map>::value);

        throwing_test_exception = false;
        map x1;
        x1.emplace(25, 63);
        x1.emplace(17, 18);
        throwing_test_exception = true;

        try {
            map x2 = boost::move(x1);
            BOOST_TEST(false);
        } catch (test_exception) {
        }
    }

    UNORDERED_AUTO_TEST(test_assign_throw_in_hash)
    {
        typedef UNORDERED_NAMESPACE::unordered_multimap<int, int,
            hash_possible_exception, equal_to_nothrow_move_assign> map;

        BOOST_TEST(!boost::is_nothrow_move_assignable<map>::value);

        throwing_test_exception = false;
        map x1;
        map x2;
        x1.emplace(25, 63);
        x1.emplace(17, 18);
        x2.emplace(38, 42);
        x2.emplace(17, 99);
        throwing_test_exception = true;

        try {
            x2 = boost::move(x1);
            BOOST_TEST(false);
        } catch (test_exception) {
        }

        throwing_test_exception = false;
        test::check_equivalent_keys(x2);
        throwing_test_exception = true;
    }

    UNORDERED_AUTO_TEST(test_assign_throw_in_equal)
    {
        typedef UNORDERED_NAMESPACE::unordered_map<int, int,
            hash_nothrow_move_assign, equal_to_possible_exception> map;

        BOOST_TEST(!boost::is_nothrow_move_assignable<map>::value);

        throwing_test_exception = false;
        map x1;
        map x2;
        x1.emplace(25, 63);
        x1.emplace(17, 18);
        throwing_test_exception = true;

        try {
            x2 = boost::move(x1);
            BOOST_TEST(false);
        } catch (test_exception) {
        }

        throwing_test_exception = false;
        test::check_equivalent_keys(x2);
        throwing_test_exception = true;
    }
}

RUN_TESTS()
