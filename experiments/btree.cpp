#include <iostream>
#include <utility>

const size_t NODESIZE = 16;

class BTree;

struct BTNode { 
    using index_t     = uint32_t;
    using node_size_t = uint16_t;
    index_t index_ { 0 };
    node_size_t size_ { 0 };
    int keys_[NODESIZE];

    friend class BTree;
};

struct BTMNode : public BTNode
{
    BTNode::index_t children_[NODESIZE + 1];
    friend class BTree;
};

struct BTLeafNode : public BTNode 
{
    int values_[NODESIZE];
    friend class BTree;
};

class BTree {
public:
    BTNode* getNode();
};

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}