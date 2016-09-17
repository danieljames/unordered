
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2011 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNORDERED_DETAIL_UNIQUE_HPP_INCLUDED
#define BOOST_UNORDERED_DETAIL_UNIQUE_HPP_INCLUDED

#include <boost/config.hpp>
#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <boost/unordered/detail/extract_key.hpp>
#include <boost/throw_exception.hpp>
#include <stdexcept>

namespace boost { namespace unordered { namespace detail {

    template <typename A> struct unique_node;
    template <typename T> struct ptr_node;
    template <typename N> struct node_algo;

    template <typename A>
    struct unique_node :
        boost::unordered::detail::value_base<
            BOOST_DEDUCED_TYPENAME
            ::boost::unordered::detail::allocator_traits<A>::value_type
        >
    {
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, unique_node<A> >::type allocator;
        typedef typename ::boost::unordered::detail::
            allocator_traits<allocator>::pointer node_pointer;
        typedef boost::unordered::detail::bucket<node_pointer> bucket;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, bucket>::type bucket_allocator;
        typedef typename ::boost::unordered::detail::
            allocator_traits<bucket_allocator>::pointer bucket_pointer;
        typedef node_pointer link_pointer;

        link_pointer next_;
        std::size_t hash_;

        unique_node() :
            next_(),
            hash_(0)
        {}

        void init(node_pointer)
        {
        }

    private:
        unique_node& operator=(unique_node const&);
    };

    template <typename A>
    struct node_traits<unique_node<A> >
    {
        typedef typename
            boost::unordered::detail::allocator_traits<A>::value_type
            value_type;
        typedef unique_node<A> node;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, node>::type allocator;
        typedef typename ::boost::unordered::detail::
            allocator_traits<allocator>::pointer node_pointer;
        typedef boost::unordered::detail::bucket<node_pointer> bucket;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, bucket>::type bucket_allocator;
        typedef typename ::boost::unordered::detail::
            allocator_traits<bucket_allocator>::pointer bucket_pointer;
        typedef node_pointer link_pointer;
        typedef boost::unordered::detail::node_algo<node> node_algo;
    };

    template <typename T>
    struct ptr_node :
        boost::unordered::detail::ptr_bucket
    {
        typedef T value_type;
        typedef boost::unordered::detail::ptr_bucket bucket;
        typedef ptr_node<T>* node_pointer;
        typedef ptr_bucket* link_pointer;
        typedef ptr_bucket* bucket_pointer;

        std::size_t hash_;
        boost::unordered::detail::value_base<T> value_base_;

        ptr_node() :
            bucket(),
            hash_(0)
        {}

        void init(node_pointer)
        {
        }

        void* address() { return value_base_.address(); }
        value_type& value() { return value_base_.value(); }
        value_type* value_ptr() { return value_base_.value_ptr(); }

    private:
        ptr_node& operator=(ptr_node const&);
    };

    template <typename T>
    struct node_traits<ptr_node<T> >
    {
        typedef T value_type;
        typedef ptr_node<T> node;
        typedef ptr_bucket bucket;
        typedef ptr_node<T>* node_pointer;
        typedef ptr_bucket* link_pointer;
        typedef ptr_bucket* bucket_pointer;
        typedef boost::unordered::detail::node_algo<node> node_algo;
    };

    // If the allocator uses raw pointers use ptr_node
    // Otherwise use node.

    template <typename A, typename NodePtr, typename BucketPtr>
    struct pick_node2
    {
        typedef boost::unordered::detail::unique_node<A> node;

        typedef typename boost::unordered::detail::allocator_traits<
            typename boost::unordered::detail::rebind_wrap<A, node>::type
        >::pointer node_pointer;

        typedef boost::unordered::detail::bucket<node_pointer> bucket;
        typedef node_pointer link_pointer;
    };

    template <typename A>
    struct pick_node2<A,
        boost::unordered::detail::ptr_node<
            BOOST_DEDUCED_TYPENAME
            ::boost::unordered::detail::allocator_traits<A>::value_type
        >*,
        boost::unordered::detail::ptr_bucket*>
    {
        typedef boost::unordered::detail::ptr_node<
            BOOST_DEDUCED_TYPENAME
            ::boost::unordered::detail::allocator_traits<A>::value_type
        > node;
        typedef boost::unordered::detail::ptr_bucket bucket;
        typedef bucket* link_pointer;
    };

    template <typename A>
    struct pick_node
    {
        typedef boost::unordered::detail::allocator_traits<
            typename boost::unordered::detail::rebind_wrap<A,
                boost::unordered::detail::ptr_node<
                    BOOST_DEDUCED_TYPENAME
                    ::boost::unordered::detail::allocator_traits<A>::value_type
                > >::type
        > tentative_node_traits;

        typedef boost::unordered::detail::allocator_traits<
            typename boost::unordered::detail::rebind_wrap<A,
                boost::unordered::detail::ptr_bucket >::type
        > tentative_bucket_traits;

        typedef pick_node2<A,
            typename tentative_node_traits::pointer,
            typename tentative_bucket_traits::pointer> pick;

        typedef typename pick::node node;
        typedef typename pick::bucket bucket;
        typedef typename pick::link_pointer link_pointer;
    };

    struct u
    {
        template <typename A>
        struct node_types
        {
            typedef typename pick_node<A>::pick pick;

            typedef typename pick::node node;
            typedef typename pick::bucket bucket;
            typedef typename pick::link_pointer link_pointer;
        };
    };

    template <typename N>
    struct node_algo
    {
        typedef node_traits<N> traits;
        typedef typename traits::node_pointer node_pointer;
        typedef typename traits::link_pointer link_pointer;
        typedef typename traits::bucket_pointer bucket_pointer;

        static node_pointer next_node(link_pointer n) {
            return static_cast<node_pointer>(n->next_);
        }

        static node_pointer next_for_find(node_pointer n) {
            return static_cast<node_pointer>(n->next_);
        }

        static link_pointer next_for_erase(link_pointer prev) {
            return prev->next_;
        }

        // Add node 'n' after 'pos'.
        // This results in a different order to the grouped implementation.
        static inline void add_to_node_group(node_pointer n, node_pointer pos) {
            n->next_ = pos->next_;
            pos->next_ = n;
        }

        static inline node_pointer extract_first_node(link_pointer prev) {
            node_pointer n = next_node(prev);
            prev->next_ = n->next_;
            return n;
        }

        // Extract a node and place it in the correct bucket.
        // TODO: For tables with equivalent keys, this doesn't preserve
        //       the order.
        // pre: prev->next_ is not null.
        template <typename Table>
        static link_pointer place_in_bucket(Table& dst, link_pointer prev)
        {
            node_pointer n = next_node(prev);
            bucket_pointer b = dst.get_bucket(dst.hash_to_bucket(n->hash_));

            if (!b->next_) {
                b->next_ = prev;
                return n;
            }
            else {
                prev->next_ = n->next_;
                n->next_ = b->next_->next_;
                b->next_->next_ = n;
                return prev;
            }
        }
    };

    template <typename Policies, typename H, typename P, typename A>
    struct table_impl : boost::unordered::detail::table<Policies, H, P, A>
    {
        friend struct boost::unordered::detail::table<Policies, H, P, A>;

    protected:
        typedef boost::unordered::detail::table<Policies, H, P, A> table;
        typedef typename table::value_type value_type;
        typedef typename table::bucket bucket;
        typedef typename table::policy policy;
        typedef typename table::node_pointer node_pointer;
        typedef typename table::node_allocator node_allocator;
        typedef typename table::node_allocator_traits node_allocator_traits;
        typedef typename table::bucket_pointer bucket_pointer;
        typedef typename table::link_pointer link_pointer;
        typedef typename table::hasher hasher;
        typedef typename table::key_equal key_equal;
        typedef typename table::const_key_type const_key_type;
        typedef typename table::extractor extractor;
        typedef typename table::iterator iterator;
        typedef typename table::const_iterator c_iterator;
        typedef typename table::node_constructor node_constructor;
        typedef typename table::node_tmp node_tmp;
        typedef typename table::node_algo node_algo;
        typedef std::pair<iterator, bool> emplace_return;

        // Constructors

        table_impl(std::size_t n,
                hasher const& hf,
                key_equal const& eq,
                node_allocator const& a)
          : table(n, hf, eq, a)
        {}

        table_impl(table_impl const& x)
          : table(x, node_allocator_traits::
                select_on_container_copy_construction(x.node_alloc()))
        {
            this->init(x);
        }

        table_impl(table_impl const& x,
                node_allocator const& a)
          : table(x, a)
        {
            this->init(x);
        }

        table_impl(table_impl& x,
                boost::unordered::detail::move_tag m)
          : table(x, m)
        {}

        table_impl(table_impl& x,
                node_allocator const& a,
                boost::unordered::detail::move_tag m)
          : table(x, a, m)
        {
            this->move_init(x);
        }

        // Node functions.

        static inline node_pointer next_node(link_pointer n) {
            return static_cast<node_pointer>(n->next_);
        }

        // Accessors

    public:
        std::size_t count(const_key_type& k) const
        {
            return this->find_node(k) ? 1 : 0;
        }

    protected:
        value_type& at_(const_key_type& k) const
        {
            if (this->size_) {
                node_pointer n = this->find_node(k);
                if (n) return n->value();
            }

            boost::throw_exception(
                std::out_of_range("Unable to find key in unordered_map."));
        }

    public:
        std::pair<iterator, iterator>
            equal_range(const_key_type& k) const
        {
            node_pointer n = this->find_node(k);
            return std::make_pair(iterator(n), iterator(n ? next_node(n) : n));
        }

        // equals

    protected:
        bool equals(table_impl const& other) const
        {
            if(this->size_ != other.size_) return false;
    
            for(node_pointer n1 = this->begin_node(); n1; n1 = next_node(n1))
            {
                node_pointer n2 = other.find_node(other.get_key(n1->value()));

                if (!n2 || n1->value() != n2->value())
                    return false;
            }
    
            return true;
        }

        // Emplace/Insert

        inline node_pointer add_node(
                node_pointer n,
                std::size_t key_hash)
        {
            n->hash_ = key_hash;
    
            bucket_pointer b = this->get_bucket(this->hash_to_bucket(key_hash));

            if (!b->next_)
            {
                link_pointer start_node = this->get_previous_start();
                
                if (start_node->next_) {
                    this->get_bucket(this->hash_to_bucket(
                        next_node(start_node)->hash_)
                    )->next_ = n;
                }

                b->next_ = start_node;
                n->next_ = start_node->next_;
                start_node->next_ = n;
            }
            else
            {
                n->next_ = b->next_->next_;
                b->next_->next_ = n;
            }

            ++this->size_;
            return n;
        }

        inline node_pointer resize_and_add_node(node_pointer n, std::size_t key_hash)
        {
            node_tmp b(n, this->node_alloc());
            this->reserve_for_insert(this->size_ + 1);
            return this->add_node(b.release(), key_hash);
        }

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#   if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
        emplace_return emplace_(boost::unordered::detail::emplace_args1<
                boost::unordered::detail::please_ignore_this_overload> const&)
        {
            BOOST_ASSERT(false);
            return emplace_return(iterator(), false);
        }

        iterator emplace_hint_(c_iterator,
            boost::unordered::detail::emplace_args1<
                boost::unordered::detail::please_ignore_this_overload> const&)
        {
            BOOST_ASSERT(false);
            return iterator();
        }
#   else
        emplace_return emplace_(
                boost::unordered::detail::please_ignore_this_overload const&)
        {
            BOOST_ASSERT(false);
            return emplace_return(iterator(), false);
        }

        iterator emplace_hint_(c_iterator,
                boost::unordered::detail::please_ignore_this_overload const&)
        {
            BOOST_ASSERT(false);
            return iterator();
        }
#   endif
#endif

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        emplace_return emplace_(BOOST_UNORDERED_EMPLACE_ARGS)
        {
#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
            return emplace_impl(
                extractor::extract(BOOST_UNORDERED_EMPLACE_FORWARD),
                BOOST_UNORDERED_EMPLACE_FORWARD);
#else
            return emplace_impl(
                extractor::extract(args.a0, args.a1),
                BOOST_UNORDERED_EMPLACE_FORWARD);
#endif
        }

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        iterator emplace_hint_(c_iterator hint,
            BOOST_UNORDERED_EMPLACE_ARGS)
        {
#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
            return emplace_hint_impl(hint,
                extractor::extract(BOOST_UNORDERED_EMPLACE_FORWARD),
                BOOST_UNORDERED_EMPLACE_FORWARD);
#else
            return emplace_hint_impl(hint,
                extractor::extract(args.a0, args.a1),
                BOOST_UNORDERED_EMPLACE_FORWARD);
#endif
        }

#if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
        template <typename A0>
        emplace_return emplace_(
                boost::unordered::detail::emplace_args1<A0> const& args)
        {
            return emplace_impl(extractor::extract(args.a0), args);
        }

        template <typename A0>
        iterator emplace_hint_(c_iterator hint,
                boost::unordered::detail::emplace_args1<A0> const& args)
        {
            return emplace_hint_impl(hint, extractor::extract(args.a0), args);
        }
#endif

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        iterator emplace_hint_impl(c_iterator hint, const_key_type& k,
            BOOST_UNORDERED_EMPLACE_ARGS)
        {
            if (hint.node_ && this->key_eq()(k, this->get_key(*hint))) {
                return iterator(hint.node_);
            }
            else {
                return emplace_impl(k, BOOST_UNORDERED_EMPLACE_FORWARD).first;
            }
        }

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        emplace_return emplace_impl(const_key_type& k,
            BOOST_UNORDERED_EMPLACE_ARGS)
        {
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);
            if (pos) {
                return emplace_return(iterator(pos), false);
            }
            else {
                return emplace_return(
                    iterator(this->resize_and_add_node(
                        boost::unordered::detail::func::construct_value_generic(
                            this->node_alloc(), BOOST_UNORDERED_EMPLACE_FORWARD),
                        key_hash)),
                    true);
            }
        }

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        iterator emplace_hint_impl(c_iterator hint, no_key,
            BOOST_UNORDERED_EMPLACE_ARGS)
        {
            node_tmp b(
                boost::unordered::detail::func::construct_value_generic(
                    this->node_alloc(), BOOST_UNORDERED_EMPLACE_FORWARD),
                this->node_alloc());
            const_key_type& k = this->get_key(b.node_->value());
            if (hint.node_ && this->key_eq()(k, this->get_key(*hint))) {
                return iterator(hint.node_);
            }
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);
            if (pos) {
                return iterator(pos);
            }
            else {
                return iterator(this->resize_and_add_node(b.release(), key_hash));
            }
        }

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        emplace_return emplace_impl(no_key, BOOST_UNORDERED_EMPLACE_ARGS)
        {
            node_tmp b(
                boost::unordered::detail::func::construct_value_generic(
                    this->node_alloc(), BOOST_UNORDERED_EMPLACE_FORWARD),
                this->node_alloc());
            const_key_type& k = this->get_key(b.node_->value());
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);
            if (pos) {
                return emplace_return(iterator(pos), false);
            }
            else {
                return emplace_return(
                    iterator(this->resize_and_add_node(b.release(), key_hash)),
                    true);
            }
        }

        template <typename Key>
        emplace_return try_emplace_(BOOST_FWD_REF(Key) k)
        {
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);
            if (pos) {
                return emplace_return(iterator(pos), false);
            }
            else {
                return emplace_return(
                    iterator(this->resize_and_add_node(
                        boost::unordered::detail::func::construct_pair(
                            this->node_alloc(), boost::forward<Key>(k)),
                        key_hash)),
                    true);
            }
        }

        template <typename Key>
        iterator try_emplace_hint_(c_iterator hint, BOOST_FWD_REF(Key) k)
        {
            if (hint.node_ && this->key_eq()(hint->first, k)) {
                return iterator(hint.node_);
            }
            else {
                return try_emplace_(k).first;
            }
        }

        template <typename Key, BOOST_UNORDERED_EMPLACE_TEMPLATE>
        emplace_return try_emplace_(BOOST_FWD_REF(Key) k,
                BOOST_UNORDERED_EMPLACE_ARGS)
        {
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);
            if (pos) {
                return emplace_return(iterator(pos), false);
            }
            else {
                return emplace_return(
                    iterator(this->resize_and_add_node(
                        boost::unordered::detail::func::construct_pair_generic(
                            this->node_alloc(), boost::forward<Key>(k),
                            BOOST_UNORDERED_EMPLACE_FORWARD),
                        key_hash)),
                    true);
            }
        }

        template <typename Key, BOOST_UNORDERED_EMPLACE_TEMPLATE>
        iterator try_emplace_hint_(c_iterator hint, BOOST_FWD_REF(Key) k,
                BOOST_UNORDERED_EMPLACE_ARGS)
        {
            if (hint.node_ && this->key_eq()(hint->first, k)) {
                return iterator(hint.node_);
            }
            else {
                return try_emplace_(k, BOOST_UNORDERED_EMPLACE_FORWARD).first;
            }
        }

        template <typename Key, typename M>
        emplace_return insert_or_assign_(
            BOOST_FWD_REF(Key) k,
            BOOST_FWD_REF(M) obj)
        {
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);

            if (pos) {
                pos->value().second = boost::forward<M>(obj);
                return emplace_return(iterator(pos), false);
            }
            else {
                return emplace_return(
                    iterator(this->resize_and_add_node(
                        boost::unordered::detail::func::construct_pair(
                            this->node_alloc(), boost::forward<Key>(k),
                            boost::forward<M>(obj)),
                        key_hash)),
                    true);
            }
        }

        template <typename NodeType, typename InsertReturnType>
        void move_insert_node_type(NodeType& np, InsertReturnType& result) {
            if (np) {
                const_key_type& k = this->get_key(np.ptr_->value());
                std::size_t key_hash = this->hash(k);
                node_pointer pos = this->find_node(key_hash, k);

                if (pos) {
                    result.node = boost::move(np);
                    result.position = iterator(pos);
                }
                else {
                    this->reserve_for_insert(this->size_ + 1);
                    result.position = iterator(this->add_node(np.ptr_, key_hash));
                    result.inserted = true;
                    np.ptr_ = node_pointer();
                }
            }
        }

        template <typename NodeType>
        iterator move_insert_node_type_with_hint(c_iterator hint, NodeType& np) {
            if (!np) { return iterator(); }
            const_key_type& k = this->get_key(np.ptr_->value());
            if (hint.node_ && this->key_eq()(k, this->get_key(*hint))) {
                return iterator(hint.node_);
            }
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);
            if (!pos) {
                this->reserve_for_insert(this->size_ + 1);
                pos = this->add_node(np.ptr_, key_hash);
                np.ptr_ = node_pointer();
            }
            return iterator(pos);
        }

        template <typename Policies2, typename H2, typename P2, typename A2>
        void merge_(table_impl<Policies2, H2, P2, A2>& other) {
            if (other.size_) {
                link_pointer prev = other.get_previous_start();

                while(prev->next_) {
                    node_pointer n = other.next_node(prev);
                    const_key_type& k = this->get_key(n->value());
                    std::size_t key_hash = this->hash(k);
                    node_pointer pos = this->find_node(key_hash, k);

                    if (pos) {
                        prev = n;
                    }
                    else {
                        this->reserve_for_insert(this->size_ + 1);
                        prev->next_ = n->next_;
                        --other.size_;
                        other.fix_bucket(other.hash_to_bucket(n->hash_), prev);
                        this->add_node(n, key_hash);
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // Insert range methods
        //
        // if hash function throws, or inserting > 1 element, basic exception
        // safety strong otherwise

        template <class InputIt>
        void insert_range(InputIt i, InputIt j)
        {
            if(i != j)
                return insert_range_impl(extractor::extract(*i), i, j);
        }

        template <class InputIt>
        void insert_range_impl(const_key_type& k, InputIt i, InputIt j)
        {
            insert_range_impl2(k, i, j);

            while(++i != j) {
                // Note: can't use get_key as '*i' might not be value_type - it
                // could be a pair with first_types as key_type without const or
                // a different second_type.
                //
                // TODO: Might be worth storing the value_type instead of the
                // key here. Could be more efficient if '*i' is expensive. Could
                // be less efficient if copying the full value_type is
                // expensive.
                insert_range_impl2(extractor::extract(*i), i, j);
            }
        }

        template <class InputIt>
        void insert_range_impl2(const_key_type& k, InputIt i, InputIt j)
        {
            // No side effects in this initial code
            std::size_t key_hash = this->hash(k);
            node_pointer pos = this->find_node(key_hash, k);
    
            if (!pos) {
                node_tmp b(
                    boost::unordered::detail::func::construct_value(this->node_alloc(), *i),
                    this->node_alloc());
                if(this->size_ + 1 > this->max_load_)
                    this->reserve_for_insert(this->size_ +
                        boost::unordered::detail::insert_size(i, j));
                this->add_node(b.release(), key_hash);
            }
        }

        template <class InputIt>
        void insert_range_impl(no_key, InputIt i, InputIt j)
        {
            node_constructor a(this->node_alloc());

            do {
                if (!a.node_) { a.create_node(); }
                boost::unordered::detail::func::call_construct(
                    a.alloc_, a.node_->value_ptr(), *i);
                node_tmp b(a.release(), a.alloc_);

                const_key_type& k = this->get_key(b.node_->value());
                std::size_t key_hash = this->hash(k);
                node_pointer pos = this->find_node(key_hash, k);

                if (pos) {
                    a.reclaim(b.release());
                }
                else {
                    // reserve has basic exception safety if the hash function
                    // throws, strong otherwise.
                    this->reserve_for_insert(this->size_ + 1);
                    this->add_node(b.release(), key_hash);
                }
            } while(++i != j);
        }

        ////////////////////////////////////////////////////////////////////////
        // Extract

        inline node_pointer extract_by_iterator(c_iterator i)
        {
            node_pointer n = i.node_;
            BOOST_ASSERT(n);
            std::size_t key_hash = n->hash_;
            std::size_t bucket_index = this->hash_to_bucket(key_hash);
            link_pointer prev = this->get_previous_start(bucket_index);
            while(prev->next_ != n) { prev = prev->next_; }
            prev->next_ = n->next_;
            --this->size_;
            this->fix_bucket(bucket_index, prev);
            n->next_ = link_pointer();
            return n;
        }

        ////////////////////////////////////////////////////////////////////////
        // Erase
        //
        // no throw

        std::size_t erase_key(const_key_type& k)
        {
            if (!this->size_) return 0;
            std::size_t key_hash = this->hash(k);
            std::size_t bucket_index = this->hash_to_bucket(key_hash);
            link_pointer prev = this->find_previous_node(k, key_hash, bucket_index);
            if (!prev) return 0;
            link_pointer next = next_node(prev)->next_;
            this->delete_nodes(prev, next);
            this->fix_bucket(bucket_index, prev);
            return 1;
        }

        iterator erase_(c_iterator r)
        {
            BOOST_ASSERT(r.node_);
            node_pointer next = next_node(r.node_);
            erase_nodes(r.node_, next);
            return iterator(next);
        }

        iterator erase_range(c_iterator r1, c_iterator r2)
        {
            if (r1 == r2) return iterator(r2.node_);
            erase_nodes(r1.node_, r2.node_);
            return iterator(r2.node_);
        }

        void erase_nodes(node_pointer i, node_pointer j)
        {
            std::size_t bucket_index = this->hash_to_bucket(i->hash_);

            // Find the node before i.
            link_pointer prev = this->get_previous_start(bucket_index);
            while(prev->next_ != i) prev = prev->next_;

            // Delete the nodes.
            do {
                this->delete_node(prev);
                bucket_index = this->fix_bucket(bucket_index, prev);
            } while (prev->next_ != j);
        }

        ////////////////////////////////////////////////////////////////////////
        // fill_buckets

        void copy_buckets(table const& src) {
            this->create_buckets(this->bucket_count_);

            for(node_pointer n = src.begin_node(); n; n = next_node(n)) {
                this->add_node(
                    boost::unordered::detail::func::construct_value(
                     this->node_alloc(), n->value()), n->hash_);
            }
        }

        void move_buckets(table const& src) {
            this->create_buckets(this->bucket_count_);

            for(node_pointer n = src.begin_node(); n; n = next_node(n)) {
                this->add_node(
                    boost::unordered::detail::func::construct_value(
                        this->node_alloc(), boost::move(n->value())), n->hash_);
            }
        }

        void assign_buckets(table const& src)
        {
            node_holder<node_allocator> holder(*this);
            for(node_pointer n = src.begin_node(); n; n = next_node(n)) {
                this->add_node(holder.copy_of(n->value()), n->hash_);
            }
        }

        void move_assign_buckets(table& src)
        {
            node_holder<node_allocator> holder(*this);
            for(node_pointer n = src.begin_node(); n; n = next_node(n)) {
                this->add_node(holder.move_copy_of(n->value()), n->hash_);
            }
        }
    };
}}}

#endif
