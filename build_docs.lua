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

local out = assert(io.open("docs/png.h.html", "w"))
out:write [[
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
<title>png.h</title>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/github-markdown-css/2.10.0/github-markdown.min.css">
<style>
.markdown-body {
  box-sizing: border-box;
  min-width: 200px;
  max-width: 980px;
  margin: 0 auto;
  padding: 45px;
}
@media (max-width: 767px) {
  .markdown-body {
    padding: 15px;
  }
}
pre.prettyprint > ol.linenums {
  padding-left: 4em;
}
pre.prettyprint > ol.linenums > li {
  list-style-type: decimal;
}
</style>
</head>
<body>
<div class="markdown-body">

<h1>png.h</h1>

<pre class="prettyprint lang-c linenums">]]

local escape_table = { ["&"] = "&amp;", ["<"] = "&lt;", [">"] = "&gt;" }
for line in io.lines "docs/png.h" do
  out:write(line:gsub("[&<>]", escape_table), "\n")
end

out:write [[</pre>
</div>
<script src="https://cdn.rawgit.com/google/code-prettify/master/loader/run_prettify.js"></script>
</body>
</html>
]]
out:close()

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
