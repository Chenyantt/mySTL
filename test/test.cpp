#include <iostream>

#include "../include/list.h"
#include "../include/vector.h"

using namespace std;

int main(){
    Vector<string> vec(4, "hello");
    vec.show();
    vec.reserve(10);
    vec.insert(vec.begin() + 3, 2, "111");
    vec.show();
    vec.erase(vec.begin()+1, vec.begin()+4);
    vec.show();
    vec.resize(6);
    vec.show();
    return 0;
}