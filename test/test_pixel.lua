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

local unpack = table.unpack or unpack

local verbose = os.getenv "VERBOSE" == "1"

local width = 512
local height = 16

local function pixel(x, y, bit_depth, channels)
  local result = {}
  if bit_depth == 8 then
    for i = 1, channels do
      result[i] = math.random(0, 255)
    end
  else
    for i = 1, channels do
      result[i] = math.random(0, 65535)
    end
  end
  return result
end

local function test(width, height, bit_depth, color_type, channels)
  local filename = ("test-%02d-%02d.png"):format(bit_depth, color_type)
  if verbose then
    io.stderr:write(filename, "\n")
  end

  local out = assert(io.open(filename, "wb"))
  local writer = assert(png.writer())
  assert(writer:set_write_fn(function (data)
    out:write(data)
  end))
  assert(writer:set_IHDR {
    width = width;
    height = height;
    bit_depth = bit_depth;
    color_type = color_type;
  })

  local rows = {}
  for y = 1, height do
    local row = {}
    for x = 1, width do
      row[x] = pixel(x, y, bit_depth, channels)
    end
    rows[y] = row
  end

  local data = {}

  for y = 1, height do
    for x = 1, width do
      assert(writer:set_pixel(x, y, unpack(rows[y][x])))
    end
  end

  assert(writer:write_png())
  assert(writer:destroy())
  out:close()

  local handle = assert(io.open(filename, "rb"))
  local reader = assert(png.reader())
  assert(reader:set_read_fn(function (n)
    return handle:read(n)
  end))
  assert(reader:read_png())

  assert(reader:get_image_width() == width)
  assert(reader:get_image_height() == height)
  assert(reader:get_bit_depth() == bit_depth)
  assert(reader:get_color_type() == color_type)
  assert(reader:get_channels() == channels)

  for y = 1, height do
    for x = 1, width do
      local result = { assert(reader:get_pixel(x, y)) }
      local expect = rows[y][x]
      assert(#result == #expect)
      for i = 1, #result do
        assert(result[i] == expect[i])
      end
    end
  end

  assert(reader:destroy())
  handle:close()
end

test(width, height, 8, png.PNG_COLOR_TYPE_GRAY, 1)
test(width, height, 8, png.PNG_COLOR_TYPE_GRAY_ALPHA, 2)
test(width, height, 8, png.PNG_COLOR_TYPE_RGB, 3)
test(width, height, 8, png.PNG_COLOR_TYPE_RGB_ALPHA, 4)
test(width, height, 16, png.PNG_COLOR_TYPE_GRAY, 1)
test(width, height, 16, png.PNG_COLOR_TYPE_GRAY_ALPHA, 2)
test(width, height, 16, png.PNG_COLOR_TYPE_RGB, 3)
test(width, height, 16, png.PNG_COLOR_TYPE_RGB_ALPHA, 4)

local writer = assert(png.writer())
assert(writer:set_write_fn(function () end))

assert(writer:set_IHDR {
  width = 16;
  height = 16;
  bit_depth = 1;
  color_type = png.PNG_COLOR_TYPE_PALETTE;
})

local result, message = writer:set_pixel(1, 1, 0)
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

assert(writer:set_IHDR {
  width = 16;
  height = 16;
  bit_depth = 8;
  color_type = png.PNG_COLOR_TYPE_PALETTE;
})

local result, message = writer:set_pixel(1, 1, 0)
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

assert(writer:set_IHDR {
  width = 16;
  height = 16;
  bit_depth = 8;
  color_type = png.PNG_COLOR_TYPE_GRAY;
})

local result, message = pcall(writer.set_pixel, writer, 256, 1, 0)
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

local result, message = pcall(writer.set_pixel, writer, 1, 256, 0)
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

local result, message = pcall(writer.set_pixel, writer, 1, 1, 256)
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)
