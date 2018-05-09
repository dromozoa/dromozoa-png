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

local handle = assert(io.open("docs/lenna.png", "rb"))
local header = handle:read(8)
if verbose then
  io.stderr:write(header:gsub("[%z\1-\31\127-\255]", function (source)
    return ([[\x%02x]]):format(source:byte(1, 1))
  end), "\n")
end
assert(png.sig_cmp(header) == 0)

assert(reader:set_sig_bytes(#header))
assert(reader:set_read_fn(function (n)
  return handle:read(n)
end))

assert(reader:read_png())

assert(not reader:get_valid(png.PNG_INFO_PLTE))
assert(not reader:get_valid(png.PNG_INFO_tRNS))
assert(reader:get_valid(png.PNG_INFO_gAMA))
assert(reader:get_valid(png.PNG_INFO_sRGB))

local IHDR = assert(reader:get_IHDR())
assert(IHDR.width == 271)
assert(IHDR.height == 580)
assert(IHDR.bit_depth == 8)
assert(IHDR.color_type == png.PNG_COLOR_TYPE_RGB_ALPHA)
assert(IHDR.interlace_type == png.PNG_INTERLACE_NONE)
assert(IHDR.compression_type == png.PNG_COMPRESSION_TYPE_BASE)
assert(IHDR.filter_type == png.PNG_FILTER_TYPE_BASE)

assert(reader:get_image_width() == 271)
assert(reader:get_image_height() == 580)
assert(reader:get_bit_depth() == 8)
assert(reader:get_color_type() == png.PNG_COLOR_TYPE_RGB_ALPHA)
assert(reader:get_interlace_type() == png.PNG_INTERLACE_NONE)
assert(reader:get_compression_type() == png.PNG_COMPRESSION_TYPE_BASE)
assert(reader:get_filter_type() == png.PNG_FILTER_TYPE_BASE)

local rows = assert(reader:get_rows())
assert(#rows == 580)
for i = 1, #rows do
  local row = rows[i]
  assert(#row == 271 * 4)
end
