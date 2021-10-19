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

class TrackDefFragment : public WLDFragment
{
public:
	TrackDefFragment(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~TrackDefFragment() { }

	std::shared_ptr<SkeletonTrack::BoneOrientation> GetData() { try { return EQEmu::any_cast<std::shared_ptr<SkeletonTrack::BoneOrientation>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<SkeletonTrack::BoneOrientation>(); } }
};

class TrackFragment : public WLDFragment
{
public:
	TrackFragment(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~TrackFragment() { }

	uint32_t GetData() { try { return EQEmu::any_cast<uint32_t>(data); } catch (EQEmu::bad_any_cast&) { return 0; } }
};

class Actor : public WLDFragment
{
public:
	Actor(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~Actor() { }

	std::shared_ptr<WLDFragmentReference> GetData() { try { return EQEmu::any_cast<std::shared_ptr<WLDFragmentReference>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<WLDFragmentReference>(); } }
};

class ObjectInstance : public WLDFragment
{
public:
	ObjectInstance(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~ObjectInstance() { }

	std::shared_ptr<Placeable> GetData() { try { return EQEmu::any_cast<std::shared_ptr<Placeable>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<Placeable>(); } }
};

class LightSource : public WLDFragment
{
public:
	LightSource(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~LightSource() { }

	std::shared_ptr<Light> GetData() { try { return EQEmu::any_cast<std::shared_ptr<Light>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<Light>(); } }
};

class LightSourceReference : public WLDFragment
{
public:
	LightSourceReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~LightSourceReference() { }

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

class LightInstance : public WLDFragment
{
public:
	LightInstance(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~LightInstance() { }
};

class BspRegionType : public WLDFragment
{
public:
	BspRegionType(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old);
	~BspRegionType() { }

	std::shared_ptr<S3D::BspRegionType> GetData() { try { return EQEmu::any_cast<std::shared_ptr<S3D::BspRegionType>>(data); } catch (EQEmu::bad_any_cast&) { return std::shared_ptr<S3D::BspRegionType>(); } }
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
