-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-png.
--
-- dromozoa-png is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-png is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-png.  If not, see <http://www.gnu.org/licenses/>.

local png = require "dromozoa.png"

local mode, filename = ...

local reader = assert(png.reader())

local handle = assert(io.open(filename, "rb"))
assert(reader:set_read_fn(function (n)
  return handle:read(n)
end))

if mode == "read_info" then
  assert(reader:read_info())
else
  assert(reader:read_png())
end

local IHDR = assert(reader:get_IHDR())
print(IHDR.width)
print(IHDR.height)
print(IHDR.bit_depth)
print(IHDR.color_type)
