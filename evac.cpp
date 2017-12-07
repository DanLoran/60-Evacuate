#include <cstdlib>
#include "evac.h"
#include "EvacRunner.h"
#include "LinkedList.h"
#include <iostream>

using namespace std;

Evac::Evac(City *citie, int numCitie, int numRoads) : numCities(numCitie), time(0)
{
  //change the name to cityAr later
  cityList = new City[numCities];
  for(int i = 0; i < numCities; i++)
  {
    cityList[citie[i].ID] = citie[i]; //sort by ID
  }
} // Evac()


void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount)
{
  /*evacCity = new City[numEvacs];
  for(int i = 0; i < numEvacs; i++)
  {
    evacCity[i] = cityList[cityIndex[evacIDs[i]]]; //find city with cityID that matches evacID, and place in evacCity
  }
  qsort(evacCity, numEvacs, sizeof(City), compare); //sort evacCities by pop
  UWTable cityDepth(numCities);*/
 routeCount = 0;
  List<City> evacCities;
  for(int i = 0; i < numEvacs; i++)
  {
    evacCities.insert(cityList[evacIDs[i]], evacCities.zeroth()); //create linked list of evac cities
  }
  cout << "made it to the start of the while loop" << endl;
  while(evacCities.zeroth().isPastEnd()) //check if header is null to see if we still have evac cities with people in them
  {
    cout << "while loop starting" << endl;
    ListItr<City> eCityItr = evacCities.zeroth();
    while(eCityItr.isPastEnd())
    { //we havent reached end of linked list
      City curECity = eCityItr.retrieve(); //get current evacCity
      vacateCity(curECity, routeCount, evacRoutes, -1, true); //pushes people out of all roads and generates evacRoutes
      //REMOVE EVACCITY IF EMPTY
      //Changed: checks that evacuees and population are both <= 0.
      if(curECity.evacuees <= 0 && curECity.population <= 0)
      {
        evacCities.removeNode(eCityItr);
        //itr does NOT need to be incremented because it now points to the node after the deleted node.
        //See LinkedList.cpp for more details.
      }
      else
      {
        eCityItr.advance(); //incriment itr
      }
    }
    time++;
    cout << "Time Incremented" << endl;
  }

} // evacuate

int Evac::min(int a, int b)
{
  if(a <= b) return a;
  else return b;
} // min

//Should accept bool isEvacCity, set to true if srcCity is an evacCity.
//Changed: now accepts a bool isEvacCity
void Evac::vacateCity(City srcCity, int &routeCount, EvacRoute *evacRoutes, int prevCityID, bool isEvacCity)
{
  for(int i = 0; i < srcCity.roadCount; i++)
  {
    Road curRoad = srcCity.roads[i];
    City dstCity = cityList[curRoad.destinationCityID];
    if(dstCity.ID != prevCityID) //make sure we don't send back to who called us
    {
      //create and store evacRoute
      int pplMoved;
      EvacRoute eRoute;
      cout << "Evac route created!" << endl;
      eRoute.roadID = curRoad.ID;
      eRoute.time = time;

      if(curRoad.peoplePerHour < dstCity.population - dstCity.evacuees)
      { //we have room in the dest city
      //Changed: this line to sum srcCity evacuees and population if isEvacCity?
      //This questionable multiplication only adds srcCity's population if its an evacCity, without an if check.
        pplMoved = min(curRoad.peoplePerHour, (srcCity.evacuees + (srcCity.population * isEvacCity) ) ); //Makes sure that we don't remove more people than the source city contains
      }
      else
      { //we dont have room in the dest city
        vacateCity(dstCity, routeCount, evacRoutes, srcCity.ID, false); //move people out of dstCity
        pplMoved = min(curRoad.peoplePerHour, dstCity.population - dstCity.evacuees); //find max people that can move to dstCity
        //See earlier comment about questionable multiplcation
        pplMoved = min(pplMoved, (srcCity.evacuees + (srcCity.population * isEvacCity) ) ); //Makes sure that we don't remove more people than the source city contains
      }
      //update evacuees that have moved
      //Changed: If isEvacCity is true, we need to first subtract from evacuees and then population. This if/else block.
      eRoute.numPeople = pplMoved;

      if(!isEvacCity || srcCity.evacuees >= pplMoved)
      {
        srcCity.evacuees -= pplMoved;
      }
      else
      {
        //First send off the evacuees, then the population.
        pplMoved -= srcCity.evacuees;
        srcCity.evacuees = 0;
        srcCity.population -= pplMoved;
      }
      //Regardless of isEvacCity, dstCity is always treated the same.
      dstCity.evacuees += pplMoved;

      evacRoutes[routeCount] = eRoute;
      routeCount++;
      if(srcCity.evacuees + (srcCity.population * isEvacCity) <= 0)
      {
        return; //Stop looking through roads if srcCity has no one left 0.
      }
    }
  }
}
/*int compare (const void * a, const void * b)
{
  if ((City*)a->population > (City*)b->population) return -1;
  if ((City*)a->population == (City*)b->population) return 0;
  if((City*)a->population < (City*)b->population) return 1;
}*/

/*UWTable::UWTable(int size): tableSize(size)
{
  known = new bool[tableSize](); //set all to false
  depth = new int[tableSize];
  roadCapacity = new int[tableSize];
  prevCityID = new int[tableSize];

  for(int i = 0; i < tableSize; i++)
  {
    depth[i] = tableSize + 1; //set each int to larger than largest possible depth
  }
}

UWTable::fill(int numEvacs, City* evacCity)
{
  for(int i = 0; i < numEvacs; i++)
  {
    int curCity = evacCity[i].ID;
    depth[curCity] = 0; //set all evac Cities to a depth of zero
    known[curCity] = true;
    for(int j = 0; j < evacCity[i].roadCount; j++)
    {
      int nextCity = evacCity[i].roads[j].destinationCityID;
      depth[nextCity] = 1; //every city from this city has depth 1
      prevCityID[nextCity] = curCity;
      roadCapacity[nextCity] =  evacCity[i].roads[j].peoplePerHour; //since this is first depth, this is the capacity
    }
  }
//Put cities in a maxHeap. Sort by biggest road leading out of the city.
}*/
