/** An implementation of a simple AVL tree for uint64_t values
 *  Nodes are references by indexed of type unsigned. 
 *  No pointers are kept in memory except for the main tree struct.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cassert>
// simple tree struct

struct node_t;
typedef struct node_t node_t;

struct node_t {
    unsigned left_index;
    unsigned right_index;
   /* struct {
        unsigned balance : 3;
    };*/
    uint64_t value;
};

struct tree_t;
typedef struct tree_t tree_t;

struct tree_t {
    size_t capacity;
    unsigned size;
    size_t root_index;
    size_t node_stack_capacity;
    size_t node_stack_index;
    unsigned* node_stack;
    node_t* nodes;
};

#define TREE_INITIAL_CAPACITY (100ull)
#define TREE_NULLNODE ((unsigned)0xfffffffful)

typedef struct tree_iterator {
    tree_t* tree;
    unsigned node;
    size_t node_stack_capacity;
    size_t node_stack_index;
    unsigned* node_stack;
    char* node_visited_stack;
} tree_iterator;

void tree_resize(tree_t* tree, size_t new_capacity);

tree_t* tree_create()
{
    tree_t* tree = malloc(sizeof(tree_t));
    memset(tree, 0, sizeof(tree_t));
    tree_resize(tree, TREE_INITIAL_CAPACITY);
    //tmp->nodes = calloc(TREE_INITIAL_CAPACITY, sizeof(node_t));
    //tmp->capacity = TREE_INITIAL_CAPACITY;
    tree->size = 0;
    tree->root_index = TREE_NULLNODE;
    //tmp->node_stack_capacity = (size_t)ceil(log(tmp->capacity) / log(2.0)) + 2;
    //tmp->node_stack = calloc(tmp->node_stack_capacity, sizeof(unsigned));
    tree->node_stack_index = 0;

    size_t nodes_size = TREE_INITIAL_CAPACITY * sizeof(node_t);
    memset((void*)tree->nodes, 0xff, nodes_size);
    return tree;
}

void tree_delete(tree_t* tree)
{
    free(tree->node_stack);
    tree->node_stack_capacity = 0;
    tree->node_stack = NULL;
    free(tree->nodes);
    tree->capacity = 0;
    tree->nodes = NULL;
    free(tree);
}

#define TREE_FILEVERSION (0x01000000u + (unsigned)(sizeof(void*)))

void tree_write_to_file(tree_t* tree, FILE* file)
{
    tree_t tree_copy = *tree;
    tree_copy.node_stack = NULL;
    tree_copy.nodes = NULL;
    tree_copy.node_stack_capacity = 0;
    tree_copy.node_stack_index = 0;
    unsigned file_version = TREE_FILEVERSION;
    size_t written = fwrite(&file_version, sizeof(unsigned), 1, file);
    written += fwrite(&tree_copy, sizeof(tree_t), 1, file);
    written += fwrite(tree->nodes, sizeof(node_t), tree->size, file);
}

tree_t* tree_load_from_file(FILE* file)
{
    unsigned file_version;
    size_t read = fread(&file_version, sizeof(unsigned), 1, file);
    if (file_version != TREE_FILEVERSION)
    {
        perror("Wrong file version");
        exit(10);
    }
    tree_t tree_copy;
    memset(&tree_copy, 0, sizeof(tree_t));
    read += fread(&tree_copy, sizeof(tree_t), 1, file);
    tree_t* tree = tree_create();
    tree_resize(tree, tree_copy.capacity);
    tree->size = tree_copy.size;
    tree->root_index = tree_copy.root_index;
    for (size_t i = 0, ii = 0; i < tree->size; ii = fread(tree->nodes, sizeof(node_t), tree->size - i, file), i += ii)
    {
        if (ii < tree->size - i)
        {
            int f = ferror(file);
        }
    }
    return tree;
}

inline
node_t* tree_get_node(tree_t* tree, unsigned index)
{
    assert(index < tree->capacity);
    return &tree->nodes[index];
}

#define tgn(tree, index) (tree_get_node(tree, index))

void tree_resize(tree_t* tree, size_t new_capacity)
{
    printf("Resizing from %zu to %zu\n", tree->capacity, new_capacity);
    assert(new_capacity >= tree->capacity);
    node_t* new_nodes = realloc(tree->nodes, sizeof(tree->nodes[0]) * new_capacity);
    if (new_nodes == NULL)
    {
        perror("out of memory for nodes");
        exit(1);
    }
    size_t new_node_stack_capacity = (size_t)ceil(log(new_capacity) / log(2.0)) + 2;
    unsigned* new_stack = realloc(tree->node_stack, sizeof(tree->node_stack[0]) * new_node_stack_capacity);
    if (new_stack == NULL)
    {
        perror("out of memory for node_stack");
        exit(1);
    }
    tree->nodes = new_nodes;
    tree->node_stack = new_stack;
    tree->node_stack_capacity = new_node_stack_capacity;
    tree->capacity = new_capacity;
}

unsigned tree_create_node(tree_t* tree, uint64_t value)
{
    size_t index = tree->size++;
    if (tree->size >= tree->capacity)
    {
        size_t new_capacity = (size_t)(tree->capacity * 1.5);
        tree_resize(tree, new_capacity);
    }
    node_t* node = tree_get_node(tree, index); // &tree->nodes[index];
    node->value = value;
    node->left_index = TREE_NULLNODE;
    node->right_index = TREE_NULLNODE;
    return index;
}

#define tree_node_index_is_null(index) (index == TREE_NULLNODE)

#define tree_has_root(tree) (!tree_node_index_is_null((tree)->root_index))

#define tree_get_root_node(tree) (tree_get_node(tree, tree->root_index))

/** 
 * Returns the node containing value
 * If value is not found then the node_stack[node_index - 1] contains the insertion point
*/
unsigned tree_find_recurse(tree_t* tree, unsigned node_index, uint64_t value)
{
    unsigned ret = TREE_NULLNODE;
    if (!tree_node_index_is_null(node_index)) {
        if (tree->node_stack_index > tree->node_stack_capacity)
        {
            perror("Out of node stack capacity");
            printf("node_stack_index: %d, node_stack_capacity: %llu\n", tree->node_stack_index, tree->node_stack_capacity);
            exit(1);
        }
        tree->node_stack[tree->node_stack_index++] = node_index;
        node_t* node = tree_get_node(tree, node_index);
        if (value < node->value) {
            ret = tree_find_recurse(tree, node->left_index, value);
        }
        else if (node->value < value) {
            ret = tree_find_recurse(tree, node->right_index, value);
        }
        else if (value == node->value) {
            ret = node_index;
        }
    }
    return ret;
}

unsigned tree_node_height(tree_t* tree, unsigned node_index)
{
    if (tree_node_index_is_null(node_index))
        return 0;
    node_t* node = tree_get_node(tree, node_index);
    size_t left_height = tree_node_height(tree, node->left_index);
    size_t right_height = tree_node_height(tree, node->right_index);
    return 1 + ((left_height > right_height) ? left_height : right_height);
}

long long tree_node_balance_factor(tree_t* tree, unsigned node_index)
{
    node_t* node = tree_get_node(tree, node_index);
    return (long long)tree_node_height(tree, node->right_index) - (long long)tree_node_height(tree, node->left_index);
}

unsigned tree_find(tree_t* tree, uint64_t value)
{
    unsigned ret = TREE_NULLNODE;
    if (tree_has_root(tree))
    {
        tree->node_stack_index = 0;
        ret = tree_find_recurse(tree, tree->root_index, value);
    }
    return ret;
}

unsigned tree_node_rotate_left(tree_t* tree, unsigned x_index, unsigned z_index)
{
    //printf("tree_node_rotate_left %llu, %llu\n", tree_get_node(tree, x_index)->value, tree_get_node(tree, z_index)->value);
    unsigned t23 = tree_get_node(tree, z_index)->left_index;
    tree_get_node(tree, x_index)->right_index = t23;
    tree_get_node(tree, z_index)->left_index = x_index;
    /*
    // 1st case, BalanceFactor(Z) == 0, only happens with deletion, not insertion:
    if (BalanceFactor(Z) == 0) { // t23 has been of same height as t4
        BalanceFactor(X) = +1;   // t23 now higher
        BalanceFactor(Z) = –1;   // t4 now lower than X
    }
    else { // 2nd case happens with insertion or deletion:
        BalanceFactor(X) = 0;
        BalanceFactor(Z) = 0;
    }*/
    return z_index;
}

unsigned tree_node_rotate_right(tree_t* tree, unsigned x_index, unsigned z_index)
{
    //printf("tree_node_rotate_right %llu, %llu\n", tree_get_node(tree, x_index)->value, tree_get_node(tree, z_index)->value);
    unsigned t23 = tree_get_node(tree, z_index)->right_index;
    tree_get_node(tree, x_index)->left_index = t23;
    tree_get_node(tree, z_index)->right_index = x_index;
    /*
    // 1st case, BalanceFactor(Z) == 0, only happens with deletion, not insertion:
    if (BalanceFactor(Z) == 0) { // t23 has been of same height as t4
        BalanceFactor(X) = +1;   // t23 now higher
        BalanceFactor(Z) = –1;   // t4 now lower than X
    }
    else { // 2nd case happens with insertion or deletion:
        BalanceFactor(X) = 0;
        BalanceFactor(Z) = 0;
    }*/
    return z_index;
}

unsigned tree_node_rotate_right_left(tree_t* tree, unsigned x_index, unsigned z_index)
{
    assert(z_index == tree_get_node(tree, x_index)->right_index);
    //printf("tree_node_rotate_right_left %llu, %llu\n", tree_get_node(tree, x_index)->value, tree_get_node(tree, z_index)->value);
    unsigned y_index = tree_get_node(tree, z_index)->left_index;
    unsigned t3 = tree_get_node(tree, y_index)->right_index;
    tree_get_node(tree, z_index)->left_index = t3;
    tree_get_node(tree, y_index)->right_index = z_index;
    unsigned t2 = tree_get_node(tree, y_index)->left_index;
    tree_get_node(tree, x_index)->right_index = t2;
    tree_get_node(tree, y_index)->left_index = x_index;
    /*
    if (BalanceFactor(Y) > 0) { // t3 was higher
        BalanceFactor(X) = –1;  // t1 now higher
        BalanceFactor(Z) = 0;
    }
    else
        if (BalanceFactor(Y) == 0) {
            BalanceFactor(X) = 0;
            BalanceFactor(Z) = 0;
        }
        else {
            // t2 was higher
            BalanceFactor(X) = 0;
            BalanceFactor(Z) = +1;  // t4 now higher
        }
    BalanceFactor(Y) = 0;
    */
    return y_index;
}

unsigned tree_node_rotate_left_right(tree_t* tree, unsigned x_index, unsigned z_index)
{
    assert(z_index == tree_get_node(tree, x_index)->left_index);
    //printf("tree_node_rotate_left_right %llu, %llu\n", tree_get_node(tree, x_index)->value, tree_get_node(tree, z_index)->value);
    unsigned y_index = tree_get_node(tree, z_index)->right_index;
    unsigned t2 = tree_get_node(tree, y_index)->left_index;
    tree_get_node(tree, z_index)->right_index = t2;
    tree_get_node(tree, y_index)->left_index = z_index;
    unsigned t3 = tree_get_node(tree, y_index)->right_index;
    tree_get_node(tree, x_index)->left_index = t3;
    tree_get_node(tree, y_index)->right_index = x_index;
    return y_index;
}

void tree_insert(tree_t* tree, uint64_t value)
{
    //printf("Inserting %llu\n", value);
    unsigned node_index = TREE_NULLNODE;
    if (!tree_has_root(tree))
    {
        node_index = tree_create_node(tree, value);
        tree->root_index = tree->size - 1;
    }
    else {
        tree->node_stack_index = 0;
        node_index = tree_find_recurse(tree, tree->root_index, value);

        if (tree_node_index_is_null(node_index))
        {
            node_index = tree->node_stack[tree->node_stack_index - 1];
            unsigned new_node_index = tree_create_node(tree, value);
            if (tree_get_node(tree, node_index)->value > value)
                tree_get_node(tree, node_index)->left_index = new_node_index;
            else
                tree_get_node(tree, node_index)->right_index = new_node_index;
            
            if (tree->node_stack_index > 1)
            {
                // Backtrack node_stack, rebalance
                do {
                    unsigned parent_node_index = tree->node_stack[tree->node_stack_index - 2];
                    long long bal = tree_node_balance_factor(tree, parent_node_index);
                    if (bal <= -2 || bal >= 2)
                    {
                        unsigned this_node_index = tree->node_stack[tree->node_stack_index - 1];
                        long long bal_cur = tree_node_balance_factor(tree, this_node_index);
                        int dir1_left = tree_get_node(tree, parent_node_index)->left_index == this_node_index;
                        int dir1_right = !dir1_left;
                        unsigned new_parent_node_index = TREE_NULLNODE;
                        if (dir1_right && (bal_cur >= 0))
                            new_parent_node_index = tree_node_rotate_left(tree, parent_node_index, this_node_index);
                        else if (dir1_left && (bal_cur <= 0))
                            new_parent_node_index = tree_node_rotate_right(tree, parent_node_index, this_node_index);
                        else if (dir1_right && (bal_cur == -1))
                            new_parent_node_index = tree_node_rotate_right_left(tree, parent_node_index, this_node_index);
                        else if (dir1_left && (bal_cur == 1))
                            new_parent_node_index = tree_node_rotate_left_right(tree, parent_node_index, this_node_index);
                        else
                        {
                            perror("No rotation can fix this!");
                            exit(2);
                        }
                        if (tree->node_stack_index == 2)
                        {
                            tree->root_index = new_parent_node_index;
                        }
                        else {
                            node_t* pp = tree_get_node(tree, tree->node_stack[tree->node_stack_index - 3]);
                            if (parent_node_index == pp->left_index)
                                pp->left_index = new_parent_node_index;
                            else if (parent_node_index == pp->right_index)
                                pp->right_index = new_parent_node_index;
                            else {
                                perror("Cannot reassign new parent node!");
                                exit(3);
                            }
                        }
                    }
                } while (--tree->node_stack_index > 1);
            }
        }
    }
}



void tree_node_print(tree_t* tree, unsigned node_index, int depth)
{
    for (int i = 0; i < depth; ++i)
        printf(".");
    if (!tree_node_index_is_null(node_index))
    {
        printf("%llu(%lld)\n", tree_get_node(tree, node_index)->value, tree_node_balance_factor(tree, node_index));
        tree_node_print(tree, tree_get_node(tree, node_index)->left_index, depth + 1);
        tree_node_print(tree, tree_get_node(tree, node_index)->right_index, depth + 1);
    }
    else {
        printf("\n");
    }
}

tree_iterator* tree_iterator_create(tree_t* tree)
{
    tree_iterator* it = malloc(sizeof(tree_iterator));
    it->tree = tree;
    it->node = TREE_NULLNODE;
    it->node_stack_capacity = it->tree->node_stack_capacity;
    it->node_stack_index = 0;
    it->node_stack = malloc(sizeof(node_t) * it->node_stack_capacity);
    it->node_visited_stack = malloc(sizeof(it->node_visited_stack[0]) * it->node_stack_capacity);
    memset(it->node_visited_stack, 0, it->node_stack_capacity);
    if (tree_has_root(it->tree))
    {
        it->node_stack[it->node_stack_index++] = it->tree->root_index;
        it->node_visited_stack[it->node_stack_index - 1] = 0;
    }
    return it;
}

void tree_iterator_delete(tree_iterator* it)
{
    free(it->node_stack);
    free(it->node_visited_stack);
    it->node_stack = NULL;
    it->node_visited_stack = NULL;
    it->node_stack_capacity = 0;
    it->node_stack_index = 0;
    it->node = TREE_NULLNODE;
    it->tree = NULL;
    free(it);
}

inline
uint64_t tree_iterator_get_value(tree_iterator* it)
{
    return tgn(it->tree, it->node_stack[it->node_stack_index - 1])->value;
}

int tree_iterator_next(tree_iterator* it)
{
    if (it->node_stack_index == 0)
        return 1 == 0;
    char nv = it->node_visited_stack[it->node_stack_index - 1];
    it->node_visited_stack[it->node_stack_index - 1] += 1;
    node_t* current_node = tgn(it->tree, it->node_stack[it->node_stack_index - 1]);
    switch (nv)
    {
    case 0: // go left side
        if (!tree_node_index_is_null(current_node->left_index))
        {
            it->node_stack[it->node_stack_index++] = current_node->left_index;
            it->node_visited_stack[it->node_stack_index - 1] = 0;
        }
        break;
    case 1: // this value
        return 0 == 0;
        break;
    case 2: // right side
        if (!tree_node_index_is_null(current_node->right_index))
        {
            it->node_stack[it->node_stack_index++] = current_node->right_index;
            it->node_visited_stack[it->node_stack_index - 1] = 0;
        }
        break;
    case 4: // go up
        it->node_stack_index--;
        break;
    default:
        break;
    }
    return tree_iterator_next(it);
}

/* how do we want to use the iterator?
 * Initialize: tree_iterator* it = tree_iterator_create(tree);
 * Use:
 * while(tree_iterator_next(it))
 * {
 *     uint64_t value = tree_iterator_get_value(it)
 * }
 * tree_iterator_delete(it);
*/

void tree_node_print_ordered(tree_t* tree, unsigned node_index)
{
    if (!tree_node_index_is_null(node_index))
    {
        tree_node_print_ordered(tree, tree_get_node(tree, node_index)->left_index);
        printf("%llu, ", tree_get_node(tree, node_index)->value);
        tree_node_print_ordered(tree, tree_get_node(tree, node_index)->right_index);
    }
}

void tree_print(tree_t* tree)
{
    tree_iterator* it = tree_iterator_create(tree);
    while (tree_iterator_next(it))
    {
        uint64_t value = tree_iterator_get_value(it);
        for (size_t i = 1; i < it->node_stack_index; ++i)
            printf("    - ");
        printf("%llu\n", value);
    }
    tree_iterator_delete(it);
}

int main(void) {
    tree_t* tree = tree_create();
    int max = 123;
    for (int i = 0; i < max; ++i)
    {
        tree_insert(tree, rand() % 100000);
    }
    tree_print(tree);

    FILE* file = NULL;
#define FILE_NAME "saved.tree"
    //FILE* file = fopen(FILE_NAME, "wb");
    errno_t err = fopen_s(&file, FILE_NAME, "wb");
    tree_write_to_file(tree, file);
    fclose(file);

    file = NULL;
    //file = fopen("saved.tree", "rb");
    err = fopen_s(&file, FILE_NAME, "rb");
    tree_t* t2 = tree_load_from_file(file);
    fclose(file);
    tree_print(t2);
    tree_delete(t2);
    tree_delete(tree);
    return 0;
}