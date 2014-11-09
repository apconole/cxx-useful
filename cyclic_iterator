// "cyclic_iterator" -*- C++ -*-

// Copyright (C) 2014 Aaron Conole
//
// This file is governed by the 'use this freely' common-sense license
// This means the following:
// - Take this header
// - #include it in your project (commercial, or non)
// - ???
// - profit

///
/// cyclic_iterator
/// Implementation of a cyclical iterator for continuous (modulo) traversal of an iterable
/// container / type.
/// This has proven useful for computing dot-products and running FIR filters using the
/// C++ std::inner_product() function.
///

/** @file cyclic_iterator
 * This is a cxx-useful header
 */

#pragma once

#ifndef __CYCLIC_ITERATOR__H__
#define __CYCLIC_ITERATOR__H__

#include <iterator>
#include <algorithm>

namespace cxx_utils
{
    /**
     * Cyclical iterators are a special class of wrapper on an iterator and associated
     * "container" structure. Cyclical iterators never reach the 'end' position, merely
     * cycling back to the beginning when they would reach end. This is curiously useful
     * modulo behavior allows using variable sized lists, or designing recurring filter
     * structures.
     */
    template <typename _InnerIter>
    class cyclic_iterator : public std::iterator<
        typename std::iterator_traits<_InnerIter>::iterator_category,
        typename std::iterator_traits<_InnerIter>::value_type,
        typename std::iterator_traits<_InnerIter>::difference_type,
        typename std::iterator_traits<_InnerIter>::pointer,
        typename std::iterator_traits<_InnerIter>::reference>
    {
    protected:
        _InnerIter m_iCyclicIter;
        _InnerIter m_iCyclicIterBegin;
        _InnerIter m_iCyclicIterEnd;
    public:
        typedef _InnerIter                                       iterator_type;
        typedef typename std::iterator_traits<_InnerIter>::difference_type difference_type;
        typedef typename std::iterator_traits<_InnerIter>::reference   reference;
        typedef typename std::iterator_traits<_InnerIter>::pointer     pointer;

        cyclic_iterator() : m_iCyclicIter(), m_iCyclicIterBegin(), m_iCyclicIterEnd() {}

        /**
         * This %iterator will constantly check for equality against @p iEnd and if
         * matching, will reset to @p iBegin
         */
        explicit
        cyclic_iterator( _InnerIter iBegin, _InnerIter iEnd ) :
            m_iCyclicIter( iBegin ),
            m_iCyclicIterBegin( iBegin ), m_iCyclicIterEnd( iEnd ) {}


        /**
         * Copy-constructor
         */
        cyclic_iterator( const cyclic_iterator& rhs ) :
            m_iCyclicIter( rhs.m_iCyclicIter ),
            m_iCyclicIterBegin( rhs.m_iCyclicIterBegin ),
            m_iCyclicIterEnd( rhs.m_iCyclicIterEnd ) {}

        /**
         * @return @c m_iCurrentIter - the %iterator used for underlying work
         */
        iterator_type
        base() const
        { return m_iCyclicIter; }

        
        /**
         *  A cyclic_iterator across other types can be copied in the normal
         *  fashion.
         */
        template <typename _Iter>
        cyclic_iterator( const cyclic_iterator<_Iter> &rhs )
            : m_iCyclicIter( rhs.base() ), m_iCyclicIterBegin( rhs.m_iCyclicIterBegin ), m_iCyclicIterEnd( rhs.m_iCyclicIterEnd ) {}

        /**
         * @return Calls operator*() of @c m_iCurrentIter. If this %iterator is a pointer,
         * this implies a simple dereference and returns a reference
         */
        reference
        operator*() const
        { return *m_iCyclicIter; }

        /**
         * @return Calls operator->() of @c m_iCurrentIter. If this %iterator is a pointer,
         * this implies a simple dereference and returns a pointer
         */
        pointer
        operator->() const
        { return &(operator*()); }

        /**
         * Post-incr. advances this %iterator by one, loops to @c m_iCyclicIterBegin if
         * @c m_iCyclicIter would equal @c m_iCyclicIterEnd, and returns a reference to
         * this.
         */
        cyclic_iterator&
        operator++()
        {
            ++m_iCyclicIter;
            if( m_iCyclicIter == m_iCyclicIterEnd )
                m_iCyclicIter = m_iCyclicIterBegin;
            return *this;
        }

        /**
         * Pre-incr. makes a copy of this, advances this %iterator by one,
         * loops to @c m_iCyclicIterBegin if @c m_iCyclicIter would equal
         * @c m_iCyclicIterEnd, and returns the copy.
         */
        cyclic_iterator
        operator++(int)
        {
            cyclic_iterator __tmp = *this;
            ++m_iCyclicIter;
            if( m_iCyclicIter == m_iCyclicIterEnd )
                m_iCyclicIter = m_iCyclicIterBegin;
            return __tmp;
        }

        
        cyclic_iterator&
        operator--()
        {
            if( m_iCyclicIter == m_iCyclicIterBegin )
                m_iCyclicIter = m_iCyclicIterEnd;
            --m_iCyclicIter;
            return *this;
        }

        cyclic_iterator
        operator--(int)
        {
            cyclic_iterator __tmp = *this;
            if( m_iCyclicIter == m_iCyclicIterBegin )
                m_iCyclicIter = m_iCyclicIterEnd;
            --m_iCyclicIter;
            return __tmp;
        }
        
        difference_type
        compute_position_fwd( difference_type n ) const
        {
            // modulo size
            difference_type ntot = std::distance( m_iCyclicIterBegin,
                                                  m_iCyclicIterEnd );
            difference_type ncur = std::distance( m_iCyclicIter,
                                                  m_iCyclicIterEnd );
            return (ncur + n) % ntot;
        }
        
        difference_type
        compute_position_rev( difference_type n ) const
        {
            // modulo size
            difference_type ntot = std::distance( m_iCyclicIterBegin,
                                                  m_iCyclicIterEnd );
            difference_type ncur = std::distance( m_iCyclicIter,
                                                  m_iCyclicIterEnd );
            return std::abs((ncur - n) % ntot);
        }
        
        cyclic_iterator
        operator+(difference_type n) const
        {
            return cyclic_iterator( (m_iCyclicIterBegin + compute_position_fwd(n)), m_iCyclicIterEnd );
        }
        
        cyclic_iterator&
        operator+=(difference_type n)
        {
            m_iCyclicIter = m_iCyclicIterBegin + compute_position_fwd(n);
            return *this;
        }
        
        cyclic_iterator
        operator-(difference_type n) const
        {
            return cyclic_iterator( m_iCyclicIterBegin + compute_position_rev(n) );
        }
        
        cyclic_iterator&
        operator-=(difference_type n)
        {
            m_iCyclicIter = m_iCyclicIterBegin + compute_position_rev(n);
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
    operator==(const cyclic_iterator<_Iterator> &lhs,
               const cyclic_iterator<_Iterator> &rhs)
    { return lhs.base() == rhs.base(); }

    template <typename _Iterator>
    inline bool
    operator<(const cyclic_iterator<_Iterator> &lhs,
              const cyclic_iterator<_Iterator> &rhs)
    { return lhs.base() < rhs.base(); }

    template <typename _Iterator>
    inline bool
    operator>(const cyclic_iterator<_Iterator> &lhs,
              const cyclic_iterator<_Iterator> &rhs)
    { return rhs.base() < lhs.base(); }
    
    template <typename _Iterator>
    inline bool
    operator!=(const cyclic_iterator<_Iterator> &lhs,
               const cyclic_iterator<_Iterator> &rhs)
    { return !(lhs == rhs); }

    template <typename _Iterator>
    inline bool
    operator>=(const cyclic_iterator<_Iterator> &lhs,
               const cyclic_iterator<_Iterator> &rhs)
    { return !( rhs < lhs ); }

    template <typename _Iterator>
    inline bool
    operator<=(const cyclic_iterator<_Iterator> &lhs,
               const cyclic_iterator<_Iterator> &rhs)
    { return !( lhs < rhs ); }
    
    
}

#endif
