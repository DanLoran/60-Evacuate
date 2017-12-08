// Author: Sean Davis
#ifndef evacH
#define evacH

#include "EvacRunner.h"
#include "LinkedList.h"

class Evac
{
public:
  Evac(City *citie, int numCitie, int numRoad);
  void evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount); // student sets evacRoutes and routeCount

private:
  City* cityList;
  bool* roadsUsed;
  int numCities;
  int numRoads;
  int time;

  //Changed: now accepts bool isEvacCity.
  void vacateCity(City* srcCity, int &routeCount, EvacRoute *evacRoutes, bool isEvacCity);
  int min(int a, int b);
  void advanceItrs(ListItr<City*> &curItr, ListItr<City*> &prevItr);
  void resetUsed();
  void setItr(ListItr<City*> &curItr, ListItr<City*> &prevItr, List<City*> const &masterList);


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
