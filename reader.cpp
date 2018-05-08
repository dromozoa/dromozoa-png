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

    void impl_set_read_fn(lua_State* L) {
      check_reader_handle(L, 1)->set_read_fn(L, 2);
      luaX_push_success(L);
    }

    void impl_read_info(lua_State* L) {
      reader_handle* self = check_reader_handle(L, 1);
      png_read_info(self->png(), self->info());
      luaX_push_success(L);
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
      luaX_set_field(L, -1, "read_info", impl_read_info);
      luaX_set_field(L, -1, "get_image_width", impl_get_image_width);
      luaX_set_field(L, -1, "get_image_height", impl_get_image_height);
      luaX_set_field(L, -1, "get_bit_depth", impl_get_bit_depth);
      luaX_set_field(L, -1, "get_color_type", impl_get_color_type);
      luaX_set_field(L, -1, "get_interlace_type", impl_get_interlace_type);
      luaX_set_field(L, -1, "get_compression_type", impl_get_compression_type);
      luaX_set_field(L, -1, "get_filter_type", impl_get_filter_type);
    }
    luaX_set_field(L, -2, "reader");
  }
}
