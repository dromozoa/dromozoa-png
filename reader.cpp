// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-png.
//
// dromozoa-png is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-png is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-png.  If not, see <http://www.gnu.org/licenses/>.

#include <vector>

#include "common.hpp"

namespace dromozoa {
  namespace {
    void push_row_pointers(lua_State* L, png_bytepp row_pointers, png_uint_32 height, png_size_t rowbytes) {
      lua_newtable(L);
      for (png_uint_32 i = 0; i < height; ++i) {
        lua_pushlstring(L, reinterpret_cast<const char*>(row_pointers[i]), rowbytes);
        luaX_set_field(L, -2, i + 1);
      }
    }

    reader_handle* check_reader_handle(lua_State* L, int arg) {
      return luaX_check_udata<reader_handle>(L, arg, "dromozoa.png.reader");
    }

    void impl_gc(lua_State* L) {
      check_reader_handle(L, 1)->~reader_handle();
    }

    void impl_call(lua_State* L) {
      luaX_new<reader_handle>(L, reader_handle::create());
      luaX_set_metatable(L, "dromozoa.png.reader");
    }

    void impl_destroy(lua_State* L) {
      check_reader_handle(L, 1)->destroy();
      luaX_push_success(L);
    }

    void impl_set_sig_bytes(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      int num_bytes = luaX_check_integer<int>(L, 2);
      png_set_sig_bytes(self->png(), num_bytes);
      luaX_push_success(L);
    }

    void impl_set_read_fn(lua_State* L) {
      check_reader_handle(L, 1)->set_read_fn(L, 2);
      luaX_push_success(L);
    }

    void impl_read_png(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      int transforms = luaX_opt_integer<int>(L, 2, PNG_TRANSFORM_IDENTITY);
      png_read_png(self->png(), self->info(), transforms, 0);
      luaX_push_success(L);
    }

    void impl_read_info(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_read_info(self->png(), self->info());
      luaX_push_success(L);
    }

    void impl_read_image(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_uint_32 height = png_get_image_height(self->png(), self->info());
      png_size_t rowbytes = png_get_rowbytes(self->png(), self->info());
      std::vector<png_byte> row_storage(height * rowbytes);
      std::vector<png_bytep> row_pointers(height);
      for (png_uint_32 i = 0; i < height; ++i) {
        row_pointers[i] = &row_storage[i * rowbytes];
      }
      png_read_image(self->png(), &row_pointers[0]);
      push_row_pointers(L, &row_pointers[0], height, rowbytes);
    }

    void impl_read_end(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_read_end(self->png(), self->end());
      luaX_push_success(L);
    }

    void impl_get_rows(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_uint_32 height = png_get_image_height(self->png(), self->info());
      png_size_t rowbytes = png_get_rowbytes(self->png(), self->info());
      if (png_bytepp row_pointers = png_get_rows(self->png(), self->info())) {
        push_row_pointers(L, row_pointers, height, rowbytes);
      }
    }

    void impl_get_valid(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      int flag = luaX_check_integer<png_uint_32>(L, 2);
      luaX_push(L, png_get_valid(self->png(), self->info(), flag) != 0);
    }

    void impl_get_IHDR(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_uint_32 width = 0;
      png_uint_32 height = 0;
      int bit_depth = 0;
      int color_type = 0;
      int interlace_type = 0;
      int compression_type = 0;
      int filter_type = 0;
      if (png_get_IHDR(self->png(), self->info(), &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_type) != 0) {
        lua_newtable(L);
        luaX_set_field(L, -1, "width", width);
        luaX_set_field(L, -1, "height", height);
        luaX_set_field(L, -1, "bit_depth", bit_depth);
        luaX_set_field(L, -1, "color_type", color_type);
        luaX_set_field(L, -1, "interlace_type", interlace_type);
        luaX_set_field(L, -1, "compression_type", compression_type);
        luaX_set_field(L, -1, "filter_type", filter_type);
      }
    }

    void impl_get_image_width(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_image_width(self->png(), self->info()));
    }

    void impl_get_image_height(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_image_height(self->png(), self->info()));
    }

    void impl_get_bit_depth(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_bit_depth(self->png(), self->info()));
    }

    void impl_get_color_type(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_color_type(self->png(), self->info()));
    }

    void impl_get_interlace_type(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_interlace_type(self->png(), self->info()));
    }

    void impl_get_compression_type(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_compression_type(self->png(), self->info()));
    }

    void impl_get_filter_type(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_filter_type(self->png(), self->info()));
    }

    void impl_get_channels(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_channels(self->png(), self->info()));
    }

    void impl_get_rowbytes(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_rowbytes(self->png(), self->info()));
    }

    void impl_get_signature(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, reinterpret_cast<const char*>(png_get_signature(self->png(), self->info())), 8);
    }

    void impl_get_x_pixels_per_meter(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_x_pixels_per_meter(self->png(), self->info()));
    }

    void impl_get_y_pixels_per_meter(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_y_pixels_per_meter(self->png(), self->info()));
    }

    void impl_get_pixels_per_meter(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_pixels_per_meter(self->png(), self->info()));
    }

    void impl_get_x_pixels_per_inch(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_x_pixels_per_inch(self->png(), self->info()));
    }

    void impl_get_y_pixels_per_inch(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_y_pixels_per_inch(self->png(), self->info()));
    }

    void impl_get_pixels_per_inch(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_pixels_per_inch(self->png(), self->info()));
    }

    void impl_get_pixel_aspect_ratio(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_pixel_aspect_ratio(self->png(), self->info()));
    }

    void impl_get_x_offset_microns(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_x_offset_microns(self->png(), self->info()));
    }

    void impl_get_y_offset_microns(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_y_offset_microns(self->png(), self->info()));
    }

    void impl_get_x_offset_inches(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_x_offset_inches(self->png(), self->info()));
    }

    void impl_get_y_offset_inches(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_y_offset_inches(self->png(), self->info()));
    }

    void impl_set_pallete_to_rgb(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_set_palette_to_rgb(self->png());
      luaX_push_success(L);
    }

    void impl_set_tRNS_to_alpha(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_set_tRNS_to_alpha(self->png());
      luaX_push_success(L);
    }

    void impl_set_expand_gray_1_2_4_to_8(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_set_expand_gray_1_2_4_to_8(self->png());
      luaX_push_success(L);
    }

#if PNG_LIBPNG_VER >= 10502
    void impl_set_expand_16(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_set_expand_16(self->png());
      luaX_push_success(L);
    }
#endif
  }

  void initialize_reader(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.png.reader");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "destroy", impl_destroy);
      luaX_set_field(L, -1, "set_read_fn", impl_set_read_fn);
      luaX_set_field(L, -1, "set_sig_bytes", impl_set_sig_bytes);

      luaX_set_field(L, -1, "read_png", impl_read_png);
      luaX_set_field(L, -1, "read_info", impl_read_info);
      luaX_set_field(L, -1, "read_image", impl_read_image);
      luaX_set_field(L, -1, "read_end", impl_read_end);

      luaX_set_field(L, -1, "get_rows", impl_get_rows);

      luaX_set_field(L, -1, "get_valid", impl_get_valid);
      luaX_set_field(L, -1, "get_IHDR", impl_get_IHDR);
      luaX_set_field(L, -1, "get_image_width", impl_get_image_width);
      luaX_set_field(L, -1, "get_image_height", impl_get_image_height);
      luaX_set_field(L, -1, "get_bit_depth", impl_get_bit_depth);
      luaX_set_field(L, -1, "get_color_type", impl_get_color_type);
      luaX_set_field(L, -1, "get_interlace_type", impl_get_interlace_type);
      luaX_set_field(L, -1, "get_compression_type", impl_get_compression_type);
      luaX_set_field(L, -1, "get_filter_type", impl_get_filter_type);
      luaX_set_field(L, -1, "get_channels", impl_get_channels);
      luaX_set_field(L, -1, "get_rowbytes", impl_get_rowbytes);
      luaX_set_field(L, -1, "get_signature", impl_get_signature);

      luaX_set_field(L, -1, "get_x_pixels_per_meter", impl_get_x_pixels_per_meter);
      luaX_set_field(L, -1, "get_y_pixels_per_meter", impl_get_y_pixels_per_meter);
      luaX_set_field(L, -1, "get_pixels_per_meter", impl_get_pixels_per_meter);
      luaX_set_field(L, -1, "get_x_pixels_per_inch", impl_get_x_pixels_per_inch);
      luaX_set_field(L, -1, "get_y_pixels_per_inch", impl_get_y_pixels_per_inch);
      luaX_set_field(L, -1, "get_pixels_per_inch", impl_get_pixels_per_inch);
      luaX_set_field(L, -1, "get_pixel_aspect_ratio", impl_get_pixel_aspect_ratio);

      luaX_set_field(L, -1, "get_x_offset_microns", impl_get_x_offset_microns);
      luaX_set_field(L, -1, "get_y_offset_microns", impl_get_y_offset_microns);
      luaX_set_field(L, -1, "get_x_offset_inches", impl_get_x_offset_inches);
      luaX_set_field(L, -1, "get_y_offset_inches", impl_get_y_offset_inches);

      luaX_set_field(L, -1, "set_pallete_to_rgb", impl_set_pallete_to_rgb);
      luaX_set_field(L, -1, "set_tRNS_to_alpha", impl_set_tRNS_to_alpha);
      luaX_set_field(L, -1, "set_expand_gray_1_2_4_to_8", impl_set_expand_gray_1_2_4_to_8);
#if PNG_LIBPNG_VER >= 10502
      luaX_set_field(L, -1, "set_expand_16", impl_set_expand_16);
#endif
    }
    luaX_set_field(L, -2, "reader");
  }
}
