# SysY_Compiler
2023春夏编译原理课程大作业

## 可能会用到的指令

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
