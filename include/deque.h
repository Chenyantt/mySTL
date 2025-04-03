#ifndef __DEQUE_H
#define __DEQUE_H

#include <memory>

#include "allocator.h"

#define DEQUE_BUFFER_SIZE 512

template <typename T>
struct DequeIterator{
    typedef std::random_access_iterator_tag    iterator_category;
    typedef T        value_type;
    typedef T*        pointer;
    typedef T&        reference;
    typedef ptrdiff_t    difference_type;

    typedef T** map_pointer;
    typedef DequeIterator self;
    typedef size_t size_type;

    map_pointer pnode_;
    pointer cur_;
    pointer first_;
    pointer last_;

    DequeIterator():pnode_(nullptr), cur_(nullptr), first_(nullptr), last_(nullptr){}

    DequeIterator(map_pointer pnode, pointer cur):pnode_(pnode), cur_(cur), first_(*pnode), last_(first_ + buffer_size()){}

    static inline size_type buffer_size(){
        if(sizeof(T) < DEQUE_BUFFER_SIZE){
            return DEQUE_BUFFER_SIZE / sizeof(T);
        }else{
            return 1;
        }
    }

    void set_node(map_pointer pnode){
        pnode_ = pnode;
        first_ = *pnode;
        last_ = first_ + buffer_size();
    }

    reference operator *(){
        return *cur_;
    }

    self& operator++(){
        ++cur_;
        if(cur_ == last_){
            set_node(pnode_ + 1);
            cur_ = first_;
        }
        return *this;
    }

    self operator++(int){
        self tmp = *this;
        ++(*this);
        return tmp;
    }

    self& operator--(){
        if(cur_ == first_){
            set_node(pnode_ - 1);
            cur_ = last_;
        }
        --cur_;
        return *this;
    }

    self operator--(int){
        self tmp = *this;
        --(*this);
        return tmp;
    }

    self& operator +=(difference_type off){
        difference_type diff = off + (cur_ - first_);
        if(diff >= 0 && diff < difference_type(buffer_size())){
            cur_ += off;
        }else{
            difference_type node_diff = (diff > 0)? (diff / buffer_size()) : (-((-diff - 1) / buffer_size()) - 1);
            set_node(pnode_ + node_diff);
            cur_ = first_ + (diff - node_diff * buffer_size());
        }
        return *this;
    }

    self operator+(difference_type off){
        self tmp = *this;
        tmp += off;
        return tmp;
    }

    self& operator-=(difference_type off){
        return (*this) += -off;
    }

    self operator-(difference_type off){
        self tmp = *this;
        tmp -= off;
        return tmp;
    }

    difference_type operator-(const self& other) const{
        return (pnode_-other.pnode_) * buffer_size() + ((cur_ - first_)  - (other.cur_ - other.first_));
    }

    bool operator==(const self& other) const{
        return this->cur_ == other.cur_;
    }

    bool operator!=(const self& other) const{
        return !(*this == other);
    }

    bool operator<(const self& other) const{
        if(pnode_ != other.pnode_) return pnode_ < other.pnode_;
        else return cur_ < other.cur_;
    }

    bool operator>(const self& other) const{
        if(pnode_ != other.pnode_) return pnode_ > other.pnode_;
        else return cur_ > other.cur_;
    }

    bool operator<=(const self& other) const{
        return !(*this > other);
    }

    bool operator>=(const self& other) const{
        return !(*this < other);
    }
};


template <typename T, template<typename N> typename ALLOC = NewAllocator>
class Deque{
    public:
    typedef T       value_type;
    typedef T*      pointer;
    typedef T&      reference;
    typedef size_t  size_type;
    typedef const value_type& const_reference;

    typedef DequeIterator<T> iterator;
    typedef typename iterator::map_pointer map_pointer;
    typedef ALLOC<pointer> MAP_ALLOC;
    typedef ALLOC<value_type> BUFFER_ALLOC;
    typedef ptrdiff_t difference_type;


    private:
    iterator begin_;
    iterator end_;
    map_pointer map_;
    size_type map_size_;

    static inline size_type buffer_size(){ return (sizeof(T) < DEQUE_BUFFER_SIZE)?(DEQUE_BUFFER_SIZE / sizeof(T)):1;}
    pointer buffer_allocate(){ return BUFFER_ALLOC::allocate(buffer_size());}
    void buffer_allocate_n(map_pointer first, map_pointer last){ while(first != last) (*first++) = buffer_allocate();}
    void buffer_deallocate(map_pointer p){ return BUFFER_ALLOC::deallocate(*p, buffer_size());}
    void buffer_deallocate_n(map_pointer first, map_pointer last){ while(first != last) buffer_deallocate(first++); }
    map_pointer map_allocate(size_type n){ return MAP_ALLOC::allocate(n);}
    void map_deallocate(){ return MAP_ALLOC::deallocate(map_, map_size_);}

    void reallocate_map(size_type n, bool is_front){
        size_type new_node_nr = n + (end_.pnode_ - begin_.pnode_ + 1);
        map_pointer old_start = begin_.pnode_, old_finish = end_.pnode_;
        if(map_size_ > new_node_nr){
            map_pointer new_start = map_ + (map_size_ - new_node_nr) / 2 + (is_front?n:0);
            if(new_start == old_start) return;
            if(new_start < old_start){
                std::copy(old_start, old_finish + 1, new_start);
            }else{
                std::copy_backward(old_start, old_finish + 1, new_start + (old_finish - old_start) + 1);
            }
            begin_.set_node(new_start);
            end_.set_node(new_start + (old_finish - old_start));
        }else{
            size_type new_cap = std::max(2 * map_size_, new_node_nr);
            map_pointer new_map = map_allocate(new_cap);
            map_pointer new_start = new_map +  (new_cap - new_node_nr) / 2 + (is_front?n:0);
            std::copy(old_start, old_finish + 1, new_start);
            map_deallocate();
            map_ = new_map;
            map_size_ = new_cap;
            begin_.set_node(new_start);
            end_.set_node(new_start + (old_finish - old_start));
        }
    }

    void reserve_front(size_type count){
        if(size_type(begin_.pnode_ - map_) < count)
            reallocate_map(count, true);
    }

    void reserve_back(size_type count){
        if(map_size_ - size_type(end_.pnode_ - map_ + 1) < count)
            reallocate_map(count, false);
    }

    void reserve_n(iterator pos, size_type count){
        size_type front_elem_nr =  size_type(pos - begin_);
        if(front_elem_nr < size() / 2){
            difference_type diff = count - (begin_.cur_ - begin_.first_);
            size_type new_node_nr = (diff + buffer_size() - 1) / buffer_size();
            reserve_front(new_node_nr);
            buffer_allocate_n(begin_.pnode_ - new_node_nr, begin_.pnode_);
            iterator old_begin = begin_;
            std::uninitialized_copy_n(begin_, count, begin_ - difference_type(count));
            std::move(old_begin + difference_type(count), old_begin + difference_type(front_elem_nr), old_begin);
            begin_ = begin_ - difference_type(count);
        }else{
            difference_type diff = count - (end_.last_ - end_.cur_ - 1);
            size_type new_node_nr = (diff + buffer_size() - 1) / buffer_size();
            reserve_back(new_node_nr);
            buffer_allocate_n(end_.pnode_ + 1, end_.pnode_ + 1 + new_node_nr);
            iterator old_end = end_;
            std::uninitialized_copy_n(end_ - difference_type(count), count, end_);
            std::move_backward(begin_ + difference_type(front_elem_nr), old_end - difference_type(count), old_end);
            end_ = end_ + difference_type(count);
        }
    }

    void map_init(size_type n){
        size_type node_nr = n / buffer_size() + 1;
        size_type map_size = node_nr + 2;
        map_ = map_allocate(map_size);
        map_size_ = map_size;
        map_pointer start_node = map_ + 1;
        buffer_allocate_n(start_node, start_node + node_nr);
        begin_.set_node(start_node);
        begin_.cur_ = begin_.first_;
        end_ = begin_ + n;
    }

    void fill_init(size_type n, value_type value = value_type()){
        map_init(n);
        std::uninitialized_fill(begin_, end_, value);
    }

    template< class InputIt >
    void copy_init(InputIt first, InputIt last){
        size_type n = std::distance(first, last);
        map_init(n);
        std::uninitialized_copy(first, last, begin_);
    }

public:
    Deque(){ fill_init(0);}

    Deque( size_type count, const_reference value = T()){ fill_init(count, value);}

    template< class InputIt >
    Deque( InputIt first, InputIt last){ copy_init(first, last);}

    Deque(const Deque& other ){ copy_init(other.begin_, other.end_);}

    Deque(Deque&& other ):begin_(other.begin_), end_(other.end_), map_(other.map_), map_size_(other.map_size_) {
        other.map_ = nullptr, other.map_size_ = 0;
    }

    ~Deque(){
        if(map_ != nullptr){
            std::destroy(begin_, end_);
            buffer_deallocate_n(begin_.pnode_, end_.pnode_ + 1);
            map_deallocate();
        }
    }

    size_type size(){ return end_ - begin_;}
    reference operator[]( size_type pos ){ return *(begin_ + pos);}
    reference front(){ return *begin_;}
    reference back(){ return *(end_-1);}
    iterator begin(){ return begin_;}
    iterator end(){ return end_;}
    bool empty() const{ return begin_ == end_;}

    void push_front( const_reference value ){
        if(begin_.cur_ == begin_.first_){
            reserve_front(1);
            *(begin_.pnode_ - 1) = buffer_allocate();
        }
        new(&(*(--begin_))) value_type(value);
    }

    void push_back( const_reference value ){
        if(end_.cur_ == end_.last_ - 1){
            reserve_back(1);
            *(end_.pnode_ + 1) = buffer_allocate();
        }
        new(&(*(end_++))) value_type(value);
    }

    iterator insert( iterator pos, const_reference value ){
        if(pos == begin_){
            push_front(value);
            return begin_;
        }else if(pos == end_){
            push_back(value);
            return end_ - 1;
        }else{
            size_type front_elem_nr = pos - begin_;
            if(front_elem_nr < size() / 2){
                push_front(*begin_);
                std::move(begin_ + 2, begin_ + 1 + front_elem_nr, begin_ + 1);
            }else{
                push_back(*(end_ - 1));
                show();
                std::move_backward(begin_ + front_elem_nr, end_ - 2, end_ - 1);
            }
            *(begin_ + front_elem_nr) = value;
            return begin_ + front_elem_nr;
        }
    }

    iterator insert( iterator pos, size_type count, const_reference value ){
        size_type front_elem_nr = pos - begin_;
        reserve_n(pos, count);
        std::fill_n(begin_ + front_elem_nr, count, value);
        return begin_ + front_elem_nr;
    }

    template< class InputIt >
    iterator insert( iterator pos, InputIt first, InputIt last ){
        size_type front_elem_nr = pos - begin_;
        size_type count = std::distance(first, last);
        reserve_n(pos, count);
        std::copy(first, last, begin_ + front_elem_nr);
        return begin_ + front_elem_nr;
    }

    iterator erase( iterator first, iterator last ){
        size_type front_elem_nr = first - begin_;
        size_type back_elem_nr = end_ - last;
        if(front_elem_nr < back_elem_nr){
            iterator ret = std::copy_backward(begin_, first, last);
            iterator old_begin = begin_;
            begin_+= (last - first);
            buffer_deallocate_n(old_begin.pnode_, begin_.pnode_);
            return ret;
        }else{
            iterator ret = std::copy(last, end_, first);
            iterator old_end = end_;
            end_-= (last - first);
            buffer_deallocate_n(end_.pnode_ + 1, old_end.pnode_ + 1);
            return ret;
        }
    }

    iterator erase( iterator pos ){
        return erase(pos, pos + 1);
    }

    void clear(){
        erase(begin_, end_);
    }

    void pop_back(){
        erase(end_ - 1);
    }

    void pop_front(){
        erase(begin_);
    }

    void resize( size_type count, const value_type& value=value_type() ){
        if(count == size()) return;
        else if(count > size()){
            insert(end_, count - size(), value);
        }else{
            erase(end_-(size() - count), end_);
        }
    }

    void show() {
        for(iterator it = begin(); it != end(); ++it){
            std::cout << *it << ' ';
        }
        std::cout << std::endl;
        std::cout << "size = " << size() << std::endl;
    }
};

#endif