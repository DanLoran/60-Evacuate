// Author: Sean Davis
#ifndef evacH
#define evacH

#include "EvacRunner.h"


class Evac
{
public:
  Evac(City *cities, int numCities, int numRoads);
  void evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount); // student sets evacRoutes and routeCount

private:
  City* cityList;
  int numCities;
  int time;
  void vacateCity(City srcCity, int &routeCount, EvacRoute *evacRoutes, int prevCityID);

}; // class Evac

int min(int a, int b)
{
  if(a <= b) return a;
  else return b;
}

//int compare (const void * a, const void * b);

/*class UWTable
{
private:
  int tableSize;
  bool* known;
  int* depth;
  int* roadCapacity;
  int* prevCityID;
public:
  UWTable(int size);
  fill();
}*/

#endif
