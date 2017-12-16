#include <cstdlib>
#include "evac.h"
#include <cstring>
#include "EvacRunner.h"
#include "QueueAr.h"
#include <iostream>

using namespace std;

MyCity MyCity::operator = (City const &oldCity)
{
  ID = oldCity.ID;
  x = oldCity.x;
  y = oldCity.y;
  roadCount = oldCity.roadCount;
  population = oldCity.population;
  evacuees = oldCity.evacuees;

  /*roads = new Road[roadCount];
  for(int j = 0; j < roadCount; j++) //deep copy all roads
  {
    roads[j] = oldCity.roads[j];
  }*/
  return *this;
}

Evac::Evac(City *citie, int numCitie, int numRoad) : numCities(numCitie), numRoadIDs(numRoad * 2), time(1)
{
  //change the name to cityAr later
  cityList = new MyCity[numCities];
  cityVisited = new bool[numCities](); //This will be used during bfsEvac.

  for(int i = 0; i < numCities; i++)
  {
    cityList[i] = citie[i]; //copy city into MyCity class
    cityList[i].roads = new Road[cityList[i].roadCount];
    for(int j = 0; j < cityList[i].roadCount; j++) //deep copy all roads
    {
      cityList[i].roads[j] = citie[i].roads[j];
    }
  }
} // Evac()


void Evac::evacuate(int *evacIDs, int numEvac, EvacRoute *evacRoutes,
    int &routeCount)
{
  numEvacs = numEvac;
  routeCount = 0;
  evacCities = new MyCity*[numEvacs];

  for(int i = 0; i < numEvacs; i++)
  {
    evacCities[i] = &cityList[evacIDs[i]]; //create array of evac cities
    evacCities[i]->isEvacCity = true;
  }

  calcDepth(numEvacs);

  while(!isEvacComplete()) //check if we still have evac cities with people in them
  {
    /*for(int indx = 0; indx < numEvacs; indx++)
    { //we are looping through evacCities
      MyCity* curECity = evacCities[indx]; //get current evacCity
      //CHANGED: check for people, not NULL ptr
      if(curECity->population + curECity->evacuees > 0) //we still have people in this city
      {
        //pushes people out of all roads and generates evacRoutes
        vacateEvac(curECity, routeCount, evacRoutes);
        //CHANGED: removed check for empty city and nulling ptrs
      }
      resetUsed(); //reset roads visited to false
    }*/
    bfsEvac(minDepth, routeCount, evacRoutes); // NOTE still writing
    //updateMinDepth();//   NOTE should be done in isEvacComplete needs to be written. Updates minDepth if need be, should also update minDepthIndex.
    time++;
    //cout << "Time Incremented" << endl;
  }

} // evacuate

void Evac::bfsEvac(int minDepth, int &routeCount, EvacRoute* evacRoutes)
{
  //Should be called exactly once per hour.
  //First we enqueue all the cities with depth == minDepth
  Queue<MyCity*> bfsQueue(numCities);
  MyCity* curCity;
  MyCity* adjCity;
  Road curRoad;
  int adjCityID;

  for(int i = minDepthIndex; i < numEvacs; i++)
  {
    if(evacCities[i]->depth == minDepth)
    {
      if(evacCities[i]->population + evacCities[i]->evacuees > 0)
      { //we have people in this city
        cityVisited[evacCities[i]->ID] = true; //mark each added city as visited.
        bfsQueue.enqueue(evacCities[i]);
      }
    }
    else
    {
      //If this triggers, there are no more cities that have minDepth.
      break;
    }
  }
  //Do bfs, writing evacRoute(s) as we go.
  while(!bfsQueue.isEmpty())
  {
    curCity = bfsQueue.dequeue();
    for(int i = 0; i < curCity->roadCount; i++)
    {
      adjCityID = curCity->roads[i].destinationCityID;
      adjCity = &cityList[adjCityID];
      if(validERoute(curCity, adjCity)) //check that adj has greater depth
      {
        //add adjCity to the queue if need be.
        if(!cityVisited[adjCityID])
        { //city has not been enqueued
          bfsQueue.enqueue(adjCity);
          cityVisited[adjCityID] = true; //now its been visited
        }
        //write the evacRoute
        curRoad = curCity->roads[i];
        int pplMoved;
        EvacRoute eRoute;
        eRoute.roadID = curRoad.ID;
        eRoute.time = time;

        //Use bool multiplication to make sure that we don't take population into account if we're not an evacCity.
        pplMoved = min(curRoad.peoplePerHour, (curCity->evacuees + curCity->population * curCity->isEvacCity)); //Makes sure that we don't remove more people than the source city containsThiscodewrittenbyD4ni3lCLor4nand3mily/\/\H3ry
        /*if(pplMoved < adjCity->population - adjCity->evacuees)
        { //we have space in the city

        }
        else */ if ( adjCity->population != adjCity->evacuees)
        { //we have a little bit of space
          pplMoved = min(pplMoved, adjCity->population - adjCity->evacuees); ////find max people that can move to adjCity
        }
        else
        {
          continue; //don't make evacRoute
        }
        //we have people to put in our evacRoute
        eRoute.numPeople = pplMoved;
        adjCity->evacuees += pplMoved;

        if(curCity->evacuees >= pplMoved)
        {
          curCity->evacuees -= pplMoved;
        }
        else
        {
          //First send off the evacuees, then the population.
          pplMoved -= curCity->evacuees;
          curCity->evacuees = 0;
          curCity->population -= pplMoved;
        }
        evacRoutes[routeCount] = eRoute;
        routeCount++;

        if((adjCity->evacuees + adjCity->population) <= 0)
        {
          break; //Stop looking through roads if srcCity has no one left 0.
        }
      }
    }
  }
  //Then reset cityVistied.
}

bool Evac::isEvacComplete() //CHANGED: new function checks for people in evacCities
{
  for(int i = minDepthIndex; i < numEvacs; i++)
  {
    if(evacCities[i]->depth != minDepth)
    { //all the cities of the current min depth are empty
      minDepthIndex = i; //this is the first element of this depth
      minDepth = evacCities[i]->depth; //this is now the mindepth
    }
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
  memset(cityVisited, false, (sizeof(bool)*numCities));
  /*for(int i = 0; i < numRoadIDs; i++)
  {
    roadsUsed[i] = false;
  }*/
}

void Evac::findOuterRing(int numEvacs)
{

  int numRoads;
  int destCityID;
  int numOuter = 0;
  for(int i = 0; i < numEvacs; i++)
  {
    numRoads = evacCities[i]->roadCount;
    for(int j = 0; i < numRoads; j++)
    {
      destCityID = evacCities[i]->roads[j].destinationCityID;
      if(!cityList[destCityID].isEvacCity)
      {
        evacCities[i]->depth = 0;
        numOuter++;
        break;
      }
    }

  }
  cout << numOuter << endl;
}

int comprar(const void* p1, const void* p2)
{
  if((**(MyCity**)p1).depth < (**(MyCity**)p2).depth) return -1;
  if((**(MyCity**)p1).depth == (**(MyCity**)p2).depth) return 0;
  if((**(MyCity**)p1).depth > (**(MyCity**)p2).depth) return 1;
}

void Evac::calcDepth(int numEvacs)
{
  Queue<MyCity*> depthHolder(numCities);
  MyCity* curCity;
  MyCity* adjCity;
  int adjCityID;
  int nextDepth;

  findOuterRing(numEvacs); //relies on isEvacCity being properly initialized.

  //First, we calculate the depth of all inner evac cities.
  int evacsQueued = 0;
  for(int i = 0; i < numEvacs; i++)
  {
    if(evacCities[i]->depth == 0)//Put all of the outer ring in a queue
    {
      depthHolder.enqueue(evacCities[i]);
      evacsQueued++;
    }
  }

  while(!depthHolder.isEmpty())
  {
    curCity = depthHolder.dequeue(); //Capture the city at the top of the stack
    nextDepth = curCity->depth - 1; //Depth counts down when moving inward from the ring.
    for(int i = 0; i < curCity->roadCount; i++) //enqueue all unvisited adjacent cities and mark their depth.
    {
      adjCityID = curCity->roads[i].destinationCityID;
      adjCity = &cityList[adjCityID];
      if(adjCity->depth == -9001 && adjCity->isEvacCity) //Check that adjCity has not yet been assigned a depth and is an evacCity.
      {
        adjCity->depth = nextDepth;
        depthHolder.enqueue(adjCity);
        evacsQueued++;
      }
    }
  }

  qsort(evacCities, numEvacs, sizeof(MyCity*), comprar); //This should sort all the pointers in evacCities by depth.
  minDepth = evacCities[0]->depth; //Initialize minDepth to lowest possible depth.
  minDepthIndex = 0; //first element at mindepth is at pos 0 in evac array

  //Now we calculate depth for the non-evac cities. Queue is now empty because while loop finished.

  for(int i = 0; i < numEvacs; i++)
  {
    if(evacCities[i]->depth == 0)
    {
      depthHolder.enqueue(evacCities[i]); //Once again, we have to enqueue the outer ring.
    }
  }

  while(!depthHolder.isEmpty())
  {
    curCity = depthHolder.dequeue(); //Capture the city at the top of the stack
    nextDepth = curCity->depth + 1; //Depth counts up when moving inward from the ring.
    for(int i = 0; i < curCity->roadCount; i++) //enqueue all unvisited adjacent cities and mark their depth.
    {
      adjCityID = curCity->roads[i].destinationCityID;
      adjCity = &cityList[adjCityID];
      if(adjCity->depth == -9001) //Check that adjCity has not yet been assigned a depth, every city without a depth is not an evacCity.
      {
        adjCity->depth = nextDepth;
        depthHolder.enqueue(adjCity);
      }
    }
  }

}
/*
bool Evac::vacateEvac(MyCity *srcCity, int &routeCount, EvacRoute *evacRoutes)
{
  bool isInternal = checkInternal(srcCity);

  for(int i = 0; i < srcCity->roadCount; i++)
  {
    Road curRoad = srcCity->roads[i];
    MyCity* dstCity = &cityList[curRoad.destinationCityID];

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

bool Evac::checkInternal(MyCity* internal)
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
void Evac::vacateCity(MyCity *srcCity, int &routeCount, EvacRoute *evacRoutes)
{
  for(int i = 0; i < srcCity->roadCount; i++)
  {
    Road curRoad = srcCity->roads[i];
    MyCity* dstCity = &cityList[curRoad.destinationCityID];
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
*/
bool Evac::validERoute(MyCity* srcCity, MyCity* dstCity)
{
  /*if(roadsUsed[curRoad.ID])
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
  }*/
  if(srcCity->depth >= dstCity->depth)
  {
    return false;
  }
  return true;
}
