
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
    };

    struct set_policy
    {
        typedef set_iterator_policy iterator_policy;
        typedef boost::unordered::detail::u node_policy;
    };

    struct multiset_policy
    {
        typedef set_iterator_policy iterator_policy;
        typedef boost::unordered::detail::g node_policy;
    };

    template <typename A>
    struct set
    {
        typedef A value_allocator;
        typedef boost::unordered::detail::allocator_traits<A> value_allocator_traits;
        typedef typename value_allocator_traits::value_type value_type;
        typedef value_type key_type2;

        typedef set_policy container_policy;
        typedef boost::unordered::detail::set_extractor<value_type> extractor;

        typedef typename boost::unordered::detail::pick_policy<value_type>::type policy;

        template <typename H, typename P>
        struct table_gen {
            typedef boost::unordered::detail::table_impl<set<A>, H, P, A> table;
        };
    };

    template <typename T, typename H, typename P, typename A>
    struct set_base {
        typedef T value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::table_impl<
            set<value_allocator>,
            H, P, value_allocator> base;
    };

    template <typename A>
    struct multiset
    {
        typedef A value_allocator;
        typedef boost::unordered::detail::allocator_traits<A> value_allocator_traits;
        typedef typename value_allocator_traits::value_type value_type;
        typedef value_type key_type2;

        typedef multiset_policy container_policy;
        typedef boost::unordered::detail::set_extractor<value_type> extractor;

        typedef typename boost::unordered::detail::pick_policy<value_type>::type policy;

        template <typename H, typename P>
        struct table_gen {
            typedef boost::unordered::detail::grouped_table_impl<multiset<A>, H, P, A> table;
        };
    };

    template <typename T, typename H, typename P, typename A>
    struct multiset_base {
        typedef T value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::grouped_table_impl<
            multiset<value_allocator>,
            H, P, value_allocator> base;
    };
}}}
