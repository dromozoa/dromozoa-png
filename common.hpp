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

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <png.h>

#include <dromozoa/bind.hpp>

namespace dromozoa {
  void error_fn(png_structp, png_const_charp message);

  class reader_handle_impl;

  class reader_handle {
  public:
    static reader_handle_impl* create();
    explicit reader_handle(reader_handle_impl* impl);
    ~reader_handle();
    void destroy();
    png_structp png(bool check_io_ptr = false) const;
    png_infop info() const;
    void set_warning_fn(lua_State* L, int index);
    void set_read_fn(lua_State* L, int index);
  private:
    scoped_ptr<reader_handle_impl> impl_;
    reader_handle(const reader_handle&);
    reader_handle& operator=(const reader_handle&);
  };

  class writer_handle_impl;

  class writer_handle {
  public:
    static writer_handle_impl* create();
    explicit writer_handle(writer_handle_impl* impl);
    ~writer_handle();
    void destroy();
    png_structp png(bool check_io_ptr = false) const;
    png_infop info() const;
    void set_warning_fn(lua_State* L, int index);
    void set_write_fn(lua_State* L, int index, int index_flush);
    void set_text(lua_State* L, int index);
    png_bytepp prepare_rows(png_uint_32 height, png_size_t rowbytes);
  private:
    scoped_ptr<writer_handle_impl> impl_;
    writer_handle(const writer_handle&);
    writer_handle& operator=(const writer_handle&);
  };
}

#endif
