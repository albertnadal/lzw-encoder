// gcc -O1 compress.c -o compress
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "trie.c"
#include "constants.h"

static inline void write_code_to_file(FILE *file, uint32_t code, bool is_32bit_code)
{
    fwrite(&code, is_32bit_code ? sizeof(uint32_t) : sizeof(uint16_t), 1, file);
}

int main(void)
{
    FILE *input_file = fopen("input.txt", "r");

    if (!input_file)
    {
        printf("Failed opening input file.\n");
        return 1;
    }

    FILE *output_file = fopen("compressed.dat", "wb");

    if (!output_file)
    {
        printf("Failed opening output file.\n");
        return 1;
    }

    // Reserve space to store the position in the file where codes will be stored using 32 bits
    fseek(output_file, sizeof(long int), SEEK_SET);

    // Get the first byte from the input file
    unsigned char c = fgetc(input_file);
    if (feof(input_file))
    {
        printf("File is empty.\n");
        return 1;
    }

    // Position in the file where codes are stored using 32 bits instead of 16 bits
    long int file_offset = -1;

    // Create the dictionary (Trie tree structure)
    TrieNode *tree = make_trie_node(); // Root node

    // Initialize the code to 0
    uint32_t current_code = 0;

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
    bool use_32bit_codes = false;

    while (!feof(input_file))
    {
        c = fgetc(input_file); // C = next input character

        TrieNode *p_plus_c;
        if (search_char(p, c, &p_plus_c) == true) // IF P + C is in the string table
        {
            p = p_plus_c; // P = P + C
        }
        else
        {
            insert_char(p, c, current_code++);                          // add P + C to the string table
            write_code_to_file(output_file, p->value, use_32bit_codes); // output the code for P
            bool char_found = search_char(tree, c, &p);                 // P = C
            assert(char_found);

            if (current_code == 0xFFFF)
            {
                use_32bit_codes = true;
                file_offset = ftell(output_file);
            }
        }
    }

    // TODO: Store the maximum code generated in the header of the output file.
    write_code_to_file(output_file, p->value, use_32bit_codes); // output the code for P

    fseek(output_file, 0, SEEK_SET);
    fwrite(&file_offset, sizeof(long int), 1, output_file);

    fclose(output_file);
    fclose(input_file);
    free_trie_node(tree);

    return 0;
}