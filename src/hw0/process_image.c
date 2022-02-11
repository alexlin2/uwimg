#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if(x >= im.w) x = im.w - 1;
    if(y >= im.h) y = im.h - 1;
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    return im.data[x + im.w * (y + im.h * c)];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if(x >= 0 && x < im.w && y >= 0 && y < im.h){
        im.data[x + im.w*(y + im.h*c)] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, im.w*im.h*im.c*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float luma = 0.299 * get_pixel(im, x, y, 0) + 0.587 * get_pixel(im, x, y, 1)
                                + .114 * get_pixel(im, x, y, 2);
            set_pixel(gray, x, y, 0, luma);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            set_pixel(im, x, y, c, get_pixel(im, x,y,c) + v);
        }
    }
}

void clamp_image(image im)
{
    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                if(get_pixel(im, x,y,c) < 0.0) set_pixel(im, x,y,c,0.0);
                if(get_pixel(im, x,y,c) > 1.0) set_pixel(im, x,y,c,1.0);
            }
        }
    }
    
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float r = get_pixel(im,x,y,0);
            float g = get_pixel(im,x,y,1);
            float b = get_pixel(im,x,y,2);
            float v = MAX(r,MAX(g, b));
            float m = MIN(r,MIN(g, b));
            float c = v - m;
            float h = 0.0;
            float s = 0.0;
            if(v != 0.0){
                s = c / v;
                if(c != 0.0) {
                    if (v == r) {
                        h = (g-b)/c;
                    } else if(v == g) {
                        h = (b-r)/c + 2;
                    } else if(v == b) {
                        h = (r-g)/c + 4;
                    }
                }
                
                if (h < 0.0) {
                    h = h/6 + 1;
                } else {
                    h = h/6;
                }

            } 
            

            set_pixel(im,x,y,0,h);
            set_pixel(im,x,y,1,s);
            set_pixel(im,x,y,2,v);


        }
    }
}

void hsv_to_rgb(image im)
{
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float s = get_pixel(im,x,y,1);
            float v = get_pixel(im,x,y,2);
            float c = s*v;
            float h = get_pixel(im,x,y,0) * 360;
            float x = c*(1-abs(fmod(h*6, 2)-1));
            float H = 360 * h;
            float m = v-c;
            float r,g,b;
            if(H >= 0 && H < 60){
                r = c,g = x,b = 0;
            }
            else if(H >= 60 && H < 120){
                r = x,g = c,b = 0;
            }
            else if(H >= 120 && H < 180){
                r = 0,g = c,b = x;
            }
            else if(H >= 180 && H < 240){
                r = 0,g = x,b = c;
            }
            else if(H >= 240 && H < 300){
                r = x,g = 0,b = c;
            }
            else{
                r = c,g = 0,b = x;
            }
            set_pixel(im,x,y,0,r+m);
            set_pixel(im,x,y,1,g+m);
            set_pixel(im,x,y,2,b+m);
        }
    }
}
