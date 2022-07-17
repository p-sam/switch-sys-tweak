/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <vapours/common.hpp>
#include <vapours/assert.hpp>
#include <vapours/util/util_parent_of_member.hpp>

namespace ams::util {

    #pragma GCC push_options
    #pragma GCC optimize ("-O3")

    /* Forward declare implementation class for Node. */
    namespace impl {

        class IntrusiveListImpl;

    }

    class IntrusiveListNode {
        NON_COPYABLE(IntrusiveListNode);
        private:
            friend class impl::IntrusiveListImpl;

            IntrusiveListNode *prev;
            IntrusiveListNode *next;
        public:
            constexpr ALWAYS_INLINE IntrusiveListNode() : prev(this), next(this) { /* ... */ }

            constexpr ALWAYS_INLINE bool IsLinked() const {
                return this->next != this;
            }
        private:
            constexpr ALWAYS_INLINE void LinkPrev(IntrusiveListNode *node) {
                /* We can't link an already linked node. */
                AMS_ASSERT(!node->IsLinked());
                this->SplicePrev(node, node);
            }

            constexpr ALWAYS_INLINE void SplicePrev(IntrusiveListNode *first, IntrusiveListNode *last) {
                /* Splice a range into the list. */
                auto last_prev = last->prev;
                first->prev = this->prev;
                this->prev->next = first;
                last_prev->next = this;
                this->prev = last_prev;
            }

            constexpr ALWAYS_INLINE void LinkNext(IntrusiveListNode *node) {
                /* We can't link an already linked node. */
                AMS_ASSERT(!node->IsLinked());
                return this->SpliceNext(node, node);
            }

            constexpr ALWAYS_INLINE void SpliceNext(IntrusiveListNode *first, IntrusiveListNode *last) {
                /* Splice a range into the list. */
                auto last_prev = last->prev;
                first->prev = this;
                last_prev->next = next;
                this->next->prev = last_prev;
                this->next = first;
            }

            constexpr ALWAYS_INLINE void Unlink() {
                this->Unlink(this->next);
            }

            constexpr ALWAYS_INLINE void Unlink(IntrusiveListNode *last) {
                /* Unlink a node from a next node. */
                auto last_prev = last->prev;
                this->prev->next = last;
                last->prev = this->prev;
                last_prev->next = this;
                this->prev = last_prev;
            }

            constexpr ALWAYS_INLINE IntrusiveListNode *GetPrev() {
                return this->prev;
            }

            constexpr ALWAYS_INLINE const IntrusiveListNode *GetPrev() const {
                return this->prev;
            }

            constexpr ALWAYS_INLINE IntrusiveListNode *GetNext() {
                return this->next;
            }

            constexpr ALWAYS_INLINE const IntrusiveListNode *GetNext() const {
                return this->next;
            }
    };
    /* DEPRECATED: static_assert(std::is_literal_type<IntrusiveListNode>::value); */

    namespace impl {

        class IntrusiveListImpl {
            NON_COPYABLE(IntrusiveListImpl);
            private:
                IntrusiveListNode root_node;
            public:
                template<bool Const>
                class Iterator;

                using value_type             = IntrusiveListNode;
                using size_type              = size_t;
                using difference_type        = ptrdiff_t;
                using pointer                = value_type *;
                using const_pointer          = const value_type *;
                using reference              = value_type &;
                using const_reference        = const value_type &;
                using iterator               = Iterator<false>;
                using const_iterator         = Iterator<true>;
                using reverse_iterator       = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;

                template<bool Const>
                class Iterator {
                    public:
                        using iterator_category = std::bidirectional_iterator_tag;
                        using value_type        = typename IntrusiveListImpl::value_type;
                        using difference_type   = typename IntrusiveListImpl::difference_type;
                        using pointer           = typename std::conditional<Const, IntrusiveListImpl::const_pointer, IntrusiveListImpl::pointer>::type;
                        using reference         = typename std::conditional<Const, IntrusiveListImpl::const_reference, IntrusiveListImpl::reference>::type;
                    private:
                        pointer node;
                    public:
                        constexpr ALWAYS_INLINE explicit Iterator(pointer n) : node(n) { /* ... */ }

                        constexpr ALWAYS_INLINE bool operator==(const Iterator &rhs) const {
                            return this->node == rhs.node;
                        }

                        constexpr ALWAYS_INLINE bool operator!=(const Iterator &rhs) const {
                            return !(*this == rhs);
                        }

                        constexpr ALWAYS_INLINE pointer operator->() const {
                            return this->node;
                        }

                        constexpr ALWAYS_INLINE reference operator*() const {
                            return *this->node;
                        }

                        constexpr ALWAYS_INLINE Iterator &operator++() {
                            this->node = this->node->next;
                            return *this;
                        }

                        constexpr ALWAYS_INLINE Iterator &operator--() {
                            this->node = this->node->prev;
                            return *this;
                        }

                        constexpr ALWAYS_INLINE Iterator operator++(int) {
                            const Iterator it{*this};
                            ++(*this);
                            return it;
                        }

                        constexpr ALWAYS_INLINE Iterator operator--(int) {
                            const Iterator it{*this};
                            --(*this);
                            return it;
                        }

                        constexpr ALWAYS_INLINE operator Iterator<true>() const {
                            return Iterator<true>(this->node);
                        }

                        constexpr ALWAYS_INLINE Iterator<false> GetNonConstIterator() const {
                            return Iterator<false>(const_cast<IntrusiveListImpl::pointer>(this->node));
                        }
                };
            public:
                constexpr ALWAYS_INLINE IntrusiveListImpl() : root_node() { /* ... */ }

                /* Iterator accessors. */
                constexpr ALWAYS_INLINE iterator begin() {
                    return iterator(this->root_node.GetNext());
                }

                constexpr ALWAYS_INLINE const_iterator begin() const {
                    return const_iterator(this->root_node.GetNext());
                }

                constexpr ALWAYS_INLINE iterator end() {
                    return iterator(&this->root_node);
                }

                constexpr ALWAYS_INLINE const_iterator end() const {
                    return const_iterator(&this->root_node);
                }

                constexpr ALWAYS_INLINE iterator iterator_to(reference v) {
                    /* Only allow iterator_to for values in lists. */
                    AMS_ASSERT(v.IsLinked());
                    return iterator(&v);
                }

                constexpr ALWAYS_INLINE const_iterator iterator_to(const_reference v) const {
                    /* Only allow iterator_to for values in lists. */
                    AMS_ASSERT(v.IsLinked());
                    return const_iterator(&v);
                }

                /* Content management. */
                constexpr ALWAYS_INLINE bool empty() const {
                    return !this->root_node.IsLinked();
                }

                constexpr ALWAYS_INLINE size_type size() const {
                    return static_cast<size_type>(std::distance(this->begin(), this->end()));
                }

                constexpr ALWAYS_INLINE reference back() {
                    return *this->root_node.GetPrev();
                }

                constexpr ALWAYS_INLINE const_reference back() const {
                    return *this->root_node.GetPrev();
                }

                constexpr ALWAYS_INLINE reference front() {
                    return *this->root_node.GetNext();
                }

                constexpr ALWAYS_INLINE const_reference front() const {
                    return *this->root_node.GetNext();
                }

                constexpr ALWAYS_INLINE void push_back(reference node) {
                    this->root_node.LinkPrev(&node);
                }

                constexpr ALWAYS_INLINE void push_front(reference node) {
                    this->root_node.LinkNext(&node);
                }

                constexpr ALWAYS_INLINE void pop_back() {
                    this->root_node.GetPrev()->Unlink();
                }

                constexpr ALWAYS_INLINE void pop_front() {
                    this->root_node.GetNext()->Unlink();
                }

                constexpr ALWAYS_INLINE iterator insert(const_iterator pos, reference node) {
                    pos.GetNonConstIterator()->LinkPrev(&node);
                    return iterator(&node);
                }

                constexpr ALWAYS_INLINE void splice(const_iterator pos, IntrusiveListImpl &o) {
                    splice_impl(pos, o.begin(), o.end());
                }

                constexpr ALWAYS_INLINE void splice(const_iterator pos, IntrusiveListImpl &o, const_iterator first) {
                    AMS_UNUSED(o);
                    const_iterator last(first);
                    std::advance(last, 1);
                    splice_impl(pos, first, last);
                }

                constexpr ALWAYS_INLINE void splice(const_iterator pos, IntrusiveListImpl &o, const_iterator first, const_iterator last) {
                    AMS_UNUSED(o);
                    splice_impl(pos, first, last);
                }

                constexpr ALWAYS_INLINE iterator erase(const_iterator pos) {
                    if (pos == this->end()) {
                        return this->end();
                    }
                    iterator it(pos.GetNonConstIterator());
                    (it++)->Unlink();
                    return it;
                }

                constexpr ALWAYS_INLINE void clear() {
                    while (!this->empty()) {
                        this->pop_front();
                    }
                }
            private:
                constexpr ALWAYS_INLINE void splice_impl(const_iterator _pos, const_iterator _first, const_iterator _last) {
                    if (_first == _last) {
                        return;
                    }
                    iterator pos(_pos.GetNonConstIterator());
                    iterator first(_first.GetNonConstIterator());
                    iterator last(_last.GetNonConstIterator());
                    first->Unlink(&*last);
                    pos->SplicePrev(&*first, &*first);
                }
        };

    }

    template<class T, class Traits>
    class IntrusiveList {
        NON_COPYABLE(IntrusiveList);
        private:
            impl::IntrusiveListImpl impl;
        public:
            template<bool Const>
            class Iterator;

            using value_type             = T;
            using size_type              = size_t;
            using difference_type        = ptrdiff_t;
            using pointer                = value_type *;
            using const_pointer          = const value_type *;
            using reference              = value_type &;
            using const_reference        = const value_type &;
            using iterator               = Iterator<false>;
            using const_iterator         = Iterator<true>;
            using reverse_iterator       = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

            template<bool Const>
            class Iterator {
                public:
                    friend class ams::util::IntrusiveList<T, Traits>;

                    using ImplIterator = typename std::conditional<Const, ams::util::impl::IntrusiveListImpl::const_iterator, ams::util::impl::IntrusiveListImpl::iterator>::type;

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type        = typename IntrusiveList::value_type;
                    using difference_type   = typename IntrusiveList::difference_type;
                    using pointer           = typename std::conditional<Const, IntrusiveList::const_pointer, IntrusiveList::pointer>::type;
                    using reference         = typename std::conditional<Const, IntrusiveList::const_reference, IntrusiveList::reference>::type;
                private:
                    ImplIterator iterator;
                private:
                    constexpr explicit ALWAYS_INLINE Iterator(ImplIterator it) : iterator(it) { /* ... */ }

                    constexpr ALWAYS_INLINE ImplIterator GetImplIterator() const {
                        return this->iterator;
                    }
                public:
                    constexpr ALWAYS_INLINE bool operator==(const Iterator &rhs) const {
                        return this->iterator == rhs.iterator;
                    }

                    constexpr ALWAYS_INLINE bool operator!=(const Iterator &rhs) const {
                        return !(*this == rhs);
                    }

                    constexpr ALWAYS_INLINE pointer operator->() const {
                        return &Traits::GetParent(*this->iterator);
                    }

                    constexpr ALWAYS_INLINE reference operator*() const {
                        return Traits::GetParent(*this->iterator);
                    }

                    constexpr ALWAYS_INLINE Iterator &operator++() {
                        ++this->iterator;
                        return *this;
                    }

                    constexpr ALWAYS_INLINE Iterator &operator--() {
                        --this->iterator;
                        return *this;
                    }

                   constexpr  ALWAYS_INLINE Iterator operator++(int) {
                        const Iterator it{*this};
                        ++this->iterator;
                        return it;
                    }

                    constexpr ALWAYS_INLINE Iterator operator--(int) {
                        const Iterator it{*this};
                        --this->iterator;
                        return it;
                    }

                    constexpr ALWAYS_INLINE operator Iterator<true>() const {
                        return Iterator<true>(this->iterator);
                    }
            };
        private:
            static constexpr ALWAYS_INLINE IntrusiveListNode &GetNode(reference ref) {
                return Traits::GetNode(ref);
            }

            static constexpr ALWAYS_INLINE IntrusiveListNode const &GetNode(const_reference ref) {
                return Traits::GetNode(ref);
            }

            static constexpr ALWAYS_INLINE reference GetParent(IntrusiveListNode &node) {
                return Traits::GetParent(node);
            }

            static constexpr ALWAYS_INLINE const_reference GetParent(IntrusiveListNode const &node) {
                return Traits::GetParent(node);
            }
        public:
            constexpr ALWAYS_INLINE IntrusiveList() : impl() { /* ... */ }

            /* Iterator accessors. */
            constexpr ALWAYS_INLINE iterator begin() {
                return iterator(this->impl.begin());
            }

            constexpr ALWAYS_INLINE const_iterator begin() const {
                return const_iterator(this->impl.begin());
            }

            constexpr ALWAYS_INLINE iterator end() {
                return iterator(this->impl.end());
            }

            constexpr ALWAYS_INLINE const_iterator end() const {
                return const_iterator(this->impl.end());
            }

            constexpr ALWAYS_INLINE const_iterator cbegin() const {
                return this->begin();
            }

            constexpr ALWAYS_INLINE const_iterator cend() const {
                return this->end();
            }

            constexpr ALWAYS_INLINE reverse_iterator rbegin() {
                return reverse_iterator(this->end());
            }

            constexpr ALWAYS_INLINE const_reverse_iterator rbegin() const {
                return const_reverse_iterator(this->end());
            }

            constexpr ALWAYS_INLINE reverse_iterator rend() {
                return reverse_iterator(this->begin());
            }

            constexpr ALWAYS_INLINE const_reverse_iterator rend() const {
                return const_reverse_iterator(this->begin());
            }

            constexpr ALWAYS_INLINE const_reverse_iterator crbegin() const {
                return this->rbegin();
            }

            constexpr ALWAYS_INLINE const_reverse_iterator crend() const {
                return this->rend();
            }

            constexpr ALWAYS_INLINE iterator iterator_to(reference v) {
                return iterator(this->impl.iterator_to(GetNode(v)));
            }

            constexpr ALWAYS_INLINE const_iterator iterator_to(const_reference v) const {
                return const_iterator(this->impl.iterator_to(GetNode(v)));
            }

            /* Content management. */
            constexpr ALWAYS_INLINE bool empty() const {
                return this->impl.empty();
            }

            constexpr ALWAYS_INLINE size_type size() const {
                return this->impl.size();
            }

            constexpr ALWAYS_INLINE reference back() {
                AMS_ASSERT(!this->impl.empty());
                return GetParent(this->impl.back());
            }

            constexpr ALWAYS_INLINE const_reference back() const {
                AMS_ASSERT(!this->impl.empty());
                return GetParent(this->impl.back());
            }

            constexpr ALWAYS_INLINE reference front() {
                AMS_ASSERT(!this->impl.empty());
                return GetParent(this->impl.front());
            }

            constexpr ALWAYS_INLINE const_reference front() const {
                AMS_ASSERT(!this->impl.empty());
                return GetParent(this->impl.front());
            }

            constexpr ALWAYS_INLINE void push_back(reference ref) {
                this->impl.push_back(GetNode(ref));
            }

            constexpr ALWAYS_INLINE void push_front(reference ref) {
                this->impl.push_front(GetNode(ref));
            }

            constexpr ALWAYS_INLINE void pop_back() {
                AMS_ASSERT(!this->impl.empty());
                this->impl.pop_back();
            }

            constexpr ALWAYS_INLINE void pop_front() {
                AMS_ASSERT(!this->impl.empty());
                this->impl.pop_front();
            }

            constexpr ALWAYS_INLINE iterator insert(const_iterator pos, reference ref) {
                return iterator(this->impl.insert(pos.GetImplIterator(), GetNode(ref)));
            }

            constexpr ALWAYS_INLINE void splice(const_iterator pos, IntrusiveList &o) {
                this->impl.splice(pos.GetImplIterator(), o.impl);
            }

            constexpr ALWAYS_INLINE void splice(const_iterator pos, IntrusiveList &o, const_iterator first) {
                this->impl.splice(pos.GetImplIterator(), o.impl, first.GetImplIterator());
            }

            constexpr ALWAYS_INLINE void splice(const_iterator pos, IntrusiveList &o, const_iterator first, const_iterator last) {
                this->impl.splice(pos.GetImplIterator(), o.impl, first.GetImplIterator(), last.GetImplIterator());
            }

            constexpr ALWAYS_INLINE iterator erase(const_iterator pos) {
                return iterator(this->impl.erase(pos.GetImplIterator()));
            }

            constexpr ALWAYS_INLINE void clear() {
                this->impl.clear();
            }
    };

    template<auto T, class Derived = util::impl::GetParentType<T>>
    class IntrusiveListMemberTraits;

    template<class Parent, IntrusiveListNode Parent::*Member, class Derived>
    class IntrusiveListMemberTraits<Member, Derived> {
        public:
            using ListType = IntrusiveList<Derived, IntrusiveListMemberTraits>;
        private:
            friend class IntrusiveList<Derived, IntrusiveListMemberTraits>;

            static constexpr ALWAYS_INLINE IntrusiveListNode &GetNode(Derived &parent) {
                return parent.*Member;
            }

            static constexpr ALWAYS_INLINE IntrusiveListNode const &GetNode(Derived const &parent) {
                return parent.*Member;
            }

            static ALWAYS_INLINE Derived &GetParent(IntrusiveListNode &node) {
                return util::GetParentReference<Member, Derived>(std::addressof(node));
            }

            static ALWAYS_INLINE Derived const &GetParent(IntrusiveListNode const &node) {
                return util::GetParentReference<Member, Derived>(std::addressof(node));
            }
    };

    template<auto T, class Derived = util::impl::GetParentType<T>>
    class IntrusiveListMemberTraitsByNonConstexprOffsetOf;

    template<class Parent, IntrusiveListNode Parent::*Member, class Derived>
    class IntrusiveListMemberTraitsByNonConstexprOffsetOf<Member, Derived> {
        public:
            using ListType = IntrusiveList<Derived, IntrusiveListMemberTraitsByNonConstexprOffsetOf>;
        private:
            friend class IntrusiveList<Derived, IntrusiveListMemberTraitsByNonConstexprOffsetOf>;

            static constexpr ALWAYS_INLINE IntrusiveListNode &GetNode(Derived &parent) {
                return parent.*Member;
            }

            static constexpr ALWAYS_INLINE IntrusiveListNode const &GetNode(Derived const &parent) {
                return parent.*Member;
            }

            static ALWAYS_INLINE Derived &GetParent(IntrusiveListNode &node) {
                return *reinterpret_cast<Derived *>(reinterpret_cast<char *>(std::addressof(node)) - GetOffset());
            }

            static ALWAYS_INLINE Derived const &GetParent(IntrusiveListNode const &node) {
                return *reinterpret_cast<const Derived *>(reinterpret_cast<const char *>(std::addressof(node)) - GetOffset());
            }

            static ALWAYS_INLINE uintptr_t GetOffset() {
                return reinterpret_cast<uintptr_t>(std::addressof(reinterpret_cast<Derived *>(0)->*Member));
            }
    };

    template<class Derived>
    class IntrusiveListBaseNode : public IntrusiveListNode{};

    template<class Derived>
    class IntrusiveListBaseTraits {
        public:
            using ListType = IntrusiveList<Derived, IntrusiveListBaseTraits>;
        private:
            friend class IntrusiveList<Derived, IntrusiveListBaseTraits>;

            static constexpr ALWAYS_INLINE IntrusiveListNode &GetNode(Derived &parent) {
                return static_cast<IntrusiveListNode &>(static_cast<IntrusiveListBaseNode<Derived> &>(parent));
            }

            static constexpr ALWAYS_INLINE IntrusiveListNode const &GetNode(Derived const &parent) {
                return static_cast<const IntrusiveListNode &>(static_cast<const IntrusiveListBaseNode<Derived> &>(parent));
            }

            static constexpr ALWAYS_INLINE Derived &GetParent(IntrusiveListNode &node) {
                return static_cast<Derived &>(static_cast<IntrusiveListBaseNode<Derived> &>(node));
            }

            static constexpr ALWAYS_INLINE Derived const &GetParent(IntrusiveListNode const &node) {
                return static_cast<const Derived &>(static_cast<const IntrusiveListBaseNode<Derived> &>(node));
            }
    };

    #pragma GCC pop_options

}
