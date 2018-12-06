#ifndef CONTAINER_H
#define CONTAINER_H
typedef struct Map *MapRef;
MapRef createMapLL();
void setMapLL(MapRef m, long k, long v);
long getMapLL(MapRef m, long k);
void printMapLL(MapRef m);
long getMapLLSectionStart(MapRef m, long addr);
#endif

