#ifndef __STACK_H
#define __STACK_H

#include "deque.h"

template<typename T, typename SEQUENCE = Deque<T>>
class Stack{
    public:
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    private:
    SEQUENCE c;

    public:

    reference top(){ return c.back();}

    bool empty() const{ return c.empty();}

    size_type size() const{ return c.size();}

    void pop(){ c.pop_back(); }

    void push( const value_type& value ){ c.push_back(value);}

    void show() { c.show();}
};

#endif