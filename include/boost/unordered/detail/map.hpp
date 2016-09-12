
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

        template <typename ValueType>
        struct value_things
        {
            typedef typename ValueType::first_type key_type2;
            typedef boost::unordered::detail::map_extractor<ValueType> extractor;
        };
    };

    struct map_policy
    {
        typedef map_iterator_policy iterator_policy;
        typedef boost::unordered::detail::u node_policy;

        template <typename H, typename P, typename A>
        struct table_gen {
            typedef boost::unordered::detail::table_impl<map_policy, H, P, A> table;
        };
    };

    struct multimap_policy
    {
        typedef map_iterator_policy iterator_policy;
        typedef boost::unordered::detail::g node_policy;

        template <typename H, typename P, typename A>
        struct table_gen {
            typedef boost::unordered::detail::grouped_table_impl<multimap_policy, H, P, A> table;
        };
    };

    template <typename K, typename M, typename H, typename P, typename A>
    struct map_base {
        typedef std::pair<K const, M> value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::table_impl<
            map_policy, H, P, value_allocator> base;
    };

    template <typename K, typename M, typename H, typename P, typename A>
    struct multimap_base {
        typedef std::pair<K const, M> value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::grouped_table_impl<
            multimap_policy, H, P, value_allocator> base;
    };
}}}
