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

local function find(data, pattern, i)
  if not i then
    i = 1
  end
  for i = i, #data do
    if data[i]:find(pattern) then
      return i
    end
  end
end

local lines = {}
for line in io.lines "docs/libpng-manual.txt" do
  lines[#lines + 1] = line
end

local head_patterns = {}

local i = assert(find(lines, "^ *TABLE OF CONTENTS$")) + 2
local j = assert(find(lines, "^$", i)) - 1
for i = i, j do
  local _, title = assert(lines[i]:match "^ *([IVX]+)%.%s+(.*)")
  head_patterns[#head_patterns + 1] = "^[IVX]+%.%s+" .. title:gsub("[%.]", "%%%1"):gsub("%s+", "%%s+") .. "$"
end

local i = 1
for k = 1, #head_patterns do
  local j = assert(find(lines, head_patterns[k], i, true), head_patterns[k])
  local out = assert(io.open(("docs/libpng-manual%02d.txt"):format(k - 1), "w"))
  for j = i, j - 1 do
    out:write(lines[j], "\n")
  end
  out:close()
  i = j
end

local out = assert(io.open(("docs/libpng-manual%02d.txt"):format(#head_patterns), "w"))
for i = i, #lines do
  out:write(lines[i], "\n")
end
out:close()
