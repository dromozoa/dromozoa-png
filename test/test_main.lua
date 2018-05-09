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

assert(png.sig_cmp "" ~= 0)
assert(png.sig_cmp "\137" == 0)
assert(png.sig_cmp "\138" ~= 0)
assert(png.sig_cmp "\137PNG" == 0)
assert(png.sig_cmp "\137PNG\r\n\26" == 0)
assert(png.sig_cmp "\137PNG\r\n\27" ~= 0)
assert(png.sig_cmp "\137PNG\r\n\26\n" == 0)

local version_number = assert(png.access_version_number())
if verbose then
  io.stderr:write(version_number, "\n")
  io.stderr:write(png.PNG_LIBPNG_VER_STRING, "\n")
  io.stderr:write(png.PNG_LIBPNG_VER_MAJOR, "\n")
  io.stderr:write(png.PNG_LIBPNG_VER_MINOR, "\n")
  io.stderr:write(png.PNG_LIBPNG_VER_RELEASE, "\n")
  io.stderr:write(png.PNG_LIBPNG_VER, "\n")
end
assert(type(png.PNG_LIBPNG_VER_STRING) == "string")
assert(png.PNG_LIBPNG_VER_MAJOR == 1)
assert(type(png.PNG_LIBPNG_VER_MINOR) == "number")
assert(type(png.PNG_LIBPNG_VER_RELEASE) == "number")
assert(png.PNG_LIBPNG_VER == version_number)

if verbose then
  local names = {}
  local name_max = 0
  for name, value in pairs(png) do
    if name:find "^PNG_" and type(value) == "number" then
      names[#names + 1] = name
      if name_max < #name then
        name_max = #name
      end
    end
  end

  table.sort(names)

  local format = "%-" .. name_max .. "s | %10d | 0x%08x\n"
  io.stderr:write(("-"):rep(70), "\n")
  for i = 1, #names do
    local name = names[i]
    io.stderr:write(format:format(name, png[name], png[name]))
  end
end
