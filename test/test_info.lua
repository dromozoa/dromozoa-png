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

local function test(n, f)
  local reader = assert(png.reader())
  local handle = assert(io.open("docs/pngtest.png"))
  assert(reader:set_read_fn(function (n)
    return handle:read(n)
  end))
  f(reader)

  local items = assert(reader:get_text())
  assert(n == #items)
  if verbose then
    for i = 1, n do
      local item = items[i]
      if verbose then
        io.stderr:write(item.key, "\n")
        io.stderr:write(item.text, "\n")
      end
    end
  end
end

test(2, function (reader)
  assert(reader:read_png())
end)

test(1, function (reader)
  assert(reader:read_info())
end)
