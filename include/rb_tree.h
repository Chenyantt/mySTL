#ifndef __RB_TREE_H
#define __RB_TREE_H

#include <algorithm>

#include "allocator.h"

#include <queue>

enum RBTreeColor{ RED = 0, BLACK = 1};

struct RBTreeNodeBase{
    RBTreeColor color;
    RBTreeNodeBase* parent;
    RBTreeNodeBase* left;
    RBTreeNodeBase* right;
};

template <typename T>
struct RBTreeNode:public RBTreeNodeBase{
    T data;
    RBTreeNode() = default;
    RBTreeNode(RBTreeNodeBase* parent, RBTreeNodeBase* left, RBTreeNodeBase* right, const T& value):data(value){
        this->parent = parent;
        this->left = left;
        this->right = right;
        this->color = RED;
    }
};

bool is_red(RBTreeNodeBase* node){
    return (node != nullptr && node->color == RED);
}

bool is_black(RBTreeNodeBase* node){
    return (node == nullptr || node->color == BLACK);
}

RBTreeNodeBase* leftmost(RBTreeNodeBase* node){
    if(node == nullptr) return nullptr;
    while(node->left) node = node->left;
    return node;
}

RBTreeNodeBase* rightmost(RBTreeNodeBase* node){
    if(node == nullptr) return nullptr;
    while(node->right) node = node->right;
    return node;
}


RBTreeNodeBase* inc(RBTreeNodeBase* node){
    if(node->right){
        return leftmost(node->right);
    }else{
        RBTreeNodeBase* p = node->parent;
        while(p->right == node) node = p, p = p->parent;
        if(node->right != p) node = p;
        return node;
    }
}

RBTreeNodeBase* dec(RBTreeNodeBase* node){
    if(node->parent->parent == node && is_red(node)) return node->right;
    else if(node->left){
        return rightmost(node->left);
    }else{
        RBTreeNodeBase* p = node->parent;
        while(p!= nullptr && p->left == node) node = p, p = p->parent;
        return p;
    }
}

template <typename T>
struct RBTreeIterator{
    typedef T  value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t	difference_type;

    typedef RBTreeIterator<T>		self;
    typedef const value_type& const_reference;

    RBTreeNodeBase* node;

    RBTreeIterator() = default;
    RBTreeIterator(RBTreeNodeBase* node):node(node){}

    self& operator++(){
        node = inc(node);
        return *this;
    }

    self operator++(int){
        self it = *this;
        ++(*this);
        return it;
    }

    self& operator--(){
        node = dec(node);
        return *this;
    }

    self operator--(int){
        self it = *this;
        --(*this);
        return it;
    }

    reference operator*(){
        return static_cast<RBTreeNode<T>*>(node)->data;
    }

    pointer operator->(){
        return &(static_cast<RBTreeNode<T>*>(node)->data);
    }

    bool operator==(const self& other) const{
        return node == other.node;
    }
    bool operator!=(const self& other) const{
        return node != other.node;
    }
};

template <typename KEY, typename VALUE, typename KEY_OF_VALUE, typename COMPARE = std::less<KEY>, typename ALLOC = NewAllocator<RBTreeNode<VALUE>>>
class RBTree{
    public:
        typedef RBTreeNodeBase node_base;
        typedef RBTreeNodeBase* node_base_ptr;
        typedef RBTreeNode<VALUE> node;
        typedef RBTreeNode<VALUE>* node_ptr;
        typedef COMPARE key_compare;
        typedef KEY_OF_VALUE key_of_value;
        typedef ALLOC allocator_type;
        typedef VALUE value_type;
        typedef size_t size_type;
        typedef RBTreeIterator<VALUE> iterator;
    private:
        node_base header_;
        size_type node_count_;
        key_compare key_compare_;

        node_ptr create_node(node_base_ptr parent, node_base_ptr left, node_base_ptr right, const VALUE& value){
            node_ptr new_node = allocator_type::allocate(1);
            new(new_node) node(parent, left, right, value);
            return new_node;
        }

        void destroy_node(node_base_ptr p){
            node_ptr node = static_cast<node_ptr>(p);
            node->~node();
            allocator_type::deallocate(node, 1);
        }

        void rotate_left(node_base_ptr node){
            node_base_ptr right_child = node->right;
            node->right = right_child->left;
            if(right_child->left != nullptr) right_child->left->parent = node;
            right_child->parent = node->parent;
            if(node->parent == &header_){
                header_.parent = right_child;
            }else if(node == node->parent->left){
                node->parent->left = right_child;
            }else{
                node->parent->right = right_child;
            }
            right_child->left = node;
            node->parent = right_child;
        }

        void rotate_right(node_base_ptr node){
            node_base_ptr left_child = node->left;
            node->left = left_child->right;
            if(left_child->right != nullptr) left_child->right->parent = node;
            left_child->parent = node->parent;
            if(node->parent == &header_){
                header_.parent = left_child;
            }else if(node == node->parent->left){
                node->parent->left = left_child;
            }else{
                node->parent->right = left_child;
            }
            left_child->right = node;
            node->parent = left_child;
        }

        void adjust(node_base_ptr node){
            if(node->parent == &header_){
                node->color = BLACK;
                return;
            }
            else if(is_black(node->parent)){
                return;
            }else{
                node_base_ptr parent = node->parent;
                node_base_ptr grandparent = parent->parent;
                if(is_red(grandparent->left) && is_red(grandparent->right)){
                    grandparent->left->color = BLACK;
                    grandparent->right->color = BLACK;
                    grandparent->color = RED;
                    adjust(grandparent);
                }else if(grandparent->left == parent && parent->right == node){
                    rotate_left(parent);
                    adjust(parent);
                }else if(grandparent->right == parent && parent->left == node){
                    rotate_right(parent);
                    adjust(parent);
                }else if(grandparent->left == parent && parent->left == node){
                    parent->color = BLACK;
                    grandparent->color = RED;
                    rotate_right(grandparent);
                }else{
                    parent->color = BLACK;
                    grandparent->color = RED;
                    rotate_left(grandparent);
                }
            }
        }

        void erase_adjust(node_base_ptr node){
            while(node->parent != &header_){ 
                node_base_ptr parent = node->parent;
                node_base_ptr bro;
                if(parent->left == node){
                    bro = parent->right;
                    if(is_red(bro)){
                        parent->color = RED;
                        bro->color = BLACK;
                        rotate_left(parent);
                        bro = parent->right;
                    }
                }else{
                    bro = parent->left;
                    if(is_red(bro)){
                        parent->color = RED;
                        bro->color = BLACK;
                        rotate_right(parent);
                        bro = parent->left;
                    }
                }

                if(is_black(bro->left) && is_black(bro->right)){
                    if(is_black(parent)){
                        bro->color = RED;
                        node = parent;
                        continue;
                    }else{
                        parent->color = BLACK;
                        bro->color = RED;
                        break;
                    }
                }else{
                    if(parent->left == node){
                        if(is_red(bro->left)){
                            bro->left->color = BLACK;
                            bro->color = RED;
                            rotate_right(bro);
                            bro = parent->right;
                        }
                        std::swap(parent->color, bro->color);
                        bro->right->color = BLACK;
                        rotate_left(parent);
                        break;
                    }else if(parent->right == node){
                        if(is_red(bro->right)){
                            bro->right->color = BLACK;
                            bro->color = RED;
                            rotate_left(bro);
                            bro = parent->left;
                        }
                        std::swap(parent->color, bro->color);
                        bro->left->color = BLACK;
                        rotate_right(parent);
                        break;
                    }
                }
            }
        }

        node_base_ptr root() const{
            return header_.parent;
        }

        const KEY& get_key(node_base_ptr p) const{
            return key_of_value()(static_cast<node*>(p)->data);
        }

        VALUE& get_value(node_base_ptr p){
            return static_cast<node*>(p)->data;
        }

        void insert_node(node_base_ptr fp, node_base_ptr new_node, bool is_left){
            if(is_left){
                fp->left = new_node;
                if(fp == header_.left) header_.left = new_node;
            }else{
                fp->right = new_node;
                if(fp == header_.right) header_.right = new_node;
            }
            ++node_count_;
        }

        void erase_node(node_base_ptr node){
            --node_count_;
            if(node_count_ == 0) header_.left = header_.right = &header_;
            else if(node == header_.left) header_.left = inc(node);
            else if(node == header_.right) header_.right = dec(node);
            
            node_base_ptr child = (node->left)?:node->right;
            node_base_ptr parent = node->parent;
            if(child) child->parent = node->parent;
            if(parent == &header_)  header_.parent = child;
            else {
                if(parent->left == node) parent->left = child;
                else parent->right = child;
            }
            destroy_node(node);
        }

        void find_insert_place(node_base_ptr p, node_base_ptr& fp, bool& is_left, node_base_ptr& lower_bound_p, const KEY& key){
            while(p != nullptr){
                if(!key_compare_(get_key(p), key)){
                    lower_bound_p = fp = p;
                    p = p->left;
                    is_left = true;
                }else{
                    fp = p;
                    p = p->right;
                    is_left = false;
                }
            }
        }

        void clear_helper(node_base_ptr node){
            if(node == nullptr) return;
            clear_helper(node->left);
            clear_helper(node->right);
            destroy_node(node);
        }

        node_base_ptr copy(node_base_ptr p){
            if(p == nullptr) return nullptr;
            node_base_ptr new_node = create_node(nullptr, nullptr, nullptr, static_cast<node*>(p)->data);
            new_node->color = p->color;
            new_node->left = copy(p->left);
            new_node->right = copy(p->right);
            if(new_node->left) new_node->left->parent = new_node;
            if(new_node->right) new_node->right->parent = new_node;
            return new_node;
        }

    public:
        RBTree():node_count_(0){
            header_.parent = nullptr;
            header_.left = &header_;
            header_.right = &header_;
            header_.color = RED;
        }

        RBTree(const RBTree& other){
            if(other.root() == nullptr){
                header_.parent = nullptr;
                header_.left = &header_;
                header_.right = &header_;
                node_count_ = 0;
            }else{
                node_base_ptr p = copy(other.root());
                header_.parent = p;
                header_.left = leftmost(p);
                header_.right = rightmost(p);
                node_count_ = other.node_count_;
                p->parent = &header_;
            }
        }

        ~RBTree(){
            clear();
        }

        iterator begin(){
            return header_.left;
        }

        iterator end(){
            return &header_;
        }

        size_type size() const{
            return node_count_;
        }

        bool empty() const{
            return node_count_ == 0;
        }

        void clear(){
            clear_helper(root());
            header_.parent = nullptr;
            header_.left = &header_;
            header_.right = &header_;
            node_count_ = 0;
        }

        void swap(RBTree& other){
            std::swap(header_, other.header_);
            std::swap(node_count_, other.node_count_);
            std::swap(key_compare_, other.key_compare_);
        }


        iterator lower_bound(const KEY& key){
            node_base_ptr p = root(), lower_bound_p = &header_;
            while(p != nullptr){
                if(!key_compare_(get_key(p), key)){
                    lower_bound_p = p;
                    p = p->left;
                }else{
                    p = p->right;
                }
            }
            return lower_bound_p;
        }

        iterator upper_bound(const KEY& key){
            node_base_ptr p = root(), upper_bound_p = &header_;
            while(p != nullptr){
                if(key_compare_(key, get_key(p))){
                    upper_bound_p = p;
                    p = p->left;
                }else{
                    p = p->right;
                }
            }
            return upper_bound_p;
        }

        iterator find(const KEY& key){
            node_base_ptr p = root();
            while(p != nullptr){
                if(key_compare_(get_key(p), key)){
                    p = p->right;
                }else if(key_compare_(key, get_key(p))){
                    p = p->left;
                }else{
                    return p;
                }
            }
            return &header_;
        }

        iterator insert_unique(const VALUE& value, const KEY& key, bool& inserted){
            node_base_ptr p = root();
            if(p == nullptr){
                node_ptr new_node = create_node(&header_, nullptr, nullptr, value);
                header_.parent = new_node;
                header_.left = new_node;
                header_.right = new_node;
                ++node_count_;
                adjust(new_node);
                inserted = true;
                return new_node;
            }else{
                node_base_ptr fp = nullptr, lower_bound_p = nullptr;
                bool is_left = false;
                find_insert_place(p, fp, is_left, lower_bound_p, key);
                if(lower_bound_p != nullptr && get_key(lower_bound_p) == key){
                    inserted = false;
                    return lower_bound_p;
                }else{
                    node_base_ptr new_node = create_node(fp, nullptr, nullptr, value);
                    insert_node(fp, new_node, is_left);
                    adjust(new_node);
                    inserted = true;
                    return new_node;
                }
            }
        }

        iterator insert_equal(const VALUE& value, const KEY& key){
            node_base_ptr p = root();
            if(p == nullptr){
                node_ptr new_node = create_node(&header_, nullptr, nullptr, value);
                header_.parent = new_node;
                header_.left = new_node;
                header_.right = new_node;
                ++node_count_;
                adjust(new_node);
                return new_node;
            }else{
                node_base_ptr fp = nullptr, lower_bound_p = nullptr;
                bool is_left = false;
                find_insert_place(p, fp, is_left, lower_bound_p, key);
                node_base_ptr new_node = create_node(fp, nullptr, nullptr, value);
                insert_node(fp, new_node, is_left);
                adjust(new_node);
                return new_node;
            }
        }

        iterator erase(iterator it){
            node_base_ptr node = it.node;
            iterator next_it = inc(node);
            if(node->left && node->right){
                node_base_ptr p = leftmost(node->right);
                std::destroy_at(&get_value(node));
                new( &get_value(node)) VALUE(get_value(p));
                node = p;
            }
            if(node->color == RED){
                erase_node(node);
            }else if(is_red(node->left)){
                node->left->color = BLACK;
                erase_node(node);
            }else if(is_red(node->right)){
                node->right->color = BLACK;
                erase_node(node);
            }else{
                erase_adjust(node);
                erase_node(node);
            }
            return next_it; 
        }

        void show()const{
            std::cout << "Node Count: " << node_count_ << std::endl;
            node_base_ptr p = root();
            if(p == nullptr){
                std::cout << "Tree is empty" << std::endl;
                return;
            }
            std::queue<node_base_ptr> q;
            q.push(p);
            while(!q.empty()){
                size_t size = q.size();
                for(size_t i = 0; i < size; ++i){
                    node_base_ptr current = q.front();
                    q.pop();
                    std::cout << get_key(current) << (current->color == RED ? "R" : "B") << " ";
                    if(current->left != nullptr) q.push(current->left);
                    if(current->right != nullptr) q.push(current->right);
                }
                std::cout << std::endl;
            }
        }

};



#endif