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

    void impl_set_warning_fn(lua_State* L) {
      check_reader_handle(L, 1)->set_warning_fn(L, 2);
      luaX_push_success(L);
    }

    void impl_set_read_fn(lua_State* L) {
      check_reader_handle(L, 1)->set_read_fn(L, 2);
      luaX_push_success(L);
    }

    void impl_read_png(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      int transforms = luaX_opt_integer<int>(L, 2, PNG_TRANSFORM_IDENTITY);
      png_read_png(self->png(true), self->info(), transforms, 0);
      luaX_push_success(L);
    }

    void impl_read_info(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_read_info(self->png(true), self->info());
      luaX_push_success(L);
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
      if (png_get_IHDR(self->png(), self->info(), &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_type)) {
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
      luaX_push(L, luaX_string_reference(png_get_signature(self->png(), self->info()), 8));
    }

    void impl_get_tIME(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_timep mod_time = 0;
      if (png_get_tIME(self->png(), self->info(), &mod_time)) {
        lua_newtable(L);
        luaX_set_field(L, -1, "year", mod_time->year);
        luaX_set_field(L, -1, "month", mod_time->month);
        luaX_set_field(L, -1, "day", mod_time->day);
        luaX_set_field(L, -1, "hour", mod_time->hour);
        luaX_set_field(L, -1, "min", mod_time->minute);
        luaX_set_field(L, -1, "sec", mod_time->second);
      }
    }

    void impl_get_text(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_textp text = 0;
      int num_text = 0;
      if (png_get_text(self->png(), self->info(), &text, &num_text)) {
        lua_newtable(L);
        for (int i = 0; i < num_text; ++i) {
          lua_newtable(L);
          luaX_set_field(L, -1, "compression", text[i].compression);
          luaX_set_field(L, -1, "key", text[i].key);
          switch (text[i].compression) {
            case PNG_TEXT_COMPRESSION_NONE:
            case PNG_TEXT_COMPRESSION_zTXt:
              luaX_set_field(L, -1, "text", luaX_string_reference(text[i].text, text[i].text_length));
              break;
            case PNG_ITXT_COMPRESSION_NONE:
            case PNG_ITXT_COMPRESSION_zTXt:
#if PNG_LIBPNG_VER >= 10500 || defined(PNG_iTXt_SUPPORTED)
              luaX_set_field(L, -1, "text", luaX_string_reference(text[i].text, text[i].itxt_length));
              luaX_set_field(L, -1, "lang", text[i].lang);
              luaX_set_field(L, -1, "lang_key", text[i].lang_key);
#endif
              break;
          }
          luaX_set_field(L, -2, i + 1);
        }
      }
    }

    void impl_get_oFFs(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_int_32 offset_x = 0;
      png_int_32 offset_y = 0;
      int unit_type = 0;
      if (png_get_oFFs(self->png(), self->info(), &offset_x, &offset_y, &unit_type)) {
        lua_newtable(L);
        luaX_set_field(L, -1, "offset_x", offset_x);
        luaX_set_field(L, -1, "offset_y", offset_y);
        luaX_set_field(L, -1, "unit_type", unit_type);
      }
    }

    void impl_get_x_offset_microns(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_x_offset_microns(self->png(), self->info()));
    }

    void impl_get_y_offset_microns(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_y_offset_microns(self->png(), self->info()));
    }

    void impl_get_pHYs(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_uint_32 res_x = 0;
      png_uint_32 res_y = 0;
      int unit_type = 0;
      if (png_get_pHYs(self->png(), self->info(), &res_x, &res_y, &unit_type)) {
        lua_newtable(L);
        luaX_set_field(L, -1, "res_x", res_x);
        luaX_set_field(L, -1, "res_y", res_y);
        luaX_set_field(L, -1, "unit_type", unit_type);
      }
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

    void impl_get_pixel_aspect_ratio(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      luaX_push(L, png_get_pixel_aspect_ratio(self->png(), self->info()));
    }

    void impl_get_row(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_uint_32 height = png_get_image_height(self->png(), self->info());
      png_uint_32 y = luaX_check_integer<png_uint_32>(L, 2, 1, height);
      if (png_bytepp row_pointers = png_get_rows(self->png(), self->info())) {
        png_size_t rowbytes = png_get_rowbytes(self->png(), self->info());
        luaX_push(L, luaX_string_reference(row_pointers[y - 1], rowbytes));
      } else {
        png_error(self->png(), "row_pointer not prepared");
      }
    }
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
      luaX_set_field(L, -1, "set_sig_bytes", impl_set_sig_bytes);
      luaX_set_field(L, -1, "set_warning_fn", impl_set_warning_fn);
      luaX_set_field(L, -1, "set_read_fn", impl_set_read_fn);
      luaX_set_field(L, -1, "read_png", impl_read_png);
      luaX_set_field(L, -1, "read_info", impl_read_info);
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
      luaX_set_field(L, -1, "get_tIME", impl_get_tIME);
      luaX_set_field(L, -1, "get_text", impl_get_text);
      luaX_set_field(L, -1, "get_oFFs", impl_get_oFFs);
      luaX_set_field(L, -1, "get_x_offset_microns", impl_get_x_offset_microns);
      luaX_set_field(L, -1, "get_y_offset_microns", impl_get_y_offset_microns);
      luaX_set_field(L, -1, "get_pHYs", impl_get_pHYs);
      luaX_set_field(L, -1, "get_x_pixels_per_meter", impl_get_x_pixels_per_meter);
      luaX_set_field(L, -1, "get_y_pixels_per_meter", impl_get_y_pixels_per_meter);
      luaX_set_field(L, -1, "get_pixels_per_meter", impl_get_pixels_per_meter);
      luaX_set_field(L, -1, "get_pixel_aspect_ratio", impl_get_pixel_aspect_ratio);
      luaX_set_field(L, -1, "get_row", impl_get_row);
    }
    luaX_set_field(L, -2, "reader");
  }
}
