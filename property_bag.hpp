// "property_bag" -*- C++ -*-

// Copyright (C) 2015 Aaron Conole
//
// This file is governed by the 'use this freely' common-sense license
// This means the following:
// - Take this header
// - #include it in your project (commercial, or non)
// - ???
// - profit

///
///

#pragma once

#include <map>
#include <atomic>
#include <memory>
#include <typeinfo>
#include <stdexcept>

#ifndef __PROPERTY_BAG__H__
#define __PROPERTY_BAG__H__

namespace cxx_utils
{
    namespace misc
    {
        class fast_rtti
        {
            static int next_magic()
            {
                static std::atomic_int magic;
                return magic++;
            }

            template <typename Magic>
            static int rtti_magic()
            {
                static int result(next_magic());
                return result;
            }

            struct base_rtti
            {
                int magic_;

                base_rtti(const int magic) : magic_(magic) { }
                virtual ~base_rtti(){}
            };

            template <typename _T>
            struct wrapped_rtti : public base_rtti
            {
                wrapped_rtti(const _T &rhs) : base_rtti(rtti_magic<_T>()),
                                              value_(rhs) { }

                const _T value_;
            };

            std::shared_ptr<base_rtti> base_value_;
            
        public:

            template <typename _T>
            fast_rtti( const _T &rhs ) :
                base_value_(std::shared_ptr<base_rtti>(new wrapped_rtti<_T>(rhs))){}

            template <typename _T>
            const _T &to() const
            {
                if( rtti_magic<_T>() != this->base_value_->magic_ )
                    throw std::bad_cast();

                wrapped_rtti<_T> *ptrderived =
                    static_cast<wrapped_rtti<_T> *>(this->base_value_.get());

                return ptrderived->value_;
            }

            template <typename _T>
            bool try_to(_T &rhs) const
            {
                if( rtti_magic<_T>() != this->base_value_->magic_ )
                    return false;

                wrapped_rtti<_T> *ptrderived =
                    static_cast<wrapped_rtti<_T> *>(this->base_value_.get());

                rhs = ptrderived->value_;
                
                return true;
            }
        };
    }
    
    namespace container
    {
        template <typename KeyType, typename Compare = std::less<KeyType>,
                  class Alloc=std::allocator<std::pair<const KeyType, cxx_utils::misc::fast_rtti> > >
        class property_bag
        {
            typedef std::map<KeyType, cxx_utils::misc::fast_rtti, Compare, Alloc> Bag;
            Bag bag_;
            
        public:
            typedef typename Bag::key_type key_type;
            typedef typename Bag::value_type value_type;
            typedef typename Bag::key_compare key_compare;
            typedef typename Bag::allocator_type allocator_type;
            typedef typename allocator_type::reference reference;
            typedef typename allocator_type::const_reference const_reference;
            typedef typename allocator_type::pointer pointer;
            typedef typename allocator_type::const_pointer const_pointer;
            typedef typename Bag::iterator iterator;
            typedef typename Bag::const_iterator const_iterator;
            typedef typename Bag::reverse_iterator reverse_iterator;
            typedef typename Bag::const_reverse_iterator const_reverse_iterator;
            typedef typename Bag::difference_type difference_type;
            typedef typename Bag::size_type size_type;

            explicit property_bag(const key_compare &comp = key_compare(),
                                  const allocator_type &ac = allocator_type()):bag_(comp, ac){}

            template <class InputIterator>
            property_bag(InputIterator first, InputIterator last,
                         const key_compare &comp = key_compare(),
                         const allocator_type &ac = allocator_type())
                : bag_(first, last, comp, ac) {}

            property_bag(const property_bag &pb) : bag_(pb.bag_) {}

            template <typename T>
            void set_property(const key_type& k, const T &val)
            {
                iterator i = bag_.find(k);
                if( i != bag_.end() )
                    bag_.erase(i);

                bag_.insert(std::pair<key_type,cxx_utils::misc::fast_rtti>(k, val));
            }

            template <typename T>
            bool get_property(const key_type &k, T& t) const
            {
                typename Bag::const_iterator i = bag_.find(k);
                if( i == bag_.end() )
                    throw std::out_of_range("requested key not found");

                return i->second.try_to(t);
            }
        };
    }
}

#endif
