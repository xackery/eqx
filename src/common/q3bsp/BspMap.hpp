#ifndef BSPMAP_INCLUDED
#define BSPMAP_INCLUDED

#include "eq_math.h"

/*
 *  Base class for renderable bsp map. Future reference for other BSP format support.
 */

class BspMap
{
public:
    BspMap(bool bspValid) : m_bspValid(bspValid) {}
    virtual ~BspMap() {}
protected:
    int      m_renderFlags = 0;
    bool     m_bspValid;
};

#endif