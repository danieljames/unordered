
// Copyright (C) 2005-2016 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered/unordered_map_fwd.hpp>
#include <boost/unordered/detail/equivalent.hpp>
#include <boost/unordered/detail/unique.hpp>

namespace boost { namespace unordered { namespace detail {
    template <typename A, typename K, typename M, typename H, typename P>
    struct map
    {
        typedef boost::unordered::detail::map<A, K, M, H, P> types;

        typedef std::pair<K const, M> value_type;
        typedef H hasher;
        typedef P key_equal;
        typedef K key_type;

        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;
        typedef boost::unordered::detail::allocator_traits<value_allocator>
            value_allocator_traits;

        struct iterator_policy {
            typedef boost::unordered::detail::u node_policy;

            template <typename Node>
            struct iterators
            {
                typedef boost::unordered::iterator_detail::
                    iterator<Node> iterator;
                typedef boost::unordered::iterator_detail::
                    c_iterator<Node> c_iterator;
            };

            template <typename Node, typename Policy>
            struct local_iterators
            {
                typedef boost::unordered::iterator_detail::
                    l_iterator<Node, Policy> iterator;
                typedef boost::unordered::iterator_detail::
                    cl_iterator<Node, Policy> c_iterator;
            };
        };

        typedef boost::unordered::detail::pick_node<value_allocator> pick;
        typedef typename pick::node node;
        typedef typename pick::bucket bucket;
        typedef typename pick::link_pointer link_pointer;

        typedef boost::unordered::detail::table_impl<types> table;
        typedef boost::unordered::detail::map_extractor<key_type, value_type>
            extractor;

        typedef typename boost::unordered::detail::pick_policy<K>::type policy;

        typedef boost::unordered::iterator_detail::
            iterator<node> iterator;
        typedef boost::unordered::iterator_detail::
            c_iterator<node> c_iterator;
        typedef boost::unordered::iterator_detail::
            l_iterator<node, policy> l_iterator;
        typedef boost::unordered::iterator_detail::
            cl_iterator<node, policy> cl_iterator;

        typedef boost::unordered::detail::policy_base<
            iterator_policy,
            value_allocator,
            policy> table_base;
    };

    template <typename A, typename K, typename M, typename H, typename P>
    struct multimap
    {
        typedef boost::unordered::detail::multimap<A, K, M, H, P> types;

        typedef std::pair<K const, M> value_type;
        typedef H hasher;
        typedef P key_equal;
        typedef K key_type;

        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;
        typedef boost::unordered::detail::allocator_traits<value_allocator>
            value_allocator_traits;

        struct iterator_policy {
            typedef boost::unordered::detail::g node_policy;

            template <typename Node>
            struct iterators
            {
                typedef boost::unordered::iterator_detail::
                    iterator<Node> iterator;
                typedef boost::unordered::iterator_detail::
                    c_iterator<Node> c_iterator;
            };

            template <typename Node, typename Policy>
            struct local_iterators
            {
                typedef boost::unordered::iterator_detail::
                    l_iterator<Node, Policy> iterator;
                typedef boost::unordered::iterator_detail::
                    cl_iterator<Node, Policy> c_iterator;
            };
        };

        typedef boost::unordered::detail::pick_grouped_node<value_allocator> pick;
        typedef typename pick::node node;
        typedef typename pick::bucket bucket;
        typedef typename pick::link_pointer link_pointer;

        typedef boost::unordered::detail::grouped_table_impl<types> table;
        typedef boost::unordered::detail::map_extractor<key_type, value_type>
            extractor;

        typedef typename boost::unordered::detail::pick_policy<K>::type policy;

        typedef boost::unordered::iterator_detail::
            iterator<node> iterator;
        typedef boost::unordered::iterator_detail::
            c_iterator<node> c_iterator;
        typedef boost::unordered::iterator_detail::
            l_iterator<node, policy> l_iterator;
        typedef boost::unordered::iterator_detail::
            cl_iterator<node, policy> cl_iterator;

        typedef boost::unordered::detail::policy_base<
            iterator_policy,
            value_allocator,
            policy> table_base;
    };
}}}
