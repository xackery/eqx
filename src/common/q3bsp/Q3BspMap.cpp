#include "q3bsp/Q3BspMap.hpp"

Q3BspMap::~Q3BspMap()
{
    delete[] entities.ents;

}