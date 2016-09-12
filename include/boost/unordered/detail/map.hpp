
// Copyright (C) 2005-2016 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/unordered/unordered_map_fwd.hpp>
#include <boost/unordered/detail/equivalent.hpp>
#include <boost/unordered/detail/unique.hpp>

namespace boost { namespace unordered { namespace detail {
    struct map_iterator_policy
    {
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

    struct map_policy
    {
        typedef map_iterator_policy iterator_policy;
        typedef boost::unordered::detail::u node_policy;
    };

    struct multimap_policy
    {
        typedef map_iterator_policy iterator_policy;
        typedef boost::unordered::detail::g node_policy;
    };

    template <typename A>
    struct map
    {
        typedef A value_allocator;
        typedef boost::unordered::detail::allocator_traits<A> value_allocator_traits;
        typedef typename value_allocator_traits::value_type value_type;
        typedef typename value_type::first_type key_type2;

        typedef map_policy container_policy;

        typedef boost::unordered::detail::map_extractor<value_type> extractor;

        typedef typename boost::unordered::detail::pick_policy<key_type2>::type policy;

        template <typename H, typename P>
        struct table_gen {
            typedef boost::unordered::detail::table_impl<map<A>, H, P> table;
        };
    };

    template <typename K, typename M, typename H, typename P, typename A>
    struct map_base {
        typedef std::pair<K const, M> value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::table_impl<
            map<value_allocator>,
            H, P> base;
    };

    template <typename A>
    struct multimap
    {
        typedef A value_allocator;
        typedef boost::unordered::detail::allocator_traits<A> value_allocator_traits;
        typedef typename value_allocator_traits::value_type value_type;
        typedef typename value_type::first_type key_type2;

        typedef multimap_policy container_policy;

        typedef boost::unordered::detail::map_extractor<value_type> extractor;

        typedef typename boost::unordered::detail::pick_policy<key_type2>::type policy;

        template <typename H, typename P>
        struct table_gen {
            typedef boost::unordered::detail::grouped_table_impl<multimap<A>, H, P> table;
        };
    };

    template <typename K, typename M, typename H, typename P, typename A>
    struct multimap_base {
        typedef std::pair<K const, M> value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::grouped_table_impl<
            multimap<value_allocator>,
            H, P> base;
    };
}}}
