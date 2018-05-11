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

local color_types = {
  ["0g"] = png.PNG_COLOR_TYPE_GRAY;
  ["2c"] = png.PNG_COLOR_TYPE_RGB;
  ["3p"] = png.PNG_COLOR_TYPE_PALETTE;
  ["4a"] = png.PNG_COLOR_TYPE_GRAY_ALPHA;
  ["6a"] = png.PNG_COLOR_TYPE_RGB_ALPHA;
}

local interlace_types = {
  n = png.PNG_INTERLACE_NONE;
  i = png.PNG_INTERLACE_ADAM7;
}

local function format_time(time)
  if time then
    return ("%04d-%02d-%02d %02d:%02d:%02d"):format(time.year, time.month, time.day, time.hour, time.min, time.sec)
  end
end

local times = {
  cm0 = "2000-01-01 12:34:56";
  cm7 = "1970-01-01 00:00:00";
  cm9 = "1999-12-31 23:59:59";
}

local langs = {
  cte = "en";
  ctf = "fi";
  ctg = "el";
  cth = "hi";
  ctj = "ja";
}

local transforms = png.PNG_TRANSFORM_EXPAND
if png.PNG_TRANSFORM_SCALE_16 then
  transforms = transforms + png.PNG_TRANSFORM_SCALE_16
else
  if verbose then
    io.stderr:write "PNG_TRANSFORM_SCALE_16 not found\n"
  end
  transforms = transforms + png.PNG_TRANSFORM_STRIP_16
end

local warning = 0
for line in io.lines "docs/PngSuite.txt" do
  local filename = "docs/" .. line
  if verbose then
    io.stderr:write(filename, "\n")
  end

  local feature, interlace_type, color_type, bit_depth = line:match "/(%w%w%w)([ni])(%d[gcpa])(%d%d)%.png$"
  if not feature then
    feature, color_type, bit_depth = assert(line:match "/(exif)(%d[gcpa])(%d%d)%.png$")
    interlace_type = "n"
  end
  bit_depth = tonumber(bit_depth)
  color_type = color_types[color_type]
  interlace_type = interlace_types[interlace_type]

  if color_type == png.PNG_COLOR_TYPE_GRAY then
    if bit_depth < 8 then
      bit_depth = 8
    end
  elseif color_type == png.PNG_COLOR_TYPE_PALETTE then
    if bit_depth < 8 then
      bit_depth = 8
    end
    color_type = png.PNG_COLOR_TYPE_RGB
  end
  if bit_depth == 16 then
    bit_depth = 8
  end
  if feature:find "^t" and not feature:find "^tp0" then
    color_type = color_type + png.PNG_COLOR_MASK_ALPHA
  end

  local reader = assert(png.reader())

  local handle = assert(io.open(filename, "rb"))
  assert(reader:set_warning_fn(function (message)
    warning = warning + 1
    if verbose then
      io.stderr:write(message, "\n")
    end
  end))
  assert(reader:set_read_fn(function (n)
    return handle:read(n)
  end))

  if feature:find "^x" then
    local result, message = reader:read_png(transforms)
    assert(not result)
    if verbose then
      io.stderr:write(message)
    end
  else
    assert(reader:read_png(transforms))
    assert(reader:get_bit_depth() == bit_depth)
    assert(reader:get_color_type() == color_type)
    assert(reader:get_interlace_type() == interlace_type)

    local width = assert(reader:get_image_width())
    local rowbytes = assert(reader:get_rowbytes())
    assert(reader:get_channels() == rowbytes / width)

    assert(format_time(reader:get_tIME()) == times[feature])

    if feature:find "^ct[1z]$" then
      local items = assert(reader:get_text())
      for i = 1, #items do
        local item = items[i]
        if verbose then
          io.stderr:write(item.key, "\n")
          io.stderr:write(item.text, "\n")
        end
        assert(item.compression)
        assert(item.key)
        assert(item.text)
        assert(not item.lang)
        assert(not item.lang_key)
      end
    elseif feature:find "^ct[e-j]" and png.PNG_LIBPNG_VER >= 10500 then
      local items = assert(reader:get_text())
      for i = 1, #items do
        local item = items[i]
        if verbose then
          io.stderr:write(item.key, "\n")
          io.stderr:write(item.text, "\n")
          io.stderr:write(item.lang, "\n")
          io.stderr:write(item.lang_key, "\n")
        end
        assert(item.compression == png.PNG_ITXT_COMPRESSION_NONE)
        assert(item.key)
        assert(item.text)
        assert(item.lang == langs[feature])
        assert(item.lang_key)
      end
    else
      assert(not reader:get_text())
    end
  end
end
assert(warning > 0)
