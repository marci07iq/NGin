#include "Texture.h"


namespace NGin {
  namespace Graphics {

    GLuint png_texture_load(string filename, int& w, int& h) {
      int comp;
      unsigned char* image = stbi_load(trimStr(filename).c_str(), &w, &h, &comp, STBI_rgb_alpha);

      if (image == nullptr)
        throw(std::string("Failed to load texture"));

      GLuint m_texture = 0;

      glGenTextures(1, &m_texture);

      glBindTexture(GL_TEXTURE_2D, m_texture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      if (comp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      else if (comp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

      //glBindTexture(GL_TEXTURE_2D, 0);

      stbi_image_free(image);

      return m_texture;
    }

    std::map<string, IconLocationFile*> ilfFiles;
    std::map<string, Icon*> icons;

    Icon* getIcon(string iconName, string ilfPath) {
      if (icons.count(iconName)) {
        return icons[iconName];
      }
      if (ilfPath != "") {
        new IconLocationFile(ilfPath);
        if (icons.count(iconName)) {
          return icons[iconName];
        }
      }
      return NULL;
    }

    IconLocationFile::IconLocationFile(string ilf_filename) {
      load(ilf_filename);
    }

    void IconLocationFile::load(string ilf_filename) {
      ifstream i(ilf_filename);

      getline(i, _ilf_name);
      ilfFiles[_ilf_name] = this;

      string iconName;
      getline(i, iconName);

      _dataSrc = Texture(iconName);

      iVec2 blocks;
      i >> blocks.x >> blocks.y;

      string icName;
      fVec2 tl;
      fVec2 size;
      while (i >> icName >> tl.x >> tl.y >> size.x >> size.y) {
        Icon* newIcon = new Icon(icName, this, tl / blocks, (tl + size) / blocks);
        _icons[icName] = newIcon;
        icons[icName] = newIcon;
      }
    }

    Icon::Icon(string name, IconLocationFile* from, fVec2 tl, fVec2 br) {
      _name = name;
      _from = from;
      _tl = tl;
      _br = br;
    }

    RawTexture::~RawTexture() {
      if (id != 0) {
        cout << "Deleted texture " << id << endl;
        glDeleteTextures(1, &id);
      }
    }

    void RawTexture::bind(GLuint uniformId, int unit) {
      //glBindTexture(GL_TEXTURE_2D, id);

      glActiveTexture(GL_TEXTURE0 + unit);
      glBindTexture(GL_TEXTURE_2D, id);
      glUniform1i(uniformId, unit);

    }

    void RawTexture::load(string filename) {
      if (endsWith(filename, "png")) {
        id = png_texture_load(filename, size.x, size.y);
        cout << "Created texture " << id << endl;
      } else {
        cout << "Unsupported file format" << endl;
      }
    }

    RawTexture::RawTexture() {

    }
  }
}