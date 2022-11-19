#include <stdlib.h>
#include <stdbool.h>

#define N 256 // Alphabet size

typedef struct TrieNode TrieNode;

struct TrieNode
{
    uint32_t value;
    TrieNode *children[N];
    bool is_leaf;
};

TrieNode *make_trie_node()
{
    TrieNode *node = (TrieNode *)calloc(1, sizeof(TrieNode));
    for (unsigned int i = 0; i < N; i++)
    {
        node->children[i] = NULL;
    }
    node->is_leaf = false;
    return node;
}

void free_trie_node(TrieNode *node)
{
    for (unsigned int i = 0; i < N; i++)
    {
        if (node->children[i] != NULL)
        {
            free_trie_node(node->children[i]);
        }
        else
        {
            continue;
        }
    }
    free(node);
}

TrieNode *insert_char(TrieNode *root, unsigned char _char, uint32_t value)
{
    unsigned int idx = (unsigned int)_char;
    if (root->children[idx] == NULL)
    {
        root->children[idx] = make_trie_node();
        TrieNode *temp = root->children[idx];
        temp->is_leaf = true;
        temp->value = value;
    }

    return root;
}

bool search_char(TrieNode *root, unsigned char _char, TrieNode **node_found)
{
    TrieNode *temp = root->children[(unsigned int)_char];

    if (temp != NULL && temp->is_leaf == true)
    {
        *node_found = temp;
        return true;
    }
    return false;
}
