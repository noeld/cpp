#include <iostream>
#include <utility>
#include <memory>
#include <vector>

struct BTNode;
enum class BTNodeType : uint8_t { KEY = 1, LEAF = 2};
struct alignas (int) BTNodeInfo {
    unsigned is_root_ : 1;
    unsigned r01_     : 7;
    BTNodeType type_ { BTNodeType::KEY };
    const uint16_t magic = 0xcaff;
    bool is_root() const noexcept { return is_root_ == 1; }
    bool is_key()  const noexcept { return BTNodeType::KEY == type_; }
    bool is_leaf() const noexcept { return BTNodeType::LEAF == type_; }
};
struct BTKeyNode;
struct BTLeafNode;

struct IBTKeyLeafVisitor {
    virtual void process(BTKeyNode*)  =0;
    virtual void process(BTLeafNode*) =0;
};

class BTree {
public:
    using index_t     = uint32_t;
    using node_size_t = uint16_t;
    using uptr        = std::unique_ptr<BTNode>;

    static constexpr const node_size_t KEY_NODE_SIZE  { 16 };
    static constexpr const node_size_t LEAF_NODE_SIZE { 16 };

    BTNode* get_root();
    BTNode* get_node(index_t index);
    index_t size() const noexcept { return size_; }
private:
    uptr create_key_node();
    uptr create_leaf_node();
private:
    index_t size_ { 0 };
    index_t root_index { 0 };
    uptr root_ { nullptr };
};

struct BTNode { 
    /*const*/ BTree::index_t index_ { 0 };
    BTree::index_t       parent_index_ { 0 };
    BTNodeInfo           info_;
    BTree::node_size_t   size_  { 0 };
    int keys_[BTree::KEY_NODE_SIZE];

    BTKeyNode* as_key_node() { return reinterpret_cast<BTKeyNode*>(this); }
    BTLeafNode* as_leaf_node() { return reinterpret_cast<BTLeafNode*>(this); }
    void accept(IBTKeyLeafVisitor* v) {
        switch(info_.type_) {
            case BTNodeType::KEY:
                v->process(this->as_key_node());
                break;
            case BTNodeType::LEAF:
                v->process(this->as_leaf_node());
                break;
            default:
                throw "Unexpected node type";
        }
    }

    friend class BTree;
};

struct BTKeyNode : public BTNode
{
    BTree::index_t children_[BTree::KEY_NODE_SIZE + 1];
    friend class BTree;
};

struct BTLeafNode : public BTNode 
{
    int values_[BTree::KEY_NODE_SIZE];
    friend class BTree;
};


template<typename T>
void print_size() {
    std::cout << "sizeof(" << typeid(T).name() << ") = " << sizeof(T) << std::endl;
}

int main(int argc, char const *argv[])
{
    print_size<BTKeyNode>();
    print_size<BTLeafNode>();
    return 0;
}