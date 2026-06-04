/*
 * main.c — XJ380 模拟器 CLI 入口
 *
 * 用法: ./xj380_emu <ELF文件>
 */

#include "xj380_emu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char **argv)
{
    bool debug_enabled = true;
    const char *elf_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "nodebug") == 0 || strcmp(argv[i], "--nodebug") == 0) {
            debug_enabled = false;
        } else if (!elf_path) {
            elf_path = argv[i];
        }
    }

    if (!elf_path) {
        fprintf(stderr, "用法: %s [nodebug|--nodebug] <XJ380 ELF 文件>\n", argv[0]);
        return 1;
    }

    xj380_emu_t *emu = xj380_create();
    if (!emu) {
        fprintf(stderr, "创建模拟器失败\n");
        return 1;
    }

    xj380_set_debug(emu, debug_enabled);

    if (xj380_load_elf(emu, elf_path) != 0) {
        fprintf(stderr, "加载失败: %s\n", xj380_strerror(emu));
        xj380_destroy(emu);
        return 1;
    }

    char *guest_argv[] = {(char *)elf_path, NULL};
    int ret = xj380_run(emu, 1, guest_argv);
    if (ret != 0) {
        fprintf(stderr, "运行错误: %s\n", xj380_strerror(emu));
    }

    xj380_destroy(emu);
    return ret;
}
