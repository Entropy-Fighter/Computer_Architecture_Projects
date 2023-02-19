
// openMP
#include <omp.h>
// INTEL SIMD
#include <emmintrin.h>
#include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>

// #include <algorithm>
#include <immintrin.h>
#include <sys/time.h>
#include <time.h>
// inplement dymanic

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#define PI 3.14159


#define IMPLEMENTATION_START
#if 1

const int NUM_THREADS = 20;

/**
 * kernel of Gaussian Blur <br>
 * which can be considered as a small image
 * @see https://en.wikipedia.org/wiki/Gaussian_blur
 * @see
 * https://datacarpentry.org/image-processing/06-blurring/#:~:text=In%20a%20Gaussian%20blur%2C%20the,pixel%20in%20the%20filtered%20image.
 *
 */
typedef struct FVec {
    unsigned int length;
    unsigned int min_length;
    unsigned int min_deta;
    float *data;
    float *sum;
} FVec;

typedef struct Image {
    /**
     * length of image
     */
    unsigned int dimX;
    /**
     * width of image
     */
    unsigned int dimY;
    /**
     * num of layer on a single pixel <br>
     * always equals to 3
     */
    unsigned int numChannels;
    /**
     * points to the image data in memory
     */
    float *data;
} Image;

/**
 * Time = v.length / 2
 * @param v
 */
void normalize_FVec(FVec v) {
    // float sum = 0.0;
    unsigned int i, j;
    int ext = v.length / 2;
    v.sum[0] = v.data[ext];

    for (i = ext + 1, j = 1; i < v.length; i++, j++) {
        v.sum[j] = v.sum[j - 1] + v.data[i] * 2;
    }
    // for (i = 0; i <= ext; i++)
    // {
    //      v.data[i] /= v.sum[v.length - ext - 1 ] ;
    //      printf("%lf ",v.sum[i]);
    // }
}

/**
 * helper function to get specific pixel at img <br>
 * Time: 1
 * @return pixel at coordinate(x, y), but may be multiple channels
 */
inline static float *get_pixel(Image img, int x, int y) {
    // boundary check for x
    if (x < 0) {
        x = 0;
    } else if (x >= img.dimX) {
        x = img.dimX - 1;
    }
    // boundary check for y
    if (y < 0) {
        y = 0;
    } else if (y >= img.dimY) {
        y = img.dimY - 1;
    }
    // y should change less to make use of cache
    // when this function is called multiple time
    return img.data + img.numChannels * (y * img.dimX + x);
}

/**
 * no need to do boundary check for x
 */
inline static float *getPixelXGuaranteed(Image img, int x, int y) {
    if (y < 0) {
        y = 0;
    } else if (y >= img.dimY) {
        y = img.dimY - 1;
    }
    return img.data + img.numChannels * (y * img.dimX + x);
}

/**
 * no need to do boundary check for y
 */
inline static float *getPixelYGuaranteed(Image img, int x, int y) {
    if (x < 0) {
        x = 0;
    } else if (x >= img.dimX) {
        x = img.dimX - 1;
    }
    return img.data + img.numChannels * (y * img.dimX + x);
}

/**
 * Gaussian function G(x) in one dimension. <br>
 * Time: depends on native functions exp and sqrt
 * @param a SD, standard deviation
 * @param b offset to y-axis
 * @param x variable of G(x)
 * @return G(x)
 * @see https://en.wikipedia.org/wiki/Gaussian_blur
 */
inline static float gd(float a, float b, float x) {
    float c = (x - b) / a;
    return exp((-.5) * c * c) / (a * sqrt(2 * PI));
}

/**
 * make kernel of Gaussian blur  <br>
 * Time: v.length * gd(...) + normalize_FVec(...) + allocate (1.5 * length)
 * space
 * @param a
 * @param x0
 * @param x1
 * @param length
 * @param min_length
 * @return
 */
FVec make_gv(float a, float x0, float x1, unsigned int length,
             unsigned int min_length) {
    FVec v;
    v.length = length;
    v.min_length = min_length;

    if (v.min_length > v.length) {
        v.min_deta = 0;
    } else {
        v.min_deta = ((v.length - v.min_length) / 2);
    }

    // Note that in Word-RAM model, allocating n word space in memory cost O(n)
    // time but we can do thing to optimalize this. So, whatever
    v.data = malloc(length * sizeof(float));
    v.sum = malloc((length / 2 + 1) * sizeof(float));

    float step = (x1 - x0) / ((float)length);
    int offset = length / 2;

    // takes time
    //#pragma omp parallel for
    for (int i = 0; i < length; i++) {
        v.data[i] = gd(a, 0.0f, (i - offset) * step);
    }
    // takes time
    normalize_FVec(v);

    return v;
}

/**
 * print every pixel value of kernex, <br>
 * useless except for redirecting output to file, because nobody cares
 * every pixel of an image. <br>
 * Time: v.length
 * @param v kernel needs to be printed
 */
void print_fvec(FVec v) {
    unsigned int i;
    printf("\n");
    for (i = 0; i < v.length; i++) {
        printf("%f ", v.data[i]);
    }
    printf("\n");
}

/**
 * copy from Image a, except that the data field of the new image is a new one
 * <br> Time: allocate (a.dimX * a.dimY * a.numChannels) space
 * @param a
 * @return a new Image
 */
Image img_sc(Image a) {
    Image b = a;
    b.data = malloc(b.dimX * b.dimY * b.numChannels * sizeof(float));
    return b;
}

/**
 * horizontal Gaussian blur
 *
 * @param a original image
 * @param gv kernel
 * @return horizontally blurred image
 */
Image gb_h(Image a, FVec gv) {
    Image b = img_sc(a);

    int ext = gv.length / 2;

    unsigned int *detas = malloc(sizeof(unsigned int) * a.dimX * a.dimY);

    // unsigned int deta =
    //     fmin(fmin(a.dimY - y - 1, y), fmin(a.dimX - x - 1, x));

    // deta = fmin(deta, gv.min_deta);

    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel for
    for (int y = 0; y < a.dimY; y++) {
        for (int x = 0; x < a.dimX; x++) {
            detas[y * a.dimX + x] = fmin(fmin(fmin(a.dimY - y - 1, y),
                                              fmin(a.dimX - x - 1, x)),
                                         gv.min_deta);
        }
    }

    // iterate through the whole picture
    // Image a is never changed
    // every loop only affect one element in (Image b).data, which is
    // independent of other element, great!!!
    // always loop 3 times
    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel for
    for (int y = 0; y < a.dimY; y++) {  // reorder the loop order to fully make use of cache
        for (int x = 0; x < a.dimX; x++) {
            float *pc = b.data + b.numChannels * (y * b.dimX + x);

            int i;  // the only index for loop

            unsigned int deta = detas[y * a.dimX + x];

            int loopTimes = gv.length - deta - deta;

            // for (i = deta; i < gv.length - deta; i++) {
            //     offset = i - ext;
            //     sum += gv.data[i] / gv.sum[ext - deta] * (float)get_pixel(a, x + offset, y)[channel];
            // }
            // a.numChannels is guaranteed to be 3
            float *pixels = malloc(sizeof(float) * loopTimes * a.numChannels);
            float *pix;

            for (i = 0; i < loopTimes; ++i) {
                pix = getPixelYGuaranteed(a, x + i + deta - ext, y);
                pixels[i] = pix[0];
                pixels[i + loopTimes] = pix[1];
                pixels[i + (loopTimes << 1)] = pix[2];
            }

            // for (i = 0; i < loopTimes; i++)
            //     sum += gv.data[i + deta] / gv.sum[ext - deta] * pixels[i];

            float tempArray[8] = {gv.sum[ext - deta], gv.sum[ext - deta],
                                  gv.sum[ext - deta], gv.sum[ext - deta],
                                  gv.sum[ext - deta], gv.sum[ext - deta],
                                  gv.sum[ext - deta], gv.sum[ext - deta]};

            __m256 op2 = _mm256_loadu_ps(tempArray);

            for (int channel = 0; channel < a.numChannels; channel++) {
                float sum = 0;

                __m256 sum1 = _mm256_setzero_ps();
                __m256 sum2 = _mm256_setzero_ps();
                __m256 sum3 = _mm256_setzero_ps();
                __m256 sum4 = _mm256_setzero_ps();

                for (i = 0; i < loopTimes / 32 * 32; i += 32) {
                    __m256 op11 = _mm256_loadu_ps(gv.data + i + 0 + deta);
                    __m256 op12 = _mm256_loadu_ps(gv.data + i + 8 + deta);
                    __m256 op13 = _mm256_loadu_ps(gv.data + i + 16 + deta);
                    __m256 op14 = _mm256_loadu_ps(gv.data + i + 24 + deta);

                    __m256 op31 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 0);
                    __m256 op32 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 8);
                    __m256 op33 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 16);
                    __m256 op34 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 24);

                    __m256 res1 = _mm256_mul_ps(_mm256_div_ps(op11, op2), op31);
                    __m256 res2 = _mm256_mul_ps(_mm256_div_ps(op12, op2), op32);
                    __m256 res3 = _mm256_mul_ps(_mm256_div_ps(op13, op2), op33);
                    __m256 res4 = _mm256_mul_ps(_mm256_div_ps(op14, op2), op34);

                    sum1 = _mm256_add_ps(sum1, res1);
                    sum2 = _mm256_add_ps(sum2, res2);
                    sum3 = _mm256_add_ps(sum3, res3);
                    sum4 = _mm256_add_ps(sum4, res4);
                }

                float result[32];

                _mm256_storeu_ps(result + 0, sum1);
                _mm256_storeu_ps(result + 8, sum2);
                _mm256_storeu_ps(result + 16, sum3);
                _mm256_storeu_ps(result + 24, sum4);

                for (i = 0; i < 32; i++) {
                    sum += result[i];
                }

                for (i = loopTimes / 32 * 32; i < loopTimes; i++) {
                    sum += gv.data[i + deta] / gv.sum[ext - deta] * pixels[i + loopTimes * channel];
                }

                pc[channel] = sum;
            }
            free(pixels);
        }
    }

    free(detas);

    return b;
}

/**
 * vertical Gaussian blur <br>
 *
 * @param a original image
 * @param gv kernel
 * @return vertically blurred image
 */
Image gb_v(Image a, FVec gv) {
    Image b = img_sc(a);

    int ext = gv.length / 2;

    unsigned int *detas = malloc(sizeof(unsigned int) * a.dimX * a.dimY);

    // unsigned int deta =
    //     fmin(fmin(a.dimY - y - 1, y), fmin(a.dimX - x - 1, x));

    // deta = fmin(deta, gv.min_deta);

    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel for
    for (int y = 0; y < a.dimY; y++) {
        for (int x = 0; x < a.dimX; x++) {
            detas[y * a.dimX + x] = fmin(fmin(fmin(a.dimY - y - 1, y),
                                              fmin(a.dimX - x - 1, x)),
                                         gv.min_deta);
        }
    }

    // iterate through the whole picture
    // Image a is never changed
    // every loop only affect one element in (Image b).data, which is
    // independent of other element, great!!!
    // always loop 3 times
    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel for
    for (int y = 0; y < a.dimY; y++) {  // reorder the loop order to fully make use of cache
        for (int x = 0; x < a.dimX; x++) {
            float *pc = b.data + b.numChannels * (y * b.dimX + x);

            int i;  // the only index for loop

            unsigned int deta = detas[y * a.dimX + x];

            int loopTimes = gv.length - deta - deta;

            // for (i = deta; i < gv.length - deta; i++) {
            //     offset = i - ext;
            //     sum += gv.data[i] / gv.sum[ext - deta] * (float)get_pixel(a, x + offset, y)[channel];
            // }
            // a.numChannels is guaranteed to be 3
            float *pixels = malloc(sizeof(float) * loopTimes * a.numChannels);
            float *pix;

            for (i = 0; i < loopTimes; ++i) {
                pix = getPixelXGuaranteed(a, x, y + i + deta - ext);
                pixels[i] = pix[0];
                pixels[i + loopTimes] = pix[1];
                pixels[i + (loopTimes << 1)] = pix[2];
            }

            // for (i = 0; i < loopTimes; i++)
            //     sum += gv.data[i + deta] / gv.sum[ext - deta] * pixels[i];

            float tempArray[8] = {gv.sum[ext - deta], gv.sum[ext - deta],
                                  gv.sum[ext - deta], gv.sum[ext - deta],
                                  gv.sum[ext - deta], gv.sum[ext - deta],
                                  gv.sum[ext - deta], gv.sum[ext - deta]};

            __m256 op2 = _mm256_loadu_ps(tempArray);

            for (int channel = 0; channel < a.numChannels; channel++) {
                float sum = 0;

                __m256 sum1 = _mm256_setzero_ps();
                __m256 sum2 = _mm256_setzero_ps();
                __m256 sum3 = _mm256_setzero_ps();
                __m256 sum4 = _mm256_setzero_ps();

                for (i = 0; i < loopTimes / 32 * 32; i += 32) {
                    __m256 op11 = _mm256_loadu_ps(gv.data + i + 0 + deta);
                    __m256 op12 = _mm256_loadu_ps(gv.data + i + 8 + deta);
                    __m256 op13 = _mm256_loadu_ps(gv.data + i + 16 + deta);
                    __m256 op14 = _mm256_loadu_ps(gv.data + i + 24 + deta);

                    __m256 op31 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 0);
                    __m256 op32 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 8);
                    __m256 op33 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 16);
                    __m256 op34 = _mm256_loadu_ps(pixels + loopTimes * channel + i + 24);

                    __m256 res1 = _mm256_mul_ps(_mm256_div_ps(op11, op2), op31);
                    __m256 res2 = _mm256_mul_ps(_mm256_div_ps(op12, op2), op32);
                    __m256 res3 = _mm256_mul_ps(_mm256_div_ps(op13, op2), op33);
                    __m256 res4 = _mm256_mul_ps(_mm256_div_ps(op14, op2), op34);

                    sum1 = _mm256_add_ps(sum1, res1);
                    sum2 = _mm256_add_ps(sum2, res2);
                    sum3 = _mm256_add_ps(sum3, res3);
                    sum4 = _mm256_add_ps(sum4, res4);
                }

                float result[32];

                _mm256_storeu_ps(result + 0, sum1);
                _mm256_storeu_ps(result + 8, sum2);
                _mm256_storeu_ps(result + 16, sum3);
                _mm256_storeu_ps(result + 24, sum4);

                for (i = 0; i < 32; i++) {
                    sum += result[i];
                }

                for (i = loopTimes / 32 * 32; i < loopTimes; i++) {
                    sum += gv.data[i + deta] / gv.sum[ext - deta] * pixels[i + loopTimes * channel];
                }

                pc[channel] = sum;
            }
            free(pixels);
        }
    }

    free(detas);

    return b;
}

/**
 * interface of Gaussian blur
 * @param a original image
 * @param gv
 * @return image blurred
 */
Image apply_gb(Image a, FVec gv) {
    Image b = gb_h(a, gv);
    Image c = gb_v(b, gv);
    free(b.data);
    return c;
}

#endif
#define IMPLEMENTATION_END




int main(int argc, char **argv) {
    // timer
    struct timeval start_time, stop_time, elapsed_time;
    gettimeofday(&start_time, NULL);

    if (argc < 6) {
        printf(
                "Usage: ./gb.exe <inputjpg> <outputname> <float: a> <float: x0> "
                "<float: x1> <unsigned int: dim>\n");
        exit(0);
    }

    float a, x0, x1;
    unsigned int dim, min_dim;

    sscanf(argv[3], "%f", &a);
    sscanf(argv[4], "%f", &x0);
    sscanf(argv[5], "%f", &x1);
    sscanf(argv[6], "%u", &dim);
    sscanf(argv[7], "%u", &min_dim);

    // generate a kernel with the parameters
    FVec v = make_gv(a, x0, x1, dim, min_dim);
    // print_fvec(v);

    Image img;
    // load original image from disk
    img.data =
            stbi_loadf(argv[1], &(img.dimX), &(img.dimY), &(img.numChannels), 0);

    // image blurred
    Image imgOut = apply_gb(img, v);

    // write image blurred to disk
    stbi_write_jpg(argv[2], imgOut.dimX, imgOut.dimY, imgOut.numChannels,
                   imgOut.data, 90);

    // timer
    gettimeofday(&stop_time, NULL);
    timersub(&stop_time, &start_time, &elapsed_time);
    printf("%f \n", elapsed_time.tv_sec + elapsed_time.tv_usec / 1000000.0);

    free(imgOut.data);
    free(v.data);
    free(v.sum);
    return 0;
}