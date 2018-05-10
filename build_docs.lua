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

local escape_table = { ["&"] = "&amp;", ["<"] = "&lt;", [">"] = "&gt;" }
local function escape(source)
  return (source:gsub("[&<>]", escape_table))
end

local function write_manual_html(section_number, section_title, data, i, j, symbol_table, toc_start, toc_end)
  local out = assert(io.open(("docs/libpng-manual%02d.html"):format(section_number), "w"))
  out:write(([[
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
<title>%s</title>
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
</style>
</head>
<body>
<div class="markdown-body">

<h1>%s</h1>

<pre>]]):format(escape(section_title), escape(section_title)))

  for i = i, j do
    local line = escape(data[i])
    if toc_start <= i and i <= toc_end then
      line = line:gsub("^(%s*)(.*)", function(wsp, title)
        return ([[%s<a href="libpng-manual%02d.html">%s</a>]]):format(wsp, i - toc_start + 1, title)
      end)
    end
    line = line:gsub("[Pp][Nn][Gg]_[%w_]+", function (symbol)
      local number = symbol_table[symbol]
      if number then
        return ([[<a href="png.h.html#L%d">%s</a>]]):format(number, symbol)
      end
    end)
    out:write(line, "\n")
  end

  out:write[[</pre>
</div>
</body>
</html>
]]
  out:close()
end

local symbol_table = {}

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
  color: #C6C6C6;
  list-style-type: decimal;
}
</style>
</head>
<body>
<div class="markdown-body">

<h1>png.h</h1>

<pre class="prettyprint lang-c linenums">
]]

local i = 0
for line in io.lines "docs/png.h" do
  i = i + 1
  out:write(escape(line), "\n")
  local symbol = line:match "PNG_[%u_]*EXPORT%(%d+, [^,]+, (png_[%w_]+),"
  if symbol then
    symbol_table[symbol] = i
  end
  local symbol = line:match "# *define (PNG_[%w_]+)"
  if symbol then
    symbol_table[symbol] = i
  end
end

out:write [[
</pre>
</div>
<script>
(function (root) {
  if (!root.exports) {
    root.exports = {};
  }
  root.exports["dromozoa-png"] = function () {
    let document = root.document;
    let items = document.querySelectorAll("pre.prettyprint > ol.linenums > li");
    for (let i = 0; i < items.length; ++i) {
      items[i].setAttribute("id", "L" + (i + 1));
    }
    if (document.location.hash) {
      document.location.href = document.location.hash;
    }
  };
}(this));
</script>
<script src="https://cdn.rawgit.com/google/code-prettify/master/loader/run_prettify.js?callback=dromozoa-png"></script>
</body>
</html>
]]
out:close()

local lines = {}
for line in io.lines "docs/libpng-manual.txt" do
  lines[#lines + 1] = line
end

local section_titles = { [0] = "TABLE OF CONTENTS" }
local section_patterns = {}

local toc_start = assert(find(lines, "^ *TABLE OF CONTENTS$")) + 2
local toc_end = assert(find(lines, "^$", toc_start)) - 1
for i = toc_start, toc_end do
  local number, title = assert(lines[i]:match "^ *([IVX]+)%.%s+(.*)")
  section_titles[#section_titles + 1] = number .. ". " .. title
  section_patterns[#section_patterns + 1] = "^[IVX]+%.%s+" .. title:gsub("[%.]", "%%%1"):gsub("%s+", "%%s+") .. "$"
end

local i = 1
for k = 1, #section_patterns do
  local j = assert(find(lines, section_patterns[k], i, true), section_patterns[k])
  write_manual_html(k - 1, section_titles[k - 1], lines, i, j -1, symbol_table, toc_start, toc_end)
  i = j
end
write_manual_html(#section_titles, section_titles[#section_titles], lines, i, #lines, symbol_table, toc_start, toc_end)


local out = assert(io.open("docs/PngSuite.html", "w"))
out:write [[
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
<title>PngSuite</title>
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
</style>
</head>
<body>
<div class="markdown-body">

<h1>PngSuite</h1>

<ul>
  <li><a href="PngSuite-2017jul19/PngSuite.LICENSE">PngSuite.LICENSE</a></li>
  <li><a href="PngSuite-2017jul19/PngSuite.README">PngSuite.README</a></li>
</ul>

<p><img src="PngSuite-2017jul19/PngSuite.png"></p>

<p>
]]

local handle = assert(io.popen("cd docs/PngSuite-2017jul19 && ls"))
for line in handle:lines() do
  if not line:find "^PngSuite" and line:find "%.png$" then
    out:write(([[
  <img src="PngSuite-2017jul19/%s">
]]):format(line))
  end
end
handle:close()

out:write [[
</p>

</div>
</body>
</html>
]]
out:close()
