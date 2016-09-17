
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2011 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNORDERED_DETAIL_EQUIVALENT_HPP_INCLUDED
#define BOOST_UNORDERED_DETAIL_EQUIVALENT_HPP_INCLUDED

#include <boost/config.hpp>
#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <boost/unordered/detail/unique.hpp>
#include <boost/unordered/detail/extract_key.hpp>

namespace boost { namespace unordered { namespace detail {

    template <typename A> struct grouped_node;
    template <typename T> struct grouped_ptr_node;
    template <typename N> struct grouped_node_algo;

    template <typename A>
    struct grouped_node :
        boost::unordered::detail::value_base<
            BOOST_DEDUCED_TYPENAME
            ::boost::unordered::detail::allocator_traits<A>::value_type
        >
    {
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, grouped_node<A> >::type allocator;
        typedef typename ::boost::unordered::detail::
            allocator_traits<allocator>::pointer node_pointer;
        typedef boost::unordered::detail::bucket<node_pointer> bucket;
        typedef typename ::boost::unordered::detail::rebind_wrap<
            A, bucket>::type bucket_allocator;
        typedef typename ::boost::unordered::detail::
            allocator_traits<bucket_allocator>::pointer bucket_pointer;
        typedef node_pointer link_pointer;

        link_pointer next_;
        node_pointer group_prev_;
        std::size_t hash_;

        grouped_node() :
            next_(),
            group_prev_(),
            hash_(0)
        {}

        void init(node_pointer self)
        {
            group_prev_ = self;
        }
    private:
        grouped_node& operator=(grouped_node const&);
    };

    template <typename A>
    struct node_traits<grouped_node<A> >
    {
        typedef typename
            boost::unordered::detail::allocator_traits<A>::value_type
            value_type;
        typedef grouped_node<A> node;
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
        typedef boost::unordered::detail::grouped_node_algo<node> node_algo;
    };

    template <typename T>
    struct grouped_ptr_node :
        boost::unordered::detail::ptr_bucket
    {
        typedef T value_type;
        typedef boost::unordered::detail::ptr_bucket bucket;
        typedef grouped_ptr_node<T>* node_pointer;
        typedef ptr_bucket* link_pointer;
        typedef ptr_bucket* bucket_pointer;

        node_pointer group_prev_;
        std::size_t hash_;
        boost::unordered::detail::value_base<T> value_base_;

        grouped_ptr_node() :
            bucket(),
            group_prev_(0),
            hash_(0)
        {}

        void init(node_pointer self)
        {
            group_prev_ = self;
        }

        void* address() { return value_base_.address(); }
        value_type& value() { return value_base_.value(); }
        value_type* value_ptr() { return value_base_.value_ptr(); }

    private:
        grouped_ptr_node& operator=(grouped_ptr_node const&);
    };

    template <typename T>
    struct node_traits<grouped_ptr_node<T> >
    {
        typedef T value_type;
        typedef grouped_ptr_node<T> node;
        typedef ptr_bucket bucket;
        typedef grouped_ptr_node<T>* node_pointer;
        typedef ptr_bucket* link_pointer;
        typedef ptr_bucket* bucket_pointer;
        typedef boost::unordered::detail::grouped_node_algo<node> node_algo;
    };

    template <typename N>
    struct grouped_node_algo
    {
        typedef node_traits<N> traits;
        typedef typename traits::node_pointer node_pointer;
        typedef typename traits::link_pointer link_pointer;
        typedef typename traits::bucket_pointer bucket_pointer;

        static node_pointer next_node(link_pointer n) {
            return static_cast<node_pointer>(n->next_);
        }

        static node_pointer next_for_find(node_pointer n) {
            return static_cast<node_pointer>(n->group_prev_->next_);
        }

        static link_pointer next_for_erase(link_pointer prev) {
            return static_cast<node_pointer>(prev->next_)->group_prev_;
        }

        static node_pointer last_for_rehash(link_pointer prev) {
            return static_cast<node_pointer>(prev->next_)->group_prev_;
        }

        // The 'void*' arguments are pointers to the table, which we
        // will ignore, but without groups they could be used to
        // access the various functions for dealing with values and keys.
        static node_pointer next_group(node_pointer n, void const*) {
            return static_cast<node_pointer>(n->group_prev_->next_);
        }

        static std::size_t count(node_pointer n, void const*) {
            std::size_t x = 0;
            node_pointer it = n;
            do {
                it = it->group_prev_;
                ++x;
            } while(it != n);

            return x;
        }

        // Adds node 'n' to the group containing 'pos'.
        // If 'pos' is the first node in group, add to the end of the group,
        // otherwise add before 'pos'. Other versions will probably behave
        // differently.
        static inline void add_to_node_group(node_pointer n, node_pointer pos) {
            n->next_ = pos->group_prev_->next_;
            n->group_prev_ = pos->group_prev_;
            pos->group_prev_->next_ = n;
            pos->group_prev_ = n;
        }

        static inline node_pointer extract_first_node(link_pointer prev) {
            node_pointer n = next_node(prev);
            if (n->group_prev_ != n) {
                node_pointer next = next_node(n);
                next->group_prev_ = n->group_prev_;
                n->group_prev_ = n;
            }
            prev->next_ = n->next_;
            return n;
        }

        // Split the groups containing 'i' and 'j' so that they can
        // be safely erased/extracted.
        static link_pointer split_groups(node_pointer i, node_pointer j)
        {
            node_pointer prev = i->group_prev_;
            if (prev->next_ != i) prev = node_pointer();

            if (j) {
                node_pointer first = j;
                while (first != i && first->group_prev_->next_ == first) {
                    first = first->group_prev_;
                }

                boost::swap(first->group_prev_, j->group_prev_);
                if (first == i) return prev;
            }

            if (prev) {
                node_pointer first = prev;
                while (first->group_prev_->next_ == first) {
                    first = first->group_prev_;
                }
                boost::swap(first->group_prev_, i->group_prev_);
            }

            return prev;
        }
    };

    // If the allocator uses raw pointers use grouped_ptr_node
    // Otherwise use grouped_node.

    template <typename A, typename NodePtr, typename BucketPtr>
    struct pick_grouped_node
    {
        typedef boost::unordered::detail::grouped_node<A> node;
    };

    template <typename A>
    struct pick_grouped_node<A,
        boost::unordered::detail::grouped_ptr_node<
            BOOST_DEDUCED_TYPENAME
            ::boost::unordered::detail::allocator_traits<A>::value_type
        >*,
        boost::unordered::detail::ptr_bucket*>
    {
        typedef boost::unordered::detail::grouped_ptr_node<
            BOOST_DEDUCED_TYPENAME
            ::boost::unordered::detail::allocator_traits<A>::value_type
        > node;
    };

    struct g
    {
        template <typename A>
        struct node_types
        {
            typedef boost::unordered::detail::allocator_traits<
                typename boost::unordered::detail::rebind_wrap<A,
                    boost::unordered::detail::grouped_ptr_node<
                        BOOST_DEDUCED_TYPENAME
                        ::boost::unordered::detail::allocator_traits<A>::value_type
                    > >::type
            > tentative_node_traits;

            typedef boost::unordered::detail::allocator_traits<
                typename boost::unordered::detail::rebind_wrap<A,
                    boost::unordered::detail::ptr_bucket >::type
            > tentative_bucket_traits;

            typedef typename pick_grouped_node<A,
                typename tentative_node_traits::pointer,
                typename tentative_bucket_traits::pointer>::node node;
        };
    };

    template <typename Policies, typename H, typename P, typename A>
    struct grouped_table_impl : boost::unordered::detail::table<Policies, H, P, A>
    {
        friend struct boost::unordered::detail::table<Policies, H, P, A>;
        template <typename Policies2, typename H2, typename P2, typename A2>
        friend struct boost::unordered::detail::table_impl;

    protected:
        typedef boost::unordered::detail::table<Policies, H, P, A> table;
        typedef typename table::value_type value_type;
        typedef typename table::node_pointer node_pointer;
        typedef typename table::node_allocator node_allocator;
        typedef typename table::node_allocator_traits node_allocator_traits;
        typedef typename table::bucket_pointer bucket_pointer;
        typedef typename table::link_pointer link_pointer;
        typedef typename table::hasher hasher;
        typedef typename table::key_equal key_equal;
        typedef typename table::const_key_type const_key_type;
        typedef typename table::iterator iterator;
        typedef typename table::const_iterator c_iterator;
        typedef typename table::node_constructor node_constructor;
        typedef typename table::node_tmp node_tmp;
        typedef typename table::node_algo node_algo;

        // Constructors

        grouped_table_impl(std::size_t n,
                hasher const& hf,
                key_equal const& eq,
                node_allocator const& a)
          : table(n, hf, eq, a)
        {}

        grouped_table_impl(grouped_table_impl const& x)
          : table(x, node_allocator_traits::
                select_on_container_copy_construction(x.node_alloc()))
        {
            this->init(x);
        }

        grouped_table_impl(grouped_table_impl const& x,
                node_allocator const& a)
          : table(x, a)
        {
            this->init(x);
        }

        grouped_table_impl(grouped_table_impl& x,
                boost::unordered::detail::move_tag m)
          : table(x, m)
        {}

        grouped_table_impl(grouped_table_impl& x,
                node_allocator const& a,
                boost::unordered::detail::move_tag m)
          : table(x, a, m)
        {
            this->move_init(x);
        }

    public:
        std::size_t count(const_key_type& k) const
        {
            node_pointer n = this->find_node(k);
            return n ? node_algo::count(n, this) : 0;
        }

        std::pair<iterator, iterator>
            equal_range(const_key_type& k) const
        {
            node_pointer n = this->find_node(k);
            return std::make_pair(iterator(n), iterator(n ? node_algo::next_group(n, this) : n));
        }

        // Equality

    protected:
        bool equals(grouped_table_impl const& other) const
        {
            if(this->size_ != other.size_) return false;
    
            for(node_pointer n1 = this->begin_node(); n1;)
            {
                node_pointer n2 = other.find_node(other.get_key(n1->value()));
                if (!n2) return false;
                node_pointer end1 = node_algo::next_group(n1, this);
                node_pointer end2 = node_algo::next_group(n2, this);
                if (!group_equals(n1, end1, n2, end2)) return false;
                n1 = end1;    
            }
    
            return true;
        }

        static bool group_equals(node_pointer n1, node_pointer end1,
                node_pointer n2, node_pointer end2)
        {
            for(;;)
            {
                if (n1->value() != n2->value()) break;

                n1 = node_algo::next_node(n1);
                n2 = node_algo::next_node(n2);
            
                if (n1 == end1) return n2 == end2;
                if (n2 == end2) return false;
            }
            
            for(node_pointer n1a = n1, n2a = n2;;)
            {
                n1a = node_algo::next_node(n1a);
                n2a = node_algo::next_node(n2a);

                if (n1a == end1)
                {
                    if (n2a == end2) break;
                    else return false;
                }

                if (n2a == end2) return false;
            }

            node_pointer start = n1;
            for(;n1 != end1; n1 = node_algo::next_node(n1))
            {
                value_type const& v = n1->value();
                if (!find(start, n1, v)) {
                    std::size_t matches = count_equal(n2, end2, v);
                    if (!matches) return false;
                    if (matches != 1 + count_equal(node_algo::next_node(n1), end1, v)) return false;
                }
            }
            
            return true;
        }

        static bool find(node_pointer n, node_pointer n2, value_type const& v)
        {
            for(;n != n2; n = node_algo::next_node(n))
                if (n->value() == v)
                    return true;
            return false;
        }

        static std::size_t count_equal(node_pointer n, node_pointer n2,
            value_type const& v)
        {
            std::size_t count = 0;
            for(;n != n2; n = node_algo::next_node(n))
                if (n->value() == v) ++count;
            return count;
        }

        // Emplace/Insert

        inline node_pointer add_node(
                node_pointer n,
                std::size_t key_hash,
                node_pointer pos)
        {
            n->hash_ = key_hash;
            if (pos) {
                node_algo::add_to_node_group(n, pos);
                if (n->next_) {
                    std::size_t next_bucket = this->hash_to_bucket(
                        node_algo::next_node(n)->hash_);
                    if (next_bucket != this->hash_to_bucket(key_hash)) {
                        this->get_bucket(next_bucket)->next_ = n;
                    }
                }
            }
            else {
                bucket_pointer b = this->get_bucket(
                    this->hash_to_bucket(key_hash));

                if (!b->next_)
                {
                    link_pointer start_node = this->get_previous_start();
                    
                    if (start_node->next_) {
                        this->get_bucket(this->hash_to_bucket(
                            node_algo::next_node(start_node)->hash_
                        ))->next_ = n;
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
            }
            ++this->size_;
            return n;
        }

        inline node_pointer add_using_hint(
                node_pointer n,
                node_pointer hint)
        {
            n->hash_ = hint->hash_;
            node_algo::add_to_node_group(n, hint);
            if (n->next_ != hint && n->next_) {
                std::size_t next_bucket = this->hash_to_bucket(
                    node_algo::next_node(n)->hash_);
                if (next_bucket != this->hash_to_bucket(n->hash_)) {
                    this->get_bucket(next_bucket)->next_ = n;
                }
            }
            ++this->size_;
            return n;
        }


#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#   if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
        iterator emplace_(boost::unordered::detail::emplace_args1<
                boost::unordered::detail::please_ignore_this_overload> const&)
        {
            BOOST_ASSERT(false);
            return iterator();
        }

        iterator emplace_hint_(c_iterator, boost::unordered::detail::emplace_args1<
                boost::unordered::detail::please_ignore_this_overload> const&)
        {
            BOOST_ASSERT(false);
            return iterator();
        }
#   else
        iterator emplace_(
                boost::unordered::detail::please_ignore_this_overload const&)
        {
            BOOST_ASSERT(false);
            return iterator();
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
        iterator emplace_(BOOST_UNORDERED_EMPLACE_ARGS)
        {
            return iterator(emplace_impl(
                boost::unordered::detail::func::construct_value_generic(
                    this->node_alloc(), BOOST_UNORDERED_EMPLACE_FORWARD)));
        }

        template <BOOST_UNORDERED_EMPLACE_TEMPLATE>
        iterator emplace_hint_(c_iterator hint, BOOST_UNORDERED_EMPLACE_ARGS)
        {
            return iterator(emplace_hint_impl(hint,
                boost::unordered::detail::func::construct_value_generic(
                    this->node_alloc(), BOOST_UNORDERED_EMPLACE_FORWARD)));
        }

        iterator emplace_impl(node_pointer n)
        {
            node_tmp a(n, this->node_alloc());
            const_key_type& k = this->get_key(a.node_->value());
            std::size_t key_hash = this->hash(k);
            node_pointer position = this->find_node(key_hash, k);
            this->reserve_for_insert(this->size_ + 1);
            return iterator(this->add_node(a.release(), key_hash, position));
        }

        iterator emplace_hint_impl(c_iterator hint, node_pointer n)
        {
            node_tmp a(n, this->node_alloc());
            const_key_type& k = this->get_key(a.node_->value());
            if (hint.node_ && this->key_eq()(k, this->get_key(*hint))) {
                this->reserve_for_insert(this->size_ + 1);
                return iterator(this->add_using_hint(a.release(), hint.node_));
            }
            else {
                std::size_t key_hash = this->hash(k);
                node_pointer position = this->find_node(key_hash, k);
                this->reserve_for_insert(this->size_ + 1);
                return iterator(this->add_node(a.release(), key_hash, position));
            }
        }

        void emplace_impl_no_rehash(node_pointer n)
        {
            node_tmp a(n, this->node_alloc());
            const_key_type& k = this->get_key(a.node_->value());
            std::size_t key_hash = this->hash(k);
            node_pointer position = this->find_node(key_hash, k);
            this->add_node(a.release(), key_hash, position);
        }

        template <typename NodeType>
        iterator move_insert_node_type(NodeType& np) {
            iterator result;

            if (np) {
                const_key_type& k = this->get_key(np.ptr_->value());
                std::size_t key_hash = this->hash(k);
                node_pointer pos = this->find_node(key_hash, k);
                this->reserve_for_insert(this->size_ + 1);
                result = iterator(this->add_node(np.ptr_, key_hash, pos));
                np.ptr_ = node_pointer();
            }

            return result;
        }

        template <typename NodeType>
        iterator move_insert_node_type_with_hint(c_iterator hint, NodeType& np) {
            iterator result;

            if (np) {
                const_key_type& k = this->get_key(np.ptr_->value());

                if (hint.node_ && this->key_eq()(k, this->get_key(*hint))) {
                    this->reserve_for_insert(this->size_ + 1);
                    result = iterator(this->add_using_hint(np.ptr_, hint.node_));
                }
                else {
                    std::size_t key_hash = this->hash(k);
                    node_pointer pos = this->find_node(key_hash, k);
                    this->reserve_for_insert(this->size_ + 1);
                    result = iterator(this->add_node(np.ptr_, key_hash, pos));
                }
                np.ptr_ = node_pointer();
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////
        // Insert range methods

        // if hash function throws, or inserting > 1 element, basic exception
        // safety. Strong otherwise
        template <class I>
        void insert_range(I i, I j, typename
            boost::unordered::detail::enable_if_forward<I, void*>::type = 0)
        {
            if(i == j) return;

            std::size_t distance = std::distance(i, j);
            if(distance == 1) {
                emplace_impl(
                    boost::unordered::detail::func::construct_value(
                        this->node_alloc(), *i));
            }
            else {
                // Only require basic exception safety here
                this->reserve_for_insert(this->size_ + distance);

                for (; i != j; ++i) {
                    emplace_impl_no_rehash(
                        boost::unordered::detail::func::construct_value(
                            this->node_alloc(), *i));
                }
            }
        }

        template <class I>
        void insert_range(I i, I j, typename
            boost::unordered::detail::disable_if_forward<I, void*>::type = 0)
        {
            for (; i != j; ++i) {
                emplace_impl(
                    boost::unordered::detail::func::construct_value(
                        this->node_alloc(), *i));
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // Erase
        //
        // no throw

        std::size_t erase_key(const_key_type& k)
        {
            if(!this->size_) return 0;

            std::size_t key_hash = this->hash(k);
            std::size_t bucket_index = this->hash_to_bucket(key_hash);
            link_pointer prev = this->find_previous_node(k, key_hash, bucket_index);
            if (!prev) return 0;

            node_pointer first_node = node_algo::next_node(prev);
            node_pointer group_end = node_algo::next_group(first_node, this);

            std::size_t deleted_count = this->delete_nodes(prev, group_end);
            this->fix_bucket(bucket_index, prev);
            return deleted_count;
        }

        ////////////////////////////////////////////////////////////////////////
        // fill_buckets

        void copy_buckets(table const& src) {
            this->create_buckets(this->bucket_count_);

            for (node_pointer n = src.begin_node(); n;) {
                std::size_t key_hash = n->hash_;
                node_pointer group_end(node_algo::next_group(n, this));
                node_pointer pos = this->add_node(
                    boost::unordered::detail::func::construct_value(
                        this->node_alloc(), n->value()), key_hash, node_pointer());
                for (n = node_algo::next_node(n); n != group_end; n = node_algo::next_node(n))
                {
                    this->add_node(
                        boost::unordered::detail::func::construct_value(
                            this->node_alloc(), n->value()), key_hash, pos);
                }
            }
        }

        void move_buckets(table const& src) {
            this->create_buckets(this->bucket_count_);

            for (node_pointer n = src.begin_node(); n;) {
                std::size_t key_hash = n->hash_;
                node_pointer group_end(node_algo::next_group(n, this));
                node_pointer pos = this->add_node(
                    boost::unordered::detail::func::construct_value(
                        this->node_alloc(), boost::move(n->value())), key_hash, node_pointer());
                for (n = node_algo::next_node(n); n != group_end; n = node_algo::next_node(n))
                {
                    this->add_node(
                        boost::unordered::detail::func::construct_value(
                            this->node_alloc(), boost::move(n->value())), key_hash, pos);
                }
            }
        }

        void assign_buckets(table const& src) {
            node_holder<node_allocator> holder(*this);
            for (node_pointer n = src.begin_node(); n;) {
                std::size_t key_hash = n->hash_;
                node_pointer group_end(node_algo::next_group(n, this));
                node_pointer pos = this->add_node(holder.copy_of(n->value()), key_hash, node_pointer());
                for (n = node_algo::next_node(n); n != group_end; n = node_algo::next_node(n))
                {
                    this->add_node(holder.copy_of(n->value()), key_hash, pos);
                }
            }
        }

        void move_assign_buckets(table& src) {
            node_holder<node_allocator> holder(*this);
            for (node_pointer n = src.begin_node(); n;) {
                std::size_t key_hash = n->hash_;
                node_pointer group_end(node_algo::next_group(n, this));
                node_pointer pos = this->add_node(holder.move_copy_of(n->value()), key_hash, node_pointer());
                for (n = node_algo::next_node(n); n != group_end; n = node_algo::next_node(n))
                {
                    this->add_node(holder.move_copy_of(n->value()), key_hash, pos);
                }
            }
        }
    };
}}}

#endif
