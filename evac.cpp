#include <cstdlib>
#include "evac.h"
#include <cstring>
#include "EvacRunner.h"
#include "QueueAr.h"
#include <iostream>

using namespace std;

Evac::Evac(City *citie, int numCitie, int numRoad) : numCities(numCitie), numRoadIDs(numRoad * 2), time(1)
{
  //change the name to cityAr later
  cityList = new City[numCities];
  roadsUsed = new bool[numRoadIDs]();
  cityDepth = new int[numCities];
  //finalSprint = false;

  for(int i = 0; i < numCities; i++)
  {
    cityList[i] = citie[i]; //sort by ID
    cityList[i].roads = new Road[cityList[i].roadCount];
    cityDepth[i] = -9001; //it's under 9000!!!!
    for(int j = 0; j < citie[i].roadCount; j++) //deep copy all roads
    {
      cityList[i].roads[j] = citie[i].roads[j];
    }
  }
} // Evac()


void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount)
{
  routeCount = 0;
  evacCities = new City*[numEvacs];

  for(int i = 0; i < numEvacs; i++)
  {
    evacCities[i] = &cityList[evacIDs[i]]; //create array of evac cities
  }
  calcDepth(numEvacs);
  //cout << "made it to the start of the while loop" << endl;
  while(checkEvacs()) //check if we still have evac cities with people in them
  {
    for(int indx = 0; indx < numEvacs; indx++)
    { //we are looping through evacCities
      City* curECity = evacCities[indx]; //get current evacCity

      if(curECity->population + curECity->evacuees > 0) //we still have people in this city
      {
        //pushes people out of all roads and generates evacRoutes
        vacateEvac(curECity, routeCount, evacRoutes);
      }
      resetUsed(); //reset roads visited to false
    }
    time++;
    //cout << "Time Incremented" << endl;
  }

} // evacuate

bool Evac::checkEvacs()
{
  for(int i = 0; i < numEvacs; i++)
  {
    if(evacCities[i]->population + evacCities[i]->evacuees > 0)
    { //we find a city with people in it
      return false;
    }
  }
  return true;
}

int Evac::min(int a, int b)
{
  if(a <= b) return a;
  else return b;
} // min

void Evac::resetUsed()
{ //no cities have been visited
  memset(roadsUsed, false, (sizeof(bool)*numRoadIDs));
  /*for(int i = 0; i < numRoadIDs; i++)
  {
    roadsUsed[i] = false;
  }*/
}

void Evac::calcDepth(int numEvacs)
{
  Queue<City*> depthHolder(numCities);
  City* curCity;
  City* adjCity;
  int adjCityID;
  int nextDepth;
  for(int i = 0; i < numEvacs; i++)
  {
    depthHolder.enqueue(evacCities[i]); //put evac cities in queue
    cityDepth[evacCities[i]->ID] = 0; //set depth to zero
  }

  while(!depthHolder.isEmpty())
  {
    curCity = depthHolder.dequeue();
    nextDepth = cityDepth[curCity->ID] + 1;
    for(int i = 0; i < curCity->roadCount; i++) //enqueue all unvisited adjacent cities and mark their depth.
    {
      adjCityID = curCity->roads[i].destinationCityID;
      adjCity = &cityList[adjCityID];
      if(cityDepth[adjCityID] == -1) //This city has not yet been assigned a depth.
      {
        cityDepth[adjCityID] = nextDepth;
        depthHolder.enqueue(adjCity);
      }
    }

  }

}
bool Evac::vacateEvac(City *srcCity, int &routeCount, EvacRoute *evacRoutes)
{
  bool isInternal = checkInternal(srcCity);

  for(int i = 0; i < srcCity->roadCount; i++)
  {
    Road curRoad = srcCity->roads[i];
    City* dstCity = &cityList[curRoad.destinationCityID];

    if(validERoute(isInternal, srcCity, dstCity, curRoad))
    {
      //create and store evacRoute
      int pplMoved;
      EvacRoute eRoute;
      eRoute.roadID = curRoad.ID;
      eRoute.time = time;

      if(curRoad.peoplePerHour < dstCity->population - dstCity->evacuees)
      { //we have room in the dest city
          pplMoved = min(curRoad.peoplePerHour, (srcCity->evacuees + srcCity->population) ); //Makes sure that we don't remove more people than the source city containsThiscodewrittenbyD4ni3lCLor4nand3mily/\/\H3ry
      }
      else
      { //we dont have room in the dest city
        if(cityDepth[dstCity->ID] == 0)
        { //this is an evacCity
          vacateEvac(dstCity, routeCount, evacRoutes);
        }
        else
        { //this is not an evacCity
          vacateCity(dstCity, routeCount, evacRoutes); //move people out of dstCity
        }
        pplMoved = min(curRoad.peoplePerHour, dstCity->population - dstCity->evacuees); //find max people that can move to dstCity
        pplMoved = min(pplMoved, (srcCity->evacuees + srcCity->population) ); //Makes sure that we don't remove more people than the source city contains
      }

      eRoute.numPeople = pplMoved;
      dstCity->evacuees += pplMoved;

      if(srcCity->evacuees >= pplMoved)
      {
        srcCity->evacuees -= pplMoved;
      }
      else
      {
        //First send off the evacuees, then the population.
        pplMoved -= srcCity->evacuees;
        srcCity->evacuees = 0;
        srcCity->population -= pplMoved;
      }

      roadsUsed[curRoad.ID] = true; //we've used this road
      evacRoutes[routeCount] = eRoute;
      routeCount++;

      if((srcCity->evacuees + srcCity->population) <= 0)
      {
        return true; //Stop looking through roads if srcCity has no one left 0.
      }
    }
  }
  return false;
}

bool Evac::checkInternal(City* internal)
{
  for (int i = 0; i < internal->roadCount; i++)
  {
    if(cityDepth[internal->roads[i].destinationCityID] != 0)
    {
      return false;
    }
  }
  return true;
}
//Should accept bool isEvacCity, set to true if srcCity is an evacCity.
//Changed: now accepts a bool isEvacCity
void Evac::vacateCity(City *srcCity, int &routeCount, EvacRoute *evacRoutes)
{
  for(int i = 0; i < srcCity->roadCount; i++)
  {
    Road curRoad = srcCity->roads[i];
    City* dstCity = &cityList[curRoad.destinationCityID];
    //TODO: add depth check
    if(validERoute(false, srcCity, dstCity, curRoad)) //make sure we haven't used this road already
    {
      //create and store evacRoute
      int pplMoved;
      EvacRoute eRoute;
      //cout << "Evac route created!" << endl;
      eRoute.roadID = curRoad.ID;
      eRoute.time = time;
      //TODO: change check for evacCity by setting evacuees = pop and pop * 2
      if(curRoad.peoplePerHour < dstCity->population - dstCity->evacuees)
      { //we have room in the dest city
      //This questionable multiplication only adds srcCity's population if its an evacCity, without an if check.
        pplMoved = min(curRoad.peoplePerHour, srcCity->evacuees); //Makes sure that we don't remove more people than the source city containsThiscodewrittenbyD4ni3lCLor4nand3mily/\/\H3ry
      }
      else
      { //we dont have room in the dest city
        vacateCity(dstCity, routeCount, evacRoutes); //move people out of dstCity
        pplMoved = min(curRoad.peoplePerHour, dstCity->population - dstCity->evacuees); //find max people that can move to dstCity
        pplMoved = min(pplMoved, srcCity->evacuees); //Makes sure that we don't remove more people than the source city contains
      }
      //update evacuees that have moved
      eRoute.numPeople = pplMoved;
      srcCity->evacuees -= pplMoved;
      dstCity->evacuees += pplMoved;

      roadsUsed[curRoad.ID] = true; //we've used this road
      evacRoutes[routeCount] = eRoute;
      routeCount++;
      if(srcCity->evacuees <= 0)
      {
        return; //Stop looking through roads if srcCity has no one left 0.
      }
    }
  }
}

bool Evac::validERoute(bool isInternal, City* srcCity, City* dstCity, Road curRoad)
{
  if(roadsUsed[curRoad.ID])
  {
    return false;
  }
  if(!isInternal && (cityDepth[srcCity->ID] > cityDepth[dstCity->ID]))
  {
    return false;
  }
  else if (dstCity->population <= 0)
  {
    return false;
  }
  return true;
}
