#ifndef KGEO_H
#define KGEO_H

#include "filter.h"

#include <vector>

class ObjectHierarchy;
class Object;

class KSimpleConfig;

// This is an import filter for files generated by the program KGeo,
// which was an interactive geometry program in kdeedu.  Kig is
// supposed to be its successor, and this import filter is part of my
// attempt to achieve that :)

// NOT ALL OF KGEO'S FORMAT IS SUPPORTED YET...

class KigFilterKGeo
  : public KigFilter
{
public:
  KigFilterKGeo();
  ~KigFilterKGeo();
  virtual bool supportMime ( const QString mime );
  virtual Result convert ( const QString from, KTempFile& to);
protected:
  Result loadMetrics (KSimpleConfig* );
  Result loadObjects (KSimpleConfig* );
  ObjectHierarchy* hier;
  std::vector<Object*> objs;

  int xMax;
  int yMax;
};

#endif
