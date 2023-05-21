# SysY_Compiler
2023春夏编译原理课程大作业

## 可能会用到的指令

### 0	编译/运行 RISC-V程序

```bash
clang hello.S -c -o hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
ld.lld hello.o -L$CDE_LIBRARY_PATH/riscv32 -lsysy -o hello
qemu-riscv32-static hello
```

### 0	后端的结果输出到文件中

在`back_main.cpp`中，将`#define cout fout`取消注释，即可将结果输出到`output[]`对应的文件中

### 1	运行项目

```bash
make
/root/SysY_Compiler/compiler/build/compiler -koopa task.c -o task.koopa
/root/SysY_Compiler/compiler/build/compiler -riscv task.c -o task.S
```

### 2	自动测试

#### 2.1	第1章

```bash
autotest -koopa -s lv1 /root/SysY_Compiler/compiler/
```

#### 2.2	第2章

```bash
autotest -riscv -s lv1 /root/SysY_Compiler/compiler/

```
