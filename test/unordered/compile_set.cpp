
// Copyright 2006-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This test creates the containers with members that meet their minimum
// requirements. Makes sure everything compiles and is defined correctly.

#include "../helpers/prefix.hpp"
#if UNORDERED_TEST_STD
#include <unordered_set>
#else
#include <boost/unordered_set.hpp>
#endif
#include "../helpers/postfix.hpp"

#include "../helpers/test.hpp"
#include "../objects/minimal.hpp"
#include "./compile_tests.hpp"

// Explicit instantiation to catch compile-time errors

#if UNORDERED_TEST_STD
#define INSTANTIATE(type) template class UNORDERED_NAMESPACE::unordered_##type
#else
#define INSTANTIATE(type)                                                      \
  template class boost::unordered::detail::instantiate_##type
#endif

#if !UNORDERED_TEST_LOOSE
INSTANTIATE(set)<int, UNORDERED_NAMESPACE::hash<int>, std::equal_to<int>,
  test::minimal::allocator<int> >;
INSTANTIATE(multiset)<int const, UNORDERED_NAMESPACE::hash<int>,
  std::equal_to<int>, test::minimal::allocator<int> >;

INSTANTIATE(set)<test::minimal::assignable const,
  test::minimal::hash<test::minimal::assignable>,
  test::minimal::equal_to<test::minimal::assignable>,
  test::minimal::allocator<int> >;
INSTANTIATE(multiset)<test::minimal::assignable,
  test::minimal::hash<test::minimal::assignable>,
  test::minimal::equal_to<test::minimal::assignable>,
  test::minimal::allocator<test::minimal::assignable> >;
#endif

UNORDERED_AUTO_TEST (test0) {
  test::minimal::constructor_param x;

  test::minimal::assignable assignable(x);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_set.\n";

  UNORDERED_NAMESPACE::unordered_set<int> int_set;

  UNORDERED_NAMESPACE::unordered_set<int, UNORDERED_NAMESPACE::hash<int>,
    std::equal_to<int>, test::minimal::cxx11_allocator<int> >
    int_set2;

  UNORDERED_NAMESPACE::unordered_set<test::minimal::assignable,
    test::minimal::hash<test::minimal::assignable>,
    test::minimal::equal_to<test::minimal::assignable>,
    test::minimal::allocator<test::minimal::assignable> >
    set;

  container_test(int_set, 0);
  container_test(int_set2, 0);
  container_test(set, assignable);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_multiset.\n";

  UNORDERED_NAMESPACE::unordered_multiset<int> int_multiset;

  UNORDERED_NAMESPACE::unordered_multiset<int, UNORDERED_NAMESPACE::hash<int>,
    std::equal_to<int>, test::minimal::cxx11_allocator<int> >
    int_multiset2;

  UNORDERED_NAMESPACE::unordered_multiset<test::minimal::assignable,
    test::minimal::hash<test::minimal::assignable>,
    test::minimal::equal_to<test::minimal::assignable>,
    test::minimal::allocator<test::minimal::assignable> >
    multiset;

  container_test(int_multiset, 0);
  container_test(int_multiset2, 0);
  container_test(multiset, assignable);
}

UNORDERED_AUTO_TEST (equality_tests) {
  typedef test::minimal::copy_constructible_equality_comparable value_type;

  UNORDERED_NAMESPACE::unordered_set<int> int_set;

  UNORDERED_NAMESPACE::unordered_set<int, UNORDERED_NAMESPACE::hash<int>,
    std::equal_to<int>, test::minimal::cxx11_allocator<int> >
    int_set2;

  UNORDERED_NAMESPACE::unordered_set<
    test::minimal::copy_constructible_equality_comparable,
    test::minimal::hash<test::minimal::copy_constructible_equality_comparable>,
    test::minimal::equal_to<
      test::minimal::copy_constructible_equality_comparable>,
    test::minimal::allocator<value_type> >
    set;

  equality_test(int_set);
  equality_test(int_set2);
  equality_test(set);

  UNORDERED_NAMESPACE::unordered_multiset<int> int_multiset;

  UNORDERED_NAMESPACE::unordered_multiset<int, UNORDERED_NAMESPACE::hash<int>,
    std::equal_to<int>, test::minimal::cxx11_allocator<int> >
    int_multiset2;

  UNORDERED_NAMESPACE::unordered_multiset<
    test::minimal::copy_constructible_equality_comparable,
    test::minimal::hash<test::minimal::copy_constructible_equality_comparable>,
    test::minimal::equal_to<
      test::minimal::copy_constructible_equality_comparable>,
    test::minimal::allocator<value_type> >
    multiset;

  equality_test(int_multiset);
  equality_test(int_multiset2);
  equality_test(multiset);
}

UNORDERED_AUTO_TEST (test1) {
  UNORDERED_NAMESPACE::hash<int> hash;
  std::equal_to<int> equal_to;
  int value = 0;

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_set." << std::endl;

  UNORDERED_NAMESPACE::unordered_set<int> set;

  UNORDERED_NAMESPACE::unordered_set<int, UNORDERED_NAMESPACE::hash<int>,
    std::equal_to<int>, test::minimal::cxx11_allocator<int> >
    set2;

  unordered_unique_test(set, value);
  unordered_set_test(set, value);
  unordered_copyable_test(set, value, value, hash, equal_to);

  unordered_unique_test(set2, value);
  unordered_set_test(set2, value);
  unordered_copyable_test(set2, value, value, hash, equal_to);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_multiset." << std::endl;

  UNORDERED_NAMESPACE::unordered_multiset<int> multiset;

  UNORDERED_NAMESPACE::unordered_multiset<int, UNORDERED_NAMESPACE::hash<int>,
    std::equal_to<int>, test::minimal::cxx11_allocator<int> >
    multiset2;

  unordered_equivalent_test(multiset, value);
  unordered_set_test(multiset, value);
  unordered_copyable_test(multiset, value, value, hash, equal_to);

  unordered_equivalent_test(multiset2, value);
  unordered_set_test(multiset2, value);
  unordered_copyable_test(multiset2, value, value, hash, equal_to);
}

UNORDERED_AUTO_TEST (test2) {
  test::minimal::constructor_param x;

  test::minimal::assignable assignable(x);
  test::minimal::copy_constructible copy_constructible(x);
  test::minimal::hash<test::minimal::assignable> hash(x);
  test::minimal::equal_to<test::minimal::assignable> equal_to(x);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_set.\n";

  UNORDERED_NAMESPACE::unordered_set<test::minimal::assignable,
    test::minimal::hash<test::minimal::assignable>,
    test::minimal::equal_to<test::minimal::assignable>,
    test::minimal::allocator<test::minimal::assignable> >
    set;

  unordered_unique_test(set, assignable);
  unordered_set_test(set, assignable);
  unordered_copyable_test(set, assignable, assignable, hash, equal_to);
  unordered_set_member_test(set, assignable);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_multiset.\n";

  UNORDERED_NAMESPACE::unordered_multiset<test::minimal::assignable,
    test::minimal::hash<test::minimal::assignable>,
    test::minimal::equal_to<test::minimal::assignable>,
    test::minimal::allocator<test::minimal::assignable> >
    multiset;

  unordered_equivalent_test(multiset, assignable);
  unordered_set_test(multiset, assignable);
  unordered_copyable_test(multiset, assignable, assignable, hash, equal_to);
  unordered_set_member_test(multiset, assignable);
}

UNORDERED_AUTO_TEST (movable1_tests) {
  test::minimal::constructor_param x;

  test::minimal::movable1 movable1(x);
  test::minimal::hash<test::minimal::movable1> hash(x);
  test::minimal::equal_to<test::minimal::movable1> equal_to(x);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_set.\n";

  UNORDERED_NAMESPACE::unordered_set<test::minimal::movable1,
    test::minimal::hash<test::minimal::movable1>,
    test::minimal::equal_to<test::minimal::movable1>,
    test::minimal::allocator<test::minimal::movable1> >
    set;

  // unordered_unique_test(set, movable1);
  unordered_set_test(set, movable1);
  unordered_movable_test(set, movable1, movable1, hash, equal_to);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_multiset.\n";

  UNORDERED_NAMESPACE::unordered_multiset<test::minimal::movable1,
    test::minimal::hash<test::minimal::movable1>,
    test::minimal::equal_to<test::minimal::movable1>,
    test::minimal::allocator<test::minimal::movable1> >
    multiset;

  // unordered_equivalent_test(multiset, movable1);
  unordered_set_test(multiset, movable1);
  unordered_movable_test(multiset, movable1, movable1, hash, equal_to);
}

UNORDERED_AUTO_TEST (movable2_tests) {
  test::minimal::constructor_param x;

  test::minimal::movable2 movable2(x);
  test::minimal::hash<test::minimal::movable2> hash(x);
  test::minimal::equal_to<test::minimal::movable2> equal_to(x);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_set.\n";

  UNORDERED_NAMESPACE::unordered_set<test::minimal::movable2,
    test::minimal::hash<test::minimal::movable2>,
    test::minimal::equal_to<test::minimal::movable2>,
    test::minimal::allocator<test::minimal::movable2> >
    set;

  // unordered_unique_test(set, movable2);
  unordered_set_test(set, movable2);
  unordered_movable_test(set, movable2, movable2, hash, equal_to);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_multiset.\n";

  UNORDERED_NAMESPACE::unordered_multiset<test::minimal::movable2,
    test::minimal::hash<test::minimal::movable2>,
    test::minimal::equal_to<test::minimal::movable2>,
    test::minimal::allocator<test::minimal::movable2> >
    multiset;

  // unordered_equivalent_test(multiset, movable2);
  unordered_set_test(multiset, movable2);
  unordered_movable_test(multiset, movable2, movable2, hash, equal_to);
}

UNORDERED_AUTO_TEST (destructible_tests) {
  test::minimal::constructor_param x;

  test::minimal::destructible destructible(x);
  test::minimal::hash<test::minimal::destructible> hash(x);
  test::minimal::equal_to<test::minimal::destructible> equal_to(x);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_set.\n";

  UNORDERED_NAMESPACE::unordered_set<test::minimal::destructible,
    test::minimal::hash<test::minimal::destructible>,
    test::minimal::equal_to<test::minimal::destructible> >
    set;

  unordered_destructible_test(set);

  BOOST_LIGHTWEIGHT_TEST_OSTREAM << "Test unordered_multiset.\n";

  UNORDERED_NAMESPACE::unordered_multiset<test::minimal::destructible,
    test::minimal::hash<test::minimal::destructible>,
    test::minimal::equal_to<test::minimal::destructible> >
    multiset;

  unordered_destructible_test(multiset);
}

// Test for ambiguity when using key convertible from iterator
// See LWG2059

#if UNORDERED_TEST_STD
#include <boost/functional/hash.hpp>
#endif

struct lwg2059_key
{
  int value;

  template <typename T> lwg2059_key(T v) : value(v) {}
};

std::size_t hash_value(lwg2059_key x)
{
  return static_cast<std::size_t>(x.value);
}

bool operator==(lwg2059_key x, lwg2059_key y) { return x.value == y.value; }

UNORDERED_AUTO_TEST (lwg2059) {
  {
    UNORDERED_NAMESPACE::unordered_set<lwg2059_key, boost::hash<lwg2059_key> >
      x;
    x.emplace(lwg2059_key(10));
    x.erase(x.begin());
  }

  {
    UNORDERED_NAMESPACE::unordered_multiset<lwg2059_key,
      boost::hash<lwg2059_key> >
      x;
    x.emplace(lwg2059_key(10));
    x.erase(x.begin());
  }
}

RUN_TESTS()
