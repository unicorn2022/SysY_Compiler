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

