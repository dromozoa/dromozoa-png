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

#include <vector>

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

    void set_write_fn(lua_State* L, int index, int index_flush) {
      luaX_reference<2>(L, index, index_flush).swap(ref_);
      png_set_write_fn(png_, this, write_fn, flush_fn);
    }

    png_bytepp prepare_rows(png_uint_32 height, size_t rowbytes) {
      size_t storage_size = height * rowbytes;
      if (row_storage_.size() != storage_size || row_pointers_.size() != height) {
        row_storage_.resize(storage_size);
        row_pointers_.resize(height);
        for (png_uint_32 i = 0; i < height; ++i) {
          row_pointers_[i] = &row_storage_[i * rowbytes];
        }
        png_set_rows(png_, info_, &row_pointers_[0]);
      }
      if (row_pointers_.empty()) {
        return 0;
      } else {
        return &row_pointers_[0];
      }
    }

  private:
    png_structp png_;
    png_infop info_;
    luaX_reference<2> ref_;
    std::vector<png_byte> row_storage_;
    std::vector<png_bytep> row_pointers_;

    writer_handle_impl(const writer_handle_impl&);
    writer_handle_impl& operator=(const writer_handle_impl&);

    static void write_fn(png_structp png, png_bytep data, png_size_t length) {
      static_cast<writer_handle_impl*>(png_get_io_ptr(png))->write(data, length);
    }

    static void flush_fn(png_structp png) {
      static_cast<writer_handle_impl*>(png_get_io_ptr(png))->flush();
    }

    void write(png_bytep data, png_size_t length) {
      lua_State* L = ref_.state();
      luaX_top_saver save_top(L);
      {
        ref_.get_field(L);
        lua_pushlstring(L, reinterpret_cast<const char*>(data), length);
        if (lua_pcall(L, 1, 1, 0) == 0) {
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
      lua_State* L = ref_.state();
      luaX_top_saver save_top(L);
      {
        ref_.get_field(L, 1);
        if (!lua_isnil(L, -1)) {
          if (lua_pcall(L, 0, 0, 0) != 0) {
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

  void writer_handle::set_write_fn(lua_State* L, int index, int index_flush) {
    impl_->set_write_fn(L, index, index_flush);
  }

  png_bytepp writer_handle::prepare_rows(png_uint_32 height, size_t rowbytes) {
    return impl_->prepare_rows(height, rowbytes);
  }
}
