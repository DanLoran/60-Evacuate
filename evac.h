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
  City** evacCities;
  int* cityDepth;
  int numCities;
  int numRoadIDs;
  int time;
  int remainingEvacs;

  bool vacateEvac(City *srcCity, int &routeCount, EvacRoute *evacRoutes); //returns if srcCity empties
  void vacateCity(City* srcCity, int &routeCount, EvacRoute *evacRoutes);
  int min(int a, int b);
  void resetUsed();
  bool checkInternal(City* internal);
  void calcDepth(int numEvacs);
  bool validERoute(bool isInternal, City* srcCity, City* dstCity, Road curRoad);

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
