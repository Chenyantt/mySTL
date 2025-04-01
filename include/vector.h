#ifndef __VECTOR_H
#define __VECTOR_H

#include <memory>

#include "allocator.h"

template<typename T, typename ALLOC = NewAllocator<T>>
class Vector{
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef const T* const_pointer;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef pointer iterator;
        typedef const_pointer const_iterator;
        typedef Vector<T, ALLOC> Self;
    private:
        pointer start_;
        pointer finish_;
        pointer end_of_storage_;

    public:
        Vector():start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}
        Vector(size_type n, const_reference value = value_type()){
            start_ = ALLOC::allocate(n);
            finish_ = end_of_storage_ = start_ + n;
            std::uninitialized_fill_n(start_, n, value);
        }
        template< class InputIt >
        Vector( InputIt first, InputIt last){
            size_type n = std::distance(first, last);
            start_ = ALLOC::allocate(n);
            finish_ = end_of_storage_ = start_ + n;
            std::uninitialized_copy(first, last, start_);
        }
        Vector(const Vector& other):Vector(other.begin(), other.end()){}
        Vector(Vector&& other):start_(other.start_), finish_(other.finish_), end_of_storage_(other.end_of_storage_){
            other.start_ = nullptr;
            other.finish_ = nullptr;
            other.end_of_storage_ = nullptr;
        }                  
        ~Vector(){
            std::destroy(start_, finish_);
            ALLOC::deallocate(start_, size());
        }
        iterator begin() { return start_; }
        iterator end() { return finish_; }
        const_iterator begin() const { return start_; }
        const_iterator end() const { return finish_; }
        size_type size() const { return finish_ - start_; }
        size_type capacity() const { return end_of_storage_ - start_; }
        bool empty() const { return begin() == end(); }
        reference front() { return (*this)[0];}
        reference back() { return (*this)[size() - 1]; }
        pointer data() { return start_;}

        void clear(){
            std::destroy(begin(), end());
            finish_ = start_;
        }

        reference operator[]( size_type pos ){ return *(begin() + pos);}
        Vector& operator=(const Vector& other){
            if(this == &other) return *this;
            if(other.size() > capacity()){
                Vector<value_type> tmp = other;
                swap(tmp);
            }else if(other.size() > size()){
                std::copy(other.begin(), other.begin() + size(), start_);
                finish_ = std::uninitialized_copy(other.begin() + size(), other.end(), finish_);
            }else{
                pointer new_finish =  std::copy(other.begin(), other.end(), start_);
                std::destroy(new_finish, end());
                finish_ = new_finish;
            }
            return *this;
        }
        Vector& operator=(Vector&& other){
            if(this == &other) return *this;
            destroy(begin(), end());
            ALLOC::deallocate(start_, capacity());
            start_ = other.start_;
            finish_ = other.finish_;
            end_of_storage_ = other.end_of_storage_;
            other.start_ = other.finish_ = other.end_of_storage_ = nullptr;
            return *this;
        }

        void swap( Vector& other ){
            std::swap(start_, other.start_);
            std::swap(finish_, other.finish_);
            std::swap(end_of_storage_, other.end_of_storage_);
        }

        void reserve( size_type new_cap ){
            if(new_cap > capacity()){
                pointer new_start = ALLOC::allocate(new_cap);
                pointer new_finish = std::uninitialized_copy(start_, finish_, new_start);
                std::destroy(begin(), end());
                ALLOC::deallocate(start_, capacity());
                start_ = new_start;
                finish_ = new_finish;
                end_of_storage_ = start_ + new_cap;
            }
        }

        iterator insert( iterator pos, const T& value ){
            return insert(pos, 1, value );
        }

        iterator insert( iterator pos, size_type count, const T& value ){
            if(count == 0) return pos;
            if(count + size() > capacity()){
                size_type new_cap = std::max(2 * capacity(), count + size());
                pointer new_start = ALLOC::allocate(new_cap);
                pointer new_finish = std::uninitialized_move(begin(), pos, new_start);
                iterator ret = new_finish;
                new_finish = std::uninitialized_fill_n(new_finish, count, value);
                new_finish = std::uninitialized_move(pos, end(), new_finish);
                std::destroy(begin(), end());
                ALLOC::deallocate(start_, capacity());
                start_ = new_start;
                finish_ = new_finish;
                end_of_storage_ = start_ + new_cap;
                return ret;
            }else if(pos + count < end()){
                std::uninitialized_move(end() - count, end(), end());
                std::move_backward(pos, end() - count, end());
                std::fill_n(pos, count, value);
                finish_ += count;
                return pos;
            }else{
                std::uninitialized_move(pos, end(), pos + count);
                std::uninitialized_fill(end(), pos + count, value);
                std::fill(pos, end(), value);
                finish_ += count;
                return pos;
            }
        }

        void push_back( const T& value ){
            insert(end(), value);
        }

        iterator erase( iterator first, iterator last ){
            iterator new_finish = std::move(last, end(), first);
            std::destroy(new_finish, end());
            finish_ = new_finish;
            return first;
        }

        iterator erase( iterator pos ){
            return erase(pos, pos + 1);
        }

        void pop_back(){
            erase(end() - 1, end());
        }


        void show() const{
            for(auto &x : *this){
                std::cout << x << " ";
            }
            std::cout << std::endl;
            std::cout<<"size: "<<size()<<' '
                     <<"capacity: "<<capacity()<<std::endl;
        }

};

#endif