#ifndef __MAP_H
#define __MAP_H

#include "rb_tree.h"
#include "allocator.h"

template <typename Key, typename T, typename Compare = std::less<Key>, typename Allocator = NewAllocator<RBTreeNode<std::pair<const Key, T>>>>
class Map{
    typedef Key key_type;
    typedef T mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef typename RBTree<const key_type, value_type, std::_Select1st<value_type>, Compare, Allocator>::iterator iterator;
    typedef size_t size_type;


    private:
        RBTree<const key_type, value_type, std::_Select1st<value_type>, Compare, Allocator> rbtree_;
    public:
        Map() = default;
        Map(const Map& other) = default;
        T& operator[]( const Key& key ){
            iterator it = rbtree_.find(key);
            if(it == rbtree_.end()){
                value_type value(key, T());
                bool inserted = false;
                it = rbtree_.insert_unique(value, key, inserted);
            }
            return it->second;
        }

        iterator begin(){ return rbtree_.begin();}

        iterator end(){ return rbtree_.end();}

        bool empty() const{ return rbtree_.empty();}
        size_type size() const{ return rbtree_.size();}

        void clear(){ rbtree_.clear();}
        void swap(Map& other){ rbtree_.swap(other.rbtree_);}

        std::pair<iterator, bool> insert( const value_type& value ){
            bool inserted = false;
            iterator it = rbtree_.insert_unique(value, value.first, inserted);
            return std::make_pair(it, inserted);
        }

        iterator erase( iterator pos ){
            return rbtree_.erase(pos);
        }

        size_type count( const Key& key ){
            iterator it = rbtree_.find(key);
            if(it == end()){
                return 0;
            }else{
                return 1;
            }
        }

        iterator lower_bound( const Key& key ){
            return rbtree_.lower_bound(key);
        }
        iterator upper_bound( const Key& key ){
            return rbtree_.upper_bound(key);
        }
        iterator find( const Key& key ){
            return rbtree_.find(key);
        }

        void show() {
            iterator it = begin();
            while(it != end()){
                std::cout << it->first << ": " << it->second << std::endl;
                ++it;
            }
        }
};




#endif