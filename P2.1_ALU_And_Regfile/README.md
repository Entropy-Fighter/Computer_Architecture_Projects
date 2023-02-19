# 最新一次运行`test.sh`的结果

- ```
	Testing files...
	        PASSED test: ALU add test
	        PASSED test: ALU mult and mulhu test
	        PASSED test: ALU div and rem test
	        PASSED test: ALU slt and sub test
	        FAILED test: ALU mulh test (Did not match expected output)
	        FAILED test: ALU comprehensive test (Did not match expected output)
	        FAILED test: ALU comprehensive test 2 (Did not match expected output)
	        PASSED test: ALU sir test
	        PASSED test: ALU cnto test
	        PASSED test: RegFile insert test
	        PASSED test: RegFile zero test
	        PASSED test: RegFile x0 test
	        PASSED test: RegFile all registers test
	Passed 10/13 tests
	
	```



# 自定义的一些子电路

## count_1s_32bits
- 一个32位的二进制数字中有几个1
- 采用递归的方式实现

## N2P

- 将一个负数转化为其绝对值——一个正数（变为对应的2's complement的表示）
- 注意，只能输入负数

## P2N

- 给一个正数前面添上负号（变为对应的2's complement的表示）
- 注意，只能输入正数



## negatAllBit

- 把一个32位的二进制数字中所有的1变为0，所有0变为1

## MathmeticalMultiplication
- 一个数学意义上的乘法
- 输入：两个32位二进制数字
- 输出：
	1. 乘法结果的低32位——一个非负数
	2. 乘法结果的高31位——一个非负数
	3. 结果是否为正数

## SignedDivision

- 对于`9 div`的封装

## signedMulHighBits

- 对于`7 mulh`的封装



# 技术文档

- [divider](http://www.cburch.com/logisim/docs/2.1.0/libs/arith/divider.html)



# 其他

2. We only grade code on the `master` branch.
3. Changes made out of `alu.circ` and `regfile.circ` will NOT be considered by autograder.
