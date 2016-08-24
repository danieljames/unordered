
// Copyright 2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(_WIN32_WCE)
// The standard windows mobile headers trigger this warning so I disable it
// before doing anything else.
#pragma warning(disable : 4201) // nonstandard extension used :
                                // nameless struct/union
#endif

#if !defined(UNORDERED_TEST_STD)
#define UNORDERED_TEST_STD 0
#endif

#if UNORDERED_TEST_STD
#define UNORDERED_NAMESPACE std
#define UNORDERED_PIECEWISE_T std::piecewise_construct_t
#define UNORDERED_PIECEWISE std::piecewise_construct
#define UNORDERED_TEST_LOOSE 1
#else
#define UNORDERED_NAMESPACE boost
#define UNORDERED_PIECEWISE_T boost::unordered::piecewise_construct_t
#define UNORDERED_PIECEWISE boost::unordered::piecewise_construct
#define UNORDERED_TEST_LOOSE 0
#endif
