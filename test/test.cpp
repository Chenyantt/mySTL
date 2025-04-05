#include <iostream>

#include "../include/list.h"
#include "../include/vector.h"
#include "../include/deque.h"
#include "../include/queue.h"
#include "../include/stack.h"


using namespace std;

#include "../include/rb_tree.h"
#include "../include/map.h"

int main() {
    Map<int, int> map;
    map.insert(std::make_pair(6, 5));
    map.insert(std::make_pair(2, 7));
    map.insert(std::make_pair(3, 3));
    map.insert(std::make_pair(5, 4));
    map.erase(map.upper_bound(4));
    map.show();
    // RBTree<int, int, _Identity<int>> tree;
    // bool inserted = false;
    // tree.show();
    // tree.insert_unique(12, 12, inserted);
    // tree.show();
    // tree.insert_unique(1, 1, inserted);
    // tree.show();

    // tree.insert_unique(9, 9, inserted);
    // tree.show();

    // tree.insert_unique(2, 2, inserted);
    // tree.show();

    // tree.insert_unique(0, 0, inserted);
    // tree.show();

    // tree.insert_unique(11, 11, inserted);
    // tree.show();

    // tree.insert_unique(7, 7, inserted);
    // tree.show();

    // tree.insert_unique(19, 19, inserted);
    // tree.show();

    // tree.insert_unique(4, 4, inserted);
    // tree.show();

    // tree.insert_unique(15, 15, inserted);
    // tree.show();

    // tree.insert_unique(18, 18, inserted);
    // tree.show();

    // tree.insert_unique(5, 5, inserted);
    // tree.show();

    // tree.insert_unique(14, 14, inserted);
    // tree.show();

    // tree.insert_unique(13, 13, inserted);
    // tree.show();

    // tree.insert_unique(10, 10, inserted);
    // tree.show();

    // tree.insert_unique(16, 16, inserted);
    // tree.show();

    // tree.insert_unique(6, 6, inserted);
    // tree.show();
    
    // tree.insert_unique(3, 3, inserted);
    // tree.show();

    // tree.insert_unique(8, 8, inserted);
    // tree.show();

    // tree.insert_unique(17, 17, inserted);
    // tree.show();

    // tree.erase(tree.lower_bound(12));
    // tree.show();
    // tree.erase(tree.lower_bound(1));
    // tree.show();

    // tree.erase(tree.lower_bound(9));
    // tree.show();

    // tree.erase(tree.lower_bound(2));
    // tree.show();

    // tree.erase(tree.lower_bound(0));
    // tree.show();

    // tree.erase(tree.lower_bound(11));
    // tree.show();

    // tree.erase(tree.lower_bound(7));
    // tree.show();

    // tree.erase(tree.lower_bound(19));
    // tree.show();

    // tree.erase(tree.lower_bound(4));
    // tree.show();

    // tree.erase(tree.lower_bound(15));
    // tree.show();

    // tree.erase(tree.lower_bound(18));
    // tree.show();

    // tree.erase(tree.lower_bound(5));
    // tree.show();

    // tree.erase(tree.lower_bound(14));
    // tree.show();

    // tree.erase(tree.lower_bound(13));
    // tree.show();

    // tree.erase(tree.lower_bound(10));
    // tree.show();

    // tree.erase(tree.lower_bound(16));
    // tree.show();

    // tree.erase(tree.lower_bound(6));
    // tree.show();
    
    // tree.erase(tree.lower_bound(3));
    // tree.show();

    // tree.erase(tree.lower_bound(8));
    // tree.show();

    // tree.erase(tree.lower_bound(17));
    // tree.show();
    return 0;
}