#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include <iostream>

template <typename T>
class NewAllotor{
    public:
        static T* allocate(size_t bytes_nr, void* = (void*)0){
            return (T*)operator new(bytes_nr);
        }
        static void deallocate(T* p, size_t){
            operator delete(p);
        }
};

class PoolAllocatorBase{
    private:
        char* start_free_;
        char* end_free_;
        char* heap_size_;
    public:
        void* refill(size_t obj_size){

        }
}

#endif