
// Copyright (C) 2003-2004 Jeremy B. Maitin-Shepard.
// Copyright (C) 2005-2011 Daniel James
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_UNORDERED_DETAIL_ALL_HPP_INCLUDED
#define BOOST_UNORDERED_DETAIL_ALL_HPP_INCLUDED

#include <boost/config.hpp>
#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <boost/unordered/detail/buckets.hpp>

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant
#endif

namespace boost { namespace unordered { namespace detail {

    ////////////////////////////////////////////////////////////////////////////
    // convert double to std::size_t

    inline std::size_t double_to_size(double f)
    {
        return f >= static_cast<double>(
            (std::numeric_limits<std::size_t>::max)()) ?
            (std::numeric_limits<std::size_t>::max)() :
            static_cast<std::size_t>(f);
    }

    // The space used to store values in a node.

    template <typename ValueType>
    struct value_base
    {
        typedef ValueType value_type;

        typename boost::aligned_storage<
            sizeof(value_type),
            boost::alignment_of<value_type>::value>::type data_;

        value_base() :
            data_()
        {}

        void* address() {
            return this;
        }

        value_type& value() {
            return *(ValueType*) this;
        }

        value_type* value_ptr() {
            return (ValueType*) this;
        }

    private:

        value_base& operator=(value_base const&);
    };

    template <typename N>
    struct table_base
    {
    protected:
        typedef N node;
        typedef boost::unordered::detail::node_traits<N> node_traits;
        typedef typename node_traits::node_pointer node_pointer;
        typedef typename node_traits::bucket_pointer bucket_pointer;
        typedef typename node_traits::link_pointer link_pointer;
        typedef typename node_traits::bucket bucket;

    public:
        typedef typename node_traits::value_type value_type;
        typedef std::size_t size_type;

    protected:

        ////////////////////////////////////////////////////////////////////////
        // Members

        std::size_t bucket_count_;
        std::size_t size_;
        float mlf_;
        std::size_t max_load_;
        bucket_pointer buckets_;

        ////////////////////////////////////////////////////////////////////////
        // Constructors

        table_base() :
            bucket_count_(0),
            size_(0),
            mlf_(1.0f),
            max_load_(0),
            buckets_() {}

        // Only call with nodes allocated with the currect allocator, or
        // one that is equal to it.
        void move_buckets_from(table_base& other)
        {
            BOOST_ASSERT(!this->buckets_);
            this->buckets_ = other.buckets_;
            this->bucket_count_ = other.bucket_count_;
            this->size_ = other.size_;
            other.buckets_ = bucket_pointer();
            other.size_ = 0;
            other.max_load_ = 0;
        }

        void swap(table_base& x)
        {
            boost::swap(this->buckets_, x.buckets_);
            boost::swap(this->bucket_count_, x.bucket_count_);
            boost::swap(this->size_, x.size_);
            std::swap(this->mlf_, x.mlf_);
            std::swap(this->max_load_, x.max_load_);
        }

        ////////////////////////////////////////////////////////////////////////
        // Node functions

        static inline node_pointer next_node(link_pointer n) {
            return static_cast<node_pointer>(n->next_);
        }

        ////////////////////////////////////////////////////////////////////////
        // Data access

        bucket_pointer get_bucket(std::size_t bucket_index) const
        {
            BOOST_ASSERT(this->buckets_);
            return this->buckets_ + static_cast<std::ptrdiff_t>(bucket_index);
        }

        link_pointer get_previous_start() const
        {
            return get_bucket(this->bucket_count_)->first_from_start();
        }

        link_pointer get_previous_start(std::size_t bucket_index) const
        {
            return get_bucket(bucket_index)->next_;
        }

        node_pointer begin_node() const
        {
            return this->size_ ? next_node(get_previous_start()) : node_pointer();
        }

        node_pointer begin_node(std::size_t bucket_index) const
        {
            if (!this->size_) return node_pointer();
            link_pointer prev = get_previous_start(bucket_index);
            return prev ? next_node(prev) : node_pointer();
        }

        void recalculate_max_load()
        {
            using namespace std;
    
            // From 6.3.1/13:
            // Only resize when size >= mlf_ * count
            this->max_load_ = this->buckets_ ? boost::unordered::detail::double_to_size(ceil(
                    static_cast<double>(this->mlf_) *
                    static_cast<double>(this->bucket_count_)
                )) : 0;

        }

    public:

        bool empty() const BOOST_NOEXCEPT
        {
            return this->size_ == 0;
        }

        size_type size() const BOOST_NOEXCEPT
        {
            return this->size_;
        }

        size_type bucket_count() const BOOST_NOEXCEPT
        {
            return this->bucket_count_;
        }

        float load_factor() const
        {
            BOOST_ASSERT(this->bucket_count_ != 0);
            return static_cast<float>(this->size_)
                / static_cast<float>(this->bucket_count_);
        }

        float max_load_factor() const BOOST_NOEXCEPT
        {
            return this->mlf_;
        }

        void max_load_factor(float z)
        {
            BOOST_ASSERT(z > 0);
            this->mlf_ = (std::max)(z, minimum_max_load_factor);
            recalculate_max_load();
        }
    };

    template <typename N, typename SetMapPolicies>
    struct iterator_base : boost::unordered::detail::table_base<N>
    {
    protected:
        typedef boost::unordered::detail::table_base<N> base;
        typedef typename base::node node;
        typedef typename base::node_pointer node_pointer;
        typedef typename base::value_type value_type;
        typedef typename SetMapPolicies::template value_things<value_type> value_things;
        typedef typename value_things::const_key_type const_key_type;
        typedef typename value_things::extractor extractor;
        typedef typename boost::unordered::detail::pick_policy<const_key_type>::type policy;
        typedef typename SetMapPolicies::template iterators<node> iterator_types;
        typedef typename SetMapPolicies::template local_iterators<node, policy> l_iterator_types;
    public:
        typedef typename base::size_type size_type;
        typedef typename iterator_types::iterator iterator;
        typedef typename iterator_types::c_iterator const_iterator;
        typedef typename l_iterator_types::iterator local_iterator;
        typedef typename l_iterator_types::c_iterator const_local_iterator;

    protected:

        const_key_type& get_key(value_type const& x) const
        {
            return extractor::extract(x);
        }

        std::size_t hash_to_bucket(std::size_t hash_value) const
        {
            return policy::to_bucket(this->bucket_count_, hash_value);
        }

        std::size_t min_buckets_for_size(std::size_t s) const
        {
            BOOST_ASSERT(this->mlf_ >= minimum_max_load_factor);

            using namespace std;

            // From insert/emplace requirements:
            //
            // size <= mlf_ * count
            // => count >= size / mlf_
            //
            // Or from rehash post-condition:
            //
            // count >= size / mlf_

            return policy::new_bucket_count(
                boost::unordered::detail::double_to_size(floor(
                    static_cast<double>(s) /
                    static_cast<double>(this->mlf_)) + 1));
        }

    public:

        std::size_t bucket_size(std::size_t index) const
        {
            node_pointer n = this->begin_node(index);
            if (!n) return 0;

            std::size_t count = 0;
            while(n && hash_to_bucket(n->hash_) == index)
            {
                ++count;
                n = base::next_node(n);
            }

            return count;
        }

        ////////////////////////////////////////////////////////////////////////
        // Iterators

        iterator begin() BOOST_NOEXCEPT
        {
            return iterator(this->begin_node());
        }

        const_iterator begin() const BOOST_NOEXCEPT
        {
            return const_iterator(this->begin_node());
        }

        iterator end() BOOST_NOEXCEPT
        {
            return iterator();
        }

        const_iterator end() const BOOST_NOEXCEPT
        {
            return const_iterator();
        }

        const_iterator cbegin() const BOOST_NOEXCEPT
        {
            return const_iterator(this->begin_node());
        }

        const_iterator cend() const BOOST_NOEXCEPT
        {
            return const_iterator();
        }

        local_iterator begin(size_type n)
        {
            return local_iterator(
                this->begin_node(n), n, this->bucket_count_);
        }

        const_local_iterator begin(size_type n) const
        {
            return const_local_iterator(
                this->begin_node(n), n, this->bucket_count_);
        }

        local_iterator end(size_type)
        {
            return local_iterator();
        }

        const_local_iterator end(size_type) const
        {
            return const_local_iterator();
        }

        const_local_iterator cbegin(size_type n) const
        {
            return const_local_iterator(
                this->begin_node(n), n, this->bucket_count_);
        }

        const_local_iterator cend(size_type) const
        {
            return const_local_iterator();
        }
    };

    template <typename NA, typename SetMapPolicies>
    struct memory_base : boost::unordered::detail::iterator_base<
            typename boost::unordered::detail::allocator_traits<NA>::value_type,
            SetMapPolicies>
    {
    protected:
        typedef boost::unordered::detail::iterator_base<
            typename boost::unordered::detail::allocator_traits<NA>::value_type,
            SetMapPolicies> base;

        typedef typename base::bucket bucket;
        typedef typename base::node_pointer node_pointer;
        typedef typename base::bucket_pointer bucket_pointer;
        typedef typename base::link_pointer link_pointer;
        typedef typename base::policy policy;
        typedef NA node_allocator;
        typedef typename boost::unordered::detail::rebind_wrap<node_allocator, bucket>::type bucket_allocator;
        typedef boost::unordered::detail::allocator_traits<node_allocator> node_allocator_traits;
        typedef boost::unordered::detail::allocator_traits<bucket_allocator> bucket_allocator_traits;

        typedef boost::unordered::detail::node_constructor<node_allocator>
            node_constructor;

        ////////////////////////////////////////////////////////////////////////
        // Members

        boost::unordered::detail::compressed<bucket_allocator, node_allocator>
            allocators_;

        ////////////////////////////////////////////////////////////////////////
        // Constructors

        memory_base(node_allocator const& a) : allocators_(a,a) {}
        memory_base(memory_base& x, boost::unordered::detail::move_tag m) : allocators_(x.allocators_,m) {}

        ////////////////////////////////////////////////////////////////////////
        // Swap and Move

        void swap_allocators(memory_base& other, false_type)
        {
            boost::unordered::detail::func::ignore_unused_variable_warning(other);

            // According to 23.2.1.8, if propagate_on_container_swap is
            // false the behaviour is undefined unless the allocators
            // are equal.
            BOOST_ASSERT(this->node_alloc() == other.node_alloc());
        }

        void swap_allocators(memory_base& other, true_type)
        {
            this->allocators_.swap(other.allocators_);
        }

        void swap(memory_base& x) {
            // I think swap can throw if Propagate::value,
            // since the allocators' swap can throw. Not sure though.
            swap_allocators(x,
                boost::unordered::detail::integral_constant<bool,
                    allocator_traits<node_allocator>::
                    propagate_on_container_swap::value>());

            this->base::swap(x);
        }


        ////////////////////////////////////////////////////////////////////////
        // Data access

        bucket_allocator const& bucket_alloc() const
        {
            return this->allocators_.first();
        }

        node_allocator const& node_alloc() const
        {
            return this->allocators_.second();
        }

        bucket_allocator& bucket_alloc()
        {
            return this->allocators_.first();
        }

        node_allocator& node_alloc()
        {
            return this->allocators_.second();
        }

        ////////////////////////////////////////////////////////////////////////
        // Load methods

    public:

        std::size_t max_size() const
        {
            using namespace std;

            // size < mlf_ * count
            return boost::unordered::detail::double_to_size(ceil(
                    static_cast<double>(this->mlf_) *
                    static_cast<double>(max_bucket_count())
                )) - 1;
        }

        std::size_t max_bucket_count() const
        {
            // -1 to account for the start bucket.
            return policy::prev_bucket_count(
                bucket_allocator_traits::max_size(this->bucket_alloc()) - 1);
        }

    protected:

        ////////////////////////////////////////////////////////////////////////
        // Create buckets

        void create_buckets(std::size_t new_count)
        {
            std::size_t length = new_count + 1;
            bucket_pointer new_buckets = bucket_allocator_traits::allocate(
                    this->bucket_alloc(), length);
            bucket_pointer constructed = new_buckets;

            BOOST_TRY {
                bucket_pointer end_ = new_buckets
                    + static_cast<std::ptrdiff_t>(length);
                for(; constructed != end_; ++constructed) {
                    new ((void*) boost::addressof(*constructed)) bucket();
                }

                if (this->buckets_)
                {
                    // Copy the nodes to the new buckets, including the dummy
                    // node if there is one.
                    (new_buckets +
                        static_cast<std::ptrdiff_t>(new_count))->next_ =
                            (this->buckets_ + static_cast<std::ptrdiff_t>(
                                this->bucket_count_))->next_;
                    destroy_buckets();
                }
                else if (bucket::extra_node)
                {
                    node_constructor a(this->node_alloc());
                    a.create_node();

                    (new_buckets +
                        static_cast<std::ptrdiff_t>(new_count))->next_ =
                            a.release();
                }
            }
            BOOST_CATCH(...) {
                for(bucket_pointer p = new_buckets; p != constructed; ++p) {
                    boost::unordered::detail::func::destroy(
                            boost::addressof(*p));
                }

                bucket_allocator_traits::deallocate(this->bucket_alloc(),
                        new_buckets, length);

                BOOST_RETHROW;
            }
            BOOST_CATCH_END

            this->bucket_count_ = new_count;
            this->buckets_ = new_buckets;
            this->recalculate_max_load();
        }

        ////////////////////////////////////////////////////////////////////////
        // Delete/destruct

        ~memory_base()
        {
            delete_buckets();
        }

        void delete_node(link_pointer prev)
        {
            node_pointer n = static_cast<node_pointer>(prev->next_);
            prev->next_ = n->next_;

            boost::unordered::detail::func::destroy_value_impl(this->node_alloc(),
                n->value_ptr());
            boost::unordered::detail::func::destroy(boost::addressof(*n));
            node_allocator_traits::deallocate(this->node_alloc(), n, 1);
            --this->size_;
        }

        std::size_t delete_nodes(link_pointer prev, link_pointer end_)
        {
            BOOST_ASSERT(prev->next_ != end_);

            std::size_t count = 0;

            do {
                delete_node(prev);
                ++count;
            } while (prev->next_ != end_);

            return count;
        }

        void delete_buckets()
        {
            if(this->buckets_) {
                if (this->size_) delete_nodes(this->get_previous_start(), link_pointer());

                if (bucket::extra_node) {
                    node_pointer n = static_cast<node_pointer>(
                            this->get_bucket(this->bucket_count_)->next_);
                    boost::unordered::detail::func::destroy(
                            boost::addressof(*n));
                    node_allocator_traits::deallocate(this->node_alloc(), n, 1);
                }

                destroy_buckets();
                this->buckets_ = bucket_pointer();
                this->max_load_ = 0;
            }

            BOOST_ASSERT(!this->size_);
        }

    public:

        void clear()
        {
            if (!this->size_) return;

            delete_nodes(this->get_previous_start(), link_pointer());
            clear_buckets();

            BOOST_ASSERT(!this->size_);
        }

    protected:

        void clear_buckets()
        {
            bucket_pointer end_ = this->get_bucket(this->bucket_count_);
            for(bucket_pointer it = this->buckets_; it != end_; ++it)
            {
                it->next_ = node_pointer();
            }
        }

        void destroy_buckets()
        {
            bucket_pointer end_ = this->get_bucket(this->bucket_count_ + 1);
            for(bucket_pointer it = this->buckets_; it != end_; ++it)
            {
                boost::unordered::detail::func::destroy(
                    boost::addressof(*it));
            }

            bucket_allocator_traits::deallocate(this->bucket_alloc(),
                this->buckets_, this->bucket_count_ + 1);
        }
    };

    template <typename Policies, typename H, typename P, typename A>
    struct table :
        boost::unordered::detail::functions<H, P>,
        boost::unordered::detail::memory_base<
            typename boost::unordered::detail::rebind_wrap<A, typename Policies::template node_types<A>::node>::type,
            typename Policies::set_map_policies
        >
    {
        template <typename Policies2, typename H2, typename P2, typename A2> friend struct table_impl;
        template <typename Policies2, typename H2, typename P2, typename A2> friend struct grouped_table_impl;
        template <typename NodeAlloc> friend struct node_holder;

    private:
        table(table const&);
        table& operator=(table const&);

    protected:
        typedef A value_allocator;
        typedef boost::unordered::detail::allocator_traits<A> value_allocator_traits;

    public:
        typedef typename value_allocator_traits::value_type value_type;
        typedef H hasher;
        typedef P key_equal;

    protected:
        typedef boost::unordered::detail::functions<H, P> functions;
        typedef boost::unordered::detail::memory_base<
            typename boost::unordered::detail::rebind_wrap<A, typename Policies::template node_types<A>::node>::type,
            typename Policies::set_map_policies
            > table_base;

        typedef typename Policies::template table_gen<H, P, A>::table table_impl;
        typedef typename functions::set_hash_functions set_hash_functions;
        typedef typename table_base::link_pointer link_pointer;
        typedef typename table_base::node_allocator node_allocator;
        typedef typename table_base::node_pointer node_pointer;
        typedef typename table_base::bucket_pointer bucket_pointer;
        typedef typename table_base::const_key_type const_key_type;
        typedef typename table_base::policy policy;

        typedef boost::unordered::detail::node_constructor<node_allocator>
            node_constructor;
        typedef boost::unordered::detail::node_tmp<node_allocator>
            node_tmp;

        ////////////////////////////////////////////////////////////////////////
        // Constructors

        table(std::size_t num_buckets,
                hasher const& hf,
                key_equal const& eq,
                node_allocator const& a) :
            functions(hf, eq),
            table_base(a)
        {
            this->bucket_count_ = policy::new_bucket_count(num_buckets);
        }

        table(table const& x, node_allocator const& a) :
            functions(x),
            table_base(a)
        {
            this->bucket_count_ = x.min_buckets_for_size(x.size_);
            this->mlf_ = x.mlf_;
        }

        table(table& x, boost::unordered::detail::move_tag m) :
            functions(x, m),
            table_base(x, m)
        {
            this->bucket_count_ = x.bucket_count_;
            this->size_ = x.size_;
            this->mlf_ = x.mlf_;
            this->max_load_ = x.max_load_;
            this->buckets_ = x.buckets_;
            x.buckets_ = bucket_pointer();
            x.size_ = 0;
            x.max_load_ = 0;
        }

        table(table& x, node_allocator const& a,
                boost::unordered::detail::move_tag m) :
            functions(x, m),
            table_base(a)
        {
            this->bucket_count_ = x.bucket_count_;
            this->mlf_ = x.mlf_;
            this->max_load_ = x.max_load_;
        }

        ////////////////////////////////////////////////////////////////////////
        // Initialisation.

        void init(table const& x)
        {
            if (x.size_) {
                static_cast<table_impl*>(this)->copy_buckets(x);
            }
        }

        void move_init(table& x)
        {
            if(this->node_alloc() == x.node_alloc()) {
                this->move_buckets_from(x);
            }
            else if(x.size_) {
                // TODO: Could pick new bucket size?
                static_cast<table_impl*>(this)->move_buckets(x);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // swap and move

        // Only swaps the allocators if propagate_on_container_swap
        void swap(table& x)
        {
            set_hash_functions op1(*this, x);
            set_hash_functions op2(x, *this);
            this->table_base::swap(x);
            op1.commit();
            op2.commit();
        }

        ////////////////////////////////////////////////////////////////////////
        // Fix buckets after delete
        //

        std::size_t fix_bucket(std::size_t bucket_index, link_pointer prev)
        {
            link_pointer next = prev->next_;
            std::size_t bucket_index2 = bucket_index;

            if (next)
            {
                bucket_index2 = this->hash_to_bucket(
                    static_cast<node_pointer>(next)->hash_);

                // If begin_node and next are in the same bucket, then
                // there's nothing to do.
                if (bucket_index == bucket_index2) return bucket_index2;

                // Update the bucket containing next.
                this->get_bucket(bucket_index2)->next_ = prev;
            }

            // Check if this bucket is now empty.
            bucket_pointer this_bucket = this->get_bucket(bucket_index);
            if (this_bucket->next_ == prev)
                this_bucket->next_ = link_pointer();

            return bucket_index2;
        }

        ////////////////////////////////////////////////////////////////////////
        // Assignment

        void assign(table const& x)
        {
            if (this != boost::addressof(x))
            {
                assign(x,
                    boost::unordered::detail::integral_constant<bool,
                        allocator_traits<node_allocator>::
                        propagate_on_container_copy_assignment::value>());
            }
        }

        void assign(table const& x, false_type)
        {
            // Strong exception safety.
            set_hash_functions new_func_this(*this, x);
            this->mlf_ = x.mlf_;
            this->recalculate_max_load();

            if (!this->size_ && !x.size_) return;

            if (x.size_ >= this->max_load_) {
                this->create_buckets(this->min_buckets_for_size(x.size_));
            }
            else {
                this->clear_buckets();
            }

            new_func_this.commit();
            static_cast<table_impl*>(this)->assign_buckets(x);
        }

        void assign(table const& x, true_type)
        {
            if (this->node_alloc() == x.node_alloc()) {
                this->allocators_.assign(x.allocators_);
                assign(x, false_type());
            }
            else {
                set_hash_functions new_func_this(*this, x);

                // Delete everything with current allocators before assigning
                // the new ones.
                this->delete_buckets();
                this->allocators_.assign(x.allocators_);

                // Copy over other data, all no throw.
                new_func_this.commit();
                this->mlf_ = x.mlf_;
                this->bucket_count_ = this->min_buckets_for_size(x.size_);
                this->max_load_ = 0;

                // Finally copy the elements.
                if (x.size_) {
                    static_cast<table_impl*>(this)->copy_buckets(x);
                }
            }
        }

        void move_assign(table& x)
        {
            if (this != boost::addressof(x))
            {
                move_assign(x,
                    boost::unordered::detail::integral_constant<bool,
                        allocator_traits<node_allocator>::
                        propagate_on_container_move_assignment::value>());
            }
        }

        void move_assign(table& x, true_type)
        {
            this->delete_buckets();
            this->move_assign_functions(x);
            this->allocators_.move_assign(x.allocators_);

            // No throw from here.
            this->mlf_ = x.mlf_;
            this->max_load_ = x.max_load_;
            this->move_buckets_from(x);
        }

        void move_assign(table& x, false_type)
        {
            if (node_alloc() == x.node_alloc()) {
                this->delete_buckets();
                this->move_assign_functions(x);

                // No throw from here.
                this->mlf_ = x.mlf_;
                this->max_load_ = x.max_load_;
                this->move_buckets_from(x);
            }
            else {
                set_hash_functions new_func_this(*this, x);
                this->mlf_ = x.mlf_;
                this->recalculate_max_load();

                if (!this->size_ && !x.size_) {
                    new_func_this.commit();
                    return;
                }

                if (x.size_ >= this->max_load_) {
                    this->create_buckets(this->min_buckets_for_size(x.size_));
                }
                else {
                    this->clear_buckets();
                }

                new_func_this.commit();
                static_cast<table_impl*>(this)->move_assign_buckets(x);
            }
        }

        // Accessors

        std::size_t hash(const_key_type& k) const
        {
            return policy::apply_hash(this->hash_function(), k);
        }

        // Find Node

        template <typename Key, typename Hash, typename Pred>
        node_pointer generic_find_node(
                Key const& k,
                Hash const& hf,
                Pred const& eq) const
        {
            return static_cast<table_impl const*>(this)->
                find_node_impl(policy::apply_hash(hf, k), k, eq);
        }

        node_pointer find_node(
                std::size_t key_hash,
                const_key_type& k) const
        {
            return static_cast<table_impl const*>(this)->
                find_node_impl(key_hash, k, this->key_eq());
        }

        node_pointer find_node(const_key_type& k) const
        {
            return static_cast<table_impl const*>(this)->
                find_node_impl(hash(k), k, this->key_eq());
        }

        // Reserve and rehash

        void reserve_for_insert(std::size_t);

    public:

        void rehash(std::size_t);
        void reserve(std::size_t);
    };

    ////////////////////////////////////////////////////////////////////////////
    // Reserve & Rehash

    // basic exception safety
    template <typename Policies, typename H, typename P, typename A>
    inline void table<Policies, H, P, A>::reserve_for_insert(std::size_t s)
    {
        if (!this->buckets_) {
            this->create_buckets((std::max)(this->bucket_count_,
                this->min_buckets_for_size(s)));
        }
        else if(s > this->max_load_) {
            std::size_t num_buckets
                = this->min_buckets_for_size((std::max)(s,
                    this->size_ + (this->size_ >> 1)));

            if (num_buckets != this->bucket_count_)
                static_cast<table_impl*>(this)->rehash_impl(num_buckets);
        }
    }

    // if hash function throws, basic exception safety
    // strong otherwise.

    template <typename Policies, typename H, typename P, typename A>
    inline void table<Policies, H, P, A>::rehash(std::size_t min_buckets)
    {
        using namespace std;

        if(!this->size_) {
            this->delete_buckets();
            this->bucket_count_ = policy::new_bucket_count(min_buckets);
        }
        else {
            min_buckets = policy::new_bucket_count((std::max)(min_buckets,
                boost::unordered::detail::double_to_size(floor(
                    static_cast<double>(this->size_) /
                    static_cast<double>(this->mlf_))) + 1));

            if(min_buckets != this->bucket_count_)
                static_cast<table_impl*>(this)->rehash_impl(min_buckets);
        }
    }

    template <typename Policies, typename H, typename P, typename A>
    inline void table<Policies, H, P, A>::reserve(std::size_t num_elements)
    {
        rehash(static_cast<std::size_t>(
            std::ceil(static_cast<double>(num_elements) / this->mlf_)));
    }
}}}

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#endif
