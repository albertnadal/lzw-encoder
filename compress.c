// gcc -O1 compress.c -o compress
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "trie.c"

int original_file_size = 0;
float compressed_file_size = 0.0f;

static inline void write_code_to_file(FILE *file, uint16_t code)
{
    fwrite(&code, sizeof(uint16_t), 1, file);
    compressed_file_size += 1.5; // 16 bits -> 2 bytes | 12 bits -> 1.5 bytes
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
    original_file_size++;
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
        original_file_size++;

        TrieNode *p_plus_c;
        if (search_char(p, c, &p_plus_c) == true)
        {
            p = p_plus_c; // P = P + C
        }
        else
        {
            insert_char(p, c, current_code++);          // add P + C to the string table
            write_code_to_file(output_file, p->value);  // output the code for P
            bool char_found = search_char(tree, c, &p); // P = C
            assert(char_found);
        }
    }

    write_code_to_file(output_file, p->value);   // output the code for P
    write_dictionary_to_file(output_file, tree); // serialize and append the table at the end of the output file

    printf("ORIGINAL FILE SIZE: %d bytes\n", original_file_size);
    printf("COMPRESSED FILE SIZE: %d bytes\n", (int)compressed_file_size);
    printf("DICTIONARY SIZE: %d bytes\n", dictionary_size);

    // TODO: Use only 12 bits, instead of the current 16 bits, to store the codes to the output file.

    fclose(output_file);
    fclose(input_file);
    free_trie_node(tree);

    return 0;
}