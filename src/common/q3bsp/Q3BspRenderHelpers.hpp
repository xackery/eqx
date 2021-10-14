#ifndef Q3BSPRENDERHELPERS_INCLUDED
#define Q3BSPRENDERHELPERS_INCLUDED

#include <vector>
#include <map>
#include "eq_math.h"

/*
 * Helper structs for Q3BspMap rendering
 */

enum Q3BspRenderFlags
{
    Q3RenderShowWireframe  = 1 << 0,
    Q3RenderShowLightmaps  = 1 << 1,
    Q3RenderUseLightmaps   = 1 << 2,
    Q3RenderAlphaTest      = 1 << 3,
    Q3RenderSkipMissingTex = 1 << 4,
    Q3RenderSkipPVS        = 1 << 5,
    Q3RenderSkipFC         = 1 << 6,
    Q3Multisampling        = 1 << 7
};


// leaf structure used for occlusion culling (PVS/frustum)
struct Q3LeafRenderable
{
    int visCluster = 0;
    int firstFace  = 0;
    int numFaces   = 0;
    glm::vec3 boundingBoxVertices[8];
};


// face structure used for rendering
struct Q3FaceRenderable
{
    int type  = 0;
    int index = 0;
};


// map statistics
struct BspStats
{
    int totalVertices   = 0;
    int totalFaces      = 0;
    int visibleFaces    = 0;
    int totalPatches    = 0;
    int visiblePatches  = 0;
};

#endif