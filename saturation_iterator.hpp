// "saturation_iterator" -*- C++ -*-

// Copyright (C) 2014 Aaron Conole
//
// This file is governed by the 'use this freely' common-sense license
// This means the following:
// - Take this header
// - #include it in your project (commercial, or non)
// - ???
// - profit

///
/// saturation_iterator
/// Implementation of a cyclical iterator for continuous (modulo) traversal of an iterable
/// container / type.
/// This has proven useful for computing dot-products and running FIR filters using the
/// C++ std::inner_product() function.
///

/** @file saturation_iterator
 * This is a cxx-useful header
 */

#pragma once

#ifndef __SATURATION_ITERATOR__H__
#define __SATURATION_ITERATOR__H__

#include <iterator>
#include <algorithm>

namespace cxx_utils
{
    namespace iterator
    {
        /**
         * Cyclical iterators are a special class of wrapper on an iterator and associated
         * "container" structure. Cyclical iterators never reach the 'end' position, merely
         * cycling back to the beginning when they would reach end. This is curiously useful
         * modulo behavior allows using variable sized lists, or designing recurring filter
         * structures.
         */
        template <typename _InnerIter>
        class saturation_iterator : public std::iterator<
            typename std::iterator_traits<_InnerIter>::iterator_category,
            typename std::iterator_traits<_InnerIter>::value_type,
            typename std::iterator_traits<_InnerIter>::difference_type,
            typename std::iterator_traits<_InnerIter>::pointer,
            typename std::iterator_traits<_InnerIter>::reference>
        {
        protected:
            _InnerIter m_iSaturationIter;
            _InnerIter m_iSaturationIterBegin;
            _InnerIter m_iSaturationIterEnd;
        public:
            typedef _InnerIter                                       iterator_type;
            typedef typename std::iterator_traits<_InnerIter>::difference_type difference_type;
            typedef typename std::iterator_traits<_InnerIter>::reference   reference;
            typedef typename std::iterator_traits<_InnerIter>::pointer     pointer;

            saturation_iterator() : m_iSaturationIter(), m_iSaturationIterBegin(), m_iSaturationIterEnd() {}

            /**
             * This %iterator will constantly check for equality against @p iEnd and if
             * matching, will reset to @p iBegin
             */
            explicit
            saturation_iterator( _InnerIter iBegin, _InnerIter iEnd ) :
                m_iSaturationIter( iBegin ),
                m_iSaturationIterBegin( iBegin ), m_iSaturationIterEnd( iEnd ) {}


            /**
             * Copy-constructor
             */
            saturation_iterator( const saturation_iterator& rhs ) :
                m_iSaturationIter( rhs.m_iSaturationIter ),
                m_iSaturationIterBegin( rhs.m_iSaturationIterBegin ),
                m_iSaturationIterEnd( rhs.m_iSaturationIterEnd ) {}

            /**
             * @return @c m_iCurrentIter - the %iterator used for underlying work
             */
            iterator_type
            base() const
            { return m_iSaturationIter; }

        
            /**
             *  A saturation_iterator across other types can be copied in the normal
             *  fashion.
             */
            template <typename _Iter>
            saturation_iterator( const saturation_iterator<_Iter> &rhs )
                : m_iSaturationIter( rhs.base() ), m_iSaturationIterBegin( rhs.m_iSaturationIterBegin ), m_iSaturationIterEnd( rhs.m_iSaturationIterEnd ) {}

            /**
             * @return Calls operator*() of @c m_iCurrentIter. If this %iterator is a pointer,
             * this implies a simple dereference and returns a reference
             */
            reference
            operator*() const
            { return *m_iSaturationIter; }

            /**
             * @return Calls operator->() of @c m_iCurrentIter. If this %iterator is a pointer,
             * this implies a simple dereference and returns a pointer
             */
            pointer
            operator->() const
            { return &(operator*()); }

            /**
             * Post-incr. advances this %iterator by one, loops to @c m_iSaturationIterBegin if
             * @c m_iSaturationIter would equal @c m_iSaturationIterEnd, and returns a reference to
             * this.
             */
            saturation_iterator&
            operator++()
            {
                ++m_iSaturationIter;
                if( m_iSaturationIter == m_iSaturationIterEnd )
                    m_iSaturationIter--;
                return *this;
            }

            /**
             * Pre-incr. makes a copy of this, advances this %iterator by one,
             * loops to @c m_iSaturationIterBegin if @c m_iSaturationIter would equal
             * @c m_iSaturationIterEnd, and returns the copy.
             */
            saturation_iterator
            operator++(int)
            {
                saturation_iterator __tmp = *this;
                ++m_iSaturationIter;
                if( m_iSaturationIter == m_iSaturationIterEnd )
                    m_iSaturationIter--;
                
                return __tmp;
            }

        
            saturation_iterator&
            operator--()
            {
                if( m_iSaturationIter != m_iSaturationIterBegin )
                    --m_iSaturationIter;
                return *this;
            }

            saturation_iterator
            operator--(int)
            {
                saturation_iterator __tmp = *this;
                if( m_iSaturationIter != m_iSaturationIterBegin )
                    --m_iSaturationIter;
                return __tmp;
            }
        
            difference_type
            compute_position_fwd( difference_type n ) const
            {
                difference_type ncur = std::distance( m_iSaturationIter,
                                                      m_iSaturationIterEnd );
                // if n > ncur-1, ncur-1, otherwise n;
                return n > ncur-1 ? ncur-1 : n;
            }
        
            difference_type
            compute_position_rev( difference_type n ) const
            {
                difference_type ncur = std::distance( m_iSaturationIterBegin,
                                                      m_iSaturationIter );
                // if ncur == 0, return ncur;
                if( !ncur ) return ncur;

                // otherwise, if n > ncur-1 ? ncur-1 : n;
                return n > ncur-1 ? ncur-1 : n;
            }
        
            saturation_iterator
            operator+(difference_type n) const
            {
                return saturation_iterator( (m_iSaturationIterBegin + compute_position_fwd(n)), m_iSaturationIterEnd );
            }
        
            saturation_iterator&
            operator+=(difference_type n)
            {
                m_iSaturationIter = m_iSaturationIterBegin + compute_position_fwd(n);
                return *this;
            }
        
            saturation_iterator
            operator-(difference_type n) const
            {
                return saturation_iterator( m_iSaturationIterBegin + compute_position_rev(n) );
            }
        
            saturation_iterator&
            operator-=(difference_type n)
            {
                m_iSaturationIter = m_iSaturationIterBegin + compute_position_rev(n);
                return *this;
            }

            reference
            operator[](difference_type n)const
            {
                return *(*this + n);
            }
        };

        template <typename _Iterator>
        inline bool
        operator==(const saturation_iterator<_Iterator> &lhs,
                   const saturation_iterator<_Iterator> &rhs)
        { return lhs.base() == rhs.base(); }

        template <typename _Iterator>
        inline bool
        operator<(const saturation_iterator<_Iterator> &lhs,
                  const saturation_iterator<_Iterator> &rhs)
        { return lhs.base() < rhs.base(); }

        template <typename _Iterator>
        inline bool
        operator>(const saturation_iterator<_Iterator> &lhs,
                  const saturation_iterator<_Iterator> &rhs)
        { return rhs.base() < lhs.base(); }
    
        template <typename _Iterator>
        inline bool
        operator!=(const saturation_iterator<_Iterator> &lhs,
                   const saturation_iterator<_Iterator> &rhs)
        { return !(lhs == rhs); }

        template <typename _Iterator>
        inline bool
        operator>=(const saturation_iterator<_Iterator> &lhs,
                   const saturation_iterator<_Iterator> &rhs)
        { return !( rhs < lhs ); }

        template <typename _Iterator>
        inline bool
        operator<=(const saturation_iterator<_Iterator> &lhs,
                   const saturation_iterator<_Iterator> &rhs)
        { return !( lhs < rhs ); }
    }    
    
}

#endif
