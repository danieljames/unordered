
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
#include <boost/detail/workaround.hpp>

void foo(UNORDERED_NAMESPACE::unordered_set<int>& x1,
  UNORDERED_NAMESPACE::unordered_map<int, int>& x2,
  UNORDERED_NAMESPACE::unordered_multiset<int>& x3,
  UNORDERED_NAMESPACE::unordered_multimap<int, int>& x4)
{
#if BOOST_WORKAROUND(__CODEGEARC__, BOOST_TESTED_AT(0x0613))
  struct dummy
  {
    UNORDERED_NAMESPACE::unordered_set<int> x1;
    UNORDERED_NAMESPACE::unordered_map<int, int> x2;
    UNORDERED_NAMESPACE::unordered_multiset<int> x3;
    UNORDERED_NAMESPACE::unordered_multimap<int, int> x4;
  };
#endif

  x1.insert(1);
  x2[2] = 2;
  x3.insert(3);
  x4.insert(std::make_pair(4, 5));
}
