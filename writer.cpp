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

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>

#include "common.hpp"

namespace dromozoa {
  namespace {
    writer_handle* check_writer_handle(lua_State* L, int arg) {
      return luaX_check_udata<writer_handle>(L, arg, "dromozoa.png.writer");
    }

    void impl_gc(lua_State* L) {
      check_writer_handle(L, 1)->~writer_handle();
    }

    void impl_call(lua_State* L) {
      luaX_new<writer_handle>(L, writer_handle::create());
      luaX_set_metatable(L, "dromozoa.png.writer");
    }

    void impl_destroy(lua_State* L) {
      check_writer_handle(L, 1)->destroy();
      luaX_push_success(L);
    }

    void impl_set_sig_bytes(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      int num_bytes = luaX_check_integer<int>(L, 2);
      png_set_sig_bytes(self->png(), num_bytes);
      luaX_push_success(L);
    }

    void impl_set_warning_fn(lua_State* L) {
      check_writer_handle(L, 1)->set_warning_fn(L, 2);
      luaX_push_success(L);
    }

    void impl_set_write_fn(lua_State* L) {
      check_writer_handle(L, 1)->set_write_fn(L, 2, 3);
      luaX_push_success(L);
    }

    void impl_set_flush(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      int nrows = luaX_check_integer<int>(L, 2);
      png_set_flush(self->png(), nrows);
      luaX_push_success(L);
    }

    void impl_set_IHDR(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      png_uint_32 width = luaX_check_integer_field<png_uint_32>(L, 2, "width");
      png_uint_32 height = luaX_check_integer_field<png_uint_32>(L, 2, "height");
      int bit_depth = luaX_check_integer_field<int>(L, 2, "bit_depth");
      int color_type = luaX_check_integer_field<int>(L, 2, "color_type");
      int interlace_type = luaX_opt_integer_field<int>(L, 2, "interlace_type", PNG_INTERLACE_NONE);
      int compression_type = luaX_opt_integer_field<int>(L, 2, "compression_type", PNG_COMPRESSION_TYPE_DEFAULT);
      int filter_type = luaX_opt_integer_field<int>(L, 2, "filter_type", PNG_FILTER_TYPE_DEFAULT);
      png_set_IHDR(self->png(), self->info(), width, height, bit_depth, color_type, interlace_type, compression_type, filter_type);
      luaX_push_success(L);
    }

    void impl_set_tIME(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      png_time mod_time = {
        luaX_check_integer_field<png_uint_16>(L, 2, "year"),
        luaX_check_integer_field<png_byte>(L, 2, "month", 1, 12),
        luaX_check_integer_field<png_byte>(L, 2, "day", 1, 31),
        luaX_check_integer_field<png_byte>(L, 2, "hour", 0, 23),
        luaX_check_integer_field<png_byte>(L, 2, "min", 0, 59),
        luaX_check_integer_field<png_byte>(L, 2, "sec", 0, 60),
      };
      png_set_tIME(self->png(), self->info(), &mod_time);
      luaX_push_success(L);
    }

    void impl_set_text(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      self->set_text(L, 2);
      luaX_push_success(L);
    }

    void impl_set_oFFs(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      png_int_32 offset_x = luaX_check_integer_field<png_uint_32>(L, 2, "offset_x");
      png_int_32 offset_y = luaX_check_integer_field<png_uint_32>(L, 2, "offset_y");
      int unit_type = luaX_check_integer_field<int>(L, 2, "unit_type");
      png_set_oFFs(self->png(), self->info(), offset_x, offset_y, unit_type);
      luaX_push_success(L);
    }

    void impl_set_pHYs(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      png_uint_32 res_x = luaX_check_integer_field<png_uint_32>(L, 2, "res_x");
      png_uint_32 res_y = luaX_check_integer_field<png_uint_32>(L, 2, "res_y");
      int unit_type = luaX_check_integer_field<int>(L, 2, "unit_type");
      png_set_pHYs(self->png(), self->info(), res_x, res_y, unit_type);
      luaX_push_success(L);
    }

    void impl_set_rows(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      png_uint_32 height = png_get_image_height(self->png(), self->info());
      png_size_t rowbytes = png_get_rowbytes(self->png(), self->info());
      if (png_bytepp row_pointers = self->prepare_rows(height, rowbytes)) {
        for (png_uint_32 y = 0; y < height; ++y) {
          luaX_get_field(L, 2, y + 1);
          if (luaX_string_reference source = luaX_to_string(L, -1)) {
            memcpy(row_pointers[y], source.data(), std::min(rowbytes, source.size()));
          }
          lua_pop(L, 1);
        }
        luaX_push_success(L);
      } else {
        png_error(self->png(), "row_pointer not prepared");
      }
    }

    void impl_set_row(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      png_uint_32 height = png_get_image_height(self->png(), self->info());
      png_uint_32 y = luaX_check_integer<png_uint_32>(L, 2, 1, height) - 1;
      luaX_string_reference source = luaX_check_string(L, 3);
      png_size_t rowbytes = png_get_rowbytes(self->png(), self->info());
      if (png_bytepp row_pointers = self->prepare_rows(height, rowbytes)) {
        memcpy(row_pointers[y], source.data(), std::min(rowbytes, source.size()));
        luaX_push_success(L);
      }
    }

    void impl_set_pixel(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      png_uint_32 width = png_get_image_width(self->png(), self->info());
      png_uint_32 height = png_get_image_height(self->png(), self->info());
      png_uint_32 x = luaX_check_integer<png_uint_32>(L, 2, 1, width) - 1;
      png_uint_32 y = luaX_check_integer<png_uint_32>(L, 3, 1, height) - 1;

      png_byte bit_depth = png_get_bit_depth(self->png(), self->info());
      if (bit_depth != 8 && bit_depth != 16) {
        png_error(self->png(), "unexpected bit_depth (8 or 16 expected)");
        return;
      }
      if (png_get_color_type(self->png(), self->info()) == PNG_COLOR_TYPE_PALETTE) {
        png_error(self->png(), "PNG_COLOR_TYPE_PALETTE not supported");
        return;
      }
      png_byte channels = png_get_channels(self->png(), self->info());
      if (channels < 1 || 4 < channels) {
        png_error(self->png(), "unexpected channels (1, 2, 3 or 4 expected");
        return;
      }

      png_size_t rowbytes = png_get_rowbytes(self->png(), self->info());
      if (png_bytepp row_pointers = self->prepare_rows(height, rowbytes)) {
        png_bytep p = row_pointers[y] + (bit_depth * channels >> 3) * x;
        if (bit_depth == 8) {
          p[0] = luaX_opt_integer<uint8_t>(L, 4, 0);
          switch (channels) {
            case 4: p[3] = luaX_opt_integer<uint8_t>(L, 7, 0); // FALLTHROUGH
            case 3: p[2] = luaX_opt_integer<uint8_t>(L, 6, 0); // FALLTHROUGH
            case 2: p[1] = luaX_opt_integer<uint8_t>(L, 5, 0); // FALLTHROUGH
          }
        } else {
          // assume network byte order
          uint16_t v = luaX_opt_integer<uint16_t>(L, 4, 0);
          p[1] = v;
          p[0] = v >> 8;
          switch (channels) {
            case 4: v = luaX_opt_integer<uint16_t>(L, 7, 0); p[7] = v; p[6] = v >> 8; // FALLTHROUGH
            case 3: v = luaX_opt_integer<uint16_t>(L, 6, 0); p[5] = v; p[4] = v >> 8; // FALLTHROUGH
            case 2: v = luaX_opt_integer<uint16_t>(L, 5, 0); p[3] = v; p[2] = v >> 8; // FALLTHROUGH
          }
        }
        luaX_push_success(L);
      }
    }

    void impl_write_png(lua_State* L) {
      writer_handle* self = check_writer_handle(L, 1);
      int transforms = luaX_opt_integer<int>(L, 2, PNG_TRANSFORM_IDENTITY);
      png_write_png(self->png(), self->info(), transforms, 0);
      luaX_push_success(L);
    }
  }

  void initialize_writer(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.png.writer");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "destroy", impl_destroy);
      luaX_set_field(L, -1, "set_sig_bytes", impl_set_sig_bytes);
      luaX_set_field(L, -1, "set_warning_fn", impl_set_warning_fn);
      luaX_set_field(L, -1, "set_write_fn", impl_set_write_fn);
      luaX_set_field(L, -1, "set_flush", impl_set_flush);
      luaX_set_field(L, -1, "set_IHDR", impl_set_IHDR);
      luaX_set_field(L, -1, "set_tIME", impl_set_tIME);
      luaX_set_field(L, -1, "set_text", impl_set_text);
      luaX_set_field(L, -1, "set_oFFs", impl_set_oFFs);
      luaX_set_field(L, -1, "set_pHYs", impl_set_pHYs);
      luaX_set_field(L, -1, "set_rows", impl_set_rows);
      luaX_set_field(L, -1, "set_row", impl_set_row);
      luaX_set_field(L, -1, "set_pixel", impl_set_pixel);
      luaX_set_field(L, -1, "write_png", impl_write_png);
    }
    luaX_set_field(L, -2, "writer");
  }
}
