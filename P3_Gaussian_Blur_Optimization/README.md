

# 一些理念 ideas

- 不要沉浸在对多线程的拜物教中，如果有神，那也只存在于**缓存**和**寄存器**中——这两个存储单元的速度是最快的



# 代码的注释

- `gb_h`和`gb_v`只在一行代码上有区别，如果两者的实现没有太大的不同，完善`gb_h`的代码后复制到`gb_v`即可

# 优化技术

- 对各技术网站/文档的总结在[这里](./技术文档/摘要.md)







## 对于项目介绍的总结

- **不能**使用更优的算法:x:
- **不能**使用编译参数优化:x:
- 可以使用多线程，使用`pthread`或`openmp`:accept:
- 可以使用SIMD指令:accept:
- 可以使用loop unrolling:accept:，推荐与SIMD一起使用
- 可以使用cache blocking:accept:

## 技术网站 / 文档

- [Oracle OpenMP Performance Considerations](https://docs.oracle.com/cd/E19205-01/819-5270/6n7c71vec/index.html)：一些OpenMP优化的指导思想，里面提到了一些缓存机制:thumbsup:

- [How to Get Good Performance by Using OpenMP!](http://akira.ruc.dk/~keld/teaching/IPDC_f10/Slides/pdf4x/4_Performance.4x.pdf)：这篇PDF覆盖了大部分优化技术。很实用:laughing:，于是我把它上传到repo里了，点[这里](./技术文档/How to Get Good Performance by Using OpenMP!.pdf)直接查看

- [IIR Gaussian Blur Filter Implementation using Intel Advanced Vector Extensions](./技术文档/IIR Gaussian Blur Filter Implementation using Intel Advanced Vector Extensions.pdf)：如题，这好像很有用:drooling_face:

- Advanced Threading and Optimization，这个PDF也不错，就是有点长。点[这里](./技术文档/Advanced Threading and Optimization.pdf)直接查看

- [INTEL SIMD instrinsics](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)，建议优先用下面的来查：

	- [Load Intrinsics](https://www.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/intrinsics/intrinsics-for-sse/load-intrinsics-2.html)
	- [Arithmetic Intrinsics](https://www.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/intrinsics/intrinsics-for-sse/arithmetic-intrinsics-2.html)
	- [Store Intrinsics](https://www.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/intrinsics/intrinsics-for-sse/store-intrinsics-2.html)

- [openMP C语言教程](https://curc.readthedocs.io/en/latest/programming/OpenMP-C.html#)

- https://stackoverflow.com/questions/16737298/what-is-the-fastest-way-to-transpose-a-matrix-in-c：一个回答实现了快速的矩阵转置，代码很有参考意义

  - ```c
  	inline void transpose4x4_SSE(float *A, float *B, const int lda, const int ldb) {
  	    __m128 row1 = _mm_load_ps(&A[0 * lda]);
  	    __m128 row2 = _mm_load_ps(&A[1 * lda]);
  	    __m128 row3 = _mm_load_ps(&A[2 * lda]);
  	    __m128 row4 = _mm_load_ps(&A[3 * lda]);
  	    _MM_TRANSPOSE4_PS(row1, row2, row3, row4);
  	    _mm_store_ps(&B[0 * ldb], row1);
  	    _mm_store_ps(&B[1 * ldb], row2);
  	    _mm_store_ps(&B[2 * ldb], row3);
  	    _mm_store_ps(&B[3 * ldb], row4);
  	}
  	
  	inline void transpose_block_SSE4x4(float *A, float *B, const int n, const int m, const int lda, const int ldb, const int block_size) {
  	#pragma omp parallel for
  	    for (int i = 0; i < n; i += block_size) {
  	        for (int j = 0; j < m; j += block_size) {
  	            int max_i2 = i + block_size < n ? i + block_size : n;
  	            int max_j2 = j + block_size < m ? j + block_size : m;
  	            for (int i2 = i; i2 < max_i2; i2 += 4) {
  	                for (int j2 = j; j2 < max_j2; j2 += 4) {
  	                    transpose4x4_SSE(&A[i2 * lda + j2], &B[j2 * ldb + i2], lda, ldb);
  	                }
  	            }
  	        }
  	    }
  	}
  	```

  - 






# 评分政策

- 如果代码能通过Autolab的测试，会得到总分的40%
- 在后期大规模测试中，如果代码运行时间小于2 min，则得到总分的20%。
- 总分剩余的40%取决于我们的代码和别人比起来有多快:anger:
- 如果后期测试代码运行崩溃了，则这一部分（占总分的60%）将没分
- 会有人工检查代码中是否使用了多线程（`pthread`或`openmp`）和SIMD指令。这两个技术**都要使用**。多线程的具体实现是自由的。
- " There are some really open-source fast implementations for Gaussian Blur algorithm available "——这句话应该属于[优化技术](#优化技术):drooling_face:

> Your grade will be divided into two parts:
>
> 1. We will first run your code on small test cases on Autolab. If you program produces the correct result and satisfy our time requirement, you receive 40% points.
> 2. After the due, we will test your code on large test cases. Your grade on this part depends on the execution time of your code. All groups run slower than 1 minute but faster than 2 minutes get 20% points and the fastest 15 groups receive 60% points. The execution time of 15th group is 60% line and 1 minute is 20% line. The grades for the remaining groups are linearly distributed according to their execution time.
> 3. For your reference, if you only add a single line of #pragma omp parallel for, the running time will be between 1 minute and 2 minutes.
> 4. If your code should not crash on either stage, or you will receive no point in that stage.
> 5. Your submission must contain meaningful use of OpenMP(or pthread) and [Intel SIMD intrinsics](https://software.intel.com/sites/landingpage/IntrinsicsGuide/). Otherwise, you will get 0 point from both stages. This check will be done manually after the deadline so there will be no feedback on this from the auto-grader.
>
> There are something that you need to keep in mind: There are some really open-source fast implementations for Gaussian Blur algorithm available. **You should not submit any existing implementations that is not written by you.** But you can refer to some technical reports and articles for the algorithms and optimizations available and implement your own. And make sure after optimization, your code should produce the same image as the original basic code.







# 提交

- 向Autolab上交`autolab.txt`
- Autolab只会看repo中的`gbfloat_fast.c`和`Makefile`



# 如何运行程序

This does gaussian blur on a user-input image. It generates a 1-dimensional Gaussian distribution kernel with qualities set by the user.

Requirements: stb_image.h and stb_image_write.h

Usage:   ./gbfloat_base <inputjpg> <outputname> <double: a> <double: x0> <double: x1> <unsigned int: dim> <unsigned int: min dim>

IMPORTANT: parameter dim and min dim must both be odd number!!

Example: `./gbfloat_base test.jpg output.jpg 0.6 -2.0 2.0 101 1`

The above is a recommended configuration. It will generate a kernel of 101 samples of a Gaussian distribution function centered at the origin. The 101 evenly-spaced samples are taken on the specified interval, in this case from -2.0 to 2.0. The specified value 'a', in this case 0.6, is a parameter of the Gaussian distribution function.

As in this project, the more the image gets to the center, the less blur it will be, so setting the min dim can change how blur will the center of the image be. You can play with some different min dim to see how the image changes.

The blurring process is done in two steps; Given image A as our input, we first convolve the kernel over the rows of image A to produce a horizontally blurred image B. We then convolve the kernel over the columns of image B to produce a horizontally and vertically blurred image C. The image C is our final blurred image, and we save it to disk.

Image reading and writing is handled by stb_image.h and stb_image_write.h.