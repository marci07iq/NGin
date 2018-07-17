#include "Plot.h"


int Plot::mouseEnter(int state) {
  mousebuttons = 0;
  return 0;
}

int Plot::mouseMoved(int mx, int my, int mox, int moy, set<key_location>& down) {
  int dx = mx - mox;
  int dy = my - moy;

  if (mousebuttons & 1) { //left, drag
    ox -= sx * dx;
    oy -= sy * dy;
    return 1;
  }

  return 0;
}

int Plot::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  if (evt._key._type == key::type_wheel) { //Zoom
    if (!isDown(down, KeyPlotZoomX)) {
      ox += ((cbx + cax) / 2.0 - mx)*(pow(1.1, -evt._key._keycode) - 1)*sx;
      sx *= pow(1.1, -evt._key._keycode);
    }

    if (!isDown(down, KeyPlotZoomY)) {
      oy += ((cby + cay) / 2.0 - my)*(pow(1.1, -evt._key._keycode) - 1)*sy;
      sy *= pow(1.1, -evt._key._keycode);
    }
    return 1;
  }
  if (checkKey(KeyPlotReset, evt._key) && evt._type == evt.evt_pressed) { //Reset
    /*double nsx, nsy;
    nsx = nsy = max(sx, sy);
    ox += ((cbx + cax) / 2.0 - mx)*(nsx - sx);
    oy += ((cby + cay) / 2.0 - my)*(nsy - sy);
    sx = nsx;
    sy = nsy;*/
    reloadAxes();
    return 1;
  }
  if (evt._key._type == key::type_mouse) {
    if (isIn(mx, my)) {
      mousebuttons ^= mousebuttons & (1 << evt._key._keycode); //remove bit for button;
      mousebuttons ^= ((evt._type == gui_event::evt_down) | (evt._type == gui_event::evt_pressed)) << evt._key._keycode;
    } else {
      mousebuttons = 0;
    }
  }
  if (evt._key._type == key::type_mouse) { //Mouse
    if (evt._key._keycode == 0 && evt._type == gui_event::evt_pressed) { //left, down
      int n = 0;
      for (auto&& dit : plotData) {
        double offset = 75 + (cbx - cax - 75) * double(n) / plotData.size();
        if (offset + 10 <= mx && mx <= offset + 30 && 5 <= my && my <= 15) {
          dit->enabled = !dit->enabled;
          return 1;
        }
        n++;
      }
    }
  }

  return 0;
}

void Plot::render(set<key_location>& down) {
  const int xedge = 75; //Line 34
  const int yedge = 40;

  //Grid

  glLineWidth(1.0f);

  glViewport(cax, cay, cbx - cax, cby - cay);

  double tickx = pow(10, floor(log10(100 * sx)));
  double ticky = pow(10, floor(log10(100 * sy)));

  if (tickx < 40 * sx) {
    tickx *= 2;
  }
  if (tickx < 40 * sx) {
    tickx *= 2;
  }
  if (ticky < 40 * sy) {
    ticky *= 2;
  }
  if (ticky < 40 * sy) {
    ticky *= 2;
  }

  double minx = ox - (cbx - cax) / 2.0*sx;
  double maxx = ox + (cbx - cax) / 2.0*sx;
  double miny = oy - (cby - cay) / 2.0*sy;
  double maxy = oy + (cby - cay) / 2.0*sy;

  minx -= fmodf(minx, tickx);
  miny -= fmodf(miny, ticky);
  maxx += tickx - fmodf(maxx, tickx);
  maxy += ticky - fmodf(maxy, ticky);

  glBegin(GL_LINES);
  setColor(textColor);
  for (double d = minx; d < maxx; d += tickx) {
    if (get(ox, sx, d, cbx - cax) > xedge) {
      glVertex2f(get(ox, sx, d, cbx - cax) - 0.5, yedge - 5.5);
      glVertex2f(get(ox, sx, d, cbx - cax) - 0.5, cby - cay);
    }
  }
  glEnd();

  glBegin(GL_LINES);
  setColor(textColor);
  for (double d = miny; d < maxy; d += ticky) {
    if (get(oy, sy, d, cby - cay) > yedge) {
      glVertex2f(xedge - 5.5, get(oy, sy, d, cby - cay) - 0.5);
      glVertex2f(cbx - cax, get(oy, sy, d, cby - cay) - 0.5);
    }
  }
  glEnd();

  //Frame

  glLineWidth(3.0f);

  glBegin(GL_LINES);
  setColor(activeColor);
  glVertex2f(cax + xedge, cay + yedge);
  glVertex2f(cax + xedge, cby);
  glVertex2f(cax + xedge, cay + yedge);
  glVertex2f(cbx, cay + yedge);
  glEnd();

  //Data

  for (auto&& dit : plotData) {
    if(dit->enabled) {
      setColor(dit->color);
      NoTypeIter* it = dit->first();
      do {
        auto nit = it->copy();
        double nittime = it->getX() + 2;
        if (nit->next()) {
          nittime = nit->getX();
        }
        glBegin(GL_LINES);
        glVertex2f(get(ox, sx, it->getX(), cbx - cax), get(oy, sy, it->getY(it->getX()), cby - cay));
        glVertex2f(get(ox, sx, nittime, cbx - cax)+1, get(oy, sy, it->getY(nittime), cby - cay));
        glEnd();
      
      } while (it->next());
      delete it;
    }
  }

  //Axis labels

  setColor(textColor);

  tickx = pow(10, floor(log10(100 * sx)));
  ticky = pow(10, floor(log10(100 * sy)));

  if (tickx < 80 * sx) {
    tickx *= 2;
  }
  if (tickx < 80 * sx) {
    tickx *= 2;
  }
  if (tickx < 80 * sx) {
    tickx *= 2;
  }
  if (ticky < 40 * sy) {
    ticky *= 2;
  }
  if (ticky < 40 * sy) {
    ticky *= 2;
  }

  minx = ox - (cbx - cax) / 2.0*sx;
  maxx = ox + (cbx - cax) / 2.0*sx;
  miny = oy - (cby - cay) / 2.0*sy;
  maxy = oy + (cby - cay) / 2.0*sy;

  minx -= fmodf(minx, tickx);
  miny -= fmodf(miny, ticky);
  maxx += tickx - fmodf(maxx, tickx);
  maxy += ticky - fmodf(maxy, ticky);

  for (double d = minx; d < maxx; d += tickx) {
    renderBitmapString(get(ox, sx, d, cbx - cax), 25, to_string(d, 2), textColor, 1);
  }
  for (double d = miny; d < maxy; d += ticky) {
    renderBitmapString(5, get(oy, sy, d, cby - cay) - 5, to_string(d, 2), textColor, 0);
  }

  //Legend

  int n = 0;

  for (auto&& dit : plotData) {
    double offset = xedge + (cbx - cax - xedge) * double(n)/ plotData.size();
    setColor(dit->color);

    glLineWidth(2.0f);

    glBegin(GL_LINES);
    glVertex2d(offset + 10, 10);
    glVertex2d(offset + 30, 10);
    if (!dit->enabled) {
      glVertex2d(offset + 15, 5);
      glVertex2d(offset + 25, 15);
    }
    glEnd();
    renderBitmapString(offset + 40, 5, dit->name, dit->color, 0);
    n++;
  }

  Graphics::resetViewport();
}

int Plot::get(double ori, double scale, double v, int h) {
  return h / 2.0 + (v - ori) / scale;
}

void Plot::reloadAxes() {
  double dax, day, dbx, dby;
  bool hasData = false;
  for (auto&& dit : plotData) {
    if(dit->enabled) {
      NoTypeIter* it = dit->first();
      do {
        auto nit = it->copy();
        double nittime = it->getX() + 2;
        if (nit->next()) {
          nittime = nit->getX();
        }
        if (!hasData) {
          hasData = true;
          glBegin(GL_LINES);
          dax = it->getX();
          day = it->getY(it->getX());
          dbx = nittime;
          dby = it->getY(nittime);
          if (dby > day) {
            swap(day, dby);
          }
          glEnd();
        }
        else {
          dax = min(it->getX(), dax);
          day = min(it->getY(it->getX()), day);
          day = min(it->getY(nittime), day);
          dbx = max(nittime, dbx);
          dby = max(it->getY(it->getX()), dby);
          dby = max(it->getY(nittime), dby);
        }
      } while (it->next());
      delete it;
    }
  }
  ox = (dax + dbx)/2;
  oy = (day + dby)/2;
  sx = (dbx - dax) * 7 / 5 / (cbx - cax);
  sy = (dby - day) * 7 / 5 / (cby - cay);
}

Plot::~Plot() {
  while (plotData.size()) {
    if (plotData.front() != NULL) {
      delete plotData.front();
      plotData.pop_front();
    }
  }
}