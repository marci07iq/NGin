#pragma once

#include <GL/glew.h>
//#define GLFW_DLL
#define GLFW_INCLUDE_GLU
#include "../GL_Lib/GLFW/glfw3.h"

#include "ClientCore.h"
#include "../Maths/Point.h"
#include "stb_image.h"

#ifdef _WIN64
#pragma comment(lib, "glew64.lib")
#else
#ifdef WIN32
#pragma comment(lib, "glew32.lib")
#endif
#endif




namespace NGin {
  namespace Graphics {
    GLuint png_texture_load(string filename, int& w, int& h);

    class RawTexture {
    public:
      GLuint id;
      iVec2 size;

      //Block copy. Use Texture for copyable version.
      RawTexture(const RawTexture&) = delete;
      void operator=(const RawTexture&) = delete;

      RawTexture();
      void load(string filename);

      void bind(GLuint uniformId, int unit = 0);

      ~RawTexture();
    };

    class Texture {
    public:
      shared_ptr<RawTexture> _raw;
      Texture() {
        _raw = NULL;
      }
      Texture(string filename) {
        _raw = make_shared<RawTexture>();
        _raw->load(filename);
      }
      void bind(GLuint uniformId, int unit = 0) {
        _raw->bind(uniformId, unit);
      }
    };

    class IconLocationFile;

    class Icon {
    public:
      string _name;
      IconLocationFile* _from;

      fVec2 _tl;
      fVec2 _br;
      //iVec2 _pxSize;

      Icon(string name, IconLocationFile* from, fVec2 tl, fVec2 br);
    };

    extern std::map<string, IconLocationFile*> ilfFiles;
    extern std::map<string, Icon*> icons;

    class IconLocationFile {
    public:
      string _ilf_name;
      Texture _dataSrc;

      std::map<string, Icon*> _icons;

      void load(string ilf_filename);

      IconLocationFile(string ilf_filename);
    };

    Icon* getIcon(string iconName, string ilfPath = "");
  }
}