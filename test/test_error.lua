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

local verbose = os.getenv "VERBOSE" == "1"

local reader = assert(png.reader())
local result, message = reader:read_png()
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

local writer = assert(png.writer())
assert(writer:set_IHDR {
  width = 1;
  height = 1;
  bit_depth = 8;
  color_type = png.PNG_COLOR_TYPE_RGB;
})
assert(writer:set_row(1, "\0\0\0"))
local result, message = writer:write_png()
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
