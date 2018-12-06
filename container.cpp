#include <iostream>
#include <map>
using namespace std;
typedef void *MapRef;

extern "C" {
MapRef createMapLL() { return (MapRef) new map<long, long>(); }
void disposeMapLL(MapRef m) { delete ((map<long, long> *)m); }

void setMapLL(MapRef m, long k, long v) {
  auto m_ = (map<long, long> *)m;
  (*m_)[k] = v;
}
long getMapLL(MapRef m, long k) {
  auto m_ = (map<long, long> *)m;
  return (*m_)[k];
}
void printMapLL(MapRef m) {
  auto m_ = (map<long, long> *)m;
  for (auto x : (*m_)) {
    cout << x.first << ", " << x.second << endl;
  }
}
long getMapLLSectionStart(MapRef m, long addr) {
  auto m_ = (map<long, long> *)m;
  for (auto x : (*m_)) {
    if (addr >= x.first && addr < (x.first + x.second))
      return x.first;
  }
  return 0;
}
}
