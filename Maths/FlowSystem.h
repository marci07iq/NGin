#pragma once

#include "Keyframe.h"

//typedef int FlowType;

//Value, Derivative, Time
//Derivative MUST BE WITHOUT floating point errors!
template <typename V, typename D,  typename T>
class FlowVertex;

//Value, Derivative, Time
//Derivative MUST BE WITHOUT floating point errors!
template <typename V, typename D, typename T>
class FlowEdge
{
public:
  D _flow; //Temporary - internal use
  D _capacity; //Temporary - internal use
  D _totalFlow; //Actual flow on edge
  D _totalCapacity; //Set capacity

  // _u -> _v
  FlowVertex<V, D, T>* _u;
  FlowVertex<V, D, T>* _v;

  bool _real;

  FlowEdge(D flow, D capacity, FlowVertex<V, D, T>* u, FlowVertex<V, D, T>* v, bool real = false)
  {
    _totalFlow = D(0);
    _flow = flow;
    _capacity = capacity;
    _totalCapacity = _capacity;
    _u = u;
    _v = v;
    _real = real;
  }
};

//Value, Derivative, Time
//Derivative MUST BE WITHOUT floating point errors!
template <typename V, typename D, typename T>
class FlowVertex
{
public:
  int _h; //Temporary - internal use
  D _flow; //Temporary - internal use
  D _firstDelta; //Temporary - internal use
  D _delta; //Actual detla
  list<FlowEdge<V, D, T>*> _edges;

  keyframe<linear<V, D, T>, V, T> _val; 
  V _maxVal; //Maximum battery storage

  //Goal (+: out)
  D _goal; //Requested delta
  //Charge max (-)
  D _maxCharge; //Maximum battery charge rate
  bool _canCharge = true; //Temporary - internal use
  //Drain max (+)
  D _maxDrain; //Maximum battery drain rate
  bool _canDrain = true; //Temporary - internal use

  int _ID; //Temporary - internal use

  FlowVertex(int h, D flow, D goal, D maxCharge, D maxDrain, V maxVal, V startVal, T startTime)
  {
    _h = h;
    _flow = flow;
    _delta = D(0);
    _goal = goal;
    _maxCharge = maxCharge;
    _maxDrain = maxDrain;
    _maxVal = maxVal;
    _val.addFrame(startTime, linear<V, D, T>{startVal, startTime});
  }
  bool load(xml_node<>* me) {

  }
  ~FlowVertex() {
    while (_edges.size()) {
      delete *(_edges.begin());
      _edges.pop_front();
    }
  }
};

//Value, derivative, time type
//Derivative MUST BE WITHOUT floating point errors!
template <typename V, typename D, typename T>
class FlowGraph
{
public:
  list<FlowVertex<V, D, T>*> _ver;

  FlowVertex<V, D, T>* _s;
  FlowVertex<V, D, T>* _t;
  T _lastUpdate;

  FlowGraph();

  void preflow(FlowVertex<V, D, T>* s);

  FlowVertex<V, D, T>* getFirstOverflow();
  
  void updateReverseFlow(FlowEdge<V, D, T>* i, D flow);

  bool push(FlowVertex<V, D, T>* u);

  void relabel(FlowVertex<V, D, T>* u);

  FlowEdge<V, D, T>* addEdge(FlowVertex<V, D, T>* u, FlowVertex<V, D, T>* v,  D maxflow, bool real = true, D flow = D(0));

  pair<FlowEdge<V, D, T>*, FlowEdge<V, D, T>*> addSymmetricEdge(FlowVertex<V, D, T>* u, FlowVertex<V, D, T>* v, D w, D flow = D(0));

  FlowVertex<V, D, T>* addVertex(D goal, D maxCharge, D maxDrain, V maxval, V startval = V(0), T startitme = T(0));

  void addRootEdges(int mode);

  void clean(bool decrease);
 
  void reset();

  void getMaxFlow();
  void getMaxFlow(FlowVertex<V, D, T>* s, FlowVertex<V, D, T>* t);

  pair<vector<T>, V> goTo(T time, FlowVertex<V, D, T>* chg = NULL, V chgval = V(0));

  ~FlowGraph();
};

template <typename V, typename D, typename T>
FlowGraph<V, D, T>::FlowGraph() {
  _lastUpdate = -100; ///TODO: better

  _s = new FlowVertex<V, D, T>(0, 0, 0, 0, 0, 0, 0, 0);
  _ver.push_back(_s);

  _t = new FlowVertex<V, D, T>(0, 0, 0, 0, 0, 0, 0, 0);
  _ver.push_back(_t);
}

template <typename V, typename D, typename T>
FlowEdge<V, D, T>* FlowGraph<V, D, T>::addEdge(FlowVertex<V, D, T>* u, FlowVertex<V, D, T>* v, D maxflow, bool real, D flow)
{
  FlowEdge<V, D, T>* n = new FlowEdge<V, D, T>(flow, maxflow, u, v, real);
  u->_edges.push_back(n);
  return n;
}

template <typename V, typename D, typename T>
pair<FlowEdge<V, D, T>*, FlowEdge<V, D, T>*> FlowGraph<V, D, T>::addSymmetricEdge(FlowVertex<V, D, T>* u, FlowVertex<V, D, T>* v, D w, D flow) {
  return{ addEdge(u, v, w, true, flow),addEdge(v, u, w, true, flow) };
}

template <typename V, typename D, typename T>
FlowVertex<V, D, T>* FlowGraph<V, D, T>::addVertex(D goal, D maxCharge, D maxDrain, V maxval, V startval, T starttime) {
  FlowVertex<V, D, T>* n = new FlowVertex<V, D, T>(0, 0, goal, maxCharge, maxDrain, maxval, startval, starttime);
  _ver.push_back(n);
  return n;
}

// perform preflow
template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::preflow(FlowVertex<V, D, T>* s)
{
  s->_h = _ver.size();

  for (auto&& it : s->_edges)
  {
    it->_flow = it->_capacity;

    it->_v->_flow += it->_flow;

    addEdge(it->_v, s, 0, false, -it->_flow);
    //it->_v->_edges.push_back(new FlowEdge(-it->_flow, 0, it->_v, s));
  }
}

// first overflowing vertex
template <typename V, typename D, typename T>
FlowVertex<V, D, T>* FlowGraph<V, D, T>::getFirstOverflow()
{
  for (auto&& it : _ver) {
    if (it != _s && it != _t) {
      if (it->_flow > D(0))
        return it;
    }
  }
  // NULL if none
  return NULL;
}

// Update reverse flow for flow added on ith Edge
template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::updateReverseFlow(FlowEdge<V, D, T>* i, D flow)
{
  FlowVertex<V, D, T>* u = i->_v;
  FlowVertex<V, D, T>* v = i->_u;

  for (auto&& j : u->_edges)
  {
    if (j->_v == v)
    {
      j->_flow -= flow;
      return;
    }
  }

  addEdge(u, v, flow, false);
}

// push from u
template <typename V, typename D, typename T>
bool FlowGraph<V, D, T>::push(FlowVertex<V, D, T>* u)
{
  for (auto&& it : u->_edges)
  {
    if (it->_flow == it->_capacity)
      continue;

    if (u->_h > it->_v->_h)
    {
      D flow = min(it->_capacity - it->_flow,
        u->_flow);

      u->_flow -= flow;

      it->_v->_flow += flow;

      it->_flow += flow;

      updateReverseFlow(it, flow);

      return true;
    }
  }

  return false;
}

// relabel u
template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::relabel(FlowVertex<V, D, T>* u)
{
  int mh = INT_MAX;

  for (auto&& it : u->_edges)
  {
    if (it->_flow == it->_capacity)
      continue;

    if (it->_v->_h < mh)
    {
      mh = it->_v->_h;
      u->_h = mh + 1;
    }
  }
}

//connect to supersink, supersource.
//0: goal, 1: charge, 2: drain
template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::addRootEdges(int mode) {
  D requested;

  for (auto&& it : _ver) {
    requested = D(0);
    switch (mode) {
    case 0:
      requested = it->_goal;
      break;
    case 1:
      if (it->_canCharge) {
        requested = it->_maxCharge;
      }
      if (it->_goal < D(0)) {
        requested += it->_goal - it->_firstDelta;
      }
      break;
    case 2:
      if (it->_canDrain) {
        requested = it->_maxDrain;
      }
      if (it->_goal > D(0)) {
        requested += it->_goal - it->_firstDelta;
      }
      break;
    }
    if (requested > D(0)) {
      addEdge(it, _t, requested, false);
    }
    if (requested < D(0)) {
      addEdge(_s, it, -requested, false);
    }
  }
}

//clean graph, decrease capacity?
template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::clean(bool decrease) {
  for (auto&& it : _ver) {
    auto et = it->_edges.begin();

    while (et != it->_edges.end()) {
      if (!(*et)->_real) {
        auto et2 = et;
        ++et;
        it->_edges.erase(et2);
      }
      else {
        if((*et)->_flow > D(0)) {
          (*et)->_u->_delta -= (*et)->_flow;
          (*et)->_v->_delta += (*et)->_flow;

          (*et)->_totalFlow += (*et)->_flow;
          if (decrease) {
            (*et)->_capacity -= (*et)->_flow;
          }
        }
        (*et)->_flow = D(0);
        
        ++et;
      }
    }
    it->_flow = D(0);
    it->_h = 0;
  }
}

template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::reset() {
  for (auto&& it : _ver) {
    it->_delta = D(0);
    for (auto&& et : it->_edges) {
      et->_capacity = et->_totalCapacity;
      et->_totalFlow = D(0);
    }
  }
}

// calc max flow
template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::getMaxFlow(FlowVertex<V, D, T>* s, FlowVertex<V, D, T>* t)
{
  _s = s;
  _t = t;
  preflow(s);

  while (getFirstOverflow() != NULL)
  {
    FlowVertex<V, D, T>* u = getFirstOverflow();
    if (!push(u))
      relabel(u);
  }
}
template <typename V, typename D, typename T>
void FlowGraph<V, D, T>::getMaxFlow() {
  //Reset the graph
  reset();
  clean(true);
  //Add root edges, goal mode
  addRootEdges(0);
  getMaxFlow(_s, _t);

  //Wipe graph
  clean(true);

  for (auto&& it : _ver) {
    it->_firstDelta = it->_delta;
  }

  //Add root edges, charge mode
  addRootEdges(1);
  getMaxFlow(_s, _t);

  //Wipe graph
  clean(true);

  //Add root edges, drain mode
  addRootEdges(2);
  getMaxFlow(_s, _t);

  clean(true);
}

template <typename V, typename D, typename T>
pair<vector<T>, V> FlowGraph<V, D, T>::goTo(T time, FlowVertex<V, D, T>* chg, V chgval) {
  vector<T> endtimes;
  V leftover = V(0);
  if (time >= _lastUpdate) {
    for (auto&& it : _ver) {
      V val = it->_val.getAt(time);
      if (chg == it) {
        val += chgval;
        val = max(V(0), min(val, it->_maxVal));
        leftover = chgval - val + it->_val.getAt(time);
      }
      it->_canCharge = (val < V(0.99) * it->_maxVal);
      it->_canDrain = (V(0.01) * it->_maxVal < val);
    }
    getMaxFlow();
    for (auto&& it : _ver) {
      V nval = it->_val.getAt(time);
      if (chg == it) {
        nval += chgval;
        nval = max(V(0), min(nval, it->_maxVal));
      }
      D change = it->_delta - it->_goal;
      if (!it->_canCharge) {
        change = min(D(0), change);
      }
      if (!it->_canDrain) {
        change = max(D(0), change);
      }
      if (change > D(0)) {
        endtimes.push_back(time + to_doubleT<time_type_s, V>(it->_maxVal - nval) / to_doubleT<T, D>(change));
      }
      if (change < D(0)) {
        endtimes.push_back(time + to_doubleT<time_type_s, V>(V(0) - nval) / to_doubleT<T, D>(change));
      }
      
      
      it->_val.addFrame(time, linear<V, D, T>(nval, time, change));
      
    }
    _lastUpdate = time;
  }
  return make_pair(endtimes, leftover);
}

template <typename V, typename D, typename T>
FlowGraph<V, D, T>::~FlowGraph() {
  while (_ver.size()) {
    delete *(_ver.begin());
    _ver.pop_front();
  }
}
