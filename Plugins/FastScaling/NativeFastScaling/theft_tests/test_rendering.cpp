#include "fastscaling_private.h"

#include "catch.hpp"
extern "C" {
#include <theft.h>
}
#include <assert.h>
#include <sys/time.h>
#include <string.h>

static Context * c;

static bool get_time_seed(theft_seed *seed)
{
    struct timeval tv;
    if (-1 == gettimeofday(&tv, NULL)) { assert(false); }
    *seed = (theft_seed)((tv.tv_sec << 32) | tv.tv_usec);
    /* printf("seed is 0x%016llx\n", *seed); */
    return true;
}

struct TestEnv {
    int max_dimensions;
};

static theft_trial_res render_should_succeed(RenderDetails * details, BitmapBgra * source, BitmapBgra * canvas) {

    bool result = RenderDetails_render(c, details, source, canvas);
    if (!result) return THEFT_TRIAL_FAIL;

    return THEFT_TRIAL_PASS;
}



void * renderdetails_random(theft * theft, theft_seed seed, void * input) {
    int filter = 0;

    do {
        filter = theft_random(theft) % 30;
    }while (!InterpolationDetails_interpolation_filter_exists((InterpolationFilter)filter));


    RenderDetails * details = RenderDetails_create_with(c, (InterpolationFilter)filter);

    if (details == NULL) return NULL;

    details->post_flip_x = theft_random(theft) % 2;
    details->post_flip_y = theft_random(theft) % 2;
    details->post_transpose = theft_random(theft) % 2;

    details->sharpen_percent_goal = (float)theft_random_double(theft);
    return (void*)details;
}
void renderdetails_free(void * details, void * unused)
{
    RenderDetails_destroy(c, (RenderDetails *)details);
}

void * BitmapBgra_random_dest(theft * theft, theft_seed seed, void * input) {
    BitmapPixelFormat fmt = (BitmapPixelFormat)(3 + theft_random(theft) % 2);
    int w = theft_random(theft) % 2049;
    int h = theft_random(theft) % 2049;
    BitmapBgra * b = BitmapBgra_create(c, w,h, false, fmt);
    b -> alpha_meaningful = !!(theft_random(theft) % 2);
    b -> can_reuse_space = !!(theft_random(theft) % 2);
    b -> stride_readonly = !!(theft_random(theft) % 2);
    b->pixels_readonly = false;
    b->matte_color[0] = theft_random(theft) % 256;
    b->matte_color[1] = theft_random(theft) % 256;
    b->matte_color[2] = theft_random(theft) % 256;
    b->matte_color[3] = theft_random(theft) % 256;
    b->compositing_mode = (BitmapCompositingMode)(theft_random(theft) % 3);
    return (void*)b;
}

void * BitmapBgra_random_zeroed_source(theft * theft, theft_seed seed, void * input) {
    BitmapPixelFormat fmt = (BitmapPixelFormat)(3 + theft_random(theft) % 2);
    int w = 1 + (theft_random(theft) % 2048);
    int h = 1 + (theft_random(theft) % 2048);

    BitmapBgra * b = BitmapBgra_create(c, w, h, true, fmt);
    if (!b) {
        char buffer[1024];
        printf("BitmapBgra_create failed: %s\n", Context_error_message(c, buffer, sizeof buffer));
        printf("dimensions: %dx%d format: %u\n", w, h, (unsigned)fmt);
        return THEFT_ERROR; // or THEFT_SKIP maybe
    }
    b -> alpha_meaningful = !!(theft_random(theft) % 2);
    b -> can_reuse_space = !!(theft_random(theft) % 2);
    b -> stride_readonly = !!(theft_random(theft) % 2);
    b->pixels_readonly = false;
    b->matte_color[0] = theft_random(theft) % 256;
    b->matte_color[1] = theft_random(theft) % 256;
    b->matte_color[2] = theft_random(theft) % 256;
    b->matte_color[3] = theft_random(theft) % 256;
    b->compositing_mode = (BitmapCompositingMode)(theft_random(theft) % 3);
    return (void*)b;
}

void bitmapbgra_free(void * details, void * unused)
{
    BitmapBgra_destroy(c, (BitmapBgra *)details);
}
/*
InterpolationDetails * interpolation;
    //How large the interoplation window needs to be before we even attempt to apply a sharpening
    //percentage to the given filter
    float minimum_sample_window_to_interposharpen;

    // If ossible to do correctly, halve the image until it is [interpolate_last_percent] times larger than needed. 3 or greater reccomended. Specify -1 to disable halving.
    float interpolate_last_percent;

    //If true, only halve when both dimensions are multiples of the halving factor
    bool halve_only_when_common_factor;

    //The actual halving factor to use.
    uint32_t halving_divisor;

    //The first convolution to apply
    ConvolutionKernel * kernel_a;
    //A second convolution to apply
    ConvolutionKernel * kernel_b;


    //If greater than 0, a percentage to sharpen the result along each axis;
    float sharpen_percent_goal;

    //If true, we should apply the color matrix
    bool apply_color_matrix;

    float color_matrix_data[25];
    float *color_matrix[5];

    //Enables profiling
    bool enable_profiling;

*/





void reboot_context(){ if (c != NULL){ Context_destroy(c); } c = Context_create();}
void destroy_context(){ if (c != NULL){ Context_destroy(c); c = NULL;}}


TEST_CASE("TestRender", "[fastscaling][thief]") {
    setbuf(stdout, NULL);
    reboot_context();


    theft_seed seed;
    if (!get_time_seed(&seed)) REQUIRE(false);


    theft_type_info renderdetails_type_info;
    memset(&renderdetails_type_info, 0, sizeof renderdetails_type_info);
    renderdetails_type_info.alloc = renderdetails_random;
    renderdetails_type_info.free = renderdetails_free;

    theft_type_info src_type_info;
    memset(&src_type_info, 0, sizeof src_type_info);
    src_type_info.alloc = BitmapBgra_random_zeroed_source;
    src_type_info.free = bitmapbgra_free;

    theft_type_info dest_type_info;
    memset(&dest_type_info, 0, sizeof dest_type_info);
    dest_type_info.alloc = BitmapBgra_random_zeroed_source;
    dest_type_info.free = bitmapbgra_free;

    theft_trial_report report;

    theft_cfg cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.seed = seed;
    cfg.name = __func__;
    cfg.fun = (theft_propfun*)render_should_succeed;
    cfg.type_info[0] = &renderdetails_type_info;
    cfg.type_info[1] = &src_type_info;
    cfg.type_info[2] = &dest_type_info;
    cfg.trials = 100;
    cfg.report = &report;

    theft * t = theft_init(0);
    theft_run_res res = theft_run(t, &cfg);
    theft_free(t);
    printf("\n");
    REQUIRE(THEFT_RUN_PASS == res);
    destroy_context();
}

static theft_trial_res before_and_after_should_match(float * bgra_array) {
    float copy[4];
    memcpy(copy, bgra_array, sizeof copy);
    linear_to_luv(copy);
    luv_to_linear(copy);
    for (int i = 0; i < 4; i++) {
        //printf("bgra_array is %f\n", bgra_array[i]);
        //printf("copy       is %f\n\n", copy[i]);
        if (bgra_array[i] != Approx(copy[i])) {
            return THEFT_TRIAL_FAIL;
        }
    }
    return THEFT_TRIAL_PASS;
}

void * allocate_random_bitmap(theft * theft, theft_seed seed, void * input) {
    float * bitmap = (float*)calloc(4, sizeof(float));
    for (int i = 0; i < 4; i++)
        bitmap[i] = (float)theft_random_double(theft);
    return (void*)bitmap;
}

void free_bitmap(void * bitmap, void * unused)
{
    free(bitmap);
}


TEST_CASE("Roundtrip RGB<->LUV property", "[fastscaling][thief]") {
    setbuf(stdout, NULL);
    reboot_context();


    theft_seed seed;
    if (!get_time_seed(&seed)) REQUIRE(false);


    theft_type_info bitmap_type_info;
    memset(&bitmap_type_info, 0, sizeof bitmap_type_info);
    bitmap_type_info.alloc = allocate_random_bitmap;
    bitmap_type_info.free = free_bitmap;

    theft_trial_report report;

    theft_cfg cfg;
    memset(&cfg, 0, sizeof cfg);
    cfg.seed = seed;
    cfg.name = __func__;
    cfg.fun = (theft_propfun*)before_and_after_should_match;
    cfg.type_info[0] = &bitmap_type_info;
    cfg.trials = 100000;
    cfg.report = &report;

    theft * t = theft_init(0);
    theft_run_res res = theft_run(t, &cfg);
    theft_free(t);
    printf("\n");
    REQUIRE(THEFT_RUN_PASS == res);
    destroy_context();
}
