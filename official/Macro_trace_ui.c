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

// Macro Trace UI – Real-time .r4meta visual macro reloads

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/inotify.h>

#define META_FILE "macros.r4meta"
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define EVENT_BUF_LEN     (1024 * (EVENT_SIZE + 16))

volatile int macro_reload_flag = 0;

void* watch_macros(void* arg) {
    int fd = inotify_init();
    if (fd < 0) { perror("inotify_init"); exit(1); }

    int wd = inotify_add_watch(fd, META_FILE, IN_MODIFY);
    if (wd == -1) { perror("inotify_add_watch"); exit(1); }

    char buffer[EVENT_BUF_LEN];
    while (1) {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0) { perror("read"); break; }

        macro_reload_flag = 1;
        printf("\033[1;35m⚡ Macro reloaded! [%s]\033[0m\n", META_FILE);
    }
    inotify_rm_watch(fd, wd);
    close(fd);
    return NULL;
}

void start_macro_watcher() {
    pthread_t thread;
    pthread_create(&thread, NULL, watch_macros, NULL);
    pthread_detach(thread);
}

void tui_macro_banner() {
    if (macro_reload_flag) {
        printf("\033[1;34m[MacroTUI]\033[0m \033[1;33mLive macro reload detected!\033[0m\n");
        macro_reload_flag = 0;
    }
}

// Export macro bundle as .zip
void export_macro_bundle(const char* outpath) {
    printf("\033[1;36m[Export]\033[0m Bundling macros to: %s\n", outpath);
    system("mkdir -p macro_export/docs && cp macros.r4meta macro_export/ && cp README.md macro_export/docs/ && cp icon.png macro_export/");
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "zip -r %s macro_export/ > /dev/null", outpath);
    system(cmd);
    printf("\033[1;32m[✓]\033[0m Export complete.\n");
}

