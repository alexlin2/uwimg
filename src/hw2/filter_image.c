#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0.0;
    for(int c = 0; c < im.c; c++){
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                sum += get_pixel(im, x, y, c);
            }
        }
    }
    for(int c = 0; c < im.c; c++){
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                set_pixel(im, x, y, c, get_pixel(im, x, y, c) / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    image filter = make_image(w,w,1);
    float avg = 1.0 / (w*w);
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < w; x++) {
            set_pixel(filter, x, y, 0, avg);
        }
    }
    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(im.c == filter.c || filter.c == 1);
    const int size = preserve ? im.c : 1;
    image new = make_image(im.w, im.h, size);
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float sum[size];
            memset(sum, 0, size * sizeof(float));
            for (int c = 0; c < im.c; c++) {
                for (int j = 0; j < filter.h; j++) {
                    for (int i = 0; i < filter.w; i++) {
                        float weight = filter.c == 1 ? get_pixel(filter, i, j, 0) : get_pixel(filter, i, j, c);
                        if(preserve) {
                            sum[c] += get_pixel(im, x - filter.w / 2 + i, y - filter.w / 2 + j, c) * weight;
                        } else {
                            sum[0] += get_pixel(im, x - filter.w / 2 + i, y - filter.w / 2 + j, c) * weight;
                        }
                        
                    }
                }
            }
            for (int n = 0; n < size; n++) {
                set_pixel(new, x, y, n, sum[n]);
            }
            
        }
    }

    return new;
}

image make_highpass_filter()
{
    image f = make_image(3,3,1);
    set_pixel(f,0,0,0,0);
    set_pixel(f,1,0,0,-1);
    set_pixel(f,2,0,0,0);
    set_pixel(f,0,1,0,-1);
    set_pixel(f,1,1,0,4);
    set_pixel(f,2,1,0,-1);
    set_pixel(f,0,2,0,0);
    set_pixel(f,1,2,0,-1);
    set_pixel(f,2,2,0,0);
    return f;
}

image make_sharpen_filter()
{
    image f = make_image(3,3,1);
    set_pixel(f,0,0,0,0);
    set_pixel(f,1,0,0,-1);
    set_pixel(f,2,0,0,0);
    set_pixel(f,0,1,0,-1);
    set_pixel(f,1,1,0,5);
    set_pixel(f,2,1,0,-1);
    set_pixel(f,0,2,0,0);
    set_pixel(f,1,2,0,-1);
    set_pixel(f,2,2,0,0);
    return f;
}

image make_emboss_filter()
{
    image f = make_image(3,3,1);
    set_pixel(f,0,0,0,-2);
    set_pixel(f,1,0,0,-1);
    set_pixel(f,2,0,0,0);
    set_pixel(f,0,1,0,-1);
    set_pixel(f,1,1,0,1);
    set_pixel(f,2,1,0,1);
    set_pixel(f,0,2,0,0);
    set_pixel(f,1,2,0,1);
    set_pixel(f,2,2,0,2);
    return f;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: For the highpass, we don't need to use preserve since we are just looking for high changes in intensity of the image. 
// For both the emboss and sharpen filter, we need to use preserve because we want to make the original image sharper or the edges in the
// original image more pronounced. 

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: We need to do post-processing for the high pass filter because the resulting image after applying the high pass filter only 
// contains frequency information. The resulting image doesn't really have a lot of information to use for other tasks. 

image make_gaussian_filter(float sigma)
{
    int w = (int)ceil(sigma*6) % 2 == 0 ? ceil(sigma*6) + 1 : ceil(sigma*6);
    image f = make_image(w, w, 1);
    for (int y = 0; y < f.h; y++) {
        for (int x = 0; x < f.w; x++) {
            set_pixel(f, x, y, 0, (1.0 / (2 * M_PI * pow(sigma, 2))) * pow(M_E, -(pow(-w/2 + x,2) + pow(-w/2 + y,2)) / (2 * pow(sigma, 2))));
        }
    }
    l1_normalize(f);
    return f;

}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image new = make_image(a.w,a.h,a.c);
    for(int c = 0; c < a.c; c++) {
        for (int y = 0; y < a.h; y++) {
            for (int x = 0; x < a.w; x++) {
                set_pixel(new, x, y, c, get_pixel(a, x, y, c) + get_pixel(b, x, y, c));
            }
        }
    }
    
    return new;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image new = make_image(a.w,a.h,a.c);
    for(int c = 0; c < a.c; c++) {
        for (int y = 0; y < a.h; y++) {
            for (int x = 0; x < a.w; x++) {
                set_pixel(new, x, y, c, get_pixel(a, x, y, c) - get_pixel(b, x, y, c));
            }
        }
    }

    return new;
}

image make_gx_filter()
{
    image f = make_image(3,3,1);
    set_pixel(f,0,0,0,-1);
    set_pixel(f,1,0,0,0);
    set_pixel(f,2,0,0,1);
    set_pixel(f,0,1,0,-2);
    set_pixel(f,1,1,0,0);
    set_pixel(f,2,1,0,2);
    set_pixel(f,0,2,0,-1);
    set_pixel(f,1,2,0,0);
    set_pixel(f,2,2,0,1);
    return f;
}

image make_gy_filter()
{
    image f = make_image(3,3,1);
    set_pixel(f,0,0,0,-1);
    set_pixel(f,1,0,0,-2);
    set_pixel(f,2,0,0,-1);
    set_pixel(f,0,1,0,0);
    set_pixel(f,1,1,0,0);
    set_pixel(f,2,1,0,0);
    set_pixel(f,0,2,0,1);
    set_pixel(f,1,2,0,2);
    set_pixel(f,2,2,0,1);
    return f;
}

void feature_normalize(image im)
{
    float min = 100000;
    float max = -99999;
    for(int c = 0; c < im.c; c++){
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float pixel = get_pixel(im,x,y,c);
                if(pixel < min) {
                    min = pixel;
                }
                if(pixel > max) {
                    max = pixel;
                }
            }
        }
    }

    float range = max - min;
    
    for(int c = 0; c < im.c; c++){
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                if (range == 0.0) {
                    set_pixel(im, x, y, c, 0);
                } else {
                    set_pixel(im, x, y, c, (get_pixel(im, x, y, c) - min) / range);
                }
            }
        }
    }
}

image *sobel_image(image im)
{
    image* outputs = calloc(2, sizeof(image));
    outputs[0].w = im.w;
    outputs[0].h = im.h;
    outputs[0].c = 1;
    outputs[0].data = calloc(im.w*im.h,sizeof(float));
    outputs[1].w = im.w;
    outputs[1].h = im.h;
    outputs[1].c = 1;
    outputs[1].data = calloc(im.w*im.h,sizeof(float));

    image fx = make_gx_filter();
    image fy = make_gy_filter();
    image gx = convolve_image(im, fx, 0);
    image gy = convolve_image(im, fy, 0);
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float gxx = get_pixel(gx, x, y, 0);
            float gyy = get_pixel(gy, x, y, 0);
            set_pixel(outputs[0], x, y, 0, sqrtf(pow(gxx,2)+pow(gyy,2)));
            set_pixel(outputs[1], x, y, 0, atan2(gyy, gxx));
        }
    }
    
    return outputs;
}

image colorize_sobel(image im)
{
    image* outputs = sobel_image(im);
    feature_normalize(outputs[0]);
    feature_normalize(outputs[1]);
    image stylized = make_image(im.w, im.h, 3);
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            set_pixel(stylized, x, y, 0, get_pixel(outputs[1], x, y, 0));
            set_pixel(stylized, x, y, 1, get_pixel(outputs[0], x, y, 0));
            set_pixel(stylized, x, y, 2, get_pixel(outputs[0], x, y, 0));
        }
    }
    hsv_to_rgb(stylized);
    return convolve_image(stylized, make_gaussian_filter(1), 1);
}
