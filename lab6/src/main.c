#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Node {
    int value;
    // it's much simpler to store height, not balance-factor
    int height;
    struct Node* left;
    struct Node* right;
};


int max(int a, int b) {
    return a > b ? a : b;
}

int get_height(struct Node* node) {
    return (node == NULL) ? 0 : node->height;
}

void update_height(struct Node* node) {
    node->height = max(get_height(node->left), get_height(node->right)) + 1;
}


// Left balance is used here
int get_balance(struct Node* node) {
    return get_height(node->left) - get_height(node->right);
}

struct Node* right_rotate(struct Node* head) {
    struct Node* new_head = head->left;
    head->left = new_head->right;
    new_head->right = head;
    update_height(head);
    update_height(new_head);
    return new_head;
}

struct Node* left_rotate(struct Node* head) {
    struct Node* new_head = head->right;
    head->right = new_head->left;
    new_head->left = head;
    update_height(head);
    update_height(new_head);
    return new_head;
}

struct Node* add_value(struct Node* node, int value) {
    if (node == NULL) {
        node = (struct Node*)malloc(sizeof(struct Node));
        node->left = NULL;
        node->right = NULL;
        node->height = 1;
        node->value = value;
        return node;
    }

    if (value > node->value) {
        node->right = add_value(node->right, value);
    } else {
        node->left = add_value(node->left, value);
    }

    update_height(node);

    int balance = get_balance(node);

    // balance should be in {-1, 0, 1}
    // if not we balancing operations should be proceed

    // Left
    if (balance > 1) {
        /* Left - Right
              x (2)                x (2)
             /                    /
            y (-1)        =>     z (1)
             \                  /
              z (0)            y (0)
        */
        if (get_balance(node->left) < 0) {
            node->left = left_rotate(node->left);
        }

        /* Left - Left
                 x (2)           y
                /              /  \   (balanced)
               y (1)      =>  z    x
             /
            z (0)
        */
        return right_rotate(node); 
    }

    // Right
    if (balance < -1) {
        /* Right - Left
            x (-2)             x (-2)
             \                  \
              y (1)       =>     z (-1)
             /                    \
            z (0)                  y (0) 
        */
        if (get_balance(node->right) > 0) {
            node->right = right_rotate(node->right);
        }
        /* Right - Right
            x (-2)                  y
             \                     / \   (balanced)
              y (-1)      =>      x   z
               \
                z (0)
        */
        return left_rotate(node); 
    }

    return node;
}

void free_tree(struct Node* head) {
    if (head == NULL) {
        return;
    }
    free_tree(head->right);
    free_tree(head->left);
    free(head);
}

void dfs_print(struct Node* node) {
    if (node == NULL) {
        return;
    }

    printf("%d ", node->value);
    dfs_print(node->left);
    dfs_print(node->right);
}

int main(int argc, char** argv) {
    int n;
    if (scanf("%d", &n) != 1) {
        printf("bad input\n");
        return 0;
    }
    if  (n < 0 || n > 2000000) {
        printf("bad n\n");
        return 0;
    }

    struct Node* head = NULL;

    for (int i = 0; i < n; i++) {
        int tmp;
        if (scanf("%d", &tmp) != 1) {
            printf("bad input\n");
            free_tree(head);
            return 0;
        }
        // Or could be rewritten to add_value(&head, tmp)
        head = add_value(head, tmp);
    }

    printf("%d\n", get_height(head));

    if (argc > 1 && !strcmp(argv[1], "-t")) {
        dfs_print(head);
        printf("\n");
    }

    free_tree(head);
}
