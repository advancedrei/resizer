/*
 * Copyright (c) Imazen LLC.
 * No part of this project, including this file, may be copied, modified,
 * propagated, or distributed except as permitted in COPYRIGHT.txt.
 * Licensed under the GNU Affero General Public License, Version 3.0.
 * Commercial licenses available at http://imageresizing.net/
 */
#ifdef _MSC_VER
#pragma unmanaged
#endif

#include "fastscaling_private.h"


bool BitmapFloat_linear_to_luv_rows(Context * context, BitmapFloat * bit, const uint32_t start_row, const  uint32_t row_count)
{
    if (!(start_row + row_count <= bit->h)) {
        CONTEXT_error(context, Invalid_internal_state); //Don't access rows past the end of the bitmap
        return false;
    }
    if ((bit->w * bit->channels) != bit->float_stride) {
        CONTEXT_error(context, Invalid_internal_state); //This algorithm can't handle padding, if present
        return false;
    }
    float * start_at = bit->float_stride * start_row  + bit->pixels;

    const float * end_at = bit->float_stride * (start_row + row_count) + bit->pixels;

    for (float* pix = start_at; pix < end_at; pix++) {
        linear_to_luv(pix);
    }
    return true;
}

bool BitmapFloat_luv_to_linear_rows(Context * context, BitmapFloat * bit, const uint32_t start_row, const  uint32_t row_count)
{
    if (!(start_row + row_count <= bit->h)) {
        CONTEXT_error(context, Invalid_internal_state);
        return false;
    }
    if ((bit->w * bit->channels) != bit->float_stride) {
        CONTEXT_error(context, Invalid_internal_state);
        return false;
    }
    float * start_at = bit->float_stride * start_row + bit->pixels;

    const float * end_at = bit->float_stride * (start_row + row_count) + bit->pixels;

    for (float* pix = start_at; pix < end_at; pix++) {
        luv_to_linear(pix);
    }
    return true;
}



bool BitmapBgra_apply_color_matrix(Context * context, BitmapBgra * bmp, const uint32_t row, const uint32_t count, float* const __restrict  m[5])
{
    const uint32_t stride = bmp->stride;
    const uint32_t ch = BitmapPixelFormat_bytes_per_pixel(bmp->fmt);
    const uint32_t w = bmp->w;
    const uint32_t h = umin(row + count, bmp->h);
    if (ch == 4) {

        for (uint32_t y = row; y < h; y++)
            for (uint32_t x = 0; x < w; x++) {
                uint8_t* const __restrict data = bmp->pixels + stride * y + x * ch;

                const uint8_t r = uchar_clamp_ff(m[0][0] * data[2] + m[1][0] * data[1] + m[2][0] * data[0] + m[3][0] * data[3] + m[4][0]);
                const uint8_t g = uchar_clamp_ff(m[0][1] * data[2] + m[1][1] * data[1] + m[2][1] * data[0] + m[3][1] * data[3] + m[4][1]);
                const uint8_t b = uchar_clamp_ff(m[0][2] * data[2] + m[1][2] * data[1] + m[2][2] * data[0] + m[3][2] * data[3] + m[4][2]);
                const uint8_t a = uchar_clamp_ff(m[0][3] * data[2] + m[1][3] * data[1] + m[2][3] * data[0] + m[3][3] * data[3] + m[4][3]);

                uint8_t* newdata = bmp->pixels + stride * y + x * ch;
                newdata[0] = b;
                newdata[1] = g;
                newdata[2] = r;
                newdata[3] = a;
            }
    } else if (ch == 3) {

        for (uint32_t y = row; y < h; y++)
            for (uint32_t x = 0; x < w; x++) {
                unsigned char* const __restrict data = bmp->pixels + stride * y + x * ch;

                const uint8_t r = uchar_clamp_ff(m[0][0] * data[2] + m[1][0] * data[1] + m[2][0] * data[0] + m[4][0]);
                const uint8_t g = uchar_clamp_ff(m[0][1] * data[2] + m[1][1] * data[1] + m[2][1] * data[0] + m[4][1]);
                const uint8_t b = uchar_clamp_ff(m[0][2] * data[2] + m[1][2] * data[1] + m[2][2] * data[0] + m[4][2]);

                uint8_t* newdata = bmp->pixels + stride * y + x * ch;
                newdata[0] = b;
                newdata[1] = g;
                newdata[2] = r;
            }
    } else {
        CONTEXT_error (context, Unsupported_pixel_format);
        return false;
    }
    return true;
}


bool BitmapFloat_apply_color_matrix(Context * context, BitmapFloat * bmp, const uint32_t row, const uint32_t count, float*  m[5])
{
    const uint32_t stride = bmp->float_stride;
    const uint32_t ch = bmp->channels;
    const uint32_t w = bmp->w;
    const uint32_t h = umin(row + count,bmp->h);
    switch (ch) {
    case 4: {
        for (uint32_t y = row; y < h; y++)
            for (uint32_t x = 0; x < w; x++) {
                float* const __restrict data = bmp->pixels + stride * y + x * ch;

                const float r = (m[0][0] * data[2] + m[1][0] * data[1] + m[2][0] * data[0] + m[3][0] * data[3] + m[4][0]);
                const float g = (m[0][1] * data[2] + m[1][1] * data[1] + m[2][1] * data[0] + m[3][1] * data[3] + m[4][1]);
                const float b = (m[0][2] * data[2] + m[1][2] * data[1] + m[2][2] * data[0] + m[3][2] * data[3] + m[4][2]);
                const float a = (m[0][3] * data[2] + m[1][3] * data[1] + m[2][3] * data[0] + m[3][3] * data[3] + m[4][3]);

                float * newdata = bmp->pixels + stride * y + x * ch;
                newdata[0] = b;
                newdata[1] = g;
                newdata[2] = r;
                newdata[3] = a;

            }
        return true;
    }
    case 3: {

        for (uint32_t y = row; y < h; y++)
            for (uint32_t x = 0; x < w; x++) {

                float* const __restrict data = bmp->pixels + stride * y + x * ch;

                const float  r =  (m[0][0] * data[2] + m[1][0] * data[1] + m[2][0] * data[0] + m[4][0]);
                const float g =  (m[0][1] * data[2] + m[1][1] * data[1] + m[2][1] * data[0] + m[4][1]);
                const float b = (m[0][2] * data[2] + m[1][2] * data[1] + m[2][2] * data[0] + m[4][2]);

                float * newdata = bmp->pixels + stride * y + x * ch;
                newdata[0] = b;
                newdata[1] = g;
                newdata[2] = r;
            }
        return true;
    }
    default: {
        CONTEXT_error (context, Unsupported_pixel_format);
        return false;
    }
    }
}



bool BitmapBgra_populate_histogram (Context * context, BitmapBgra * bmp, uint64_t * histograms, const uint32_t histogram_size_per_channel, const uint32_t histogram_count, uint64_t * pixels_sampled)
{
    const uint32_t row = 0;
    const uint32_t count = bmp->h;
    const uint32_t stride = bmp->stride;
    const uint32_t ch = BitmapPixelFormat_bytes_per_pixel (bmp->fmt);
    const uint32_t w = bmp->w;
    const uint32_t h = umin (row + count, bmp->h);

    const int shift = 8 - intlog2 (histogram_size_per_channel);

    if (ch == 4 || ch == 3) {

        if (histogram_count == 1){

            for (uint32_t y = row; y < h; y++){
                for (uint32_t x = 0; x < w; x++) {
                    uint8_t* const __restrict data = bmp->pixels + stride * y + x * ch;

                    histograms[(306 * data[2] + 601 * data[1] + 117 * data[0]) >> shift]++;
                }
            }
        } else if (histogram_count == 3){
            for (uint32_t y = row; y < h; y++){
                for (uint32_t x = 0; x < w; x++) {
                    uint8_t* const __restrict data = bmp->pixels + stride * y + x * ch;
                    histograms[data[2] >> shift]++;
                    histograms[(data[1] >> shift) + histogram_size_per_channel]++;
                    histograms[(data[0] >> shift) + 2 * histogram_size_per_channel]++;
                }
            }
        }
        else if (histogram_count == 2){
            for (uint32_t y = row; y < h; y++){
                for (uint32_t x = 0; x < w; x++) {
                    uint8_t* const __restrict data = bmp->pixels + stride * y + x * ch;
                    //Calculate luminosity and saturation
                    histograms[(306 * data[2] + 601 * data[1] + 117 * data[0]) >> shift]++;
                    histograms[histogram_size_per_channel + (max(255,max(abs ((int)data[2] - (int)data[1]),abs ((int)data[1] - (int)data[0]))) >> shift)]++;
                }
            }
        }
        else{
            CONTEXT_error (context, Invalid_internal_state);
            return false;
        }

        *(pixels_sampled) = (h - row) * w;
    }
    else {
        CONTEXT_error (context, Unsupported_pixel_format);
        return false;
    }
    return true;
}



 // Gamma correction  http://www.4p8.com/eric.brasseur/gamma.html#formulas

#ifdef EXPOSE_SIGMOID

 static void Context_sigmoid_internal (Context * c, float x_coefficent, float x_offset, float constant){
     c->colorspace.sigmoid.constant = constant; //1
     c->colorspace.sigmoid.x_coeff = x_coefficent; //2
     c->colorspace.sigmoid.x_offset = x_offset; //-1
     c->colorspace.sigmoid.y_offset = 0;
     c->colorspace.sigmoid.y_coeff = 1;

     c->colorspace.sigmoid.y_coeff = 1 / (sigmoid (&c->colorspace.sigmoid, 1.0) - sigmoid (&c->colorspace.sigmoid, 0));
     c->colorspace.sigmoid.y_offset = -1 * sigmoid (&c->colorspace.sigmoid, 0);

 }

 static float derive_constant (float x, float slope, float sign){
     return (float)((-2.0f * slope * fabs (x) + sign * sqrtf ((float)(1.0f - 4.0f * slope * fabs (x))) + 1.0f) / 2.0f * slope);
 }

#endif





 void Context_set_floatspace (Context * context,  WorkingFloatspace space, float a, float b, float c){
     context->colorspace.floatspace = space;


     context->colorspace.apply_srgb = (space & Floatspace_linear) > 0;
     context->colorspace.apply_gamma = (space & Floatspace_gamma) > 0;

#ifdef EXPOSE_SIGMOID
     context->colorspace.apply_sigmoid = (space & Floatspace_sigmoid) > 0;
     if ((space & Floatspace_sigmoid_3) > 0){
         Context_sigmoid_internal (context, -2, a, derive_constant (a + b * -2, c, 1));
     }
     else if ((space & Floatspace_sigmoid_2) > 0){
         Context_sigmoid_internal (context, -b, (1 + c) * b, -1 * (b + a));
     }
     else if ((space & Floatspace_sigmoid) > 0){
         Context_sigmoid_internal (context, a, b, c);
     }
#endif
     if (context->colorspace.apply_gamma){
         context->colorspace.gamma = a;
         context->colorspace.gamma_inverse = (float)(1.0 / ((double)a));
     }

     for (uint32_t n = 0; n < 256; n++) {
         context->colorspace.byte_to_float[n] = Context_srgb_to_floatspace_uncached (context, n);
     }
 }






float Context_byte_to_floatspace (Context * c, uint8_t srgb_value){
    return Context_srgb_to_floatspace (c, srgb_value);
}

 uint8_t Context_floatspace_to_byte (Context * c, float space_value){
    return Context_floatspace_to_srgb (c, space_value);
}
