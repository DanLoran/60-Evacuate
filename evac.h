// Author: Sean Davis
#ifndef evacH
#define evacH

#include "EvacRunner.h"

class MyCity
{
public:
  int ID;
  int x;
  int y;
  int population;
  int evacuees;
  Road *roads;
  int roadCount;
  int depth; //added
  int isEvacCity; //added
  MyCity() : evacuees(0), roads(NULL), roadCount(0), depth(-9001), isEvacCity(false){}
  //MyCity(City oldCity);
  MyCity operator = (City const &oldCity);
  ~MyCity(){delete [] roads;}
};

class Evac
{
public:
  Evac(City *citie, int numCitie, int numRoad);
  void evacuate(int *evacIDs, int numEvac, EvacRoute *evacRoutes,
    int &routeCount); // student sets evacRoutes and routeCount

private:
  MyCity* cityList;
  bool* cityVisited;
  MyCity** evacCities;
  int numCities;
  int numRoadIDs;
  int time;
  int numEvacs;
  int minDepth;
  int minDepthIndex; //is index of first city of minDepth depth.

  bool vacateEvac(City *srcCity, int &routeCount, EvacRoute *evacRoutes); //returns if srcCity empties
  void vacateCity(City* srcCity, int &routeCount, EvacRoute *evacRoutes);
  bool isEvacComplete(); //CHANGED: added checkEvacs
  int min(int a, int b);
  void resetUsed();
  bool checkInternal(City* internal);
  void calcDepth(int numEvacs);
  bool validERoute(MyCity* srcCity, MyCity* dstCity);


  void findOuterRing(int numEvacs);
  void bfsEvac(int minDepth, int &routeCount, EvacRoute* EvacRoutes);
  //bool finalSprint;

}; // class Evac

int comprar(const void* p1, const void* p2);

#endif
