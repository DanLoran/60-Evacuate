#include <cstdlib>
#include "evac.h"
#include "EvacRunner.h"
#include "LinkedList.h"
#include <iostream>

using namespace std;

Evac::Evac(City *citie, int numCitie, int numRoad) : numCities(numCitie), numRoads(numRoad), time(1)
{
  //change the name to cityAr later
  cityList = new City[numCities];
  for(int i = 0; i < numCities; i++)
  {
    cityList[i] = citie[i]; //sort by ID
    cityList[i].roads = new Road[cityList[i].roadCount];
    for(int j = 0; j < citie[i].roadCount; j++) //deep copy all roads
    {
      cityList[i].roads[j] = citie[i].roads[j];
    }
  }
  roadsUsed = new bool[numRoads]();
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
  List<City*> evacCities;
  ListItr<City*> eCityItr = evacCities.zeroth();
  ListItr<City*> prevItr;
  for(int i = 0; i < numEvacs; i++)
  {
    evacCities.insert(&cityList[evacIDs[i]], eCityItr); //create linked list of evac cities
  }
  //cout << "made it to the start of the while loop" << endl;
  while(!(evacCities.isEmpty())) //check if header is null to see if we still have evac cities with people in them
  {
    setItr(eCityItr, prevItr, evacCities); //set to one before eCityItr to track what points the the current node
    resetUsed(); //reset cities visited to false
    //City curECity; //Tried this, met up with sigbert.
    //cout << "while loop starting" << endl;
    while(!eCityItr.isPastEnd())
    { //we havent reached end of linked list
      //TODO: create curECity outside of while loop.
      City* curECity = eCityItr.retrieve(); //get current evacCity
      //TODO: Instead of just passing ID of past city, should pass array of past ID cities.
      vacateCity(curECity, routeCount, evacRoutes, true); //pushes people out of all roads and generates evacRoutes
      //REMOVE EVACCITY IF EMPTY
      //Changed: checks that evacuees and population are both <= 0.
      if(curECity->evacuees <= 0 && curECity->population <= 0)
      {
        //cout << "this shouldn't be triggering tho" << endl;
        evacCities.removeNode(prevItr, eCityItr);
        //itr does NOT need to be incremented because it now points to the node after the deleted node.
        //See LinkedList.cpp for more details.
      }
      else
      {
        advanceItrs(eCityItr, prevItr); //incriment itr
      }
    }
    time++;
    //cout << "Time Incremented" << endl;
  }

} // evacuate

void Evac::setItr(ListItr<City*> &curItr, ListItr<City*> &prevItr, List<City*> const &masterList)
{
  curItr = masterList.first();
  prevItr = masterList.zeroth(); //set to one before curItr to track what points the the current node
}

void Evac::advanceItrs(ListItr<City*> &curItr, ListItr<City*> &prevItr)
{
  curItr.advance();
  prevItr.advance();
}
int Evac::min(int a, int b)
{
  if(a <= b) return a;
  else return b;
} // min

void Evac::resetUsed()
{ //no cities have been visited
  for(int i = 0; i < numRoads; i++)
  {
    roadsUsed[i] = false;
  }
}

//Should accept bool isEvacCity, set to true if srcCity is an evacCity.
//Changed: now accepts a bool isEvacCity
void Evac::vacateCity(City* srcCity, int &routeCount, EvacRoute *evacRoutes, bool isEvacCity)
{

  for(int i = 0; i < srcCity->roadCount; i++)
  {
    Road curRoad = srcCity->roads[i];
    City* dstCity = &cityList[curRoad.destinationCityID];
    if(!roadsUsed[curRoad.ID]) //make sure we haven't used this road already
    {
      //create and store evacRoute
      int pplMoved;
      EvacRoute eRoute;
      //cout << "Evac route created!" << endl;
      eRoute.roadID = curRoad.ID;
      eRoute.time = time;

      if(curRoad.peoplePerHour < dstCity->population - dstCity->evacuees)
      { //we have room in the dest city
      //Changed: this line to sum srcCity evacuees and population if isEvacCity?
      //This questionable multiplication only adds srcCity's population if its an evacCity, without an if check.
        pplMoved = min(curRoad.peoplePerHour, (srcCity->evacuees + (srcCity->population * isEvacCity) ) ); //Makes sure that we don't remove more people than the source city contains
      }
      else
      { //we dont have room in the dest city
        vacateCity(dstCity, routeCount, evacRoutes, false); //move people out of dstCity
        pplMoved = min(curRoad.peoplePerHour, dstCity->population - dstCity->evacuees); //find max people that can move to dstCity
        //See earlier comment about questionable multiplcation
        pplMoved = min(pplMoved, (srcCity->evacuees + (srcCity->population * isEvacCity) ) ); //Makes sure that we don't remove more people than the source city contains
      }
      //update evacuees that have moved
      //Changed: If isEvacCity is true, we need to first subtract from evacuees and then population. This if/else block.
      eRoute.numPeople = pplMoved;

      if(!isEvacCity || srcCity->evacuees >= pplMoved)
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
      //Regardless of isEvacCity, dstCity is always treated the same.
      dstCity->evacuees += pplMoved;

      roadsUsed[curRoad.ID] = true; //we've used this road
      evacRoutes[routeCount] = eRoute;
      routeCount++;
      if(srcCity->evacuees + (srcCity->population * isEvacCity) <= 0)
      {
        return; //Stop looking through roads if srcCity has no one left 0.
      }
    }
  }
  //cout << "Finished the for loop boss" << endl;
}
