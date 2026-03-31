#ifndef PA4_H
#define PA4_H

#include <stdio.h>
#include <stdlib.h>
#include "hbt.h"

Tnode *HBT_create(FILE *fp, int *mem_fail);
Tnode *HBT_insert(Tnode *node, int key, int *mem_fail); // insert duplicates to the left of node with same key
Tnode *HBT_delete(Tnode *node, int key); // delete first duplicate node, replace with immediate predecessor (largest in left)
void HBT_write_file(Tnode *node, FILE *fp);
void HBT_free(Tnode *node);
void HBT_print(Tnode *node);
Tnode *HBT_load(FILE *fp, int *validity);
int check_AVL(Tnode* node);
int is_BST(Tnode* node, long min, long max);

Tnode *rebalance(Tnode *node);
Tnode *create_node(int key);
Tnode *get_predecessor(Tnode *node);
Tnode *delete_predecessor_node(Tnode *node);
Tnode *rotate_left(Tnode *node);
Tnode *rotate_right(Tnode *node);
int get_height(Tnode *node);

#endif