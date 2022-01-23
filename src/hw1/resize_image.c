#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, (int)round(x), (int)round(y), c);
}

image nn_resize(image im, int w, int h)
{
    image resized = make_image(w, h, im.c);
    float scale_x = (float) im.w  / (float)w;
    float scale_y = (float) im.h  / (float)h;
    float offset_x = 0.5*scale_x - 0.5;
    float offset_y = 0.5*scale_y - 0.5;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < im.c; c++) {
                set_pixel(resized, x, y, c, nn_interpolate(im, x * scale_x + offset_x , y * scale_y + offset_y , c));
            }
        }
    }
    return resized;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    float x1 = floor(x);
    float x2 = ceil(x);
    float y1 = floor(y);
    float y2 = ceil(y);
    float d1 = y2 - y;
    float d2 = y - y1;
    float d3 = x - x1;
    float d4 = x2 - x;
    float v1 = get_pixel(im, x1, y1, c);
    float v2 = get_pixel(im, x2, y1, c);
    float v3 = get_pixel(im, x1, y2, c);
    float v4 = get_pixel(im, x2, y2, c);
    float q1 = v1*d1 + v3*d2;
    float q2 = v2*d1 + v4*d2;
    float q = q2*d3 + q1*d4;
    return q;
}

image bilinear_resize(image im, int w, int h)
{
    image resized = make_image(w, h, im.c);
    float scale_x = (float) im.w  / (float)w;
    float scale_y = (float) im.h  / (float)h;
    float offset_x = 0.5*scale_x - 0.5;
    float offset_y = 0.5*scale_y - 0.5;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < im.c; c++) {
                set_pixel(resized, x, y, c, bilinear_interpolate(im, x * scale_x + offset_x , y * scale_y + offset_y , c));
            }
        }
    }
    return resized;
}

