#include <iostream>

#include "../include/list.h"

using namespace std;

#include <list>

int main(){
    List<int> l1,l2;
    for(int i=0;i<5;i++){
        l2.insert(l2.end(), i);
    }
    for(int i=0;i<5;i++){
        l1.insert(l1.end(), i);
    }
    List<int>::iterator t = l1.begin();
    List<int>::iterator it = l1.insert(l1.begin(), l2.begin(), l2.end());
    l1.insert(it, -999);
    l1.erase(++l1.begin(), t);
    cout << l1.size() << endl;
    l1.show();
    l1.clear();
    cout << l1.size() << endl;
    l1.insert(l1.end(), l2.begin(), l2.end());
    l1.insert(++l1.end(), 5);
    l1.show();
    l2.push_back(100);
    l2.push_front(200);
    l1.assign(l2.begin(), l2.end());
    cout << l1.size() << endl;
    l1.show();
    l1.resize(10);
    cout << l1.size() << endl;
    l1.show();
    l1.resize(5, 500);
    cout << l1.size() << endl;
    l1.show();
    cout<<l1.front()<<endl;
    cout<<l1.back()<<endl;
    return 0;
}