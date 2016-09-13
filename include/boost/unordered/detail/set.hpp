
// Copyright (C) 2005-2016 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered/unordered_set_fwd.hpp>
#include <boost/unordered/detail/equivalent.hpp>
#include <boost/unordered/detail/unique.hpp>

namespace boost { namespace unordered { namespace detail {
    struct set_iterator_policy
    {
        template <typename Node>
        struct iterators
        {
            typedef boost::unordered::iterator_detail::
                c_iterator<Node> iterator;
            typedef boost::unordered::iterator_detail::
                c_iterator<Node> c_iterator;
        };

        template <typename Node, typename Policy>
        struct local_iterators
        {
            typedef boost::unordered::iterator_detail::
                cl_iterator<Node, Policy> iterator;
            typedef boost::unordered::iterator_detail::
                cl_iterator<Node, Policy> c_iterator;
        };

        template <typename ValueType>
        struct value_things
        {
            typedef ValueType const const_key_type;
            typedef boost::unordered::detail::set_extractor<ValueType> extractor;
        };
    };

    struct set_policy : boost::unordered::detail::u
    {
        typedef set_iterator_policy set_map_policies;

        template <typename H, typename P, typename A>
        struct table_gen {
            typedef boost::unordered::detail::table_impl<set_policy, H, P, A> table;
        };
    };

    struct multiset_policy : boost::unordered::detail::g
    {
        typedef set_iterator_policy set_map_policies;

        template <typename H, typename P, typename A>
        struct table_gen {
            typedef boost::unordered::detail::grouped_table_impl<multiset_policy, H, P, A> table;
        };
    };

    template <typename T, typename H, typename P, typename A>
    struct set_base {
        typedef T value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::table_impl<
            set_policy, H, P, value_allocator> base;
    };

    template <typename T, typename H, typename P, typename A>
    struct multiset_base {
        typedef T value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::grouped_table_impl<
            multiset_policy, H, P, value_allocator> base;
    };
}}}
