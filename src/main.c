#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Julia.h"
#include "os_generic.h"

#define WIDTH 800
#define HEIGHT 800

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Julia Set - SDL2",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);

    bool running = true;
    SDL_Event event;

    double zoom = 1.0;
    double max_iterations = 400.0;
    double scale_x, scale_y;
    double frame_start = OGGetAbsoluteTime();

    MyComplex *center = create_complex(0.0, 0.741699);
    // MyComplex *c = create_complex(-0.8, 0.156); // test avec une bonne valeur
    MyComplex *c = create_complex(-0.532770, 0.581281); // Great Render for Julia
    // MyComplex *c = create_complex(-0.5251993, -0.5251993); // test avec une bonne valeur

    uint32_t *pixels = calloc(WIDTH * HEIGHT, sizeof(uint32_t));

    const char *renderer_name = SDL_GetCurrentVideoDriver();
    omp_set_num_threads(8);
    printf("Renderer backend: %s\n", renderer_name);

    // double a = 348743031.861789;              // angle in radians (change speed here)
    // double r = 0.7885;
    // complex_edit_from_angle(a, r, c);

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();

    while (running)
    {
        // Events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }

        double move_speed = 0.005 * zoom; // Déplacement proportionnel au zoom

        SDL_PumpEvents();
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_LEFT])
            complex_edit(complex_get_real(center) - move_speed, complex_get_imaginary(center), center);
        if (keystates[SDL_SCANCODE_RIGHT])
            complex_edit(complex_get_real(center) + move_speed, complex_get_imaginary(center), center);
        if (keystates[SDL_SCANCODE_UP])
            complex_edit(complex_get_real(center), complex_get_imaginary(center) + move_speed, center);
        if (keystates[SDL_SCANCODE_DOWN])
            complex_edit(complex_get_real(center), complex_get_imaginary(center) - move_speed, center);

        // Compute fractal
        scale_x = (3.0 / (double)WIDTH) * zoom;
        scale_y = (3.0 / (double)HEIGHT) * zoom;

        // MyComplex *z0 = create_complex(0, 0);

#pragma omp parallel for
        for (uint32_t y = 0; y < HEIGHT; y += 1)
        {
            for (uint32_t x = 0; x < WIDTH; x += 1)
            {
                MyComplex *z0 = create_complex(0, 0);
                pixel_to_complex(x, y, center, scale_x, scale_y, WIDTH, HEIGHT, z0);
                double iter = complex_to_iteration_smooth(z0, c, max_iterations);
                uint32_t color = iteration_to_color(iter, max_iterations);
                pixels[y * WIDTH + x] = 0xFF000000 | color;
                destroy_complex(z0);
            }
        }

        // for (uint32_t x = 0; x < WIDTH; x++)
        // {
        //     for (uint32_t y = 0; y < HEIGHT; y++)
        //     {
        //         pixel_to_complex(x, y, center, scale_x, scale_y, WIDTH, HEIGHT,z0);
        //         // MyComplex *z0 = pixel_to_complex(x, y, center, scale_x, scale_y, WIDTH, HEIGHT);
        //         double iter = complex_to_iteration_smooth(z0, c, max_iterations);
        //         uint32_t color = iteration_to_color(iter, max_iterations);
        //         pixels[y * WIDTH + x] = 0xFF000000 | color; // SDL wants ARGB
        //     }
        // }

        // destroy_complex(z0);

        // Render
        SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // memset(pixels, 0, WIDTH * HEIGHT * sizeof(uint32_t));

        // printf("Loop\n");

        // zoom *= 0.99;
        // zoom -= 1.0;
        double t = OGGetAbsoluteTime();

        // OGSleep(1);
        SDL_Delay(100);

        zoom *= 0.98;
        // zoom -= 0.01;
        // zoom += 1.0 / pow(1.02, t/10000);
        // zoom += 1.0 / pow(1.02, t * 30);

        // double a = fmod(t * 0.3, 2 * M_PI);              // angle in radians (change speed here)
        // double r = 0.7885;
        // complex_edit_from_angle(a, r, c);

        // complex_print_debug(c);

        frameCount++;

        // Calculate FPS every second
        Uint32 elapsed = SDL_GetTicks() - startTime;
        if (elapsed >= 1000)
        {
            // printf("FPS: %d\n", frameCount);
            frameCount = 0;
            startTime = SDL_GetTicks();
        }
    }

    free(pixels);
    destroy_complex(center);
    destroy_complex(c);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
