#include <iostream>

#include "../include/allocator.h"

using namespace std;

int main(){
    int* p = NewAllotor<int>::allocate(4);
    *p = 5;
    allocator<int> alloc;
    cout << *p << endl;
    NewAllotor<int>::deallocate(p, 4);
    return 0;
}