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
  void initialize_main(lua_State* L);
  void initialize_reader(lua_State* L);
  void initialize_writer(lua_State* L);

  void initialize(lua_State* L) {
    initialize_main(L);
    initialize_reader(L);
    initialize_writer(L);
  }
}

extern "C" int luaopen_dromozoa_png(lua_State* L) {
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
