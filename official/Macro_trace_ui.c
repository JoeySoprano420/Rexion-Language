// DOC: Macro trace visualization and zip export system
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "macro_expander.h"
#include "tui.h"

void draw_macro_trace_banner(const char* macro_file) {
    tui_set_color(TUI_COLOR_CYAN);
    tui_print_center("============================");
    tui_print_center("⚡ MACRO TRACE ACTIVE");
    tui_print_center("📦 Loaded: macros.r4meta");
    tui_print_center("============================");
    tui_reset_color();
}

void show_macro_expansion_example() {
    printf("\nExpanded Macros:\n");
    for (int i = 0; i < macro_count; i++) {
        tui_set_color(TUI_COLOR_YELLOW);
        printf("\n|%s|\n", macros[i].name);
        tui_reset_color();
        printf("%s\n", macros[i].expansion);
    }
}

void export_macro_bundle(const char* output_zip) {
    char command[1024];
    snprintf(command, sizeof(command),
        "zip -r %s official/macros.r4meta icons/ docs/macro_README.md > /dev/null",
        output_zip);
    int result = system(command);
    if (result == 0) {
        printf("\n[EXPORT] Macro bundle exported to %s\n", output_zip);
    } else {
        fprintf(stderr, "[ERROR] Export failed.\n");
    }
}

void auto_watch_r4meta_and_reload(const char* macro_file) {
    time_t last_modified = 0;
    struct stat st;

    while (1) {
        if (stat(macro_file, &st) == 0 && st.st_mtime != last_modified) {
            last_modified = st.st_mtime;
            load_macros(macro_file);
            draw_macro_trace_banner(macro_file);
            printf("⚡ Macro reloaded from %s\n", macro_file);
            show_macro_expansion_example();
        }
        sleep(2);
    }
}
