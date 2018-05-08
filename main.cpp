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
  void initialize_main(lua_State* L) {
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_GRAY", PNG_COLOR_TYPE_GRAY);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_GRAY_ALPHA", PNG_COLOR_TYPE_GRAY_ALPHA);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_PALETTE", PNG_COLOR_TYPE_PALETTE);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_RGB", PNG_COLOR_TYPE_RGB);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_RGB_ALPHA", PNG_COLOR_TYPE_RGB_ALPHA);
    luaX_set_field(L, -1, "PNG_COLOR_MASK_PALETTE", PNG_COLOR_MASK_PALETTE);
    luaX_set_field(L, -1, "PNG_COLOR_MASK_COLOR", PNG_COLOR_MASK_COLOR);
    luaX_set_field(L, -1, "PNG_COLOR_MASK_ALPHA", PNG_COLOR_MASK_ALPHA);
    luaX_set_field(L, -1, "PNG_INTERLACE_NONE", PNG_INTERLACE_NONE);
    luaX_set_field(L, -1, "PNG_INTERLACE_ADAM7", PNG_INTERLACE_ADAM7);
    luaX_set_field(L, -1, "PNG_COMPRESSION_TYPE_BASE", PNG_COMPRESSION_TYPE_BASE);
    luaX_set_field(L, -1, "PNG_FILTER_TYPE_BASE", PNG_FILTER_TYPE_BASE);
    luaX_set_field(L, -1, "PNG_INTRAPIXEL_DIFFERENCING", PNG_INTRAPIXEL_DIFFERENCING);
  }
}
