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

#include <string.h>

#include "common.hpp"

namespace dromozoa {
  class reader_handle_impl {
  public:
    reader_handle_impl() : png_(), info_() {
      png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, error_fn, 0);
      if (!png_) {
        error_fn(0, "png_create_read_struct failed");
      }
    }

    ~reader_handle_impl() {
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
      png_destroy_read_struct(&png_, &info_, 0);
    }

    png_structp png() const {
      return png_;
    }

    png_infop info() const {
      return info_;
    }

    void set_read_fn(lua_State* L, int index) {
      luaX_reference<>(L, index).swap(ref_);
      png_set_read_fn(png_, this, read_fn);
    }

  private:
    png_structp png_;
    png_infop info_;
    luaX_reference<> ref_;

    reader_handle_impl(const reader_handle_impl&);
    reader_handle_impl& operator=(const reader_handle_impl&);

    static void read_fn(png_structp png, png_bytep data, png_size_t length) {
      static_cast<reader_handle_impl*>(png_get_io_ptr(png))->read(data, length);
    }

    void read(png_bytep data, png_size_t length) {
      lua_State* L = ref_.state();
      luaX_top_saver save_top(L);
      {
        ref_.get_field(L);
        luaX_push(L, length);
        if (lua_pcall(L, 1, 1, 0) == 0) {
          size_t result = 0;
          if (const char* ptr = lua_tolstring(L, -1, &result)) {
            if (result == length) {
              memcpy(data, ptr, result);
            } else {
              png_error(png_, "read error");
            }
          } else {
            png_error(png_, "read error");
          }
        } else {
          png_error(png_, lua_tostring(L, -1));
        }
      }
    }
  };

  reader_handle_impl* reader_handle::create() {
    scoped_ptr<reader_handle_impl> impl(new reader_handle_impl());
    impl->create();
    return impl.release();
  }

  reader_handle::reader_handle(reader_handle_impl* impl) : impl_(impl) {}

  reader_handle::~reader_handle() {}

  void reader_handle::destroy() {
    impl_->destroy();
  }

  png_structp reader_handle::png() const {
    return impl_->png();
  }

  png_infop reader_handle::info() const {
    return impl_->info();
  }

  void reader_handle::set_read_fn(lua_State* L, int index) {
    impl_->set_read_fn(L, index);
  }
}