﻿/**
 * File:   soft_g2d.c
 * Author: AWTK Develop Team
 * Brief:  soft g2d
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-05-09 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "base/utils.h"
#include "blend/soft_g2d.h"
#include "blend/pixel_pack_unpack.h"

ret_t soft_fill_rect(bitmap_t* fb, rect_t* dst, color_t c) {
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  uint32_t bpp = 0;
  uint32_t data = 0;
  uint32_t offset = 0;
  uint32_t* p = NULL;
  return_value_if_fail(fb != NULL && dst != NULL, RET_BAD_PARAMS);
  return_value_if_fail(fb->format == BITMAP_FMT_RGB565 || fb->format == BITMAP_FMT_RGBA,
                       RET_BAD_PARAMS);

  w = dst->w;
  h = dst->h;
  offset = (fb->w - dst->w);

  bpp = fb->format == BITMAP_FMT_RGB565 ? 2 : 4;
  p = (uint32_t*)((fb->data) + (dst->y * fb->w + dst->x) * bpp);
  if (fb->format == BITMAP_FMT_RGB565) {
    uint32_t fbw = fb->w;
    uint16_t* p16 = (uint16_t*)p;
    uint16_t d16 = rgb_to_565(c.rgba.r, c.rgba.g, c.rgba.b);

    for (y = 0; y < h; y++) {
      tk_memset16(p16, d16, w);
      p16 += fbw;
    }

    return RET_OK;
  } else if (fb->format == BITMAP_FMT_RGBA) {
    data = rgb_to_rgba8888(c.rgba.r, c.rgba.g, c.rgba.b);
  } else {
    data = rgb_to_image8888(c.rgba.r, c.rgba.g, c.rgba.b);
  }

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      *p++ = data;
    }
    p += offset;
  }

  return RET_OK;
}

ret_t soft_copy_image(bitmap_t* fb, bitmap_t* img, rect_t* src, xy_t dx, xy_t dy) {
  uint32_t i = 0;
  uint32_t bpp = 0;
  uint32_t size = 0;
  uint8_t* src_p = NULL;
  uint8_t* dst_p = NULL;
  return_value_if_fail(fb != NULL && img != NULL && src != NULL, RET_BAD_PARAMS);
  return_value_if_fail(fb->format == img->format, RET_BAD_PARAMS);
  return_value_if_fail(fb->format == BITMAP_FMT_RGB565 || fb->format == BITMAP_FMT_RGBA ||
                           fb->format == BITMAP_FMT_BGRA,
                       RET_BAD_PARAMS);

  bpp = fb->format == BITMAP_FMT_RGB565 ? 2 : 4;
  src_p = (uint8_t*)(img->data) + (src->y * img->w + src->x) * bpp;
  dst_p = (uint8_t*)(fb->data) + (dy * fb->w + dx) * bpp;

  if (fb->w == img->w && fb->h == img->h && src->w == img->w && src->x == 0) {
    size = (img->w * src->h);
    tk_pixel_copy(dst_p, src_p, size, bpp);

    return RET_OK;
  } else {
    size = src->w;
    for (i = 0; i < src->h; i++) {
      tk_pixel_copy(dst_p, src_p, size, bpp);
      dst_p += bpp * fb->w;
      src_p += bpp * img->w;
    }
  }

  return RET_OK;
}

ret_t soft_blend_image(bitmap_t* fb, bitmap_t* img, rect_t* dst, rect_t* src, uint8_t alpha) {
  return_value_if_fail(fb != NULL && img != NULL && src != NULL && dst != NULL, RET_BAD_PARAMS);
  return_value_if_fail(fb->format == BITMAP_FMT_RGB565 || fb->format == BITMAP_FMT_RGBA,
                       RET_BAD_PARAMS);
  return_value_if_fail(img->format == BITMAP_FMT_RGB565 || img->format == BITMAP_FMT_RGBA,
                       RET_BAD_PARAMS);

  if (fb->format == BITMAP_FMT_RGB565) {
    if (img->format == BITMAP_FMT_RGB565) {
      return blend_image_565_565(fb, img, dst, src, alpha);
    } else {
      return blend_image_565_8888(fb, img, dst, src, alpha);
    }
  } else {
    if (img->format == BITMAP_FMT_RGB565) {
      return blend_image_8888_565(fb, img, dst, src, alpha);
    } else {
      return blend_image_8888_8888(fb, img, dst, src, alpha);
    }
  }
}
