#include "Graphics.h"


namespace NGin {
  namespace Graphics {

    void Image::render(set<key_location>& down) {
      if (texture == 0) {
        texture = png_texture_load(filename.c_str(), w, h);
      }
      float rax, rbx, ray, rby;

      switch (align) {
      case 0: //Fill
        rax = cax;
        rbx = cbx;
        ray = cay;
        rby = cby;
        break;
      case 1: //Enclose, lock aspect
        rax = max(cax, (cax + cbx - ((cby - cay) * w) / h) / 2);
        rbx = min(cbx, (cax + cbx + ((cby - cay) * w) / h) / 2);
        ray = max(cay, (cay + cby - ((cbx - cax) * h) / w) / 2);
        rby = min(cby, (cay + cby + ((cbx - cax) * h) / w) / 2);
        break;
      case 2: //Center, lock size
        rax = (cax + cbx - w) / 2;
        rbx = (cax + cbx + w) / 2;
        ray = (cay + cby - h) / 2;
        rby = (cay + cby + h) / 2;
        break;
      default:
        throw "Invalid image align";
        return;
      }

      rax = rax / current->width * 2.0 - 1;
      ray = ray / current->height * 2.0 - 1;
      rbx = rbx / current->width * 2.0 - 1;
      rby = rby / current->height * 2.0 - 1;
      
      float* poss = new float[12];
      float* texs = new float[12];

      poss[0] = rax;
      poss[1] = ray;
      poss[2] = rax;
      poss[3] = rby;
      poss[4] = rbx;
      poss[5] = rby;

      poss[6] = rbx;
      poss[7] = rby;
      poss[8] = rbx;
      poss[9] = ray;
      poss[10] = rax;
      poss[11] = ray;

      texs[0] = 0;
      texs[1] = 1;
      texs[2] = 0;
      texs[3] = 0;
      texs[4] = 1;
      texs[5] = 0;

      texs[6] = 1;
      texs[7] = 0;
      texs[8] = 1;
      texs[9] = 1;
      texs[10] = 0;
      texs[11] = 1;

      GLuint vbo_p, vbo_l, vao;

      glGenBuffers(1, &vbo_p);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_p);
      glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), poss, GL_STATIC_DRAW);

      glGenBuffers(1, &vbo_l);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
      glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), texs, GL_STATIC_DRAW);

      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_p);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

      Gll::gllTextureS->bind();

      GLint loc = glGetUniformLocation(Gll::gllTextureS->_pID, "tex");
      if (loc != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(loc, 0);
      }
      else {
        cout << "tex not found!" << endl;
      }

      glDisable(GL_DEPTH_TEST);

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glEnable(GL_DEPTH_TEST);

      Gll::gllTextureS->unbind();

      glDeleteBuffers(1, &vbo_p);
      glDeleteBuffers(1, &vbo_l);
      glDeleteVertexArrays(1, &vao);

      delete[] texs;
      delete[] poss;
    }
  }
}