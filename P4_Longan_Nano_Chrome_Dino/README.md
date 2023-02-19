

# 开发流程（方便debug）

1. 按照[这篇文档](./supplemental_material_for_project4/README.md)将图片转为RGB565，存入`src/img.h`中（原图都在`supplemental_material_for_project4/image`中）
2. **假定图片转化成功**:beetle:。自顶向下实现游戏
	- 将各个类的静态变量做成全局变量
	- 按照`GamePanel`类搭建框架，首先是`run`方法
	- 用结构体加上函数实现`Dino`类
	- 将`GamePanel`类的普通属性做成全局变量——该类只会有一个实例对象
	- 将`Cactus`类和`Pterosaur`统一为`Obstacle`结构体——在Java源码中我没有这样写，**故可能有错**:beetle:
	- 设计存放`Obstacle`的容器（一个数组即可）:beetle:，写一大堆的`paint`函数
		- 按照[演示视频](./甲方需求.mp4)，背景是黑色的，所以游戏上的内容需要是白色——这和Java写的正好相反
	- 完成与碰撞相关的函数（`dino`和`obstacle`碰撞）



# 要求

- 最终实现的效果应该与这个[视频](./甲方需求.mp4)所演示的内容相近

## 具体要求——形式

- 在`src/assembly`文件夹下，至少包含3个`.S`文件。
- `src/assembly`文件夹下不允许出现其他类型的文件
- 40%的汇编代码需要注释
- 汇编代码至少300行

## 具体要求——内容

- 需要有开始界面
- 需要记录分数
- 小恐龙需要有两种行为
- 有两种障碍物





# 情報

1. 这几个文件中有实现项目需要的函数：

	- > Please read `src/utils.c`, `src/assembly/example.S`, `src/systick.c` and `src/lcd/lcd.c` carefully. They contain helper functions and/or information that may not be mentioned in this webpage.

2. [Notes on Lab11 and Project4](https://piazza.com/class/ky06dwortmv48g?cid=308)

	  - [这里](./supplemental_material_for_project4/)提供了项目所需要的图片、将图片转换为`RGB565`的方法——使用方法读[这个](./supplemental_material_for_project4/README.md) :star:
	  
	    - > `img.h` contains 11 byte arrays, correspond to the images in `image` folder. You can include this header file in your project and pass the array to `LCD_ShowPicture()`.
	  
	    - 可以用c语言的方式快速地调用图片，而不是插tf卡


3. 汇编是累赘？——https://piazza.com/class/ky06dwortmv48g?cid=315

	- > You only need to write 300 lines of RISC-V. Everything else can be implemented in C

	- 在`example.S`中有这样一句话：

	- >  The above lines declares a function that can be accessed from C code.

	- 汇编函数的作用是供C代码使用，以**应付要求**？——大概吧

4. 屏幕参数

	- Size: 160x80
	- Top-left point is (0, 0), while bottom-right is (159, 79)——和Java绘图坐标系一致

# 提交

向Autolab提交`autolab.txt`



# 感想

- 2022年5月25日13:35:54，by chenyh1：OOP思想真是编程利器，这和编程语言无关
