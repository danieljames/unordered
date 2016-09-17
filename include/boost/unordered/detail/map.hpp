
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
            typedef typename boost::unordered::detail::pair_traits<ValueType>::first_type const const_key_type;
            typedef typename boost::unordered::detail::pair_traits<ValueType>::second_type mapped_type;
            typedef boost::unordered::detail::map_extractor<ValueType> extractor;
        };
    };

    struct map_policy : boost::unordered::detail::u
    {
        typedef map_iterator_policy set_map_policies;

        template <typename H, typename P, typename A>
        struct table_gen {
            typedef boost::unordered::detail::table_impl<map_policy, H, P, A> table;
        };
    };

#if BOOST_UNORDERED_INTEROPERABLE_NODES
    struct multimap_policy : boost::unordered::detail::u
#else
    struct multimap_policy : boost::unordered::detail::g
#endif
    {
        typedef map_iterator_policy set_map_policies;

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

        typedef boost::unordered::detail::u p;
        typedef boost::unordered::node_handle_map<p, K, M, A> node_type;
        typedef boost::unordered::insert_return_type_map<p, K, M, A> insert_return_type;
    };

    template <typename K, typename M, typename H, typename P, typename A>
    struct multimap_base {
        typedef std::pair<K const, M> value_type;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, value_type>::type value_allocator;

        typedef boost::unordered::detail::grouped_table_impl<
            multimap_policy, H, P, value_allocator> base;

#if BOOST_UNORDERED_INTEROPERABLE_NODES
        typedef boost::unordered::detail::u p;
#else
        typedef boost::unordered::detail::g p;
#endif
        typedef boost::unordered::node_handle_map<p, K, M, A> node_type;
    };

    template <typename K, typename M, typename H, typename P, typename A>
    class instantiate_map
    {
        typedef boost::unordered_map<K, M, H, P, A> container;
        container x;
        typename container::node_type node_type;
        typename container::insert_return_type insert_return_type;
    };

    template <typename K, typename M, typename H, typename P, typename A>
    class instantiate_multimap
    {
        typedef boost::unordered_multimap<K, M, H, P, A> container;
        container x;
        typename container::node_type node_type;
    };
}}}
