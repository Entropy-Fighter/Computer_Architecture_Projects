# 下一步要做的事情
- debug!
- debug!!
- debug!!!
- debug!!!!

# 需求

- 输入：RVC二进制指令集（16位和32位指令混杂）

- 输出： RISC-V 32位二进制指令集

# 程序分析/架构

1. 输入的RVC集合（一堆二进制码，起始地址为`rvc_codes`）在内存中是连续的，不知道哪条指令是16位，哪条指令是32位。

	- 对于地址`add`，使用`lh`(load halfword)，判断其是不是16位指令（存储在寄存器的低/后16位中），如果是，处理该指令。如果不是，使用`lw`(load word)，看这个32位指令是不是SB或UJ类型，如果是，看第5点，如果不是，不需要处理。
	- 如果当前指令为16位，下一次判断`add + 2`；如果当前指令为32位，下一次判断`add + 4`
2. 所有指令处理完后，统一输出。故需要数组存储处理好的指令
3. 在**寄存器———变量对照**中，写明各个函数使用到的每个寄存器用于什么目的，以方便互相读代码和debug
4. 匹配指令可以直接用逻辑运算符`&`完成——因为它们本质上是一个数值
5. 对于SB和UJ类型，不管输入是16位还是32位，都需要重新计算`imm`。还是像p1.1中那样：
	- 定义`relocate()`方法
	- 记录哪些指令需要重写计算`imm`和原先`imm`移动的距离
6. 开发工具类，包含以下函数：

	  - 对于一个32位整数，打印其对应的二进制数——用于输出






# 细节相关

1. 注意sign-extended问题——压缩规则和使用RISC-V编程，两者都有这个问题

2. 为了使用VS Code中的RISC-V Venus Simulator插件进行debug(而不是使用网页版Venus。因为在VS Code中debug体验好**很多**)，**不使用**`ecall`来申请堆空间（因为插件不兼容），而是**直接在栈中存储**（理论上，只要操作合理，栈中的值也不会被修改）。

1. 关于label和缩进问题。如果label用于循环，则其中的代码块应该缩进。如果label用于`goto`，之后的代码不用缩进

1. 类似于`judgeCADD`的label，其本身在代码中没有任何作用，只是单纯的标记，以便使用`Ctrl + F`查找该模块。故这种label顶格写（没有缩进）

5. **切记**，使用索引值`i`获取RISC-V数组`arr`中的第`i - 1`个值时，该值的地址是`arr + 4 * i`，不是`arr + i`。即，使用以下代码才是正确的：

	- ```assembly
		mv t3, s2
		slli t3, t3, 2  
		add t3, a4, t3  # t3 = answer + s2
		sw t2, 0(t3)    # answer[s2] = t2
		```

2. [piazza上p1.2讨论帖](https://piazza.com/class/ky06dwortmv48g?cid=140)

	- 不会检查内存泄露问题。为了省事，可以不写`free`函数/功能

2. [p1.2网页](https://robotics.shanghaitech.edu.cn/courses/ca/22s/projects/1.2/) **Submission** 一栏

	- > You need to have **meaningful** comments not less than 25% of the total lines of code you wrote.

	- 不能写**空**注释或者**伪**注释

	- Autolab测评时只使用`translator.S`文件，还是提交`autolab.txt`文件



# 寄存器———变量对照

## 全局变量

- | 寄存器 | 用途                                                         |
  | ------ | ------------------------------------------------------------ |
  | `a2`   | 指令总数                                                     |
  | `a3`   | 输入的指令集的首地址                                         |
  | `a4`   | `answer[a2 + 1]`处理好的指令集的首地址。索引0不用            |
  | `a5`   | `originalLength[a2 + 1]`的首地址。索引0不用                  |
  | `s4`   | 记录使用`jal`和branch的指令的数量                            |
  | `s5`   | p1.1中的`jumpAt[]`，即使用`jal`和branch的指令的位置组成的数组的首地址。索引0不用 |
  | `s6`   | p1.1中的`jumpDistance[]`，即使用`jal`和branch的指令移动的行数组成的数组的首地址。注意，这里面存放的值的单位为bit，即`imm * 2 * 8 `，之后需要人为根据`prefixSum[]`算出到底移动了多少行——和p1.1相反。索引0不用 |
  | `s7`   | p1.1中的`jumpType[]`，存入`0`表示SB类型，存入`1`表示UJ类型。索引0不用 |
  |        |                                                              |
  |        |                                                              |
  |        |                                                              |
  |        |                                                              |



## mainLoop

| 寄存器 | 用途                                                |
| ------ | --------------------------------------------------- |
| `s2`   | $s2 \in [1 ,\, a2]$，控制指令的迭代                 |
| `s3`   | 当前指令的地址，下一个指令为`s3 + 2`或`s3 + 4`      |
| `t1`   | 当前指令的内容。其值不能被`lhu`和`lw`以外的指令改变 |





## reolcate

| 寄存器 | 用途                                |
| ------ | ----------------------------------- |
| `s2`   | $s2 \in [1 ,\, s4)$，控制指令的迭代 |
|        |                                     |
|        |                                     |





## decompressXXX

| 寄存器 | 用途         |
| ------ | ------------ |
| `t2`   | 处理好的指令 |
|        |              |
|        |              |



# 技术文档

- [Venus wiki on GitHub](https://github.com/ThaumicMekanism/venus/wiki/Venus's-optimized-clib)
- [Dec <---> Bin(two's complement)在线转换以及转换规则](https://www.exploringbinary.com/twos-complement-converter/)
- 





# 函数原型

- **建议**除了`main`以外的函数都在该原型的基础上修改：
- `functionPrototype.S`文件内容如下：

```assembly
function:
    addi sp, sp, -32 #reserve 8 regs
    #below regs can be treated as temp vars
	sw ra, 0(sp)# return address reserved
	sw s1, 4(sp)
	sw s2, 8(sp)
	sw s3, 12(sp)
	sw s4, 16(sp)
	sw s5, 20(sp)
	sw s6, 24(sp)
	sw s7, 28(sp)
    #above regs can bre treated as temp vars
    #s1 - s7 can be used freely
##############################################################################
在这里写函数的实现


############################################################################
    #restore saved regs
    lw ra,0(sp)#return address restored
	lw s1,4(sp)
	lw s2,8(sp)
	lw s3,12(sp)
	lw s4,16(sp)
	lw s5,20(sp)
	lw s6,24(sp)
	lw s7,28(sp)
	addi sp,sp,32

    jr ra
#end of function
```



# 备注

1. 2022年3月22日07:50:39，by chenyh1：我不小心在本地创建了`main`分支，并push上去了。但显示我没有权限删除这个分支，希望没事
2. 2022年3月26日17:04:46，by chenyh1：感叹，为什么RISC-V没有代码格式化工具？


ans[s10]是要处理的指令的位置，s11是要填入的imm，temp registers可以随便用
