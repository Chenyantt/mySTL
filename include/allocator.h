#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include <iostream>
#include <cstring>

template <typename T>
class NewAllotor{
    public:
        static T* allocate(size_t bytes_nr, void* = static_cast<void*>(nullptr)){
            return static_cast<T*>(::operator new(bytes_nr));
        }
        static void deallocate(T* p, size_t){ 
            operator delete(p);
        }
};

class PoolAllocatorBase{
    protected:  
        struct chunk_node{
            chunk_node* next;
        };
        enum{POOL_ALIGN = 8};
        enum{MAX_CHUNK_SIZE = 128};
        enum{POOL_ARRAY_SIZE = MAX_CHUNK_SIZE / POOL_ALIGN};
    private:
        char* start_free_;
        char* end_free_;
        size_t heap_size_;
        chunk_node* pool_array_[POOL_ARRAY_SIZE];
    private:
        size_t round_up(size_t obj_size){
            return (obj_size + POOL_ALIGN - 1) & (~(POOL_ALIGN - 1));
        }
        void* allocate_chunk(int& obj_nr, size_t obj_size){
            size_t alloc_size = obj_nr * obj_size;
            void* result = nullptr;
            if(end_free_ - start_free_
                 >= alloc_size){
                result = start_free_;
                start_free_ = start_free_ + alloc_size;
                return result;
            }else if(end_free_ - start_free_ >= obj_size){
                obj_nr = (end_free_ - start_free_) / obj_size;
                result = start_free_;
                start_free_ = start_free_ + obj_size * obj_nr;
                return result;
            }else{
                int t = obj_size / POOL_ALIGN - 1;
                for(int i = t - 1, sz = obj_size >> 1;i >= 0; i--, sz = sz >> 1){
                    if(end_free_ - start_free_ >= sz){
                        reinterpret_cast<chunk_node*>(start_free_)->next = pool_array_[i];
                        pool_array_[i] = reinterpret_cast<chunk_node*>(start_free_);
                        start_free_ = start_free_ + sz;
                    }
                }
                void* p = nullptr;
                size_t new_size = (alloc_size << 1) + (heap_size_ >> 4);
                p = ::operator new(new_size, std::nothrow);
                if(p == nullptr){
                    for(int i = t + 1, sz = obj_size << 1; i < POOL_ARRAY_SIZE; i++, sz = sz << 1){
                        if(pool_array_[i] != nullptr){
                            start_free_ = reinterpret_cast<char*>(pool_array_[i]), end_free_ = reinterpret_cast<char*>(pool_array_[i]) + sz;
                            pool_array_[i] = pool_array_[i]->next;
                            return allocate_chunk(obj_nr, obj_size);
                        }
                    }
                    return nullptr;
                }else{
                    start_free_ = static_cast<char*>(p), end_free_ =  static_cast<char*>(p) + new_size;
                    heap_size_ += new_size;
                    return allocate_chunk(obj_nr, obj_size);
                }      
            }
        }
    protected:
        PoolAllocatorBase():start_free_(0),end_free_(0),heap_size_(0){
            memset(pool_array_, 0, sizeof pool_array_);
        }
        chunk_node** get_free_list(size_t obj_size){
            return pool_array_+ (round_up(obj_size) / POOL_ALIGN - 1);
        }
        void* refill(size_t obj_size){
            int obj_nr = 20;
            size_t round_obj_size = round_up(obj_size);
            void* p = allocate_chunk(obj_nr, round_obj_size);
            if(p == nullptr){
                
            }
            int off = round_obj_size / POOL_ALIGN - 1;
            pool_array_[off] = reinterpret_cast<chunk_node*>((static_cast<char*>(p) + obj_size));
            char* start = static_cast<char*>(p) + obj_size;
            for(int i = 1; i < obj_nr; i++){
               reinterpret_cast<chunk_node*>(start)->next = pool_array_[off];
               pool_array_[off] = reinterpret_cast<chunk_node*>(start);
               start += obj_size;
            }
            return p;
        }
};

template <typename T>
class PollAllocator:public PoolAllocatorBase{
public:
    T* allocate(size_t bytes_nr, void* = static_cast<void*>(nullptr)){
        if(bytes_nr > MAX_CHUNK_SIZE){
            return static_cast<T*>(::operator new(bytes_nr));
        }
        chunk_node** free_list = get_free_list(bytes_nr);
        if(*free_list == nullptr){
            void* p = refill(bytes_nr);
            return static_cast<T*>(p);
        }else{
            void* p = static_cast<void*>(*free_list);
            *free_list = (*free_list)->next;
            return static_cast<T*>(p);
        }
    }

    void deallocate(T* p, size_t bytes_nr){ 
        if(bytes_nr > MAX_CHUNK_SIZE){
            ::operator delete(p);
        }else{
            chunk_node** free_list = get_free_list(bytes_nr);
            reinterpret_cast<chunk_node*>(p)->next = *free_list;
            *free_list = reinterpret_cast<chunk_node*>(p);
        }
    }
};

#endif