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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "token_type.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <zip.h>

#define MAX_SYMBOLS 128
#define USE_PRINTF 0
#define USE_SYSCALL 1

#define MACRO_META_PATH "macros.r4meta"
#define MACRO_ZIP_EXPORT "macro_bundle.zip"

pthread_t macro_watcher_thread;
int macro_reload_flag = 0;

typedef struct {
    char name[32];
    char reg[4];
    int is_float;
} Symbol;

Symbol symtab[MAX_SYMBOLS];
int symbol_count = 0;

const char* allocate_register(const char* varname, int is_float) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symtab[i].name, varname) == 0)
            return symtab[i].reg;
    }
    snprintf(symtab[symbol_count].name, sizeof(symtab[symbol_count].name), "%s", varname);
    snprintf(symtab[symbol_count].reg, sizeof(symtab[symbol_count].reg), "R%d", symbol_count + 1);
    symtab[symbol_count].is_float = is_float;
    return symtab[symbol_count++].reg;
}

void emit_ir_header() {
    printf("[IR] section .code\n");
    printf("[IR] entry main\n");
}

void emit_ir_operation(const char* op, const char* arg1, const char* arg2) {
    if (arg2)
        printf("[IR] %s %s, %s\n", op, arg1, arg2);
    else
        printf("[IR] %s %s\n", op, arg1);
}

void generate_intermediate_code() {
    emit_ir_header();
    const char* r1 = allocate_register("x", 0);
    const char* r2 = allocate_register("y", 0);
    const char* r3 = allocate_register("result", 0);
    const char* fr1 = allocate_register("f1", 1);
    const char* fr2 = allocate_register("f2", 1);

    emit_ir_operation("LOAD", r1, "5");
    emit_ir_operation("LOAD", r2, "3");
    emit_ir_operation("ADD", r3, r1);
    emit_ir_operation("ADD", r3, r2);
    emit_ir_operation("STORE", "result", r3);
    emit_ir_operation("FLOAT_LOAD", fr1, "3.14");
    emit_ir_operation("FLOAT_LOAD", fr2, "2.71");
    emit_ir_operation("FLOAT_ADD", fr1, fr2);
    emit_ir_operation(USE_PRINTF ? "PRINT_FLOAT_PRINTF" : "PRINT_FLOAT_SYSCALL", fr1, NULL);
    emit_ir_operation("PRINT", "result", NULL);
    emit_ir_operation("HALT", NULL, NULL);
}

void generate_asm_from_ir(); // Forward declaration

void* watch_macros(void* arg) {
    int fd = inotify_init();
    if (fd < 0) perror("inotify_init");
    inotify_add_watch(fd, MACRO_META_PATH, IN_MODIFY);

    char buf[4096];
    while (1) {
        int length = read(fd, buf, sizeof(buf));
        if (length > 0) {
            macro_reload_flag = 1;
            printf("\n⚡ Macro reloaded!\n");
            // Reload and trigger IR regeneration or TUI pulse
            generate_intermediate_code();
            generate_asm_from_ir();
        }
        sleep(1);
    }
    close(fd);
    return NULL;
}

void export_macro_bundle() {
    int error;
    zip_t* archive = zip_open(MACRO_ZIP_EXPORT, ZIP_CREATE | ZIP_TRUNCATE, &error);
    if (!archive) {
        fprintf(stderr, "[ERROR] Cannot create ZIP bundle\n");
        return;
    }

    zip_file_add(archive, "macros.r4meta", zip_source_file(archive, MACRO_META_PATH, 0, 0), ZIP_FL_OVERWRITE);
    zip_file_add(archive, "README.md", zip_source_file(archive, "README.md", 0, 0), ZIP_FL_OVERWRITE);
    zip_file_add(archive, "icon.png", zip_source_file(archive, "icon.png", 0, 0), ZIP_FL_OVERWRITE);
    zip_file_add(archive, "macro_bundle.json", zip_source_file(archive, "macro_bundle.json", 0, 0), ZIP_FL_OVERWRITE);

    zip_close(archive);
    printf("📦 Macro bundle exported to %s\n", MACRO_ZIP_EXPORT);
}

void start_macro_watcher() {
    pthread_create(&macro_watcher_thread, NULL, watch_macros, NULL);
}

void generate_asm_from_ir() {
    FILE* f = fopen("rexion.asm", "w");
    if (!f) { perror("Failed to write ASM"); return; }

    fprintf(f,
        "section .data\n"
        "result dq 0\n"
        "buffer db 64 dup(0)\n"
        "fltval dq 3.14\n"
        "fltval2 dq 2.71\n"
        "fltstr db 64 dup(0)\n"
        "newline db 0xA, 0\n"
        "fmt db '%%f', 10, 0\n"
        "section .text\n"
        "extern printf\n"
        "global _start\n"
        "_start:\n"
        "    mov rax, 5\n"
        "    mov rbx, 3\n"
        "    add rcx, rax\n"
        "    add rcx, rbx\n"
        "    mov [result], rcx\n"
        "\n"
        "    fld qword [fltval]\n"
        "    fadd qword [fltval2]\n"
        "    fstp qword [fltstr]\n"
        "\n"
        "    %s\n"
        "\n"
        "    mov rdi, rcx\n"
        "    mov rsi, buffer\n"
        "    call int_to_str\n"
        "    mov rdx, rax\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, buffer\n"
        "    syscall\n"
        "    mov rax, 1\n"
        "    mov rdi, 1\n"
        "    mov rsi, newline\n"
        "    mov rdx, 1\n"
        "    syscall\n"
        "    mov eax, 60\n"
        "    xor edi, edi\n"
        "    syscall\n"
        "\n"
        "int_to_str:\n"
        "    mov rbx, 10\n"
        "    mov rax, rdi\n"
        "    xor rcx, rcx\n"
        "    add rsi, 63\n"
        "    mov byte [rsi], 0\n"
        "convert_loop:\n"
        "    xor rdx, rdx\n"
        "    div rbx\n"
        "    add dl, '0'\n"
        "    dec rsi\n"
        "    mov [rsi], dl\n"
        "    inc rcx\n"
        "    test rax, rax\n"
        "    jnz convert_loop\n"
        "    mov rax, rcx\n"
        "    ret\n"
        , USE_PRINTF ? "lea rdi, [rel fmt]\n    movq xmm0, [fltstr]\n    mov rax, 1\n    call printf" : "call float_to_str\n    mov rdx, rax\n    mov rax, 1\n    mov rdi, 1\n    mov rsi, buffer\n    syscall");

    fclose(f);
    printf("[ASM] rexion.asm generated from IR\n");
}
