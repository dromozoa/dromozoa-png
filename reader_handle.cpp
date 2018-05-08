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
  class reader_handle_impl {
  public:
    reader_handle_impl() : png_(), info_(), end_() {
      png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, throw_png_runtime_error, 0);
      if (!png_) {
        throw png_runtime_error("png_create_read_struct failed");
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
        throw png_runtime_error("png_create_info_struct failed");
      }

      end_ = png_create_info_struct(png_);
      if (!info_) {
        throw png_runtime_error("png_create_info_struct failed");
      }
    }

    void destroy() {
      png_destroy_read_struct(&png_, &info_, &end_);
    }

    png_structp png() const {
      return png_;
    }

    png_infop info() const {
      return info_;
    }

    png_infop end() const {
      return end_;
    }

  private:
    png_structp png_;
    png_infop info_;
    png_infop end_;
    reader_handle_impl(const reader_handle_impl&);
    reader_handle_impl& operator=(const reader_handle_impl&);
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

  png_infop reader_handle::end() const {
    return impl_->end();
  }
}
