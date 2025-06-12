// peephole_optimizer.c – Rexion Peephole Optimizer
#include <stdio.h>
#include <string.h>

#define MAX_IR 1024
#define MAX_LEN 128

typedef struct {
    char op[MAX_LEN];
    char arg1[MAX_LEN];
    char arg2[MAX_LEN];
} IRInstruction;

IRInstruction ir[MAX_IR];
int ir_count = 0;

void load_ir_from_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) { perror("load_ir_from_file"); return; }
    while (fscanf(f, "%s %s %s", ir[ir_count].op, ir[ir_count].arg1, ir[ir_count].arg2) != EOF) {
        ir_count++;
    }
    fclose(f);
}

void save_ir_to_file(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) { perror("save_ir_to_file"); return; }
    for (int i = 0; i < ir_count; i++) {
        fprintf(f, "%s %s %s\n", ir[i].op, ir[i].arg1, ir[i].arg2);
    }
    fclose(f);
}

// Optimization Passes

void optimize_redundant_loads() {
    for (int i = 1; i < ir_count; i++) {
        if (strcmp(ir[i].op, "LOAD") == 0 && strcmp(ir[i-1].op, "LOAD") == 0) {
            if (strcmp(ir[i].arg1, ir[i-1].arg1) == 0 && strcmp(ir[i].arg2, ir[i-1].arg2) == 0) {
                for (int j = i; j < ir_count - 1; j++) ir[j] = ir[j+1];
                ir_count--;
                i--;
            }
        }
    }
}

void optimize_useless_add_zero() {
    for (int i = 0; i < ir_count; i++) {
        if (strcmp(ir[i].op, "ADD") == 0 && strcmp(ir[i].arg2, "0") == 0) {
            strcpy(ir[i].op, "NOP");
            ir[i].arg1[0] = '\0';
            ir[i].arg2[0] = '\0';
        }
    }
}

void optimize_mov_to_same_register() {
    for (int i = 0; i < ir_count; i++) {
        if (strcmp(ir[i].op, "MOV") == 0 && strcmp(ir[i].arg1, ir[i].arg2) == 0) {
            strcpy(ir[i].op, "NOP");
            ir[i].arg1[0] = '\0';
            ir[i].arg2[0] = '\0';
        }
    }
}

void fold_constant_adds() {
    for (int i = 0; i < ir_count - 2; i++) {
        if (strcmp(ir[i].op, "LOAD") == 0 && strcmp(ir[i+1].op, "LOAD") == 0 && strcmp(ir[i+2].op, "ADD") == 0) {
            int val1, val2;
            if (sscanf(ir[i].arg2, "%d", &val1) == 1 &&
                sscanf(ir[i+1].arg2, "%d", &val2) == 1 &&
                strcmp(ir[i].arg1, ir[i+2].arg1) != 0 &&
                strcmp(ir[i+1].arg1, ir[i+2].arg1) != 0) {

                int result = val1 + val2;
                snprintf(ir[i].arg2, MAX_LEN, "%d", result);
                strcpy(ir[i].arg1, ir[i+2].arg1);
                for (int j = i+1; j < ir_count - 2; j++) ir[j] = ir[j+2];
                ir_count -= 2;
                i--;
            }
        }
    }
}

void run_all_peephole_passes() {
    optimize_redundant_loads();
    optimize_useless_add_zero();
    optimize_mov_to_same_register();
    fold_constant_adds();
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.ir> <output.ir>\n", argv[0]);
        return 1;
    }

    load_ir_from_file(argv[1]);
    run_all_peephole_passes();
    save_ir_to_file(argv[2]);

    printf("[✔] Peephole optimization complete. Output saved to %s\n", argv[2]);
    return 0;
}
