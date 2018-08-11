#ifndef __libsv_sv_image_h__
#define __libsv_sv_image_h__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SV_IMAGE_MAX_CHANNEL    3

/// format bit layout
/// 0      8      16     24     32
/// |  id  | Ybpp | Ubpp | Vbpp | 

#define SV_MAKE_PIX_FMT(id, Ybpp, Ubpp, Vbpp) (Ybpp | (Ubpp << 8) | (Vbpp << 16) | (id << 24))
#define SV_GET_FMT_ID(fmt)   ((fmt & 0xff000000) >> 24)
#define SV_GET_FMT_YBPP(fmt) (fmt & 0x000000ff)
#define SV_GET_FMT_UBPP(fmt) ((fmt & 0x0000ff00) >> 8)
#define SV_GET_FMT_VBPP(fmt) ((fmt & 0x00ff0000) >> 16)


typedef enum{
    SV_PIX_FMT_GRAY8    = SV_MAKE_PIX_FMT(0, 8, 0, 0),
    SV_PIX_FMT_BGR   = SV_MAKE_PIX_FMT(1, 24, 0, 0),
    SV_PIX_FMT_BGRA = SV_MAKE_PIX_FMT(2, 32, 0, 0),        
    SV_PIX_FMT_RGB   = SV_MAKE_PIX_FMT(3, 24, 0, 0),        
    SV_PIX_FMT_RGBA = SV_MAKE_PIX_FMT(4, 32, 0, 0),        
    SV_PIX_FMT_J420     = SV_MAKE_PIX_FMT(5, 8, 2, 2), 
    SV_PIX_FMT_NV12     = SV_MAKE_PIX_FMT(6, 8, 4, 0),        
    SV_PIX_FMT_NV21     = SV_MAKE_PIX_FMT(7, 8, 4, 0),        // plane: 1; bbp: 32
}sv_pix_fmt_t;

enum{
    SV_ORIENT_TL = 1, ///< Horizontal (normal)
    SV_ORIENT_TR = 2, ///< Mirrored horizontal
    SV_ORIENT_BR = 3, ///< Rotate 180
    SV_ORIENT_BL = 4, ///< Mirrored vertical
    SV_ORIENT_LT = 5, ///< Mirrored horizontal & rotate 270 CW
    SV_ORIENT_RT = 6, ///< Rotate 90 CW
    SV_ORIENT_RB = 7, ///< Mirrored horizontal & rotate 90 CW
    SV_ORIENT_LB = 8  ///< Rotate 270 CW
};

typedef struct
{
    union {
        uint8_t *data = NULL;
        uint8_t *y;
    };
    uint8_t *u = NULL;
    uint8_t *v = NULL;

    union {
        int stride = NULL;
        int ystride;
    };

    int ustride = NULL;
    int vstride = NULL;
    int width = 0;
    int height = 0;
    sv_pix_fmt_t format = SV_PIX_FMT_GRAY8;
    int orient = 1;
    int *refcount = NULL; ///< NULL for external data,
                          ///< NonNULL for internal data
} sv_image_t;

sv_image_t * sv_image_create(int width=0, int height=0, sv_pix_fmt_t fmt=SV_PIX_FMT_GRAY8, int orient=1);
void         sv_image_destroy(sv_image_t* image);
int          sv_image_size(const sv_image_t* image);
int          sv_image_capacity(const sv_image_t * image);

sv_image_t * sv_image_clone(const sv_image_t* src);
void         sv_image_copyTo(const sv_image_t* src, sv_image_t* dst);
void         sv_image_swap(sv_image_t* lhs, sv_image_t* rhs);

sv_image_t * sv_image_rotate(const sv_image_t* image, int dst_orient);
sv_image_t * sv_image_convert(const sv_image_t* image, sv_pix_fmt_t dst_fmt);
sv_image_t * sv_image_resize(const sv_image_t* image, int dst_width, int dst_height);
sv_image_t * sv_image_roi(const sv_image_t* image, int x, int y, int width, int height);
sv_image_t * sv_image_crop(const sv_image_t* image, int x, int y, int width, int height);
sv_image_t * sv_image_warp(const sv_image_t* image, const float M[6], int width, int height);

sv_image_t * sv_image_from_plane_1(int width, int height, sv_pix_fmt_t fmt, uint8_t* data, int stride, int orient = 1);
sv_image_t * sv_image_from_plane_2(
                    int width, int height, sv_pix_fmt_t fmt, 
                    uint8_t* data, 
                    int stride, 
                    int orient = 1);

#define sv_image_from_gray(width, height, data, stride...) \
        sv_image_from_plane_1(width, height, SV_PIX_FMT_GRAY8, data, ## stride)

#define sv_image_from_bgr(width, height, data, stride...) \
        sv_image_from_plane_1(width, height, SV_PIX_FMT_BGR, data, ## stride)

#define sv_image_from_bgra(width, height, data, stride...) \
        sv_image_from_plane_1(width, height, SV_PIX_FMT_BGRA, data, ## stride)

#define sv_image_from_rgb(width, height, data, stride...) \
        sv_image_from_plane_1(width, height, SV_PIX_FMT_RGB, data, ## stride)

#define sv_image_from_rgba(width, height, data, stride...) \
        sv_image_from_plane_1(width, height, SV_PIX_FMT_RGBA, data, ## stride)

const char*  sv_image_fmt_str(int fmt);

void sv_image_str(const sv_image_t* image);

#ifdef __cplusplus
}
#endif


#endif  // __libsv_sv_image_h__
