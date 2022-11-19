// gcc -O1 compress.c -o compress
#include <stdio.h>
#include <assert.h>
#include "trie.c"
#include "constants.h"

static inline void write_code_to_file(FILE *file, uint32_t code, size_t bytes_per_code)
{
    fwrite(&code, bytes_per_code, 1, file);
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

    // Reserve space to store the position in the file where codes will be stored using 24 bits (3 bytes)
    fseek(output_file, sizeof(long int), SEEK_SET);

    // Reserve space to store the position in the file where codes will be stored using 32 bits (4 bytes)
    fseek(output_file, sizeof(long int), SEEK_CUR);

    // Reserve space to store the value of the maximum code generated
    fseek(output_file, sizeof(uint32_t), SEEK_CUR);

    // Get the first byte from the input file
    unsigned char c = fgetc(input_file);
    if (feof(input_file))
    {
        printf("File is empty.\n");
        return 1;
    }

    // Positions in the file where codes are stored using 24 bits and 32 bits
    long int file_offset_24bit_codes = 0, file_offset_32bit_codes = 0;

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
    size_t bytes_per_code = 2; // 16 bits initially

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
            insert_char(p, c, current_code++);                         // add P + C to the string table
            write_code_to_file(output_file, p->value, bytes_per_code); // output the code for P
            bool char_found = search_char(tree, c, &p);                // P = C
            assert(char_found);

            if (current_code == 0xFFFF) // 2^16 - 1
            {
                bytes_per_code = 3; // 24 bits
                file_offset_24bit_codes = ftell(output_file);
            }
            else if (current_code == 0xFFFFFF) // 2^24 - 1
            {
                bytes_per_code = 4; // 32 bits
                file_offset_32bit_codes = ftell(output_file);
            }
        }
    }

    // TODO: Limit the size (length) of new sequences using the MAX_SEQUENCE_SIZE definition to avoid out of memory issues.
    // TODO: Test compression of small, medium, large and extra large files.
    write_code_to_file(output_file, p->value, bytes_per_code); // output the code for P

    // Set header values (file offset for 32bit codes and max code value)
    fseek(output_file, 0, SEEK_SET);
    fwrite(&file_offset_24bit_codes, sizeof(long int), 1, output_file);
    fwrite(&file_offset_32bit_codes, sizeof(long int), 1, output_file);
    fwrite(&current_code, sizeof(uint32_t), 1, output_file);

    fclose(output_file);
    fclose(input_file);
    free_trie_node(tree);

    return 0;
}