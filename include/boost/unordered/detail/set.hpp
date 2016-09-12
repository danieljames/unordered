
// Copyright (C) 2005-2016 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered/unordered_set_fwd.hpp>
#include <boost/unordered/detail/equivalent.hpp>
#include <boost/unordered/detail/unique.hpp>

namespace boost { namespace unordered { namespace detail {
    template <typename A, typename T>
    struct set
    {
        typedef T value_type;
        typedef T key_type;

        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        struct iterator_policy {
            typedef boost::unordered::detail::u node_policy;

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

        typedef boost::unordered::detail::set_extractor<value_type> extractor;

        typedef typename boost::unordered::detail::pick_policy<T>::type policy;

        typedef boost::unordered::detail::policy_base<
            iterator_policy,
            value_allocator,
            policy> table_base;

        template <typename H, typename P>
        struct table_gen {
            typedef boost::unordered::detail::table_impl<set<A,T>, H, P> table;
        };
    };

    template <typename T, typename H, typename P, typename A>
    struct set_base {
        typedef T value_type;
        typedef T key_type;

        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::table_impl<
            set<value_allocator, T>,
            H, P> base;
    };

    template <typename A, typename T>
    struct multiset
    {
        typedef T value_type;
        typedef T key_type;

        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        struct iterator_policy {
            typedef boost::unordered::detail::g node_policy;

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

        typedef boost::unordered::detail::set_extractor<value_type> extractor;

        typedef typename boost::unordered::detail::pick_policy<T>::type policy;

        typedef boost::unordered::detail::policy_base<
            iterator_policy,
            value_allocator,
            policy> table_base;

        template <typename H, typename P>
        struct table_gen {
            typedef boost::unordered::detail::grouped_table_impl<multiset<A,T>, H, P> table;
        };
    };

    template <typename T, typename H, typename P, typename A>
    struct multiset_base {
        typedef T value_type;
        typedef T key_type;

        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::grouped_table_impl<
            multiset<value_allocator, T>,
            H, P> base;
    };
}}}
