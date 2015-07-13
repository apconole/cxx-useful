// "cyclic_iterator" -*- C++ -*-

// Copyright (C) 2015 Aaron Conole
//
// This file is governed by the 'use this freely' common-sense license
// This means the following:
// - Take this header
// - #include it in your project (commercial, or non)
// - ???
// - profit

// for systems which support pragma once
#pragma once

#include <atomic>
#include <stdint.h>
#include <stdexcept>

#ifndef LOCK_FREE_HEAP_NODE__H
#define LOCK_FREE_HEAP_NODE__H

namespace cxx_utils
{
    namespace concurrent
    {
        struct lfstack_pop_empty : public std::runtime_error
        {
            lfstack_pop_empty() : std::runtime_error("pop on empty stack"){}
            ~lfstack_pop_empty() throw() {}
        };

        template<typename T>
        class lfstack
        {
            struct lfstackNode
            {
                lfstackNode* next;
                T           val;

                lfstackNode(const T &value) : next(0), val(value) { }
            };

            typedef lfstackNode* node_pointer;
            typedef lfstackNode  node_type;
            typedef std::atomic<lfstackNode*> atomic_node_type;

            atomic_node_type head;
            
        public:

            typedef T               value_type;
            typedef value_type*     pointer;
            typedef const pointer   const_pointer;
            typedef std::size_t     size_type;
            typedef std::size_t     difference_type;

        private:

            /**
             * @brief Internal routine to pop element from HEAD and replace
             * head with the 'next' value
             */
            node_pointer _pop_front()
            {
                node_pointer ret = 0;
                if( head )
                {
                    do
                    {
                        // @todo: need a fence
                        if( !head ) throw lfstack_pop_empty();
                        ret = head.load(std::memory_order_relaxed);
                    }while( !head.compare_exchange_weak(ret, ret->next,
                                                        std::memory_order_release,
                                                        std::memory_order_relaxed) );
                }
                return ret;
            }

            void _push_front(const value_type& val)
            {
                node_pointer newhead = new node_type(val);

                do
                {
                    newhead->next = head.load(std::memory_order_relaxed);
                }while(!head.compare_exchange_weak(newhead->next, newhead,
                                                   std::memory_order_release,
                                                   std::memory_order_relaxed));
            }
            
        public:
            void clear()
            {
                node_pointer frontoflist = 0;
                while ( (frontoflist = _pop_front()) )
                    delete frontoflist;
                
            }

            void push_front(const T &type)
            {
                _push_front( type );
            }

            void push(const T& type)
            {
                _push_front( type );
            }
            
            T pop_front()
            {
                T result = T();
                node_pointer front = _pop_front();
                if( front )
                {
                    result = front->val;
                    delete front;
                }
                return result;
            }

            T pop()
            {
                return pop_front();
            }
            
            bool empty(){ return head.load(std::memory_order_relaxed) == 0; }
            
            lfstack() : head(0) {}
            ~lfstack(){ clear(); }
        };
    }
}

#endif
