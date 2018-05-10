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

#include <stddef.h>

#include <vector>

#include "common.hpp"

namespace dromozoa {
  namespace {
    class text_impl {
    public:
      text_impl(int compression, const std::string& key, const std::string& text, const std::string& lang, const std::string& lang_key) : compression_(compression), key_(key), text_(text), lang_(lang), lang_key_(lang_key) {}

      void get(png_textp out) const {
        out->compression = compression_;
        out->key = const_cast<png_charp>(key_.c_str());
        out->text = const_cast<png_charp>(text_.c_str());
        switch (compression_) {
          case PNG_TEXT_COMPRESSION_NONE:
          case PNG_TEXT_COMPRESSION_zTXt:
            out->text_length = text_.size();
            break;
          case PNG_ITXT_COMPRESSION_NONE:
          case PNG_ITXT_COMPRESSION_zTXt:
#if PNG_LIBPNG_VER >= 10500 || defined(PNG_iTXt_SUPPORTED)
            out->itxt_length = text_.size();
            out->lang = const_cast<png_charp>(lang_.c_str());
            out->lang_key = const_cast<png_charp>(lang_key_.c_str());
#endif
            break;
        }
      }

    private:
      int compression_;
      std::string key_;
      std::string text_;
      std::string lang_;
      std::string lang_key_;
    };
  }

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

    void set_warning_fn(lua_State* L, int index) {
      if (lua_isnoneornil(L, index)) {
        luaX_reference<>().swap(warning_fn_);
        png_set_error_fn(png_, this, error_fn, 0);
      } else {
        luaX_reference<>(L, index).swap(warning_fn_);
        png_set_error_fn(png_, this, error_fn, warning_fn);
      }
    }

    void set_write_fn(lua_State* L, int index, int index_flush) {
      if (lua_isnoneornil(L, index)) {
        luaX_reference<2>().swap(write_fn_);
        png_set_write_fn(png_, 0, 0, 0);
      } else {
        luaX_reference<2>(L, index, index_flush).swap(write_fn_);
        png_set_write_fn(png_, this, write_fn, flush_fn);
      }
    }

    void set_text(lua_State* L, int index) {
      std::vector<text_impl> text_storage;
      for (int i = 1; ; ++i) {
        luaX_get_field(L, index, i);
        if (lua_isnil(L, -1)) {
          lua_pop(L, 1);
          break;
        } else {
          int compression = luaX_check_integer_field<int>(L, -1, "compression");
          std::string key;
          std::string text;
          std::string lang;
          std::string lang_key;

          luaX_get_field(L, -1, "key");
          if (const char* ptr = lua_tostring(L, -1)) {
            key.assign(ptr);
          }
          lua_pop(L, 1);

          luaX_get_field(L, -1, "text");
          size_t length = 0;
          if (const char* ptr = lua_tolstring(L, -1, &length)) {
            text.assign(ptr, length);
          }
          lua_pop(L, 1);

          luaX_get_field(L, -1, "lang");
          if (const char* ptr = lua_tostring(L, -1)) {
            lang.assign(ptr);
          }
          lua_pop(L, 1);

          luaX_get_field(L, -1, "lang_key");
          if (const char* ptr = lua_tostring(L, -1)) {
            lang_key.assign(ptr);
          }
          lua_pop(L, 2);

          text_storage.push_back(text_impl(compression, key, text, lang, lang_key));
        }
      }
      std::vector<png_text> text(text_storage.size());

      text_storage.swap(text_storage_);
      text.swap(text_);

      if (text_.empty()) {
        png_set_text(png_, info_, 0, 0);
      } else {
        for (size_t i = 0; i < text_.size(); ++i) {
          text_storage_[i].get(&text_[i]);
        }
        png_set_text(png_, info_, &text_[0], text_.size());
      }
    }

    png_bytepp prepare_rows(png_uint_32 height, png_size_t rowbytes) {
      size_t storage_size = height * rowbytes;
      if (row_storage_.size() != storage_size || row_pointers_.size() != height) {
        std::vector<png_byte> row_storage(storage_size);
        std::vector<png_bytep> row_pointers(height);
        row_storage.swap(row_storage_);
        row_pointers.swap(row_pointers_);
        if (storage_size == 0) {
          png_set_rows(png_, info_, 0);
        } else {
          for (size_t i = 0; i < row_pointers_.size(); ++i) {
            row_pointers_[i] = &row_storage_[i * rowbytes];
          }
          png_set_rows(png_, info_, &row_pointers_[0]);
        }
      }
      return png_get_rows(png_, info_);
    }

  private:
    png_structp png_;
    png_infop info_;
    luaX_reference<> warning_fn_;
    luaX_reference<2> write_fn_;
    std::vector<text_impl> text_storage_;
    std::vector<png_text> text_;
    std::vector<png_byte> row_storage_;
    std::vector<png_bytep> row_pointers_;

    writer_handle_impl(const writer_handle_impl&);
    writer_handle_impl& operator=(const writer_handle_impl&);

    static void warning_fn(png_structp png, png_const_charp message) {
      static_cast<writer_handle_impl*>(png_get_error_ptr(png))->warning(message);
    }

    static void write_fn(png_structp png, png_bytep data, png_size_t length) {
      static_cast<writer_handle_impl*>(png_get_io_ptr(png))->write(data, length);
    }

    static void flush_fn(png_structp png) {
      static_cast<writer_handle_impl*>(png_get_io_ptr(png))->flush();
    }

    void warning(png_const_charp message) {
      lua_State* L = warning_fn_.state();
      luaX_top_saver save_top(L);
      {
        warning_fn_.get_field(L);
        luaX_push(L, message);
        if (lua_pcall(L, 1, 0, 0) != 0) {
          png_error(png_, lua_tostring(L, -1));
        }
      }
    }

    void write(png_bytep data, png_size_t length) {
      lua_State* L = write_fn_.state();
      luaX_top_saver save_top(L);
      {
        write_fn_.get_field(L);
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
      lua_State* L = write_fn_.state();
      luaX_top_saver save_top(L);
      {
        write_fn_.get_field(L, 1);
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

  void writer_handle::set_warning_fn(lua_State* L, int index) {
    impl_->set_warning_fn(L, index);
  }

  void writer_handle::set_write_fn(lua_State* L, int index, int index_flush) {
    impl_->set_write_fn(L, index, index_flush);
  }

  void writer_handle::set_text(lua_State* L, int index) {
    impl_->set_text(L, index);
  }

  png_bytepp writer_handle::prepare_rows(png_uint_32 height, png_size_t rowbytes) {
    return impl_->prepare_rows(height, rowbytes);
  }
}
