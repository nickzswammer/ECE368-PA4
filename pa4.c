#include <string.h>
#include "tree.h"

int main(int argc, char *argv[]){
    // validate input
    if (argc < 3){
        // if no option provided
        fprintf(stderr, "ERROR! Usage: ./pa4 -b/-e <input_file> <output_file>(-b only)\n");
        return EXIT_FAILURE;
    }

    // functionality for -b and -e
    if (strcmp(argv[1], "-b") == 0){
        // requires four args ./pa4 -b in_ops out_tree
        if (argc != 4){
            fprintf(stderr, "ERROR! Usage: ./pa4 -b <in_ops> <out_tree>\n");
            return EXIT_FAILURE;
        }

        // perform tasks for -b
        char *ops_file = argv[2];
        char *tree_file = argv[3];

        FILE *fp_in = fopen(ops_file, "rb");
        if (fp_in == NULL) {
            fprintf(stdout, "%d\n", -1);
            return EXIT_FAILURE;
        }

        FILE *fp_out = fopen(tree_file, "wb");
        if (fp_out == NULL){
            fclose(fp_in);
            fprintf(stdout, "%d\n", -1);
            return EXIT_FAILURE;
        }

        int mem_fail = 0;
        Tnode *tree = HBT_create(fp_in, &mem_fail);
        HBT_write_file(tree, fp_out);

        HBT_free(tree);

        fclose(fp_in);
        fclose(fp_out);

        if (mem_fail) {
            fprintf(stdout, "0\n");
            return EXIT_FAILURE;
        }

    } else if (strcmp(argv[1], "-e") == 0){
        // requires three args ./pa4 -e in_tree
        if (argc != 3){
            fprintf(stderr, "ERROR! Usage: ./pa4 -e <in_tree>\n");
            return EXIT_FAILURE;
        }

        // perform tasks for -e
        char *tree_file = argv[2];
        FILE *fp = fopen(tree_file, "rb");

        if (fp == NULL) {
            printf("-1,0,0\n");
            return EXIT_FAILURE;
        }

        int validity = 1;
        Tnode *tree = HBT_load(fp, &validity);

        // check no leftover bytes
        if (validity == 1) {
            unsigned char extra;
            if (fread(&extra, 1, 1, fp) != 0) {
                validity = 0;  // extra bytes = wrong format
            }
        }

        if (validity == 0){
            printf("0,0,0\n"); // wrong format
            HBT_free(tree);
            fclose(fp);
            return EXIT_FAILURE;
        }
        int bst = is_BST(tree, HBT_MIN, HBT_MAX);
        int balanced = (check_AVL(tree) != -2) ? 1 : 0;

        printf("1,%d,%d\n", bst, balanced);
        HBT_free(tree);
        fclose(fp);
        return EXIT_SUCCESS;

    } else {
        // unknown
        fprintf(stderr, "ERROR! Usage: ./pa4 -b/-e <input_file> <output_file>(-b only)\n");
        return EXIT_FAILURE;        
    }

    fprintf(stdout, "%d\n", 1);
    return EXIT_SUCCESS;
}