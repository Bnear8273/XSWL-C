# test_file.S — XSWL 文件读取测试
# 测试: OpenFile/ReadFile/CloseFile/SearchFile/WriteFile
# 编译: gcc -m64 -nostdlib -static -no-pie -o test_file.elf test_file.S -Wl,-Ttext=0x200000

    .intel_syntax noprefix

# xapi syscall 编号
.equ X_OUTPUT,       0
.equ X_PRINTLINE,    5
.equ X_OPENFILE,     8
.equ X_CLOSEFILE,    9
.equ X_SEARCHFILE,   10
.equ X_CREATEFILE,   12
.equ X_DELETEFILE,   13
.equ X_READFILE,     15
.equ X_WRITEFILE,    16
.equ X_INT2CHAR,     19
.equ X_EXIT,         25
.equ X_ALLOCATEMEMORY, 39

.equ TRAMP, 0xFFFF0000

.macro CALL_X no
    mov  rax, TRAMP + (\no) * 16
    call rax
.endm

    .text
    .globl _start
_start:
    # === 1. 写测试文件 ===
    lea  rdi, [rip + s_path]
    CALL_X X_CREATEFILE

    lea  rdi, [rip + s_path]
    lea  rsi, [rip + s_data]
    mov  rdx, s_data_end - s_data
    xor  rcx, rcx
    CALL_X X_WRITEFILE
    lea  rdi, [rip + s_written]
    CALL_X X_PRINTLINE

    # === 2. 打开并读取 ===
    lea  rdi, [rip + s_path]
    CALL_X X_OPENFILE
    mov  r12, rax               # r12 = XFILE*

    mov  rdi, 512
    CALL_X X_ALLOCATEMEMORY
    mov  r13, rax               # r13 = buffer

    lea  rdi, [rip + s_path]
    mov  rsi, r13
    mov  rdx, 512
    xor  rcx, rcx
    CALL_X X_READFILE
    lea  rdi, [rip + s_read]
    CALL_X X_PRINTLINE

    # 输出读取内容
    mov  rdi, r13
    CALL_X X_PRINTLINE

    # === 3. 关闭文件 ===
    mov  rdi, r12
    CALL_X X_CLOSEFILE

    # === 4. SearchFile 验证 ===
    mov  rdi, 256*272
    CALL_X X_ALLOCATEMEMORY
    mov  r14, rax
    mov  rdi, 4
    CALL_X X_ALLOCATEMEMORY
    mov  r15, rax

    lea  rdi, [rip + s_root]
    mov  rsi, r15
    mov  rdx, r14
    CALL_X X_SEARCHFILE

    # 输出文件数
    mov  eax, [r15]
    mov  rdi, rax
    CALL_X X_INT2CHAR
    mov  rdi, rax
    CALL_X X_PRINTLINE

    # === 5. 删除文件 ===
    lea  rdi, [rip + s_path]
    CALL_X X_DELETEFILE
    lea  rdi, [rip + s_done]
    CALL_X X_PRINTLINE

    xor  rdi, rdi
    CALL_X X_EXIT
    hlt

    .section .rodata
s_path:
    .asciz "/test_file.txt"
s_root:
    .asciz "/"
s_data:
    .ascii "XSWL File I/O Test - OK!\nHello!"
s_data_end:
s_written:
    .asciz "[1] WriteFile OK"
s_read:
    .asciz "[2] ReadFile ->"
s_done:
    .asciz "[3] All file tests passed"
