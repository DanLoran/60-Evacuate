#include <cstdlib>
#include "evac.h"
#include "EvacRunner.h"
#include "LinkedList.h"

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
    evacCities.insert(cityList[evacIDs[i]], evacCities.first()); //create linked list of evac cities
  }

  while(evacCities.zeroth().isPastEnd()) //check if header is null to see if we still have evac cities with people in them
  {
    ListItr<City> eCityItr = evacCities.first();
    while(eCityItr.isPastEnd())
    { //we havent reached end of linked list
      City curECity = eCityItr.retrieve(); //get current evacCity
      vacateCity(curECity, routeCount, evacRoutes, -1); //pushes people out of all roads and generates evacRoutes
      //REMOVE EVACCITY IF EMPTY
      if(curECity.evacuees <= 0)
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
  }

} // evacuate

int Evac::min(int a, int b)
{
  if(a <= b) return a;
  else return b;
} // min

void Evac::vacateCity(City srcCity, int &routeCount, EvacRoute *evacRoutes, int prevCityID)
{
  for(int i = 0; i < srcCity.roadCount; i++)
  {
    Road curRoad = srcCity.roads[i];
    City dstCity = cityList[curRoad.destinationCityID];
    if(dstCity.ID != prevCityID) //make sure we don't send back to who called us
    {
      //create and store evacRoute
      int pplMoved;
      EvacRoute curRoute;
      curRoute.roadID = curRoad.ID;
      curRoute.time = time;

      if(curRoad.peoplePerHour < dstCity.population - dstCity.evacuees)
      { //we have room in the dest city
        pplMoved = min(curRoad.peoplePerHour, srcCity.evacuees); //Makes sure that we don't remove more people than the source city contains
      }
      else
      { //we dont have room in the dest city
        vacateCity(dstCity, routeCount, evacRoutes, srcCity.ID); //move people out of dstCity
        pplMoved = min(curRoad.peoplePerHour, dstCity.population - dstCity.evacuees); //find max people that can move to dstCity
        pplMoved = min(pplMoved, srcCity.evacuees); //Makes sure that we don't remove more people than the source city contains
      }
      //update evacuees that have moved
      curRoute.numPeople = pplMoved;
      srcCity.evacuees -= pplMoved;
      dstCity.evacuees += pplMoved;
      evacRoutes[routeCount] = curRoute;
      routeCount++;
      if(srcCity.evacuees <= 0)
      {
        return; //End this if srcCity is 0.
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
