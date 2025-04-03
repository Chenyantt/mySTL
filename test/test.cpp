#include <iostream>

#include "../include/list.h"
#include "../include/vector.h"
#include "../include/deque.h"

using namespace std;

int main() {
    Deque<std::string> dq1(7,"abc");
    dq1.show();
    Deque<std::string> dq2(std::move(dq1));
    dq1[0]="hello";
    dq1[2]="world";
    dq1.show();
    dq2.show();
    dq2.push_back("1");
    dq2.push_back("2");
    dq2.push_back("3");
    dq2.push_back("4");
    dq2.show();
    dq2.insert(dq2.begin(), 5, "aaaa");
    dq2.show();
    dq2.erase(dq2.begin()+8);
    dq2.show();
    dq2.pop_back();
    dq2.resize(19, "dddd");
    dq2.show();
    return 0;
}