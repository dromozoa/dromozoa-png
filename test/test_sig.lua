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

assert(png.sig_cmp "" ~= 0)
assert(png.sig_cmp "\137" == 0)
assert(png.sig_cmp "\138" ~= 0)
assert(png.sig_cmp "\137PNG" == 0)
assert(png.sig_cmp "\137PNG\r\n\26" == 0)
assert(png.sig_cmp "\137PNG\r\n\27" ~= 0)
assert(png.sig_cmp "\137PNG\r\n\26\n" == 0)
