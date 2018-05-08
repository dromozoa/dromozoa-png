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
      try {
        luaX_new<reader_handle>(L, reader_handle::create());
        luaX_set_metatable(L, "dromozoa.png.reader");
      } catch (const png_runtime_error& e) {
        luaX_push(L, luaX_nil);
        luaX_push(L, e.what());
      }
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
      try {
        reader_handle* self = check_reader_handle(L, 1);
        png_read_info(self->png(), self->info());
        luaX_push_success(L);
      } catch (const png_runtime_error& e) {
        luaX_push(L, luaX_nil);
        luaX_push(L, e.what());
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
      luaX_set_field(L, -1, "set_read_fn", impl_set_read_fn);
      luaX_set_field(L, -1, "read_info", impl_read_info);
    }
    luaX_set_field(L, -2, "reader");
  }
}
