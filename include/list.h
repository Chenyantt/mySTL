#ifndef __LIST_H
#define __LIST_H

#include "allocator.h"

struct ListNodeBase{
    ListNodeBase* prev_;
    ListNodeBase* next_;
};

template<typename T>
struct ListNode: public ListNodeBase
{
    T data_;
};

template<typename T>
struct ListHeader: public ListNodeBase
{
    size_t size_;
    ListHeader():size_(0){
        this->next_ = this->prev_ = this;
    }
};

template<typename T>
struct ListIterator{
    using Self = ListIterator<T>;
    using Node = ListNode<T>;

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    ListIterator(const ListNodeBase* node):node_(const_cast<ListNodeBase*>(node)){}
    reference operator *() const{
        return static_cast<Node* const>(this->node_)->data_;
    }
    
    Self& operator ++(){
        this->node_ = this->node_->next_;
        return *this;
    }

    Self operator++(int) {
        Self it = *this;
        this->node_ = this->node_->next_;
        return it;
    }

    Self& operator --(){
        this->node_ = this->node_->prev_;
        return *this;
    }

    Self operator--(int) {
        Self it = *this;
        this->node_ = this->node_->prev_;
        return it;
    }

    bool operator==(const Self& it) const{
        return it.node_ == this->node_;
    }

    bool operator!=(const Self& it) const{
        return !(it.node_ == this->node_);
    }

public:
    ListNodeBase* node_;
};

template<typename T, typename ALLOC=NewAllotor<ListNode<T>>>
class List{
    public:
    using iterator = ListIterator<T>;
    using Node = ListNode<T>;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using size_type = size_t;

    private:
    ListHeader<T> head;

    static Node* create_new_node(const value_type& data){
        Node* node = ALLOC::allocate(1);
        node->data_ = data;
        return node;
    }

    static void destroy_node(Node* node){
        std::destroy_at(&node->data_);
        ALLOC::deallocate(node, 1);
    }

    public:

    iterator begin() const{
        return iterator(head.next_);
    }

    iterator end() const{
        return iterator(&head);
    }

    size_t size() const{
        return head.size_;
    }

    bool empty() const{
        return head.size_ == 0;
    }

    iterator insert(const iterator pos, const value_type& value ){
        Node* node = create_new_node(value);
        node->next_ = pos.node_;
        node->prev_ = pos.node_->prev_;
        pos.node_->prev_->next_ = node;
        pos.node_->prev_ = node;
        head.size_++;
        return iterator(node);
    }

    iterator insert(const iterator pos, size_type count, const value_type& value ){
        iterator it = pos;
        for(size_t i = 0; i < count; i++){
            it = insert(it, value);
        }
        return it;
    }

    template< class InputIt >
    iterator insert( const iterator pos, InputIt first, InputIt last ){
        if(first == last) return pos;
        iterator it = insert(pos, *(first++));
        for(; first != last; ++first){
            insert(pos, *first);
        }
        return it;
    }

    reference front(){
        return *begin();
    }

    reference back(){
        return *(--end());
    }

    void push_back(const T& value){
        insert(end(), value);
    }

    void push_front(const T& value){
        insert(begin(), value);
    }

    void pop_back(){
        if(!empty()){
            erase(--end());
        }
    }

    void pop_front(){
        if(!empty()){
            erase(begin());
        }
    }

    void assign( size_type count, const T& value ){
        iterator it = begin();
        size_type i = 0;
        for(; i < count && it != end(); ++i, ++it){
            std::destroy_at(&(*it));
            *it = value;
        }
        if(i < count) insert(it, count - i, value);
        if(it != end()) erase(it, end());
    }

    template< class InputIt >
    void assign( InputIt first, InputIt last ){
        iterator it = begin();
        for(; first != last && it != end(); ++first, ++it){
            std::destroy_at(&(*it));
            *it = *first;
        }
        if(first != last) insert(it, first, last);
        if(it != end()) erase(it, end());
    }

    iterator erase(iterator pos){
        Node* node = static_cast<Node*>(pos.node_);
        iterator it = iterator(node->next_);
        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;
        destroy_node(node);
        head.size_--;
        return it;
    }

    iterator erase( iterator first, iterator last ){
        iterator it = first;
        while(it != last){
            it = erase(it);
        }
        return it;
    }

    void clear(){
        iterator it = begin();
        while(it != end()){
            Node* node = static_cast<Node*>(it.node_);
            ++it;
            destroy_node(node);
        }
        head.size_ = 0;
        head.next_ = head.prev_ = &head;
    }

    void resize( size_type count, const value_type& value ){
        if(count < head.size_){
            iterator it = begin();
            for(size_t i = 0; i < count; ++i, ++it);
            erase(it, end());
        }else if(count > head.size_){
            insert(end(), count - head.size_, value);
        }
    }

    void resize( size_type count){
        if(count < head.size_){
            iterator it = begin();
            for(size_t i = 0; i < count; ++i, ++it);
            erase(it, end());
        }else if(count > head.size_){
            insert(end(), count - head.size_, value_type());
        }
    }

    ~List(){
        clear();
    }

    void show() const{
        for(auto it = begin(); it != end(); ++it){
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }
};

#endif