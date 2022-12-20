// gcc -O1 decompress.c -o decompress
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "constants.h"

void free_table(char *table[], uint32_t total_codes)
{
    for (uint32_t code = 0; code < total_codes; code++)
    {
        if (table[code] != NULL)
        {
            free(table[code]);
        }
    }
    free(table);
}

void get_current_code_size(size_t *code_size, uint32_t *new, FILE *input_file, long int file_offset_24bit_codes, long int file_offset_32bit_codes)
{
    if ((ftell(input_file) < file_offset_24bit_codes) || (file_offset_24bit_codes == 0))
    {
        *new = 0;
        *code_size = 2;
    }
    else if ((ftell(input_file) < file_offset_32bit_codes) || (file_offset_32bit_codes == 0))
    {
        *new = 0;
        *code_size = 3;
    }
    else
    {
        *code_size = 4;
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

    uint32_t total_codes;
    long int file_offset_24bit_codes, file_offset_32bit_codes;
    fread(&file_offset_24bit_codes, sizeof(long int), 1, input_file);
    fread(&file_offset_32bit_codes, sizeof(long int), 1, input_file);
    fread(&total_codes, sizeof(uint32_t), 1, input_file);

    // Read the first code (uint16_t) from the input file
    uint32_t old = 0, new = 0, current_code = 0;
    fread(&old, sizeof(uint16_t), 1, input_file);

    if (feof(input_file))
    {
        printf("File is empty.\n");
        return 1;
    }

    // Declare the table
    char **table = (char **)calloc(total_codes, sizeof(char *)); // Initializes all allocated char pointers to 0 (NULL)
    if (table == NULL)
    {
        printf("Can't allocate memory.");
        return 1;
    }

    /*
      1    Initialize table with single character strings
      2    OLD = first input code
      3    output translation of OLD
      4    WHILE not end of input stream
      5        NEW = next input code
      6        IF NEW is not in the string table
      7            S = translation of OLD + C
      8        ELSE
      9            S = translation of NEW
      10       output of S
      11       C = first character of S
      12       translation of OLD + C to the string table
      13       OLD = NEW
      14   END WHILE
    */

    // Initialize table with single character strings
    for (current_code = 0; current_code < 256; current_code++)
    {
        table[current_code] = (char *)malloc(sizeof(char) + 1);
        if (table[current_code] == NULL)
        {
            printf("Can't allocate memory.");
            return 1;
        }
        table[current_code][0] = (char)current_code;
        table[current_code][1] = '\0';
    }

    assert(old < 256);
    write_contents_to_file(output_file, table[old], 1); // output translation of OLD

    char c = '\n';
    char s[MAX_SEQUENCE_SIZE];
    size_t current_code_size;

    while (!feof(input_file))
    {
        get_current_code_size(&current_code_size, &new, input_file, file_offset_24bit_codes, file_offset_32bit_codes);
        fread(&new, current_code_size, 1, input_file); // NEW = next input code

        if (table[new] == NULL) // IF NEW is not in the string table
        {
            sprintf(s, "%s%c", table[old], c); // S = translation of OLD + C
        }
        else
        {
            strcpy(s, table[new]); // S = translation of NEW
        }

        c = s[0];                                          // C = first character of S
        if (c == EOF) break;

        write_contents_to_file(output_file, s, strlen(s)); // output of S
        table[current_code] = (char *)malloc((sizeof(char) * strlen(table[old])) + 2);
        if (table[current_code] == NULL)
        {
            printf("Can't allocate memory.");
            return 1;
        }
        sprintf(table[current_code++], "%s%c", table[old], c); // translation of OLD + C to the string table
        old = new;                                             // OLD = NEW
    }

    fclose(output_file);
    fclose(input_file);
    free_table(table, total_codes);

    return 0;
}
