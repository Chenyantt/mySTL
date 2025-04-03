#include <iostream>

#include "../include/list.h"
#include "../include/vector.h"
#include "../include/deque.h"
#include "../include/queue.h"
#include "../include/stack.h"

using namespace std;

int main() {
    Stack<int> st;
    cout<<st.empty()<<endl;
    st.push(12);
    st.push(32);
    st.show();
    cout<<st.top()<<endl;
    st.pop();
    st.show();
    st.push(111);
    st.show();
    return 0;
}