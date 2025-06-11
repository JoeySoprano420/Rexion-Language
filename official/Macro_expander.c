// DOC: Expands .r4 macros into .rexasm blocks using .r4meta data
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h" // your JSON loader
#include "utils.h"

#define MAX_MACROS 256

typedef struct {
    char name[64];
    char expansion[1024];
} Macro;

Macro macros[MAX_MACROS];
int macro_count = 0;

void load_macros(const char* meta_file) {
    FILE* f = fopen(meta_file, "r");
    if (!f) { perror("Cannot open .r4meta"); exit(1); }

    json_object* root = json_parse_file(meta_file);
    json_object* items = json_object_object_get(root, "macros");
    macro_count = json_object_array_length(items);

    for (int i = 0; i < macro_count; i++) {
        json_object* entry = json_object_array_get_idx(items, i);
        strcpy(macros[i].name, json_object_get_string(json_object_object_get(entry, "name")));
        strcpy(macros[i].expansion, json_object_get_string(json_object_object_get(entry, "expansion")));
    }

    fclose(f);
    printf("Loaded %d macros from %s\n", macro_count, meta_file);
}

const char* expand_macro(const char* name) {
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0)
            return macros[i].expansion;
    }
    return NULL;
}

void rewrite_r4_to_rexasm(const char* r4_file, const char* output_file) {
    FILE* in = fopen(r4_file, "r");
    FILE* out = fopen(output_file, "w");
    if (!in || !out) { perror("Failed to rewrite"); exit(1); }

    char line[1024];
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '|') {
            char macro_name[64];
            sscanf(line, "|%[^|]|", macro_name);
            const char* exp = expand_macro(macro_name);
            if (exp) {
                fprintf(out, ";; [Macro: %s]\n%s\n", macro_name, exp);
            } else {
                fprintf(out, ";; [Unknown macro: %s]\n", macro_name);
            }
        } else {
            fputs(line, out);
        }
    }

    fclose(in);
    fclose(out);
    printf("Expanded .r4 macros to .rexasm at %s\n", output_file);
}
