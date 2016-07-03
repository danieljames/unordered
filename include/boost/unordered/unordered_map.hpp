
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2011 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/unordered for documentation

#ifndef BOOST_UNORDERED_UNORDERED_MAP_HPP_INCLUDED
#define BOOST_UNORDERED_UNORDERED_MAP_HPP_INCLUDED

#include <boost/config.hpp>
#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <boost/unordered/detail/map.hpp>
#include <boost/functional/hash.hpp>
#include <boost/move/move.hpp>
#include <boost/core/explicit_operator_bool.hpp>

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
#include <initializer_list>
#endif

#if defined(BOOST_MSVC)
#pragma warning(push)
#if BOOST_MSVC >= 1400
#pragma warning(disable:4396) //the inline specifier cannot be used when a
                              // friend declaration refers to a specialization
                              // of a function template
#endif
#endif

namespace boost
{
namespace unordered
{
    template <class K, class T, class H, class P, class A>
    class unordered_map :
        public boost::unordered::detail::map_base<K, T, H, P, A>::base
    {
#if defined(BOOST_UNORDERED_USE_MOVE)
        BOOST_COPYABLE_AND_MOVABLE(unordered_map)
#endif       

    public:

        typedef K key_type;
        typedef std::pair<const K, T> value_type;
        typedef T mapped_type;
        typedef H hasher;
        typedef P key_equal;
        typedef A allocator_type;

    private:

        typedef boost::unordered::detail::map_base<K, T, H, P, A> types;
        typedef typename types::base table;
        typedef typename table::value_allocator_traits value_allocator_traits;

    public:

        typedef typename value_allocator_traits::pointer pointer;
        typedef typename value_allocator_traits::const_pointer const_pointer;

        typedef value_type& reference;
        typedef value_type const& const_reference;

        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        typedef typename table::const_iterator const_iterator;
        typedef typename table::iterator iterator;
        typedef typename types::node_type node_type;
        typedef typename types::insert_return_type insert_return_type;

        // constructors

        explicit unordered_map(
                size_type = boost::unordered::detail::default_bucket_count,
                const hasher& = hasher(),
                const key_equal& = key_equal(),
                const allocator_type& = allocator_type());

        explicit unordered_map(allocator_type const&);

        template <class InputIt>
        unordered_map(InputIt, InputIt);

        template <class InputIt>
        unordered_map(
                InputIt, InputIt,
                size_type,
                const hasher& = hasher(),
                const key_equal& = key_equal());

        template <class InputIt>
        unordered_map(
                InputIt, InputIt,
                size_type,
                const hasher&,
                const key_equal&,
                const allocator_type&);

        template <class InputIt>
        unordered_map(
                InputIt, InputIt,
                size_type,
                const hasher&,
                const allocator_type&);

        template <class InputIt>
        unordered_map(
                InputIt, InputIt,
                size_type,
                const allocator_type&);

        // copy/move constructors

        unordered_map(unordered_map const&);

        unordered_map(unordered_map const&, allocator_type const&);

#if defined(BOOST_UNORDERED_USE_MOVE)
        unordered_map(BOOST_RV_REF(unordered_map) other)
                BOOST_NOEXCEPT_IF(table::nothrow_move_constructible)
            : table(other, boost::unordered::detail::move_tag())
        {
        }
#elif !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unordered_map(unordered_map&& other)
                BOOST_NOEXCEPT_IF(table::nothrow_move_constructible)
            : table(other, boost::unordered::detail::move_tag())
        {
        }
#endif

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unordered_map(unordered_map&&, allocator_type const&);
#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
        unordered_map(
                std::initializer_list<value_type>,
                size_type = boost::unordered::detail::default_bucket_count,
                const hasher& = hasher(),
                const key_equal&l = key_equal(),
                const allocator_type& = allocator_type());
        unordered_map(
                std::initializer_list<value_type>,
                size_type,
                const hasher&,
                const allocator_type&);
        unordered_map(
                std::initializer_list<value_type>,
                size_type,
                const allocator_type&);
#endif

        // Destructor

        ~unordered_map() BOOST_NOEXCEPT;

        // Assign

#if defined(BOOST_UNORDERED_USE_MOVE)
        unordered_map& operator=(BOOST_COPY_ASSIGN_REF(unordered_map) x)
        {
            this->assign(x);
            return *this;
        }

        unordered_map& operator=(BOOST_RV_REF(unordered_map) x)
        {
            this->move_assign(x);
            return *this;
        }
#else
        unordered_map& operator=(unordered_map const& x)
        {
            this->assign(x);
            return *this;
        }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unordered_map& operator=(unordered_map&& x)
        {
            this->move_assign(x);
            return *this;
        }
#endif
#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
        unordered_map& operator=(std::initializer_list<value_type>);
#endif

        allocator_type get_allocator() const BOOST_NOEXCEPT
        {
            return this->node_alloc();
        }

        // extract

        node_type extract(const_iterator position)
        {
            return node_type(this->extract_by_iterator(position), this->node_alloc());
        }

        node_type extract(const key_type& k)
        {
            return node_type(this->extract_by_key(k), this->node_alloc());
        }

        // emplace

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
        template <class... Args>
        std::pair<iterator, bool> emplace(BOOST_FWD_REF(Args)... args)
        {
            return this->emplace_(boost::forward<Args>(args)...);
        }

        template <class... Args>
        iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(Args)... args)
        {
            return this->emplace_hint_(hint, boost::forward<Args>(args)...);
        }

        template <class... Args>
        std::pair<iterator, bool> try_emplace(key_type const& k, BOOST_FWD_REF(Args)... args)
        {
            return this->try_emplace_(k, boost::forward<Args>(args)...);
        }

        template <class... Args>
        iterator try_emplace(const_iterator hint, key_type const& k, BOOST_FWD_REF(Args)... args)
        {
            return this->try_emplace_hint_(hint, k, boost::forward<Args>(args)...);
        }

        template <class... Args>
        std::pair<iterator, bool> try_emplace(BOOST_RV_REF(key_type) k, BOOST_FWD_REF(Args)... args)
        {
            return this->try_emplace_(boost::move(k), boost::forward<Args>(args)...);
        }

        template <class... Args>
        iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k, BOOST_FWD_REF(Args)... args)
        {
            return this->try_emplace_hint_(hint, boost::move(k), boost::forward<Args>(args)...);
        }
#else

#if !BOOST_WORKAROUND(__SUNPRO_CC, BOOST_TESTED_AT(0x5100))

        // 0 argument emplace requires special treatment in case
        // the container is instantiated with a value type that
        // doesn't have a default constructor.

        std::pair<iterator, bool> emplace(
                boost::unordered::detail::empty_emplace
                    = boost::unordered::detail::empty_emplace(),
                value_type v = value_type())
        {
            return this->emplace(boost::move(v));
        }

        iterator emplace_hint(const_iterator hint,
                boost::unordered::detail::empty_emplace
                    = boost::unordered::detail::empty_emplace(),
                value_type v = value_type()
            )
        {
            return this->emplace_hint(hint, boost::move(v));
        }

#endif

        template <typename Key>
        std::pair<iterator, bool> try_emplace(BOOST_FWD_REF(Key) k)
        {
            return this->try_emplace_(boost::forward<Key>(k));
        }

        template <typename Key>
        iterator try_emplace(const_iterator hint, BOOST_FWD_REF(Key) k)
        {
            return this->try_emplace_hint_(hint, boost::forward<Key>(k));
        }

        template <typename A0>
        std::pair<iterator, bool> emplace(BOOST_FWD_REF(A0) a0)
        {
            return this->emplace_(
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0))
            );
        }

        template <typename A0>
        iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(A0) a0)
        {
            return this->emplace_hint_(hint,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0))
            );
        }

        template <typename A0>
        std::pair<iterator, bool> try_emplace(key_type const& k,
            BOOST_FWD_REF(A0) a0)
        {
            return this->try_emplace_(k,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0))
            );
        }

        template <typename A0>
        iterator try_emplace(const_iterator hint, key_type const& k,
            BOOST_FWD_REF(A0) a0)
        {
            return this->try_emplace_hint_(hint, k,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0))
            );
        }

        template <typename A0>
        std::pair<iterator, bool> try_emplace(BOOST_RV_REF(key_type) k,
            BOOST_FWD_REF(A0) a0)
        {
            return this->try_emplace_(boost::move(k), boost::forward<A0>(a0));
        }

        template <typename A0>
        iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k,
            BOOST_FWD_REF(A0) a0)
        {
            return this->try_emplace_hint_(hint, boost::move(k), boost::forward<A0>(a0));
        }

        template <typename A0, typename A1>
        std::pair<iterator, bool> emplace(
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->emplace_(
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1))
            );
        }

        template <typename A0, typename A1>
        iterator emplace_hint(const_iterator hint,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->emplace_hint_(hint,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1))
            );
        }

        template <typename A0, typename A1>
        std::pair<iterator, bool> try_emplace(key_type const& k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->try_emplace_(k,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1)));
        }

        template <typename A0, typename A1>
        iterator try_emplace(const_iterator hint, key_type const& k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->try_emplace_hint_(hint, k,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1)));
        }

        template <typename A0, typename A1>
        std::pair<iterator, bool> try_emplace(BOOST_RV_REF(key_type) k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->try_emplace_(boost::move(k),
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1)));
        }

        template <typename A0, typename A1>
        iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->try_emplace_hint_(hint, boost::move(k),
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1)));
        }

        template <typename A0, typename A1, typename A2>
        std::pair<iterator, bool> emplace(
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->emplace_(
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2))
            );
        }

        template <typename A0, typename A1, typename A2>
        iterator emplace_hint(const_iterator hint,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->emplace_hint_(hint,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2))
            );
        }

        template <typename A0, typename A1, typename A2>
        std::pair<iterator, bool> try_emplace(key_type const& k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->try_emplace_(k,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2)));
        }

        template <typename A0, typename A1, typename A2>
        iterator try_emplace(const_iterator hint, key_type const& k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->try_emplace_impl_(hint, k,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2))).first;
        }

        template <typename A0, typename A1, typename A2>
        std::pair<iterator, bool> try_emplace(BOOST_RV_REF(key_type) k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->try_emplace_(boost::move(k),
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2)));
        }

        template <typename A0, typename A1, typename A2>
        iterator try_emplace(const_iterator hint, BOOST_RV_REF(key_type) k,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->try_emplace_hint_(hint, boost::move(k),
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2)));
        }

#define BOOST_UNORDERED_EMPLACE(z, n, _)                                    \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            std::pair<iterator, bool> emplace(                              \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->emplace_(                                      \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }                                                               \
                                                                            \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            iterator emplace_hint(                                          \
                    const_iterator hint,                                    \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->emplace_hint_(hint,                            \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }                                                               \
                                                                            \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            std::pair<iterator, bool> try_emplace(                          \
                    key_type const& k,                                      \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->try_emplace_(k,                                \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }                                                               \
                                                                            \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            iterator try_emplace(                                           \
                    const_iterator hint,                                    \
                    key_type const& k,                                      \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->try_emplace_hint_(hint, k,                     \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }                                                               \
                                                                            \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            std::pair<iterator, bool> try_emplace(                          \
                    BOOST_RV_REF(key_type) k,                               \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->try_emplace_(boost::move(k),                   \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }                                                               \
                                                                            \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            iterator try_emplace(                                           \
                    const_iterator hint,                                    \
                    BOOST_RV_REF(key_type) k,                               \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->try_emplace_hint_(hint, boost::move(k),        \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }

        BOOST_PP_REPEAT_FROM_TO(4, BOOST_UNORDERED_EMPLACE_BOUND,
            BOOST_UNORDERED_EMPLACE, _)

#undef BOOST_UNORDERED_EMPLACE

#endif

        std::pair<iterator, bool> insert(value_type const& x)
        {
            return this->emplace(x);
        }

        std::pair<iterator, bool> insert(BOOST_RV_REF(value_type) x)
        {
            return this->emplace(boost::move(x));
        }

        iterator insert(const_iterator hint, value_type const& x)
        {
            return this->emplace_hint(hint, x);
        }

        iterator insert(const_iterator hint, BOOST_RV_REF(value_type) x)
        {
            return this->emplace_hint(hint, boost::move(x));
        }

        template <class M>
        std::pair<iterator, bool> insert_or_assign(key_type const& k, BOOST_FWD_REF(M) obj)
        {
            return this->insert_or_assign_(k, boost::forward<M>(obj));
        }

        template <class M>
        iterator insert_or_assign(const_iterator, key_type const& k, BOOST_FWD_REF(M) obj)
        {
            return this->insert_or_assign_(k, boost::forward<M>(obj)).first;
        }

        template <class M>
        std::pair<iterator, bool> insert_or_assign(BOOST_RV_REF(key_type) k, BOOST_FWD_REF(M) obj)
        {
            return this->insert_or_assign_(boost::move(k), boost::forward<M>(obj));
        }

        template <class M>
        iterator insert_or_assign(const_iterator, BOOST_RV_REF(key_type) k, BOOST_FWD_REF(M) obj)
        {
            return this->insert_or_assign_(boost::move(k), boost::forward<M>(obj)).first;
        }

        template <class InputIt> void insert(InputIt, InputIt);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
        void insert(std::initializer_list<value_type>);
#endif

        insert_return_type insert(BOOST_RV_REF(node_type) np) {
            insert_return_type result;
            this->move_insert_node_type(np, result);
            return boost::move(result);
        }

        iterator insert(const_iterator hint, BOOST_RV_REF(node_type) np) {
            return this->move_insert_node_type_with_hint(hint, np);
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    private:
        // Note: Use r-value node_type to insert.
        insert_return_type insert(node_type&);
        iterator insert(const_iterator, node_type& np);
    public:
#endif

        iterator erase(const_iterator);
        size_type erase(const key_type&);
        iterator erase(const_iterator, const_iterator);
        void quick_erase(const_iterator it) { erase(it); }
        void erase_return_void(const_iterator it) { erase(it); }

        void swap(unordered_map&);

        // observers

        mapped_type& operator[](const key_type&);
        mapped_type& at(const key_type&);
        mapped_type const& at(const key_type&) const;

        // lookup

        iterator find(const key_type&);
        const_iterator find(const key_type&) const;

        template <class CompatibleKey, class CompatibleHash,
            class CompatiblePredicate>
        iterator find(
                CompatibleKey const&,
                CompatibleHash const&,
                CompatiblePredicate const&);

        template <class CompatibleKey, class CompatibleHash,
            class CompatiblePredicate>
        const_iterator find(
                CompatibleKey const&,
                CompatibleHash const&,
                CompatiblePredicate const&) const;

        std::pair<iterator, iterator>
        equal_range(const key_type&);
        std::pair<const_iterator, const_iterator>
        equal_range(const key_type&) const;

        // bucket interface

        size_type bucket(const key_type& k) const
        {
            return this->hash_to_bucket(this->hash(k));
        }

#if !BOOST_WORKAROUND(__BORLANDC__, < 0x0582)
        friend bool operator==<K,T,H,P,A>(
                unordered_map const&, unordered_map const&);
        friend bool operator!=<K,T,H,P,A>(
                unordered_map const&, unordered_map const&);
#endif
    }; // class template unordered_map

    template <class K, class T, class H, class P, class A>
    class unordered_multimap :
        public boost::unordered::detail::multimap_base<K, T, H, P, A>::base
    {
#if defined(BOOST_UNORDERED_USE_MOVE)
        BOOST_COPYABLE_AND_MOVABLE(unordered_multimap)
#endif
    public:

        typedef K key_type;
        typedef std::pair<const K, T> value_type;
        typedef T mapped_type;
        typedef H hasher;
        typedef P key_equal;
        typedef A allocator_type;

    private:

        typedef boost::unordered::detail::multimap_base<K, T, H, P, A> types;
        typedef typename types::base table;
        typedef typename table::value_allocator_traits value_allocator_traits;

    public:

        typedef typename value_allocator_traits::pointer pointer;
        typedef typename value_allocator_traits::const_pointer const_pointer;

        typedef value_type& reference;
        typedef value_type const& const_reference;

        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        typedef typename table::const_iterator const_iterator;
        typedef typename table::iterator iterator;
        typedef typename types::node_type node_type;

        // constructors

        explicit unordered_multimap(
                size_type = boost::unordered::detail::default_bucket_count,
                const hasher& = hasher(),
                const key_equal& = key_equal(),
                const allocator_type& = allocator_type());
        explicit unordered_multimap(
                size_type,
                const allocator_type&);
        explicit unordered_multimap(
                size_type,
                const hasher&,
                const allocator_type&);

        explicit unordered_multimap(allocator_type const&);

        template <class InputIt>
        unordered_multimap(InputIt, InputIt);

        template <class InputIt>
        unordered_multimap(
                InputIt, InputIt,
                size_type,
                const hasher& = hasher(),
                const key_equal& = key_equal());

        template <class InputIt>
        unordered_multimap(
                InputIt, InputIt,
                size_type,
                const hasher&,
                const key_equal&,
                const allocator_type&);

        template <class InputIt>
        unordered_multimap(
                InputIt, InputIt,
                size_type,
                const hasher&,
                const allocator_type&);

        template <class InputIt>
        unordered_multimap(
                InputIt, InputIt,
                size_type,
                const allocator_type&);

        // copy/move constructors

        unordered_multimap(unordered_multimap const&);

        unordered_multimap(unordered_multimap const&, allocator_type const&);
        unordered_multimap(BOOST_RV_REF(unordered_multimap), allocator_type const&);

#if defined(BOOST_UNORDERED_USE_MOVE)
        unordered_multimap(BOOST_RV_REF(unordered_multimap) other)
                BOOST_NOEXCEPT_IF(table::nothrow_move_constructible)
            : table(other, boost::unordered::detail::move_tag())
        {
        }
#elif !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unordered_multimap(unordered_multimap&& other)
                BOOST_NOEXCEPT_IF(table::nothrow_move_constructible)
            : table(other, boost::unordered::detail::move_tag())
        {
        }
#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
        unordered_multimap(
                std::initializer_list<value_type>,
                size_type = boost::unordered::detail::default_bucket_count,
                const hasher& = hasher(),
                const key_equal&l = key_equal(),
                const allocator_type& = allocator_type());
        unordered_multimap(
                std::initializer_list<value_type>,
                size_type,
                const hasher&,
                const allocator_type&);
        unordered_multimap(
                std::initializer_list<value_type>,
                size_type,
                const allocator_type&);
#endif

        // Destructor

        ~unordered_multimap() BOOST_NOEXCEPT;

        // Assign

#if defined(BOOST_UNORDERED_USE_MOVE)
        unordered_multimap& operator=(
                BOOST_COPY_ASSIGN_REF(unordered_multimap) x)
        {
            this->assign(x);
            return *this;
        }

        unordered_multimap& operator=(BOOST_RV_REF(unordered_multimap) x)
        {
            this->move_assign(x);
            return *this;
        }
#else
        unordered_multimap& operator=(unordered_multimap const& x)
        {
            this->assign(x);
            return *this;
        }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        unordered_multimap& operator=(unordered_multimap&& x)
        {
            this->move_assign(x);
            return *this;
        }
#endif
#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
        unordered_multimap& operator=(std::initializer_list<value_type>);
#endif

        allocator_type get_allocator() const BOOST_NOEXCEPT
        {
            return this->node_alloc();
        }

        // extract

        node_type extract(const_iterator position)
        {
            return node_type(this->extract_by_iterator(position), this->node_alloc());
        }

        node_type extract(const key_type& k)
        {
            return node_type(this->extract_by_key(k), this->node_alloc());
        }

        // emplace

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
        template <class... Args>
        iterator emplace(BOOST_FWD_REF(Args)... args)
        {
            return this->emplace_(boost::forward<Args>(args)...);
        }

        template <class... Args>
        iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(Args)... args)
        {
            return this->emplace_hint_(hint, boost::forward<Args>(args)...);
        }
#else

#if !BOOST_WORKAROUND(__SUNPRO_CC, BOOST_TESTED_AT(0x5100))

        // 0 argument emplace requires special treatment in case
        // the container is instantiated with a value type that
        // doesn't have a default constructor.

        iterator emplace(
                boost::unordered::detail::empty_emplace
                    = boost::unordered::detail::empty_emplace(),
                value_type v = value_type())
        {
            return this->emplace(boost::move(v));
        }

        iterator emplace_hint(const_iterator hint,
                boost::unordered::detail::empty_emplace
                    = boost::unordered::detail::empty_emplace(),
                value_type v = value_type()
            )
        {
            return this->emplace_hint(hint, boost::move(v));
        }

#endif

        template <typename A0>
        iterator emplace(BOOST_FWD_REF(A0) a0)
        {
            return this->emplace_(
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0))
            );
        }

        template <typename A0>
        iterator emplace_hint(const_iterator hint, BOOST_FWD_REF(A0) a0)
        {
            return this->emplace_hint_(hint,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0))
            );
        }

        template <typename A0, typename A1>
        iterator emplace(
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->emplace_(
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1))
            );
        }

        template <typename A0, typename A1>
        iterator emplace_hint(const_iterator hint,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1)
        {
            return this->emplace_hint_(hint,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1))
            );
        }

        template <typename A0, typename A1, typename A2>
        iterator emplace(
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->emplace_(
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2))
            );
        }

        template <typename A0, typename A1, typename A2>
        iterator emplace_hint(const_iterator hint,
            BOOST_FWD_REF(A0) a0,
            BOOST_FWD_REF(A1) a1,
            BOOST_FWD_REF(A2) a2)
        {
            return this->emplace_hint_(hint,
                boost::unordered::detail::create_emplace_args(
                    boost::forward<A0>(a0),
                    boost::forward<A1>(a1),
                    boost::forward<A2>(a2))
            );
        }

#define BOOST_UNORDERED_EMPLACE(z, n, _)                                    \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            iterator emplace(                                               \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->emplace_(                                      \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }                                                               \
                                                                            \
            template <                                                      \
                BOOST_PP_ENUM_PARAMS_Z(z, n, typename A)                    \
            >                                                               \
            iterator emplace_hint(                                          \
                    const_iterator hint,                                    \
                    BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_FWD_PARAM, a)      \
            )                                                               \
            {                                                               \
                return this->emplace_hint_(hint,                            \
                    boost::unordered::detail::create_emplace_args(          \
                        BOOST_PP_ENUM_##z(n, BOOST_UNORDERED_CALL_FORWARD,  \
                            a)                                              \
                ));                                                         \
            }

        BOOST_PP_REPEAT_FROM_TO(4, BOOST_UNORDERED_EMPLACE_BOUND,
            BOOST_UNORDERED_EMPLACE, _)

#undef BOOST_UNORDERED_EMPLACE

#endif

        iterator insert(value_type const& x)
        {
            return this->emplace(x);
        }

        iterator insert(BOOST_RV_REF(value_type) x)
        {
            return this->emplace(boost::move(x));
        }

        iterator insert(const_iterator hint, value_type const& x)
        {
            return this->emplace_hint(hint, x);
        }

        iterator insert(const_iterator hint, BOOST_RV_REF(value_type) x)
        {
            return this->emplace_hint(hint, boost::move(x));
        }

        template <class InputIt> void insert(InputIt, InputIt);

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
        void insert(std::initializer_list<value_type>);
#endif

        iterator insert(BOOST_RV_REF(node_type) np) {
            return this->move_insert_node_type(np);
        }

        iterator insert(const_iterator hint, BOOST_RV_REF(node_type) np) {
            return this->move_insert_node_type_with_hint(hint, np);
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    private:
        // Note: Use r-value node_type to insert.
        iterator insert(node_type&);
        iterator insert(const_iterator, node_type& np);
    public:
#endif

        iterator erase(const_iterator);
        size_type erase(const key_type&);
        iterator erase(const_iterator, const_iterator);
        void quick_erase(const_iterator it) { erase(it); }
        void erase_return_void(const_iterator it) { erase(it); }

        void swap(unordered_multimap&);

        // lookup

        iterator find(const key_type&);
        const_iterator find(const key_type&) const;

        template <class CompatibleKey, class CompatibleHash,
            class CompatiblePredicate>
        iterator find(
                CompatibleKey const&,
                CompatibleHash const&,
                CompatiblePredicate const&);

        template <class CompatibleKey, class CompatibleHash,
            class CompatiblePredicate>
        const_iterator find(
                CompatibleKey const&,
                CompatibleHash const&,
                CompatiblePredicate const&) const;

        std::pair<iterator, iterator>
        equal_range(const key_type&);
        std::pair<const_iterator, const_iterator>
        equal_range(const key_type&) const;

        // bucket interface

        size_type bucket(const key_type& k) const
        {
            return this->hash_to_bucket(this->hash(k));
        }

#if !BOOST_WORKAROUND(__BORLANDC__, < 0x0582)
        friend bool operator==<K,T,H,P,A>(
                unordered_multimap const&, unordered_multimap const&);
        friend bool operator!=<K,T,H,P,A>(
                unordered_multimap const&, unordered_multimap const&);
#endif
    }; // class template unordered_multimap

////////////////////////////////////////////////////////////////////////////////

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(
            size_type n, const hasher &hf, const key_equal &eql,
            const allocator_type &a)
      : table(n, hf, eql, a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(allocator_type const& a)
      : table(boost::unordered::detail::default_bucket_count,
            hasher(), key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(
            unordered_map const& other, allocator_type const& a)
      : table(other, a)
    {
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K,T,H,P,A>::unordered_map(InputIt f, InputIt l)
      : table(boost::unordered::detail::initial_size(f, l),
        hasher(), key_equal(), allocator_type())
    {
        this->insert_range(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K,T,H,P,A>::unordered_map(
            InputIt f, InputIt l,
            size_type n,
            const hasher &hf,
            const key_equal &eql)
      : table(boost::unordered::detail::initial_size(f, l, n),
            hf, eql, allocator_type())
    {
        this->insert_range(f, l);
    }
    
    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K,T,H,P,A>::unordered_map(
            InputIt f, InputIt l,
            size_type n,
            const hasher &hf,
            const key_equal &eql,
            const allocator_type &a)
      : table(boost::unordered::detail::initial_size(f, l, n), hf, eql, a)
    {
        this->insert_range(f, l);
    }
    
    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K,T,H,P,A>::unordered_map(
            InputIt f, InputIt l,
            size_type n,
            const hasher &hf,
            const allocator_type &a)
      : table(boost::unordered::detail::initial_size(f, l, n),
            hf, key_equal(), a)
    {
        this->insert_range(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_map<K,T,H,P,A>::unordered_map(
            InputIt f, InputIt l,
            size_type n,
            const allocator_type &a)
      : table(boost::unordered::detail::initial_size(f, l, n),
            hasher(), key_equal(), a)
    {
        this->insert_range(f, l);
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::~unordered_map() BOOST_NOEXCEPT {}

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(
            unordered_map const& other)
      : table(other)
    {
    }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(
            unordered_map&& other, allocator_type const& a)
      : table(other, a, boost::unordered::detail::move_tag())
    {
    }

#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(
            std::initializer_list<value_type> list, size_type n,
            const hasher &hf, const key_equal &eql, const allocator_type &a)
      : table(
            boost::unordered::detail::initial_size(
                list.begin(), list.end(), n),
            hf, eql, a)
    {
        this->insert_range(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(
            std::initializer_list<value_type> list, size_type n,
            const hasher &hf, const allocator_type &a)
      : table(
            boost::unordered::detail::initial_size(
                list.begin(), list.end(), n),
            hf, key_equal(), a)
    {
        this->insert_range(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>::unordered_map(
            std::initializer_list<value_type> list, size_type n,
            const allocator_type &a)
      : table(
            boost::unordered::detail::initial_size(
                list.begin(), list.end(), n),
            hasher(), key_equal(), a)
    {
        this->insert_range(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_map<K,T,H,P,A>& unordered_map<K,T,H,P,A>::operator=(
            std::initializer_list<value_type> list)
    {
        this->clear();
        this->insert_range(list.begin(), list.end());
        return *this;
    }

#endif

    // modifiers

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    void unordered_map<K,T,H,P,A>::insert(InputIt first, InputIt last)
    {
        this->insert_range(first, last);
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
    template <class K, class T, class H, class P, class A>
    void unordered_map<K,T,H,P,A>::insert(
            std::initializer_list<value_type> list)
    {
        this->insert_range(list.begin(), list.end());
    }
#endif

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::iterator
        unordered_map<K,T,H,P,A>::erase(const_iterator position)
    {
        return this->erase_(position);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::size_type
        unordered_map<K,T,H,P,A>::erase(const key_type& k)
    {
        return this->erase_key(k);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::iterator
        unordered_map<K,T,H,P,A>::erase(
            const_iterator first, const_iterator last)
    {
        return this->erase_range(first, last);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_map<K,T,H,P,A>::swap(unordered_map& other)
    {
        this->table::swap(other);
    }

    // observers

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::mapped_type&
        unordered_map<K,T,H,P,A>::operator[](const key_type &k)
    {
        return this->try_emplace(k).first->second;
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::mapped_type&
        unordered_map<K,T,H,P,A>::at(const key_type& k)
    {
        return this->at_(k).second;
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::mapped_type const&
        unordered_map<K,T,H,P,A>::at(const key_type& k) const
    {
        return this->at_(k).second;
    }

    // lookup

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::iterator
        unordered_map<K,T,H,P,A>::find(const key_type& k)
    {
        return iterator(this->find_node(k));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_map<K,T,H,P,A>::const_iterator
        unordered_map<K,T,H,P,A>::find(const key_type& k) const
    {
        return const_iterator(this->find_node(k));
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
    typename unordered_map<K,T,H,P,A>::iterator
        unordered_map<K,T,H,P,A>::find(
            CompatibleKey const& k,
            CompatibleHash const& h,
            CompatiblePredicate const& eq)
    {
        return iterator(this->generic_find_node(k, h, eq));
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
    typename unordered_map<K,T,H,P,A>::const_iterator
        unordered_map<K,T,H,P,A>::find(
            CompatibleKey const& k,
            CompatibleHash const& h,
            CompatiblePredicate const& eq) const
    {
        return const_iterator(this->generic_find_node(k, h, eq));
    }

    template <class K, class T, class H, class P, class A>
    std::pair<
            typename unordered_map<K,T,H,P,A>::iterator,
            typename unordered_map<K,T,H,P,A>::iterator>
        unordered_map<K,T,H,P,A>::equal_range(const key_type& k)
    {
        return this->table::equal_range(k);
    }

    template <class K, class T, class H, class P, class A>
    std::pair<
            typename unordered_map<K,T,H,P,A>::const_iterator,
            typename unordered_map<K,T,H,P,A>::const_iterator>
        unordered_map<K,T,H,P,A>::equal_range(const key_type& k) const
    {
        return this->table::equal_range(k);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator==(
            unordered_map<K,T,H,P,A> const& m1,
            unordered_map<K,T,H,P,A> const& m2)
    {
#if BOOST_WORKAROUND(__CODEGEARC__, BOOST_TESTED_AT(0x0613))
        struct dummy { unordered_map<K,T,H,P,A> x; };
#endif
        return m1.equals(m2);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator!=(
            unordered_map<K,T,H,P,A> const& m1,
            unordered_map<K,T,H,P,A> const& m2)
    {
#if BOOST_WORKAROUND(__CODEGEARC__, BOOST_TESTED_AT(0x0613))
        struct dummy { unordered_map<K,T,H,P,A> x; };
#endif
        return !m1.equals(m2);
    }

    template <class K, class T, class H, class P, class A>
    inline void swap(
            unordered_map<K,T,H,P,A> &m1,
            unordered_map<K,T,H,P,A> &m2)
    {
#if BOOST_WORKAROUND(__CODEGEARC__, BOOST_TESTED_AT(0x0613))
        struct dummy { unordered_map<K,T,H,P,A> x; };
#endif
        m1.swap(m2);
    }

////////////////////////////////////////////////////////////////////////////////

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            size_type n, const hasher &hf, const key_equal &eql,
            const allocator_type &a)
      : table(n, hf, eql, a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(allocator_type const& a)
      : table(boost::unordered::detail::default_bucket_count,
            hasher(), key_equal(), a)
    {
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            unordered_multimap const& other, allocator_type const& a)
      : table(other, a)
    {
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(InputIt f, InputIt l)
      : table(boost::unordered::detail::initial_size(f, l),
        hasher(), key_equal(), allocator_type())
    {
        this->insert_range(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            InputIt f, InputIt l,
            size_type n,
            const hasher &hf,
            const key_equal &eql)
      : table(boost::unordered::detail::initial_size(f, l, n),
            hf, eql, allocator_type())
    {
        this->insert_range(f, l);
    }
    
    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            InputIt f, InputIt l,
            size_type n,
            const hasher &hf,
            const key_equal &eql,
            const allocator_type &a)
      : table(boost::unordered::detail::initial_size(f, l, n), hf, eql, a)
    {
        this->insert_range(f, l);
    }
    
    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            InputIt f, InputIt l,
            size_type n,
            const hasher &hf,
            const allocator_type &a)
      : table(boost::unordered::detail::initial_size(f, l, n),
            hf, key_equal(), a)
    {
        this->insert_range(f, l);
    }

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            InputIt f, InputIt l,
            size_type n,
            const allocator_type &a)
      : table(boost::unordered::detail::initial_size(f, l, n),
            hasher(), key_equal(), a)
    {
        this->insert_range(f, l);
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::~unordered_multimap() BOOST_NOEXCEPT {}

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            unordered_multimap const& other)
      : table(other)
    {
    }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            unordered_multimap&& other, allocator_type const& a)
      : table(other, a, boost::unordered::detail::move_tag())
    {
    }

#endif

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            std::initializer_list<value_type> list, size_type n,
            const hasher &hf, const key_equal &eql, const allocator_type &a)
      : table(
            boost::unordered::detail::initial_size(
                list.begin(), list.end(), n),
            hf, eql, a)
    {
        this->insert_range(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            std::initializer_list<value_type> list, size_type n,
            const hasher &hf, const allocator_type &a)
      : table(
            boost::unordered::detail::initial_size(
                list.begin(), list.end(), n),
            hf, key_equal(), a)
    {
        this->insert_range(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>::unordered_multimap(
            std::initializer_list<value_type> list, size_type n,
            const allocator_type &a)
      : table(
            boost::unordered::detail::initial_size(
                list.begin(), list.end(), n),
            hasher(), key_equal(), a)
    {
        this->insert_range(list.begin(), list.end());
    }

    template <class K, class T, class H, class P, class A>
    unordered_multimap<K,T,H,P,A>& unordered_multimap<K,T,H,P,A>::operator=(
            std::initializer_list<value_type> list)
    {
        this->clear();
        this->insert_range(list.begin(), list.end());
        return *this;
    }

#endif

    // modifiers

    template <class K, class T, class H, class P, class A>
    template <class InputIt>
    void unordered_multimap<K,T,H,P,A>::insert(InputIt first, InputIt last)
    {
        this->insert_range(first, last);
    }

#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
    template <class K, class T, class H, class P, class A>
    void unordered_multimap<K,T,H,P,A>::insert(
            std::initializer_list<value_type> list)
    {
        this->insert_range(list.begin(), list.end());
    }
#endif

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K,T,H,P,A>::iterator
        unordered_multimap<K,T,H,P,A>::erase(const_iterator position)
    {
        return this->erase_(position);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K,T,H,P,A>::size_type
        unordered_multimap<K,T,H,P,A>::erase(const key_type& k)
    {
        return this->erase_key(k);
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K,T,H,P,A>::iterator
        unordered_multimap<K,T,H,P,A>::erase(
            const_iterator first, const_iterator last)
    {
        return this->erase_range(first, last);
    }

    template <class K, class T, class H, class P, class A>
    void unordered_multimap<K,T,H,P,A>::swap(unordered_multimap& other)
    {
        this->table::swap(other);
    }

    // lookup

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K,T,H,P,A>::iterator
        unordered_multimap<K,T,H,P,A>::find(const key_type& k)
    {
        return iterator(this->find_node(k));
    }

    template <class K, class T, class H, class P, class A>
    typename unordered_multimap<K,T,H,P,A>::const_iterator
        unordered_multimap<K,T,H,P,A>::find(const key_type& k) const
    {
        return const_iterator(this->find_node(k));
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
    typename unordered_multimap<K,T,H,P,A>::iterator
        unordered_multimap<K,T,H,P,A>::find(
            CompatibleKey const& k,
            CompatibleHash const& h,
            CompatiblePredicate const& eq)
    {
        return iterator(this->generic_find_node(k, h, eq));
    }

    template <class K, class T, class H, class P, class A>
    template <class CompatibleKey, class CompatibleHash,
        class CompatiblePredicate>
    typename unordered_multimap<K,T,H,P,A>::const_iterator
        unordered_multimap<K,T,H,P,A>::find(
            CompatibleKey const& k,
            CompatibleHash const& h,
            CompatiblePredicate const& eq) const
    {
        return const_iterator(this->generic_find_node(k, h, eq));
    }

    template <class K, class T, class H, class P, class A>
    std::pair<
            typename unordered_multimap<K,T,H,P,A>::iterator,
            typename unordered_multimap<K,T,H,P,A>::iterator>
        unordered_multimap<K,T,H,P,A>::equal_range(const key_type& k)
    {
        return this->table::equal_range(k);
    }

    template <class K, class T, class H, class P, class A>
    std::pair<
            typename unordered_multimap<K,T,H,P,A>::const_iterator,
            typename unordered_multimap<K,T,H,P,A>::const_iterator>
        unordered_multimap<K,T,H,P,A>::equal_range(const key_type& k) const
    {
        return this->table::equal_range(k);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator==(
            unordered_multimap<K,T,H,P,A> const& m1,
            unordered_multimap<K,T,H,P,A> const& m2)
    {
#if BOOST_WORKAROUND(__CODEGEARC__, BOOST_TESTED_AT(0x0613))
        struct dummy { unordered_multimap<K,T,H,P,A> x; };
#endif
        return m1.equals(m2);
    }

    template <class K, class T, class H, class P, class A>
    inline bool operator!=(
            unordered_multimap<K,T,H,P,A> const& m1,
            unordered_multimap<K,T,H,P,A> const& m2)
    {
#if BOOST_WORKAROUND(__CODEGEARC__, BOOST_TESTED_AT(0x0613))
        struct dummy { unordered_multimap<K,T,H,P,A> x; };
#endif
        return !m1.equals(m2);
    }

    template <class K, class T, class H, class P, class A>
    inline void swap(
            unordered_multimap<K,T,H,P,A> &m1,
            unordered_multimap<K,T,H,P,A> &m2)
    {
#if BOOST_WORKAROUND(__CODEGEARC__, BOOST_TESTED_AT(0x0613))
        struct dummy { unordered_multimap<K,T,H,P,A> x; };
#endif
        m1.swap(m2);
    }

    template <typename P, class K, class T, class A>
    class node_handle_map
    {
        BOOST_MOVABLE_BUT_NOT_COPYABLE(node_handle_map)

        template <typename Types>
        friend struct ::boost::unordered::detail::table_impl;
        template <typename Types>
        friend struct ::boost::unordered::detail::grouped_table_impl;

        typedef std::pair<K const, T> value_type;
        typedef typename boost::unordered::detail::rebind_wrap<A, value_type>::type value_allocator;
        typedef boost::unordered::detail::allocator_traits<value_allocator> value_allocator_traits;
        typedef typename P::template node_types<value_allocator>::node node;
        typedef typename boost::unordered::detail::rebind_wrap<A, node>::type node_allocator;
        typedef boost::unordered::detail::allocator_traits<node_allocator> node_allocator_traits;
        typedef typename node_allocator_traits::pointer node_pointer;
    public:
        typedef K key_type;
        typedef T mapped_type;
        typedef A allocator_type;
    private:
        node_pointer ptr_;
        bool has_alloc_;
        boost::unordered::detail::value_base<value_allocator> alloc_;
    public:
        BOOST_CONSTEXPR node_handle_map() BOOST_NOEXCEPT : ptr_(), has_alloc_(false) {}

        /*BOOST_CONSTEXPR */ node_handle_map(node_pointer ptr, allocator_type const& a) : ptr_(ptr), has_alloc_(false) {
            if (ptr_) {
                new ((void*) &alloc_) value_allocator(a);
                has_alloc_ = true;
            }
        }

        ~node_handle_map() {
            if (has_alloc_ && ptr_) {
                node_allocator node_alloc(alloc_.value());
                boost::unordered::detail::node_tmp<node_allocator> tmp(ptr_, node_alloc);
            }
            if (has_alloc_) {
                alloc_.value_ptr()->~value_allocator();
            }
        }

        node_handle_map(BOOST_RV_REF(node_handle_map) n) BOOST_NOEXCEPT : ptr_(n.ptr_), has_alloc_(false) {
            if (n.has_alloc_) {
                new ((void*) &alloc_) value_allocator(boost::move(n.alloc_.value()));
                has_alloc_ = true;
                n.ptr_ = node_pointer();
                n.alloc_.value_ptr()->~value_allocator();
                n.has_alloc_ = false;
            }
        }

        node_handle_map& operator=(BOOST_RV_REF(node_handle_map) n) {
            BOOST_ASSERT(!has_alloc_ ||
                value_allocator_traits::propagate_on_container_move_assignment::value ||
                (n.has_alloc_ && alloc_.value() == n.alloc_.value()));

            if (ptr_) {
                node_allocator node_alloc(alloc_.value());
                boost::unordered::detail::node_tmp<node_allocator> tmp(ptr_, node_alloc);
                ptr_ = node_pointer();
            }

            if (has_alloc_) {
                alloc_.value_ptr()->~value_allocator();
                has_alloc_ = false;
            }

            if (!has_alloc_ && n.has_alloc_) {
                move_allocator(n);
            }

            ptr_ = n.ptr_;
            n.ptr_ = node_pointer();

            return *this;
        }

        key_type& key() const {
            return const_cast<key_type&>(ptr_->value().first);
        }

        mapped_type& mapped() const {
            return ptr_->value().second;
        }

        allocator_type get_allocator() const {
            return alloc_.value();
        }

        BOOST_EXPLICIT_OPERATOR_BOOL_NOEXCEPT()

        bool operator!() const BOOST_NOEXCEPT {
            return ptr_ ? 0 : 1;
        }

        bool empty() const BOOST_NOEXCEPT {
            return ptr_ ? 0 : 1;
        }

        void swap(node_handle_map& n)
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
            noexcept(value_allocator_traits::propagate_on_container_swap::value
                    /* || value_allocator_traits::is_always_equal::value */)
#endif
        {
            if (!has_alloc_) {
                if (n.has_alloc_) {
                    move_allocator(n);
                }
            }
            else if (!n.has_alloc_) {
                n.move_allocator(*this);
            }
            else {
                swap_impl(n,
                    boost::unordered::detail::integral_constant<bool,
                        value_allocator_traits::propagate_on_container_swap::value>());
            }
            boost::swap(ptr_, n.ptr_);
        }

    private:
        void move_allocator(node_handle_map& n) {
            new ((void*) &alloc_) value_allocator(boost::move(n.alloc_.value()));
            n.alloc_.value_ptr()->~value_allocator();
            has_alloc_ = true;
            n.has_alloc_ = false;
        }

        void swap_impl(node_handle_map&, boost::unordered::detail::false_type)
        {
        }

        void swap_impl(node_handle_map& n, boost::unordered::detail::true_type)
        {
            boost::swap(alloc_, n.alloc_);
        }
    };

    template <class P, class K, class T, class A>
    void swap(node_handle_map<P, K, T, A>& x, node_handle_map<P, K, T, A>& y)
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
        noexcept(noexcept(x.swap(y)))
#endif
    {
        x.swap(y);
    }

    template <class P, class K, class T, class A>
    struct insert_return_type_map
    {
    private:
        BOOST_MOVABLE_BUT_NOT_COPYABLE(insert_return_type_map)

        typedef typename boost::unordered::detail::rebind_wrap<A, std::pair<K const, T> >::type value_allocator;
        typedef typename P::template node_types<value_allocator>::node node_;

    public:
        bool inserted;
        boost::unordered::iterator_detail::iterator<node_> position;
        boost::unordered::node_handle_map<P, K, T, A> node;

        insert_return_type_map() : inserted(false), position(), node() {}

        insert_return_type_map(BOOST_RV_REF(insert_return_type_map) x)  BOOST_NOEXCEPT
            : inserted(x.inserted), position(x.position), node(boost::move(x.node)) {}

        insert_return_type_map& operator=(BOOST_RV_REF(insert_return_type_map) x) {
            inserted = x.inserted;
            position = x.position;
            node = boost::move(x.node);
            return *this;
        }
    };

    template <class P, class K, class T, class A>
    void swap(insert_return_type_map<P, K, T, A>& x, insert_return_type_map<P, K, T, A>& y)
    {
        boost::swap(x.node, y.node);
        boost::swap(x.inserted, y.inserted);
        boost::swap(x.position, y.position);
    }
} // namespace unordered
} // namespace boost

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#endif // BOOST_UNORDERED_UNORDERED_MAP_HPP_INCLUDED
