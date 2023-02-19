# 总结
1. Work on the projects as early as possible. THE EARLIER THE BETTER!
2. Write codes together with **comments**, otherwise, it is extremely hard for others to recognize what they are.
3. Encapsulation is important(programming in OOP): Be careful with details! Don't debug at the last gasp, try to do it when some basic blocks are fixed.
4. Cooperate more closely though standardization and functions interface.
5. Be careful with str functions and memory management, also, strange C89.
6. For this project, it is better to first convert the string to the numbers. It should be much easier.
7. It is better to firstly understand the mechanism of the whole project before coding, which helps avoid some bugs. (for example, sign-extended imm, zero-extended imm, other characteristics of RVC and so on)
8. A utils kit helps a lot, but programmers need to make sure the function in it works well.
9. Debugging in C is annoying.

# 下一步要做的事
- 开始做project1.2


# 分析需求

[p1.1网页](https://robotics.shanghaitech.edu.cn/courses/ca/22s/projects/1.1/)

输入：32位的RISC - V指令编码文件

输出：输入文件压缩后产生的文件，该文件包含32位和16位指令编码。

# 设计思路 / 程序架构

1. 如[p1.1网页](https://robotics.shanghaitech.edu.cn/courses/ca/22s/projects/1.1/)所述，可以被压缩的指令是有限的。故只需对输入的指令进行有限次的验证，就可决定该指令是否能被压缩。验证函数（可能有多个）应该在`compression.c`中完成
2. 验证完成后，应使用函数进行实际的压缩。这些函数也定义在`compression.c`中
3. `utils.c`文件负责I/0文件流和字符串的互相转换。
4. 尽可能少的在`translator.c`中添加代码。具体来说，`translate`函数应该使用`utils.c`中定义的函数，而不是自己处理文件I/0流
7. 对于`jal`指令，由于有些指令被压缩为16位，移动的距离可能变小。故可以采用如下结构：
	1. 每处理一条指令，先不写入文件，而是存入字符串数组，即`char**`（大小为50——每个文件指令数上限）。
	2. 标记使用`jal`指令的行数，可以用队列（自己定义`Queue`类）存储
	3. 为了提高速度，可以用前缀和数组记录指令大小，这样对于每条`jal`指令，只需要`O(1)`时间算出新的移动距离
8. 项目的难度不高，工作主要是匹配指令类型，故重点在于代码模式设计得是否合理，写得是否好看



# 技术文档

- [C文件读写](https://www.runoob.com/cprogramming/c-file-io.html)
- [c语言中全局变量在多个文件中使用](https://www.cnblogs.com/ys77/p/11541583.html)
- [C语言fgets和fputs函数的用法详解（以字符串的形式读写文件）](http://c.biancheng.net/view/2070.html)
- [strcat函数](http://www.cplusplus.com/reference/cstring/strcat/?kw=strcat)
- [十进制二进制(2's complement)互相转换](https://www.exploringbinary.com/twos-complement-converter/)，内容在该网页的最下面——它本来是个在线转换器







