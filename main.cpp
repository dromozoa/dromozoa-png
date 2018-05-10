// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-png.
//
// dromozoa-png is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-png is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-png.  If not, see <http://www.gnu.org/licenses/>.

#include "common.hpp"

namespace dromozoa {
  namespace {
    void impl_access_version_number(lua_State* L) {
      luaX_push(L, png_access_version_number());
    }

    void impl_sig_cmp(lua_State* L) {
      // no png_const_bytep prior to libpng 1.5.x
      size_t length = 0;
      png_bytep header = reinterpret_cast<png_bytep>(const_cast<char*>(luaL_checklstring(L, 1, &length)));
      luaX_push(L, png_sig_cmp(header, 0, length));
    }
  }

  void initialize_main(lua_State* L) {
    luaX_set_field(L, -1, "access_version_number", impl_access_version_number);
    luaX_set_field(L, -1, "sig_cmp", impl_sig_cmp);

    luaX_set_field(L, -1, "PNG_LIBPNG_VER_STRING", PNG_LIBPNG_VER_STRING);
    luaX_set_field(L, -1, "PNG_LIBPNG_VER_MAJOR", PNG_LIBPNG_VER_MAJOR);
    luaX_set_field(L, -1, "PNG_LIBPNG_VER_MINOR", PNG_LIBPNG_VER_MINOR);
    luaX_set_field(L, -1, "PNG_LIBPNG_VER_RELEASE", PNG_LIBPNG_VER_RELEASE);
    luaX_set_field(L, -1, "PNG_LIBPNG_VER", PNG_LIBPNG_VER);

    luaX_set_field(L, -1, "PNG_TRANSFORM_IDENTITY", PNG_TRANSFORM_IDENTITY);
    luaX_set_field(L, -1, "PNG_TRANSFORM_STRIP_16", PNG_TRANSFORM_STRIP_16);
    luaX_set_field(L, -1, "PNG_TRANSFORM_STRIP_ALPHA", PNG_TRANSFORM_STRIP_ALPHA);
    luaX_set_field(L, -1, "PNG_TRANSFORM_PACKING", PNG_TRANSFORM_PACKING);
    luaX_set_field(L, -1, "PNG_TRANSFORM_PACKSWAP", PNG_TRANSFORM_PACKSWAP);
    luaX_set_field(L, -1, "PNG_TRANSFORM_EXPAND", PNG_TRANSFORM_EXPAND);
    luaX_set_field(L, -1, "PNG_TRANSFORM_INVERT_MONO", PNG_TRANSFORM_INVERT_MONO);
    luaX_set_field(L, -1, "PNG_TRANSFORM_SHIFT", PNG_TRANSFORM_SHIFT);
    luaX_set_field(L, -1, "PNG_TRANSFORM_BGR", PNG_TRANSFORM_BGR);
    luaX_set_field(L, -1, "PNG_TRANSFORM_SWAP_ALPHA", PNG_TRANSFORM_SWAP_ALPHA);
    luaX_set_field(L, -1, "PNG_TRANSFORM_SWAP_ENDIAN", PNG_TRANSFORM_SWAP_ENDIAN);
    luaX_set_field(L, -1, "PNG_TRANSFORM_INVERT_ALPHA", PNG_TRANSFORM_INVERT_ALPHA);
    luaX_set_field(L, -1, "PNG_TRANSFORM_STRIP_FILLER", PNG_TRANSFORM_STRIP_FILLER);
#ifdef PNG_TRANSFORM_STRIP_FILLER_BEFORE
    luaX_set_field(L, -1, "PNG_TRANSFORM_STRIP_FILLER_BEFORE", PNG_TRANSFORM_STRIP_FILLER_BEFORE);
#endif
#ifdef PNG_TRANSFORM_STRIP_FILLER_AFTER
    luaX_set_field(L, -1, "PNG_TRANSFORM_STRIP_FILLER_AFTER", PNG_TRANSFORM_STRIP_FILLER_AFTER);
#endif
#ifdef PNG_TRANSFORM_GRAY_TO_RGB
    luaX_set_field(L, -1, "PNG_TRANSFORM_GRAY_TO_RGB", PNG_TRANSFORM_GRAY_TO_RGB);
#endif
#ifdef PNG_TRANSFORM_EXPAND_16
    luaX_set_field(L, -1, "PNG_TRANSFORM_EXPAND_16", PNG_TRANSFORM_EXPAND_16);
#endif
#ifdef PNG_TRANSFORM_SCALE_16
    luaX_set_field(L, -1, "PNG_TRANSFORM_SCALE_16", PNG_TRANSFORM_SCALE_16);
#endif

    luaX_set_field(L, -1, "PNG_INFO_gAMA", PNG_INFO_gAMA);
    luaX_set_field(L, -1, "PNG_INFO_sBIT", PNG_INFO_sBIT);
    luaX_set_field(L, -1, "PNG_INFO_cHRM", PNG_INFO_cHRM);
    luaX_set_field(L, -1, "PNG_INFO_PLTE", PNG_INFO_PLTE);
    luaX_set_field(L, -1, "PNG_INFO_tRNS", PNG_INFO_tRNS);
    luaX_set_field(L, -1, "PNG_INFO_bKGD", PNG_INFO_bKGD);
    luaX_set_field(L, -1, "PNG_INFO_hIST", PNG_INFO_hIST);
    luaX_set_field(L, -1, "PNG_INFO_pHYs", PNG_INFO_pHYs);
    luaX_set_field(L, -1, "PNG_INFO_oFFs", PNG_INFO_oFFs);
    luaX_set_field(L, -1, "PNG_INFO_tIME", PNG_INFO_tIME);
    luaX_set_field(L, -1, "PNG_INFO_pCAL", PNG_INFO_pCAL);
    luaX_set_field(L, -1, "PNG_INFO_sRGB", PNG_INFO_sRGB);
    luaX_set_field(L, -1, "PNG_INFO_iCCP", PNG_INFO_iCCP);
    luaX_set_field(L, -1, "PNG_INFO_sPLT", PNG_INFO_sPLT);
    luaX_set_field(L, -1, "PNG_INFO_sCAL", PNG_INFO_sCAL);
    luaX_set_field(L, -1, "PNG_INFO_IDAT", PNG_INFO_IDAT);
#ifdef PNG_INFO_eXIf
    luaX_set_field(L, -1, "PNG_INFO_eXIf", PNG_INFO_eXIf);
#endif

    luaX_set_field(L, -1, "PNG_COLOR_TYPE_GRAY", PNG_COLOR_TYPE_GRAY);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_GRAY_ALPHA", PNG_COLOR_TYPE_GRAY_ALPHA);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_PALETTE", PNG_COLOR_TYPE_PALETTE);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_RGB", PNG_COLOR_TYPE_RGB);
    luaX_set_field(L, -1, "PNG_COLOR_TYPE_RGB_ALPHA", PNG_COLOR_TYPE_RGB_ALPHA);
    luaX_set_field(L, -1, "PNG_COLOR_MASK_PALETTE", PNG_COLOR_MASK_PALETTE);
    luaX_set_field(L, -1, "PNG_COLOR_MASK_COLOR", PNG_COLOR_MASK_COLOR);
    luaX_set_field(L, -1, "PNG_COLOR_MASK_ALPHA", PNG_COLOR_MASK_ALPHA);
    luaX_set_field(L, -1, "PNG_INTERLACE_NONE", PNG_INTERLACE_NONE);
    luaX_set_field(L, -1, "PNG_INTERLACE_ADAM7", PNG_INTERLACE_ADAM7);
    luaX_set_field(L, -1, "PNG_COMPRESSION_TYPE_BASE", PNG_COMPRESSION_TYPE_BASE);
    luaX_set_field(L, -1, "PNG_FILTER_TYPE_BASE", PNG_FILTER_TYPE_BASE);

    luaX_set_field(L, -1, "PNG_OFFSET_PIXEL", PNG_OFFSET_PIXEL);
    luaX_set_field(L, -1, "PNG_OFFSET_MICROMETER", PNG_OFFSET_MICROMETER);

    luaX_set_field(L, -1, "PNG_RESOLUTION_UNKNOWN", PNG_RESOLUTION_UNKNOWN);
    luaX_set_field(L, -1, "PNG_RESOLUTION_METER", PNG_RESOLUTION_METER);
  }
}
