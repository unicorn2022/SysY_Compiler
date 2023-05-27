# SysY_Compiler

2023春夏编译原理课程大作业

## 一、使用方法

### 1.1	编译项目

```bash
make
```

### 1.2	运行项目，生成koopa/riscv文件

```bash
make koopa
make riscv
```

生成的文件在：`SysY_Compiler/compiler/test/`中

### 1.3	运行RISCV文件

```bash
make run
```

### 1.4	自动测试

```bash
make test-riscv LEVEL=6
make test-koopa LEVEL=4
```

`LEVEL`为测试的章节数

### 1.5 Task 测试

首先，在 `compiler/test` 目录下建立 `taski` 文件夹（i 为 task 序号）。以 task1 为例，其目录应为：

```
test/task1
|-- 1.in
|-- 2.in
|-- 3.in
|-- 4.in
`-- task1.c
```

目录中必须包含 `taski.c` 文件，即将会被编译器编译成可执行文件的源代码。目录中还需要包含任意数量的输入文件 `file.in`，后缀必须为 `.in`，文件名不限

在 `compiler` 目录下，运行以下命令以进行 Task 测试：

```bash
make task TASK=1
```

其中，`TASK` 是指定的 task 序号。

### 1.6 Tester

在 `compiler` 目录下使用以下命令运行所有 tester（以 linux-amd64 架构为例）：

```bash
make task TASK=1
../tester/quicksort/quicksort-linux-amd64 ./task.sh
make task TASK=2
../tester/matrix/matrix-linux-amd64 ./task.sh
make task TASK=3
../tester/advisor/advisor-linux-amd64 ./task.sh
```

**注意：task.sh的格式必须手动改为LF**

# 二、调试代码

RISCV输出字符`a\n`

```assembly
debug_start:
	addi s0, ra, 0
	li   a0, 97
	call putch
	li   a0, 10
	call putch
	addi ra, s0, 0
	li   a0, 0
	ret
debug_end:
```

