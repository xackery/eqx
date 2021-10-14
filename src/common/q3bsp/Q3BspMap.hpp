#ifndef Q3BSPMAP_INCLUDED
#define Q3BSPMAP_INCLUDED
#include <map>
#include <set>
#include <vector>
#include "Q3Bsp.hpp"
#include "BspMap.hpp"

class  GameTexture;
class  Q3BspBiquadPatch;
struct Q3BspPatch;

/*
 *  Quake III map
 */

class Q3BspMap : public BspMap
{
public:
    Q3BspMap(bool bspValid) : BspMap(bspValid) {}
    ~Q3BspMap();

    // bsp data
    Q3BspHeader     header;
    Q3BspEntityLump entities;
    std::vector<Q3BspTextureLump>   textures;
    std::vector<Q3BspPlaneLump>     planes;
    std::vector<Q3BspNodeLump>      nodes;
    std::vector<Q3BspLeafLump>      leaves;
    std::vector<Q3BspLeafFaceLump>  leafFaces;
    std::vector<Q3BspLeafBrushLump> leafBrushes;
    std::vector<Q3BspModelLump>     models;
    std::vector<Q3BspBrushLump>     brushes;
    std::vector<Q3BspBrushSideLump> brushSides;
    std::vector<Q3BspVertexLump>    vertices;
    std::vector<Q3BspMeshVertLump>  meshVertices;
    std::vector<Q3BspEffectLump>    effects;
    std::vector<Q3BspFaceLump>      faces;
    std::vector<Q3BspLightMapLump>  lightMaps;
    std::vector<Q3BspLightVolLump>  lightVols;
    Q3BspVisDataLump                visData;
private:    
};

#endif
