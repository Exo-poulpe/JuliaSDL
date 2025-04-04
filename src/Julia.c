#include "Julia.h"

typedef struct MyComplex
{
    double real;
    double imagine;
} MyComplex;

MyComplex *create_complex(double value_real, double value_imagine)
{
    MyComplex *result = calloc(1, sizeof(MyComplex));
    result->real = value_real;
    result->imagine = value_imagine;
    return result;
}

MyComplex *complex_add(MyComplex *a, MyComplex *b)
{
    MyComplex *res = create_complex(a->real + b->real, a->imagine + b->imagine);
    return res;
}

MyComplex *complex_mul(MyComplex *a, MyComplex *b)
{
    MyComplex *res = create_complex(a->real * b->real - a->imagine * b->imagine, a->real * b->imagine + a->imagine * b->real);
    return res;
}

MyComplex *complex_clone(MyComplex *a)
{
    MyComplex *res = create_complex(a->real, a->imagine);
    return res;
}

double complex_get_real(MyComplex* a) {
    return a->real;
}
double complex_get_imaginary(MyComplex* a) {
    return a->imagine;
}

void complex_print_debug(MyComplex* a) {
    printf("Complex\n\tReal : %Lf\n\tImaginary : %Lf\n\n",a->real,a->imagine);
}

void complex_local_mul(MyComplex *a, MyComplex *b, MyComplex *out)
{
    out->real = a->real * b->real - a->imagine * b->imagine;
    out->imagine = a->real * b->imagine + a->imagine * b->real;
}
void complex_local_add(MyComplex *a, MyComplex *b, MyComplex *out)
{
    out->real = a->real + b->real;
    out->imagine = a->imagine + b->imagine;
}

void destroy_complex(MyComplex *a)
{
    free(a);
}

double complex_real(MyComplex *a)
{
    return a->real;
}

double complex_img(MyComplex *a)
{
    return a->imagine;
}

double complex_abs(MyComplex *a)
{
    double res = sqrt((pow(a->real, 2) + pow(a->imagine, 2)));
    return res;
}

void complex_edit(double real, double imagine, MyComplex *out)
{
    out->real = real;
    out->imagine = imagine;
}

// Take the pixel and the scale to return the complex value
void pixel_to_complex(uint32_t x, uint32_t y, MyComplex *center, double scale_x, double scale_y, uint32_t screen_w, uint32_t screen_h, MyComplex *out)
{
    double tmp_real = center->real + (x - screen_w / 2.0) * scale_x;
    double tmp_imagine = center->imagine - (y - screen_h / 2.0) * scale_y;

    out->real = tmp_real;
    out->imagine = tmp_imagine;

    // MyComplex* res = create_complex(tmp_real,tmp_imagine);

    // return res;
}

// MyComplex *pixel_to_complex_return(uint32_t x, uint32_t y, MyComplex *center, double scale_x, double scale_y, uint32_t screen_w, uint32_t screen_h)
// {
//     double tmp_real = center->real + (x - screen_w / 2.0) * scale_x;
//     double tmp_imagine = center->imagine - (y - screen_h / 2.0) * scale_y;

//     MyComplex *res = create_complex(tmp_real, tmp_imagine);

//     return res;
// }

// Return 24 bit color but 32 bit is ok for rawdraw
// uint32_t complex_to_iteration(MyComplex* z0, MyComplex* c, uint32_t max_iterations) {
//     MyComplex* z = complex_clone(z0);
//     for (uint32_t i=0;i<max_iterations;i+=1) {
//         complex_local_mul(z,z,z);
//         complex_local_add(z,c,z);
//         if(complex_abs(z) > 2.0) {
//             destroy_complex(z);
//             return i;
//         }
//     }
//     destroy_complex(z);
//     return max_iterations;
// }

double complex_to_iteration(MyComplex *z0, MyComplex *c, double max_iterations)
{
    MyComplex z = *z0;
    for (uint32_t i = 0; i < max_iterations; i++)
    {
        complex_local_mul(&z, &z, &z);
        complex_local_add(&z, c, &z);
        if (complex_abs(&z) > 2.0)
            return i;
    }
    return max_iterations;
}

// double complex_to_iteration_smooth(MyComplex *z0, MyComplex *c, double max_iterations)
// {
//     MyComplex* z = z0;

//     for (double i = 0.0; i < max_iterations; i++)
//     {
//         complex_local_mul(z, z, z);
//         complex_local_add(z, c, z);
//         double abs = complex_abs(z);
//         if (abs > 1e6) return max_iterations;
//         if (abs > 2.0)
//         {
//             return i + 1 - log2(log2(abs));
//         }
//     }
//     return max_iterations;
// }


MyComplex *pixel_to_complex_return(uint32_t x, uint32_t y, MyComplex *center, double scale_x, double scale_y, uint32_t screen_w, uint32_t screen_h)
{
    double zx = 1.5 * (x - screen_w / 2.0) / (0.5 * screen_w);
    double zy = (y - screen_h / 2.0) / (0.5 * screen_h);
    return create_complex(zx, zy);
}


double complex_to_iteration_smooth(MyComplex *z0, MyComplex *c, double max_iterations)
{
    double zx = z0->real;
    double zy = z0->imagine;
    double cx = c->real;
    double cy = c->imagine;

    double i = 0.0;
    while (zx * zx + zy * zy < 4.0 && i < max_iterations)
    {
        double xtemp = zx * zx - zy * zy + cx;
        zy = 2.0 * zx * zy + cy;
        zx = xtemp;
        i += 1.0;
    }

    if (i >= max_iterations)
        return max_iterations;

    double mag = sqrt(zx * zx + zy * zy);
    return i + 1.0 - log2(log2(mag));
}


void complex_edit_from_angle(double a, double r, MyComplex *out)
{
    out->real = r * cos(a);
    out->imagine = r * sin(a);
}

uint32_t color_from_enum(MyColor color_name)
{
    switch (color_name)
    {
    case BLUE:
        return 0x0000FF;
    case YELLOW:
        return 0xFFFF00;
    case ORANGE:
        return 0xFFA500;
    case RED:
        return 0xFF0000;
    default:
        return 0xffffffff;
    }
}

// MyColor iteration_to_color(uint32_t iteration) {
//     if (iteration > 30) return RED;
//     if (iteration > 20) return ORANGE;
//     if (iteration > 10) return YELLOW;
//     return BLUE;
// }
// uint32_t iteration_to_color(uint32_t iteration) {
//     if (iteration == 300) return 0x000000;
//     uint8_t r = (iteration * 9) % 256;
//     uint8_t g = (iteration * 7) % 256;
//     uint8_t b = (iteration * 11) % 256;
//     return (r << 16) | (g << 8) | b;
// }

// THE BEST
uint32_t iteration_to_color(double iter,double max_iter) {
    double t = log(1.0 + iter) / log(1.0 + max_iter);// iter / (double)max_iter; // normalize 0.0 to 1.0

    // Smooth gradient using sine (feel free to swap for better math!)
    uint8_t r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
    uint8_t g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
    uint8_t b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);

    return (r << 16) | (g << 8) | b;
}

// uint32_t iteration_to_color(double iter, double max_iter)
// {
//     double t = log(1.0 + iter) / log(1.0 + max_iter); // Normalize
//     uint8_t glow = (uint8_t)(pow(t, 0.4) * 255.0);    // Bright edge boost

//     // Glow blue with white center
//     uint8_t r = glow / 3;
//     uint8_t g = glow / 3;
//     uint8_t b = glow;

//     return (r << 16) | (g << 8) | b;
// }


// uint32_t iteration_to_color(double iter, double max_iter) {
//     if (iter >= max_iter) return 0x000000;  // escaped too late = black

//     int v = (int)(iter) % 16;

//     switch (v) {
//         case  0: return 0xff0000;
//         case  1: return 0x00ff00;
//         case  2: return 0x0000ff;
//         case  3: return 0xffff00;
//         case  4: return 0x00ffff;
//         case  5: return 0xff00ff;
//         case  6: return 0xffffff;
//         case  7: return 0x888888;
//         case  8: return 0x440000;
//         case  9: return 0x004400;
//         case 10: return 0x000044;
//         case 11: return 0x222222;
//         case 12: return 0xffa500;
//         case 13: return 0x8b008b;
//         case 14: return 0x808000;
//         case 15: return 0xff69b4;
//     }

//     return 0xffffff;
// }
