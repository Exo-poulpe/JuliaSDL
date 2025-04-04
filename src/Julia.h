#ifndef JULIA_H
#define JULIA_H

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>



typedef struct MyComplex MyComplex;

typedef enum MyColor {
    BLUE = 0,
    YELLOW = 1,
    ORANGE = 2,
    RED = 3,
}MyColor;


MyComplex* create_complex(double value_real,double value_imagine);
MyComplex* complex_add(MyComplex* a,MyComplex* b);
MyComplex* complex_mul(MyComplex* a,MyComplex* b);
MyComplex* complex_clone(MyComplex* a);
void complex_print_debug(MyComplex* a);
double complex_get_real(MyComplex* a);
double complex_get_imaginary(MyComplex* a);
double complex_real(MyComplex* a);
double complex_img(MyComplex* a);
void complex_edit_from_angle(double a, double r, MyComplex* out);
void complex_edit(double real,double imagine,MyComplex* out);
void complex_local_mul(MyComplex* a,MyComplex* b, MyComplex* out);
void complex_local_add(MyComplex* a,MyComplex* b, MyComplex* out);
void destroy_complex(MyComplex* a);
double complex_abs(MyComplex* a);
double complex_to_iteration_smooth(MyComplex* z0, MyComplex* c, double max_iterations);
MyComplex* pixel_to_complex_return(uint32_t x, uint32_t y,MyComplex* center, double scale_x, double scale_y,uint32_t screen_w, uint32_t screen_h);
// Take the pixel and the scale to return the complex value
// MyComplex* pixel_to_complex(uint32_t x, uint32_t y,MyComplex* center, double scale_x, double scale_y,uint32_t screen_w, uint32_t screen_h);
void pixel_to_complex(uint32_t x, uint32_t y,MyComplex* center, double scale_x, double scale_y,uint32_t screen_w, uint32_t screen_h,MyComplex* out);
// Return 24 bit color but 32 bit is ok for rawdraw
double complex_to_iteration(MyComplex* z0, MyComplex* c, double max_iterations);
uint32_t iteration_to_color(double iteration,double max_iter);

#endif