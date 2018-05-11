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

local writer = assert(png.writer())

local flush = 0
local out = assert(io.open("test.png", "wb"))
assert(writer:set_write_fn(function (data)
  if verbose then
    io.stderr:write("write ", #data, "\n")
  end
  out:write(data)
end, function ()
  flush = flush + 1
  if verbose then
    io.stderr:write "flush\n"
  end
  out:flush()
end))
assert(writer:set_flush(2))

assert(writer:set_IHDR {
  width = 2;
  height = 3;
  bit_depth = 8;
  color_type = png.PNG_COLOR_TYPE_RGB;
})
assert(writer:set_tIME(os.date "!*t"))
assert(writer:set_text {
  {
    compression = png.PNG_TEXT_COMPRESSION_zTXt;
    key = "Title";
    text = "test";
  };
  {
    compression = png.PNG_TEXT_COMPRESSION_NONE;
    key = "Description";
    text = ("X"):rep(16) .. "\0foo\0bar\0baz";
  };
  {
    compression = png.PNG_ITXT_COMPRESSION_zTXt;
    key = "Title";
    text = "テスト";
    lang = "ja";
    lang_key = "タイトル";
  };
  {
    compression = png.PNG_ITXT_COMPRESSION_NONE;
    key = "Description";
    text = [[
あいうえお
かきくけこ
さしすせそ
たちつてと
なにぬねの
]];
    lang = "ja";
    lang_key = "詳細";
  };
})
assert(writer:set_oFFs {
  offset_x = 0;
  offset_y = 10;
  unit_type = png.PNG_OFFSET_PIXEL;
})
assert(writer:set_pHYs {
  res_x = math.ceil(144 / 0.0254);
  res_y = math.ceil(216 / 0.0254);
  unit_type = png.PNG_RESOLUTION_METER;
})

assert(writer:set_rows {
  string.char(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  string.char(0x00, 0xFF, 0x00, 0x00, 0x7F, 0x7F);
})
assert(writer:set_row(1, string.char(0xFF, 0x00, 0x00, 0x7F, 0x7F, 0x00)))
assert(writer:set_row(3, string.char(0x00, 0x00, 0xFF, 0x7F, 0x00, 0x7F)))

collectgarbage()
collectgarbage()

assert(writer:write_png())
assert(flush == 1)

out:close()
