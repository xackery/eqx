#ifndef EQEMU_COMMON_WLD_FRAGMENT_H
#define EQEMU_COMMON_WLD_FRAGMENT_H

#include <stdint.h>
#include "s3d_texture_brush_set.h"
#include "placeable.h"
#include "s3d_geometry.h"
#include "s3d_bsp.h"
#include "light.h"
#include "any.h"
#include "wld_fragment_reference.h"
#include "s3d_skeleton_track.h"

namespace EQEmu
{

namespace S3D
{

class S3DLoader;
class WLDFragment
{
public:
	WLDFragment() { }
	~WLDFragment() { }

	int type;
	int name;
	EQEmu::Any data;
};

class BitmapName : public WLDFragment
{
public:
	BitmapName(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~BitmapName() { }

	std::shared_ptr<Texture> GetData() { try { return EQEmu::any_cast<std::shared_ptr<Texture>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<Texture>(); } }
};

class BitmapInfo : public WLDFragment
{
public:
	BitmapInfo(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~BitmapInfo() { }

	std::shared_ptr<TextureBrush> GetData() { try { return EQEmu::any_cast<std::shared_ptr<TextureBrush>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<TextureBrush>(); } }
};

class BitmapInfoReference : public WLDFragment
{
public:
	BitmapInfoReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~BitmapInfoReference() { }

	uint32_t GetData() { try { return EQEmu::any_cast<uint32_t>(data); } catch (EQEmu::bad_any_cast&) { return 0; } }
};

class SkeletonHierarchy : public WLDFragment
{
public:
	SkeletonHierarchy(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~SkeletonHierarchy() { }

	std::shared_ptr<SkeletonTrack> GetData() { try { return EQEmu::any_cast<std::shared_ptr<SkeletonTrack>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<SkeletonTrack>(); } }
};

class SkeletonHierarchyReference : public WLDFragment
{
public:
	SkeletonHierarchyReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~SkeletonHierarchyReference() { }

	uint32_t GetData() { try { return EQEmu::any_cast<uint32_t>(data); } catch (EQEmu::bad_any_cast&) { return 0; } }
};

class TrackDefinition : public WLDFragment
{
public:
	TrackDefinition(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~TrackDefinition() { }

	std::shared_ptr<SkeletonTrack::BoneOrientation> GetData() { try { return EQEmu::any_cast<std::shared_ptr<SkeletonTrack::BoneOrientation>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<SkeletonTrack::BoneOrientation>(); } }
};

class TrackInstance : public WLDFragment
{
public:
	TrackInstance(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~TrackInstance() { }

	uint32_t GetData() { try { return EQEmu::any_cast<uint32_t>(data); } catch (EQEmu::bad_any_cast&) { return 0; } }
};

class Actor : public WLDFragment
{
public:
	Actor(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~Actor() { }

	std::shared_ptr<WLDFragmentReference> GetData() { try { return EQEmu::any_cast<std::shared_ptr<WLDFragmentReference>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<WLDFragmentReference>(); } }
};

class ActorInstance : public WLDFragment
{
public:
	ActorInstance(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~ActorInstance() { }

	std::shared_ptr<Placeable> GetData() { try { return EQEmu::any_cast<std::shared_ptr<Placeable>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<Placeable>(); } }
};

class LightDefinition : public WLDFragment
{
public:
	LightDefinition(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~LightDefinition() { }

	std::shared_ptr<Light> GetData() { try { return EQEmu::any_cast<std::shared_ptr<Light>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<Light>(); } }
};

class LightDefinitionReference : public WLDFragment
{
public:
	LightDefinitionReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~LightDefinitionReference() { }

	uint32_t GetData() { try { return EQEmu::any_cast<uint32_t>(data); } catch (EQEmu::bad_any_cast&) { return 0; } }
};

class  BspTree : public WLDFragment
{
public:
	 BspTree(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~ BspTree() { }

	std::shared_ptr<S3D::BSPTree> GetData() { try { return EQEmu::any_cast<std::shared_ptr<S3D::BSPTree>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<S3D::BSPTree>(); } }
};

class BspRegion : public WLDFragment
{
public:
	BspRegion(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~BspRegion() { }
};

class PointLight : public WLDFragment
{
public:
	PointLight(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~PointLight() { }
};

class Zone : public WLDFragment
{
public:
	Zone(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~Zone() { }

	std::shared_ptr<S3D::Zone> GetData() { try { return EQEmu::any_cast<std::shared_ptr<S3D::Zone>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<S3D::Zone>(); } }
};

class MeshReference : public WLDFragment
{
public:
	MeshReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~MeshReference() { }

	uint32_t GetData() { try { return EQEmu::any_cast<uint32_t>(data); } catch (EQEmu::bad_any_cast&) { return 0; } }
};

class Material : public WLDFragment
{
public:
	Material(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~Material() { }

	std::shared_ptr<TextureBrush> GetData() { try { return EQEmu::any_cast<std::shared_ptr<TextureBrush>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<TextureBrush>(); } }
};

class MaterialList : public WLDFragment
{
public:
	MaterialList(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~MaterialList() { }

	std::shared_ptr<TextureBrushSet> GetData() { try { return EQEmu::any_cast<std::shared_ptr<TextureBrushSet>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<TextureBrushSet>(); } }
};

class Mesh : public WLDFragment
{
public:
	Mesh(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~Mesh() { }
	std::shared_ptr<Geometry> GetData() { try { return EQEmu::any_cast<std::shared_ptr<Geometry>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<Geometry>(); } }
};

}

}

#endif
