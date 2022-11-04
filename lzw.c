// gcc -O1 lzw.c -o lzw
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "trie.c"

static inline void output_code(FILE *file, uint16_t code)
{
    fwrite(&code, sizeof(uint16_t), 1, file);
    printf("%u,", code);
}

int main(void)
{
    FILE *input_file = fopen("input.txt", "r");

    if (!input_file)
    {
        printf("Failed opening input file.\n");
        return 1;
    }

    FILE *output_file = fopen("output.bin", "wb");

    if (!output_file)
    {
        printf("Failed opening output file.\n");
        return 1;
    }

    // Get the first byte from the input file
    unsigned char c = fgetc(input_file);
    if (feof(input_file))
    {
        printf("File is empty.\n");
        return 1;
    }

    // Create the dictionary (Trie tree structure)
    TrieNode *tree = make_trie_node(); // Root node

    // Initialize the code to 0
    uint16_t current_code = 0;

    /*
      1     Initialize table with single character strings
      2     P = first input character
      3     WHILE not end of input stream
      4          C = next input character
      5          IF P + C is in the string table
      6            P = P + C
      7          ELSE
      8            add P + C to the string table
      9            output the code for P
      10           P = C
      11    END WHILE
      12    output code for P
    */

    // Initialize table with single character strings
    for (current_code = 0; current_code < 256; current_code++)
    {
        insert_char(tree, (unsigned char)current_code, current_code);
    }

    TrieNode *p;
    bool char_found = search_char(tree, c, &p);
    assert(char_found);

    while (!feof(input_file))
    {
        c = fgetc(input_file); // C = next input character

        TrieNode *p_plus_c;
        if (search_char(p, c, &p_plus_c) == true)
        {
            p = p_plus_c; // P = P + C
        }
        else
        {
            insert_char(p, c, current_code++);          // add P + C to the string table
            output_code(output_file, p->value);         // output the code for P
            bool char_found = search_char(tree, c, &p); // P = C
            assert(char_found);
        }
    }

    output_code(output_file, p->value); // output the code for P

    // TODO: Implement function to extract the dictionary from the Trie tree
    // TODO: Append the dictionary in the end of the output file.
    // TODO: Use only 12 bits, instead of the current 16 bits, to store the codes to the output file.

    fclose(output_file);
    fclose(input_file);
    free_trie_node(tree);

    return 0;
}