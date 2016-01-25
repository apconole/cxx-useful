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
#include <memory>

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
            typedef std::shared_ptr<node_type> node_shared_pointer;

            atomic_node_type head;
            atomic_node_type freelist;
            std::atomic<unsigned int> pop_counter;

            bool throws_on_empty;
            
        public:

            typedef T               value_type;
            typedef value_type*     pointer;
            typedef const pointer   const_pointer;
            typedef std::size_t     size_type;
            typedef std::size_t     difference_type;

        private:

            void _release_node_chain(node_pointer node)
            {
                while(node)
                {
                    node_pointer nxt = node->next;
                    delete node;
                    node = nxt;
                }
            }

            void release_internal_node(node_pointer node)
            {
                if (pop_counter == 1)
                {
                    node_pointer dnodes = freelist.exchange(nullptr);
                    if(!--pop_counter)
                    {
                        /* this was the only release thread - just delete*/
                        _release_node_chain(dnodes);
                    }
                    else if (dnodes)
                    {
                        /* re-enqueue on the freelist */
                        node_pointer listnode = dnodes;
                        while(listnode)
                        {
                            node_pointer nxt = listnode->next;
                            do
                            {
                                listnode->next = freelist.load();
                            }while(!freelist.compare_exchange_weak(listnode->next, listnode));
                            listnode = nxt;
                        }
                    }
                    delete node;
                }
                else
                {
                    do
                    {
                        node->next = freelist.load();
                    }while(!freelist.compare_exchange_weak(node->next, node));
                    --pop_counter;
                }
            }

            /**
             * @brief Internal routine to pop element from HEAD and replace
             * head with the 'next' value
             */
            node_pointer _pop_front()
            {
                node_pointer old_head = head.load();
                while(old_head &&
                      !head.compare_exchange_weak(old_head, old_head->next,
                                                  std::memory_order_release,
                                                  std::memory_order_relaxed));
                
                return old_head;
            }
            
            bool _do_pop_front(T &result)
            {
                bool ret = false;
                ++pop_counter; /* released in release_internal_node */

                node_pointer front = _pop_front();
                if( front )
                {
                    result = front->val;
                    release_internal_node(front);
                    ret = true;
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
                T tmp = T();
                bool throwval = throws_on_empty;
                throws_on_empty = false;
                while ( !empty() && _do_pop_front(tmp) );
                throws_on_empty = throwval;
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
                if (!_do_pop_front(result) && throws_on_empty)
                    throw lfstack_pop_empty();
                return result;
            }

            T pop()
            {
                return pop_front();
            }
            
            bool empty(){ return head.load(std::memory_order_relaxed) == 0; }

            bool get_throw_configuration() { return throws_on_empty; }
            void set_throw_configuration(bool throws) { throws_on_empty = throws; }
            
            lfstack() : head(0), freelist(0), pop_counter(0), throws_on_empty(true) {}
            ~lfstack(){ clear(); }
        };
    }
}

#endif
