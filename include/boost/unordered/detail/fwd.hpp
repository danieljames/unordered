
// Copyright (C) 2008-2011 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNORDERED_FWD_HPP_INCLUDED
#define BOOST_UNORDERED_FWD_HPP_INCLUDED

#include <boost/config.hpp>
#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

// Just a guess......
#if defined(BOOST_LIBSTDCXX11) && BOOST_LIBSTDCXX_VERSION > 40700
#define BOOST_UNORDERED_HAVE_PIECEWISE_CONSTRUCT 1
#elif defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 3500 && __cplusplus >= 201103
#define BOOST_UNORDERED_HAVE_PIECEWISE_CONSTRUCT 0
#endif

#if BOOST_UNORDERED_HAVE_PIECEWISE_CONSTRUCT
#include <utility>
#endif

namespace boost
{
namespace unordered
{
#if BOOST_UNORDERED_HAVE_PIECEWISE_CONSTRUCT
    using std::piecewise_construct_t;
    using std::piecewise_construct;
#else
    struct piecewise_construct_t {};
    const piecewise_construct_t piecewise_construct = piecewise_construct_t();
#endif
}
}

#endif
