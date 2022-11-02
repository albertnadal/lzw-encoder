// gcc lzw.c -o lzw
#include <stdio.h>
#include <stdbool.h>
#include "trie.c"

#define SEQUENCE_MAX_SIZE 1024
#define CODE_MAX_VALUE 65536 // 16 bits per code

int main(void)
{
    FILE *file = fopen("sample.txt", "r");

    if (!file)
    {
        printf("Failed opening file.\n");
        return 1;
    }

    unsigned char sequence[SEQUENCE_MAX_SIZE];
    unsigned int sequence_idx = 0;
    uint16_t current_code = 0;

    // Get the first byte from the stream
    sequence[sequence_idx++] = fgetc(file);
    if (feof(file))
    {
        printf("File is empty.\n");
        return 1;
    }

    // Create the dictionary (Trie tree structure)
    TrieNode *tree = make_trie_tree();

    // Initialize the dictionary with single char sequences (0 to 255 codes)
    for (current_code = 0; current_code < 256; current_code++)
    {
        insert_char(tree, (unsigned char)current_code);
    }

    unsigned char next_char;
    TrieNode *node;

    while (!feof(file))
    {
        next_char = fgetc(file);
        sequence[sequence_idx] = next_char;
        sequence[++sequence_idx] = '\0';
        bool need_reset_sequence = false;

        if (search_trie(tree, sequence, &node) == false)
        {
            if (current_code < CODE_MAX_VALUE - 1)
            {
                insert_string(tree, sequence, current_code++);
            }
            else
            {
                printf(" Reached code max value. No more available codes left.\n");
            }

            need_reset_sequence = true;
        }
        else if (sequence_idx + 1 >= SEQUENCE_MAX_SIZE)
        {
            need_reset_sequence = true;
        }

        if (need_reset_sequence)
        {
            sequence[sequence_idx - 1] = '\0';
            search_trie(tree, sequence, &node);
            printf("%u,", node->value); // TODO: Append the code in an output file in a 12 bit packet

            sequence[0] = next_char;
            sequence[1] = '\0';
            sequence_idx = 1;
        }
    }

    // TODO: Calculate the size of the compressed output and compare it with the size of the original uncompressed file.
    // TODO: Try to manually recreate the decompression and check that the output matches with the original file.
    // TODO: Store the codes in the output file.
    // TODO: Store the dictionary in the end of the output file.

    fclose(file);
    free_trie_node(tree);

    return 0;
}