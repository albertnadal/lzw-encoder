// gcc -O1 decompress.c -o decompress
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "constants.h"

void free_table(char *table[])
{
    for (uint32_t code = 0; code < 1000000; code++)
    {
        if (table[code] != NULL)
        {
            free(table[code]);
        }
    }
}

static inline void write_contents_to_file(FILE *file, char *buffer, unsigned int size)
{
    fwrite(buffer, sizeof(char) * size, 1, file);
}

int main(void)
{
    FILE *input_file = fopen("compressed.dat", "r");

    if (!input_file)
    {
        printf("Failed opening input file.\n");
        return 1;
    }

    FILE *output_file = fopen("decompressed.dat", "wb");

    if (!output_file)
    {
        printf("Failed opening output file.\n");
        return 1;
    }

    long int file_offset_four_bytes_codes;
    fread(&file_offset_four_bytes_codes, sizeof(long int), 1, input_file);

    // Read the first code (uint16_t) from the input file
    uint32_t old, new, current_code = 0;
    fread(&old, sizeof(uint16_t), 1, input_file);

    if (feof(input_file))
    {
        printf("File is empty.\n");
        return 1;
    }

    // Declare the table
    char *table[1000000] = {NULL}; // 0xFFFF = 2^16 = 65535 available codes

    /*
      1    Initialize table with single character strings
      2    OLD = first input code
      3    output translation of OLD
      4    WHILE not end of input stream
      5        NEW = next input code
      6        IF NEW is not in the string table
      7               S = translation of OLD
      8               S = S + C
      9       ELSE
      10              S = translation of NEW
      11       output translation of S
      12       C = first character of S
      13       OLD + C to the string table
      14       OLD = NEW
      15   END WHILE
    */

    // Initialize table with single character strings
    for (current_code = 0; current_code < 256; current_code++)
    {
        table[current_code] = (char *)malloc(sizeof(char) + 1);
        table[current_code][0] = (char)current_code;
        table[current_code][1] = '\0';
    }

    assert(old < 256);
    write_contents_to_file(output_file, table[old], 1); // output translation of OLD

    char c = table[old][0];
    char s[MAX_SEQUENCE_SIZE];

    while (!feof(input_file))
    {
        fread(&new, (ftell(input_file) < file_offset_four_bytes_codes) ? sizeof(uint16_t) : sizeof(uint32_t), 1, input_file);

        if (table[new] == NULL)
        {
            sprintf(s, "%s%c", table[old], table[old][0]); // S = translation of OLD + C
        }
        else
        {
            strcpy(s, table[new]); // S = translation of NEW
        }

        write_contents_to_file(output_file, s, strlen(s)); // output translation of S
        c = s[0];                                          // C = first character of S
        table[current_code] = (char *)malloc((sizeof(char) * strlen(table[old])) + 2);
        sprintf(table[current_code++], "%s%c", table[old], c); // OLD + C to the string table
        old = new;                                             // OLD = NEW
    }

    fclose(output_file);
    fclose(input_file);
    free_table(table);

    return 0;
}
