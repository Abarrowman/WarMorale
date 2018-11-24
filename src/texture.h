#pragma once

#include <stb/stb_image.h>

class texture {
private:
  GLuint tex_;
  int width_;
  int height_;
  unsigned char* data_;

public:

  texture(const char* filename) {
    glGenTextures(1, &tex_);
    glBindTexture(GL_TEXTURE_2D, tex_);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    int channel_count;
    data_ = stbi_load(filename, &width_, &height_, &channel_count, 4);
    if (!data_) {
      fprintf(stderr, "Could not read file %s\n", filename);
      exit(-1);
    }

    glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGBA,
      width_,
      height_,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      data_);

    //glGenerateMipmap(GL_TEXTURE_2D);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  }

  // do not copy or assign
  texture(texture&) = delete;
  texture& operator=(const texture&) = delete;

  //moving is ok
  texture(texture&& old) : tex_(old.tex_), width_(old.width_), height_(old.height_), data_(old.data_) {
    old.tex_ = 0;
    old.data_ = nullptr;
  }
  // move assinging is not ok
  texture& operator= (texture&& old) = delete;

  ~texture() {
    glDeleteTextures(1, &tex_);
    stbi_image_free(data_);
  }

  void activate_bind(GLenum texture) const {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, tex_);
  }

  int width() const {
    return width_;
  }

  int height() const {
    return height_;
  }

  int tex() const {
    return tex_;
  }

  unsigned char* data() const {
    return data_;
  }
};