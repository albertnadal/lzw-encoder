#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define N 256 // Alphabet size

typedef struct TrieNode TrieNode;

struct TrieNode
{
    uint16_t value;
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

int dictionary_size = 0;

void write_dictionary_to_file(FILE *file, TrieNode *node)
{
    for (unsigned int i = 0; i < N; i++)
    {
        if (node->children[i] != NULL && node->children[i]->is_leaf)
        {
            fwrite(&i, sizeof(unsigned char), 1, file);
            fwrite(&(node->children[i]->value), sizeof(uint16_t), 1, file);
            dictionary_size += (sizeof(unsigned char) + sizeof(uint16_t));
            write_dictionary_to_file(file, node->children[i]);
        }
    }
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

TrieNode *insert_char(TrieNode *root, unsigned char _char, uint16_t value)
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

TrieNode *insert_string(TrieNode *root, unsigned char *string, uint16_t value)
{
    TrieNode *temp = root;

    for (int i = 0; string[i] != '\0'; i++)
    {
        unsigned int idx = (unsigned int)string[i];
        if (temp->children[idx] == NULL)
        {
            temp->children[idx] = make_trie_node();
        }

        temp = temp->children[idx];
    }

    temp->is_leaf = true;
    temp->value = value;
    return root;
}

bool search_trie(TrieNode *root, unsigned char *word, TrieNode **node_found)
{
    TrieNode *temp = root;

    for (int i = 0; word[i] != '\0'; i++)
    {
        int position = (int)word[i];
        if (temp->children[position] == NULL)
            return false;
        temp = temp->children[position];
    }

    if (temp != NULL && temp->is_leaf == true)
    {
        *node_found = temp;
        return true;
    }
    return false;
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

unsigned int check_divergence(TrieNode *root, unsigned char *word)
{
    TrieNode *temp = root;
    int len = strlen((char *)word);
    if (len == 0)
        return 0;

    unsigned int last_index = 0;
    for (unsigned int i = 0; i < len; i++)
    {
        unsigned int position = (unsigned int)word[i];
        if (temp->children[position])
        {
            for (unsigned int j = 0; j < N; j++)
            {
                if (j != position && temp->children[j])
                {
                    last_index = i + 1;
                    break;
                }
            }

            temp = temp->children[position];
        }
    }
    return last_index;
}

unsigned char *find_longest_prefix(TrieNode *root, unsigned char *word)
{
    if (!word || word[0] == '\0')
        return NULL;

    unsigned int len = strlen((char *)word);
    unsigned char *longest_prefix = (unsigned char *)calloc(len + 1, sizeof(unsigned char));

    for (int i = 0; word[i] != '\0'; i++)
        longest_prefix[i] = word[i];
    longest_prefix[len] = '\0';

    unsigned int branch_idx = check_divergence(root, longest_prefix) - 1;
    if (branch_idx >= 0)
    {
        longest_prefix[branch_idx] = '\0';
        longest_prefix = (unsigned char *)realloc(longest_prefix, (branch_idx + 1) * sizeof(unsigned char));
    }

    return longest_prefix;
}

bool is_leaf_node(TrieNode *root, unsigned char *word)
{
    TrieNode *temp = root;
    for (unsigned int i = 0; word[i]; i++)
    {
        unsigned int position = (unsigned int)word[i];
        if (temp->children[position])
        {
            temp = temp->children[position];
        }
    }
    return temp->is_leaf;
}

TrieNode *delete_string(TrieNode *root, unsigned char *word)
{
    if (!root)
        return NULL;

    if (!word || word[0] == '\0')
        return root;

    if (!is_leaf_node(root, word))
    {
        return root;
    }

    TrieNode *temp = root;
    unsigned char *longest_prefix = find_longest_prefix(root, word);
    if (longest_prefix[0] == '\0')
    {
        free(longest_prefix);
        return root;
    }

    int i;
    for (i = 0; longest_prefix[i] != '\0'; i++)
    {
        int position = (int)longest_prefix[i];
        if (temp->children[position] != NULL)
        {
            temp = temp->children[position];
        }
        else
        {
            free(longest_prefix);
            return root;
        }
    }

    int len = strlen((char *)word);
    for (; i < len; i++)
    {
        unsigned int position = (unsigned int)word[i];
        if (temp->children[position])
        {
            TrieNode *rm_node = temp->children[position];
            temp->children[position] = NULL;
            free_trie_node(rm_node);
        }
    }

    free(longest_prefix);
    return root;
}
