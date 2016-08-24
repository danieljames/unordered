
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

void foo(UNORDERED_NAMESPACE::unordered_set<int>&,
        UNORDERED_NAMESPACE::unordered_map<int, int>&,
        UNORDERED_NAMESPACE::unordered_multiset<int>&,
        UNORDERED_NAMESPACE::unordered_multimap<int, int>&);

int main()
{
    UNORDERED_NAMESPACE::unordered_set<int> x1;
    UNORDERED_NAMESPACE::unordered_map<int, int> x2;
    UNORDERED_NAMESPACE::unordered_multiset<int> x3;
    UNORDERED_NAMESPACE::unordered_multimap<int, int> x4;

    foo(x1, x2, x3, x4);
    
    return 0;
}
