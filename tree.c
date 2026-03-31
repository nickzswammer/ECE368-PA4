#include "tree.h"
#include "hbt.h"

Tnode *HBT_create(FILE *fp, int *mem_fail){
    if (fp == NULL) return NULL;

    Tnode *tree = NULL;

    int key;
    char op;

    while (fread(&key, sizeof(int), 1, fp) == 1){
        if (fread(&op, sizeof(char), 1, fp) == 1){
            if (op == 'i'){
                tree = HBT_insert(tree, key, mem_fail);
            } else if (op == 'd'){
                tree = HBT_delete(tree, key);
            }
        }
    }
    return tree;
}

Tnode *HBT_insert(Tnode *node, int key, int *mem_fail){
    if (node == NULL){
        Tnode *new_node = create_node(key);
        if (new_node == NULL) *mem_fail = 1;
        return new_node;
    }

    // key > node->key
    if (key <= node->key){
        node->left = HBT_insert(node->left, key, mem_fail);
    } else {
        node->right = HBT_insert(node->right, key, mem_fail);
    }

    return rebalance(node);
}

Tnode *HBT_delete(Tnode *node, int key){
    if (node == NULL) return NULL;

    if (node->key > key){
        node->left = HBT_delete(node->left, key);
    } else if (node->key < key){
        node->right = HBT_delete(node->right, key);
    } else {
        // 0 or 1 child
        if (node->left == NULL){
            Tnode *temp = node->right;
            free(node);
            return temp;
        } else if (node->right == NULL){
            Tnode *temp = node->left;
            free(node);
            return temp;
        }
        // two children
        Tnode *predecessor = get_predecessor(node); // get predecessor
        node->key = predecessor->key; // set current node to that predecessor
        node->left = delete_predecessor_node(node->left); // delete predecessor
    }
    return rebalance(node);
}

void HBT_free(Tnode *node){
    if (node == NULL) return;

    HBT_free(node->left);
    HBT_free(node->right);

    free(node);
}

void HBT_write_file(Tnode *node, FILE *fp){
    if (fp == NULL || node == NULL) return;

    unsigned char c = 0;
    int key = node->key;
    if (node->right != NULL) c |= (1u << 0);
    if (node->left != NULL) c |= (1u << 1);

    fwrite(&key, sizeof(int), 1, fp);
    fwrite(&c, sizeof(char), 1, fp);

    HBT_write_file(node->left, fp);
    HBT_write_file(node->right, fp);
}

Tnode* HBT_load(FILE* fp, int* validity) {
    if (*validity == 0) return NULL;

    int key;
    unsigned char mask;

    // Try to read a node
    size_t read_key = fread(&key, sizeof(int), 1, fp);
    if (read_key == 0) return NULL; // Normal end of a branch

    // If we read a key but can't read the mask, the file is truncated
    if (fread(&mask, sizeof(char), 1, fp) != 1) {
        *validity = 0;
        return NULL;
    }

    Tnode* node = create_node(key);
    if (node == NULL) return NULL;

    // BIT 1: Left Child
    if (mask & (1u << 1)) {
        node->left = HBT_load(fp, validity);
        // If the mask said a child exists, but load returned NULL 
        // AND validity is now 0, it was a truncated file.
        if (node->left == NULL && *validity == 0) return node;
        // If the mask said a child exists, but load returned NULL 
        // AND validity is STILL 1, it means the file ended too early.
        if (node->left == NULL && *validity == 1) {
            *validity = 0;
            return node;
        }
    }

    // Right child
    if (mask & (1u << 0)) {
        node->right = HBT_load(fp, validity);
        if (node->right == NULL) {
            *validity = 0;  // mask promised a child but got NULL
            return node;
        }
    }

    return node;
}

// BST Check: min < key <= max (because duplicates go left)
int is_BST(Tnode* node, long min, long max) {
    if (node == NULL) return 1;
    if (node->key < min || node->key > max) return 0;

    return is_BST(node->left, min, node->key) && 
           is_BST(node->right, (long)node->key + 1, max);
}

// Balance Check: Returns height if balanced, -2 if unbalanced
int check_AVL(Tnode* node) {
    if (node == NULL) return -1; // Match prompt: empty height is -1

    int lh = check_AVL(node->left);
    int rh = check_AVL(node->right);

    if (lh == -2 || rh == -2) return -2; // Child is already unbalanced

    int diff = lh - rh;
    if (diff < -1 || diff > 1) return -2; // Current node unbalanced

    return (lh > rh ? lh : rh) + 1;
}

// pre order
void HBT_print(Tnode *node){
    if (node == NULL) return;
    printf("%d\n", node->key);
    HBT_print(node->left);
    HBT_print(node->right);
}

Tnode *get_predecessor(Tnode *node){
    // go down left tree
    Tnode *current = node->left;

    // keep going right
    while(current->right != NULL){
        current = current->right;
    }
    return current;
}

Tnode *delete_predecessor_node(Tnode *node){
    if (node->right == NULL){
        Tnode *temp = node->left;
        free(node);
        return temp;
    }
    node->right = delete_predecessor_node(node->right);
    return rebalance(node);
}

Tnode *rebalance(Tnode *node){
    if (node == NULL) return NULL;

    int lh = get_height(node->left);
    int rh = get_height(node->right);
    int bal = lh - rh;
    node->balance = bal;

    // left side unbalanced
    if (bal > 1){
        // LL
        if (get_height(node->left->left) >= get_height(node->left->right)){
            return rotate_right(node);
        } else { // LR
            node->left = rotate_left(node->left);
            return rotate_right(node);
        }
    } else if (bal < -1){
        // RR
        if (get_height(node->right->right) >= get_height(node->right->left)){
            return rotate_left(node);
        } else { // RL
            node->right = rotate_right(node->right);
            return rotate_left(node);
        }
    }

    return node;
}

Tnode *create_node(int key){
    Tnode *node = malloc(sizeof(Tnode));
    if (node == NULL) return NULL;

    node->key = key;
    node->balance = 0;
    node->left = NULL;
    node->right = NULL;

    return node;
}

Tnode *rotate_left(Tnode *node){
    Tnode *new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;
    node->balance = get_height(node->left) - get_height(node->right);
    new_root->balance = get_height(new_root->left) - get_height(new_root->right);
    return new_root;
}

Tnode *rotate_right(Tnode *node){
    Tnode *new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    node->balance = get_height(node->left) - get_height(node->right);
    new_root->balance = get_height(new_root->left) - get_height(new_root->right);

    return new_root;
}

int get_height(Tnode *node){
    if (node == NULL) return -1;
    int lh = get_height(node->left);
    int rh = get_height(node->right);
    return (lh > rh ? lh : rh) + 1;
}
