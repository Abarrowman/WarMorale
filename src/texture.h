#pragma once

#include <stb/stb_image.h>

class texture {
public:
  GLuint tex;
  int width;
  int height;

  texture(const char* filename) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    int channel_count;
    unsigned char* data = stbi_load(filename, &width, &height, &channel_count, 4);
    if (!data) {
      fprintf(stderr, "Could not read file %s\n", filename);
      exit(-1);
    }

    glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGBA,
      width,
      height,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      data);
    stbi_image_free(data);

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
  texture(texture&& old) : tex(old.tex), width(old.width), height(old.height) {
    old.tex = 0;
  }

  ~texture() {
    glDeleteTextures(1, &tex);
  }

  void activate_bind(GLenum texture) const {
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_2D, tex);
  }
};