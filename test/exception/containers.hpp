
// Copyright 2006-2009 Daniel James.
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
#include "../objects/exception.hpp"

typedef UNORDERED_NAMESPACE::unordered_set<test::exception::object,
  test::exception::hash, test::exception::equal_to,
  test::exception::allocator<test::exception::object> >
  test_set;
typedef UNORDERED_NAMESPACE::unordered_multiset<test::exception::object,
  test::exception::hash, test::exception::equal_to,
  test::exception::allocator2<test::exception::object> >
  test_multiset;
typedef UNORDERED_NAMESPACE::unordered_map<test::exception::object,
  test::exception::object, test::exception::hash, test::exception::equal_to,
  test::exception::allocator2<
    std::pair<test::exception::object const, test::exception::object> > >
  test_map;
typedef UNORDERED_NAMESPACE::unordered_multimap<test::exception::object,
  test::exception::object, test::exception::hash, test::exception::equal_to,
  test::exception::allocator<
    std::pair<test::exception::object const, test::exception::object> > >
  test_multimap;
typedef UNORDERED_NAMESPACE::unordered_set<
  std::pair<test::exception::object, test::exception::object>,
  test::exception::hash, test::exception::equal_to,
  test::exception::allocator<
    std::pair<test::exception::object, test::exception::object> > >
  test_pair_set;
typedef UNORDERED_NAMESPACE::unordered_multiset<
  std::pair<test::exception::object, test::exception::object>,
  test::exception::hash, test::exception::equal_to,
  test::exception::allocator2<
    std::pair<test::exception::object, test::exception::object> > >
  test_pair_multiset;

#define CONTAINER_SEQ (test_set)(test_multiset)(test_map)(test_multimap)
#define CONTAINER_PAIR_SEQ                                                     \
  (test_pair_set)(test_pair_multiset)(test_map)(test_multimap)
