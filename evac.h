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
  //Changed: now accepts bool isEvacCity.
  void vacateCity(City &srcCity, int &routeCount, EvacRoute *evacRoutes, int prevCityID, bool isEvacCity);
  int min(int a, int b);

}; // class Evac


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
