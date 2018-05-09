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
  class writer_handle_impl {
  public:
    writer_handle_impl() : png_(), info_() {
      png_ = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, error_fn, 0);
      if (!png_) {
        error_fn(0, "png_create_write_struct failed");
      }
    }

    ~writer_handle_impl() {
      if (png_) {
        destroy();
      }
    }

    void create() {
      info_ = png_create_info_struct(png_);
      if (!info_) {
        png_error(png_, "png_create_info_struct failed");
      }
    }

    void destroy() {
      png_destroy_write_struct(&png_, &info_);
    }

    png_structp png() const {
      return png_;
    }

    png_infop info() const {
      return info_;
    }

    void set_write_fn(lua_State* L, int index_write, int index_flush) {
      luaX_reference<2>(L, index_write, index_flush).swap(write_fn_);
      png_set_write_fn(png_, this, write_fn, flush_fn);
    }

  private:
    png_structp png_;
    png_infop info_;
    luaX_reference<2> write_fn_;

    writer_handle_impl(const writer_handle_impl&);
    writer_handle_impl& operator=(const writer_handle_impl&);

    static void write_fn(png_structp png, png_bytep data, png_size_t length) {
      static_cast<writer_handle_impl*>(png_get_io_ptr(png))->write(data, length);
    }

    static void flush_fn(png_structp png) {
      static_cast<writer_handle_impl*>(png_get_io_ptr(png))->flush();
    }

    void write(png_bytep data, png_size_t length) {
      lua_State* L = write_fn_.state();
      luaX_top_saver save_top(L);
      {
        write_fn_.get_field(L);
        lua_pushlstring(L, reinterpret_cast<const char*>(data), length);
        int r = lua_pcall(L, 1, 1, 0);
        if (r == 0) {
          if (luaX_is_integer(L, -1)) {
            if (static_cast<png_size_t>(lua_tointeger(L, -1)) != length) {
              png_error(png_, "write error");
            }
          }
        } else {
          png_error(png_, lua_tostring(L, -1));
        }
      }
    }

    void flush() {
      lua_State* L = write_fn_.state();
      luaX_top_saver save_top(L);
      {
        write_fn_.get_field(L);
        if (!lua_isnil(L, -1)) {
          int r = lua_pcall(L, 0, 0, 0);
          if (r != 0) {
            png_error(png_, lua_tostring(L, -1));
          }
        }
      }
    }
  };

  writer_handle_impl* writer_handle::create() {
    scoped_ptr<writer_handle_impl> impl(new writer_handle_impl());
    impl->create();
    return impl.release();
  }

  writer_handle::writer_handle(writer_handle_impl* impl) : impl_(impl) {}

  writer_handle::~writer_handle() {}

  void writer_handle::destroy() {
    impl_->destroy();
  }

  png_structp writer_handle::png() const {
    return impl_->png();
  }

  png_infop writer_handle::info() const {
    return impl_->info();
  }

  void writer_handle::set_write_fn(lua_State* L, int index_write, int index_flush) {
    impl_->set_write_fn(L, index_write, index_flush);
  }
}
