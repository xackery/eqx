#include "wld_fragment.h"
#include "wld_structs.h"
#include "s3d_loader.h"

// TODO: DEFAULTPALETTEFILE is 0x01
// TODO: USERDATA is 0x02
// BitmapName is 0x03
EQEmu::S3D::BitmapName::BitmapName(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment03 *header = (wld_fragment03*)frag_buffer;
	frag_buffer += sizeof(wld_fragment03);
	uint32_t count = header->texture_count;
	if(!count)
		count = 1;
	
	std::shared_ptr<Texture> tex(new Texture);
	auto &frames = tex->GetTextureFrames();
	frames.resize(count);
	for(uint32_t i = 0; i < count; ++i) {
		uint16_t name_len = *(uint16_t*)frag_buffer;
		frag_buffer += sizeof(uint16_t);

		char *texture_name = frag_buffer;
		decode_string_hash(texture_name, name_len);
		frames[i] = texture_name;

		frag_buffer += name_len;
	}

	data = tex;
}

// BitmapInfo aka SIMPLESPRITEDEF is 0x04
EQEmu::S3D::BitmapInfo::BitmapInfo(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment04 *header = (wld_fragment04*)frag_buffer;
	frag_buffer += sizeof(wld_fragment04);

	if (header->flags & (1 << 2))
		frag_buffer += sizeof(uint32_t);

	if (header->flags & (1 << 3))
		frag_buffer += sizeof(uint32_t);

	uint32_t count = header->texture_count;
	if(!count)
		count = 1;

	std::shared_ptr<TextureBrush> brush(new TextureBrush);
	for (uint32_t i = 0; i < count; ++i) {
		wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
		frag_buffer += sizeof(wld_fragment_reference);

		WLDFragment &frag = out[ref->id - 1];
		try {
			std::shared_ptr<Texture> tex = EQEmu::any_cast<std::shared_ptr<Texture>>(frag.data);
			brush->GetTextures().push_back(tex);
		} catch (EQEmu::bad_any_cast&) {
			brush->GetTextures().push_back(std::shared_ptr<Texture>(new Texture()));
		}
	}

	data = brush;
}

// BitmapInfoReference aka SIMPLESPRITEINST is 0x05
EQEmu::S3D::BitmapInfoReference::BitmapInfoReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
	uint32_t frag_id = ref->id - 1;
	data = frag_id;
}

// TODO: 2DSPRITEDEF 0x06 Only found in gequip files. Seems to represent 2d sprites in the world (coins).

// TODO: Fragment7 is 0x07, only found in gequip files. This fragment can be referenced by an actor fragment.

// TODO: 3DSPRITEDEF Camera is 0x08

// TODO: CameraReference is 0x09 Related to 3DSPRITEDEF, Maybe the 3DSPRITETAG to map the string. When I added a second ACTION to ACTORINST test, a second 0x09 was added. maybe from the 3DSPRITE %s. bodies are both compressed to 0 1 0

// TODO: 4DSPRITEDEF - 0xa

// TODO: FUN_004079a0 - 0xb

// TODO: PARTICLESPRITEDEF - 0xc

// TODO: Unknown 0xd

// TODO: COMPOSITESPRITEDEF 0xe

// TODO: Unknown 0xf

// SkeletonHierarchy aka HIERARCHICALSPRITEDEF is 0x10 
EQEmu::S3D::SkeletonHierarchy::SkeletonHierarchy(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment10 *header = (wld_fragment10*)frag_buffer;
	frag_buffer += sizeof(wld_fragment10);

	std::shared_ptr<SkeletonTrack> track(new SkeletonTrack());
	track->SetName(&hash[-(int32_t)frag_name]);

	if(header->flag & 1) {
		int32_t param0 = *(int32_t*)frag_buffer;
		frag_buffer += sizeof(int32_t);
		int32_t param1 = *(int32_t*)frag_buffer;
		frag_buffer += sizeof(int32_t);
		int32_t param2 = *(int32_t*)frag_buffer;
		frag_buffer += sizeof(int32_t);
	}

	if (header->flag & 2) {
		float params2 = *(float*)frag_buffer;
		frag_buffer += sizeof(float);
	}

	auto &bones = track->GetBones();
	std::vector<std::pair<int, int>> tree;
	for(uint32_t i = 0; i < header->track_ref_count; ++i) {
		wld_fragment10_track_ref_entry *ent = (wld_fragment10_track_ref_entry*)frag_buffer;
		frag_buffer += sizeof(wld_fragment10_track_ref_entry);

		std::shared_ptr<SkeletonTrack::Bone> bone(new SkeletonTrack::Bone);
		if (ent->frag_ref2 > 0 && out[ent->frag_ref2 - 1].type == 0x2d) {
			MeshReference &f = reinterpret_cast<MeshReference&>(out[ent->frag_ref2 - 1]);
			auto m_ref = f.GetData();

			Mesh &fmod = reinterpret_cast<Mesh&>(out[m_ref]);
			auto mod = fmod.GetData();

			bone->model = mod;

			if (ent->frag_ref != 0) {
				TrackInstance &f_oref = reinterpret_cast<TrackInstance&>(out[ent->frag_ref - 1]);
				auto or_ref = f_oref.GetData();

				TrackDefinition &fori = reinterpret_cast<TrackDefinition&>(out[or_ref]);
				auto orient = fori.GetData();

				bone->orientation = orient;
			}
		}

		bones.push_back(bone);

		for(uint32_t j = 0; j < ent->tree_piece_count; ++j) {
			int32_t tree_piece_ref = *(int32_t*)frag_buffer;
			frag_buffer += sizeof(int32_t);

			tree.push_back(std::make_pair(i, tree_piece_ref));
		}
	}

	for (size_t i = 0; i < tree.size(); ++i) {
		int &bone_src = tree[i].first;
		int &bone_dest = tree[i].second;

		bones[bone_src]->children.push_back(bones[bone_dest]);
	}

	if (header->flag & 512) {
		uint32_t sz = *(uint32_t*)frag_buffer;
		frag_buffer += sizeof(uint32_t);
		for (uint32_t i = 0; i < sz; ++i) {
			int32_t ref = *(int32_t*)frag_buffer;

			if(out[ref - 1].type = 0x2d) {
				MeshReference &f = reinterpret_cast<MeshReference&>(out[ref - 1]);
				auto mod_ref = f.GetData();

				if (out[mod_ref].type == 0x36) {
					Mesh &f = reinterpret_cast<Mesh&>(out[mod_ref]);
					auto mod = f.GetData();
				}
			}

			frag_buffer += sizeof(int32_t);
		}

		for (uint32_t i = 0; i < sz; ++i) {
			int32_t data = *(int32_t*)frag_buffer;
			frag_buffer += sizeof(int32_t);
		}
	}

	data = track;
}

// SkeletonHierarchyReference is 0x11
EQEmu::S3D::SkeletonHierarchyReference::SkeletonHierarchyReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
	uint32_t frag_id = ref->id - 1;
	data = frag_id;
}

// TrackDefinition is 0x12
EQEmu::S3D::TrackDefinition::TrackDefinition(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	std::shared_ptr<SkeletonTrack::BoneOrientation> orientation(new SkeletonTrack::BoneOrientation);
	wld_fragment12 *header = (wld_fragment12*)frag_buffer;

	orientation->rotate_denom = header->rot_denom;
	orientation->rotate_x_num = header->rot_x_num;
	orientation->rotate_y_num = header->rot_y_num;
	orientation->rotate_z_num = header->rot_z_num;
	orientation->shift_denom = header->shift_denom;
	orientation->shift_x_num = header->shift_x_num;
	orientation->shift_y_num = header->shift_y_num;
	orientation->shift_z_num = header->shift_z_num;

	data = orientation;
}

// TrackInstance is 0x13
EQEmu::S3D::TrackInstance::TrackInstance(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
	uint32_t frag_id = ref->id - 1;
	data = frag_id;
}

// Actor aka ActorDef 0x14
EQEmu::S3D::Actor::Actor(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment14 *header = (wld_fragment14*)frag_buffer;
	frag_buffer += sizeof(wld_fragment14);

	std::shared_ptr<WLDFragmentReference> ref(new WLDFragmentReference());
	ref->SetName(&hash[-(int32_t)frag_name]);
	ref->SetMagicString(&hash[-header->ref]);

	if(header->flag & 1) {
		frag_buffer += sizeof(int32_t);
	}

	if (header->flag & 2) {
		frag_buffer += sizeof(int32_t);
	}
	
	for(uint32_t i = 0; i < header->entries; ++i) {
		uint32_t sz = *(uint32_t*)frag_buffer;
		frag_buffer += sizeof(uint32_t);
		for(uint32_t j = 0; j < sz; ++j) {
			frag_buffer += sizeof(int32_t);
			frag_buffer += sizeof(float);
		}
	}

	for(uint32_t i = 0; i < header->entries2; ++i) {
		uint32_t f_ref = *(uint32_t*)frag_buffer;
		frag_buffer += sizeof(uint32_t);

		ref->GetFrags().push_back(f_ref);
	}

	//Encoded string length + string here, purpose unknown.

	data = ref;
}

// ActorInstance aka ObjectInstance is 0x15
EQEmu::S3D::ActorInstance::ActorInstance(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
	frag_buffer += sizeof(wld_fragment_reference);

	wld_fragment15 *header = (wld_fragment15*)frag_buffer;
	if(ref->id <= 0) {
		std::shared_ptr<Placeable> plac(new Placeable());
		plac->SetLocation(header->x, header->y, header->z);
		plac->SetRotation(
			header->rotate_x / 512.f * 360.f,
			header->rotate_y / 512.f * 360.f,
			header->rotate_z / 512.f * 360.f
			);
		plac->SetScale(header->scale_x, header->scale_y, header->scale_y);
		
		const char *model_str = (const char*)&hash[-ref->id];
		plac->SetName(model_str);
		data = plac;
	}
}

// TODO: ACTORINSTANCETEST 0x16

// TODO: POLYHEDRONDEFINITION 0x17

// TODO: unknown 0x18

// TODO: SPHERELISTDEFINITION  0x19

// TODO: unknown 0x1A

// LightDefinition aka LightSource is 0x1B
EQEmu::S3D::LightDefinition::LightDefinition(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	std::shared_ptr<Light> light(new Light());
	wld_fragment1B *header = (wld_fragment1B*)frag_buffer;

	light->SetLocation(0.0f, 0.0f, 0.0f);
	light->SetRadius(0.0f);
	
	if (header->flags & (1 << 3)) {
		light->SetColor(header->color[0], header->color[1], header->color[2]);
	} else {
		light->SetColor(1.0f, 1.0f, 1.0f);
	}

	data = light;
}

// LightDefinitionReference is 0x1C
EQEmu::S3D::LightDefinitionReference::LightDefinitionReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
	uint32_t frag_id = ref->id - 1;
	data = frag_id;
}

// TODO: Pointlight is 0x1D

// TODO: unknown 0x1E

// TODO: SoundDefinition is 0x1F

// TODO: SoundInstance is 0x20

//  BspTree aka WorldTree is 0x21
EQEmu::S3D:: BspTree:: BspTree(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment21 *header = (wld_fragment21*)frag_buffer;
	frag_buffer += sizeof(wld_fragment21);

	std::shared_ptr<S3D::BSPTree> tree(new S3D::BSPTree());
	auto &nodes = tree->GetNodes();
	nodes.resize(header->count);
	for(uint32_t i = 0; i < header->count; ++i) {
		wld_fragment21_data *data = (wld_fragment21_data*)frag_buffer;
		frag_buffer += sizeof(wld_fragment21_data);

		auto &node = nodes[i];
		node.number = i;
		node.normal[0] = data->normal[0];
		node.normal[1] = data->normal[1];
		node.normal[2] = data->normal[2];
		node.split_dist = data->split_dist;
		node.region = data->region;
		node.left = data->node[0];
		node.right = data->node[1];
	}

	data = tree;
}

// BspRegion aka Region is 0x22 
EQEmu::S3D::BspRegion::BspRegion(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	//bsp regions
}

// TODO: ACTIVEGEOMETRYREGION is 0x23

// TODO: SKYREGION is 0x24

// TODO: DIRECTIONALLIGHT is 0x25

// TODO: BLITSPRITEDEFINITION is 0x26

// TODO: unknown is 0x27

// PointLight aka LightInstance is 0x28
EQEmu::S3D::PointLight::PointLight(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
	frag_buffer += sizeof(wld_fragment_reference);

	wld_fragment_28 *header = (wld_fragment_28*)frag_buffer;
	frag_buffer += sizeof(wld_fragment_28);

	if(ref->id == 0)
		return;

	WLDFragment &frag = out[ref->id - 1];
	uint32_t ref_id = 0;
	try {
		ref_id = EQEmu::any_cast<uint32_t>(frag.data);
	}
	catch (EQEmu::bad_any_cast&) { }

	frag = out[ref_id];
	try {
		std::shared_ptr<Light> l = EQEmu::any_cast<std::shared_ptr<Light>>(frag.data);
		l->SetLocation(header->x, header->y, header->z);
		l->SetRadius(header->rad);
	}
	catch (EQEmu::bad_any_cast&) {}
}

// Zone aka BspRegionType is 0x29
EQEmu::S3D::Zone::Zone(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_29 *header = (wld_fragment_29*)frag_buffer;
	frag_buffer += sizeof(wld_fragment_29);

	std::shared_ptr<S3D::BSPRegion> region(new S3D::BSPRegion());
	int32_t ref_id = (int32_t)frag_name;
	char *region_name = &hash[-ref_id];
	region->SetName(region_name);

	for(uint32_t i = 0; i < header->region_count; ++i) {
		uint32_t ref_id = *(uint32_t*)frag_buffer;
		frag_buffer += sizeof(uint32_t);
		region->AddRegion(ref_id);
	}

	uint32_t str_length = *(uint32_t*)frag_buffer;
	if(str_length > 0) {
		frag_buffer += sizeof(uint32_t);
		char *str = frag_buffer;
		decode_string_hash(str, str_length);
		region->SetExtendedInfo(str);
	}

	data = region;
}

// TODO: AmbientLight is 0x2A

// TODO: DirectionalLight is 0x2B

// TODO: LegacyMesh is 0x2C

// MeshReference is 0x2D
EQEmu::S3D::MeshReference::MeshReference(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;
	uint32_t frag_id = ref->id - 1;
	data = frag_id;
}

// Material is 0x30
EQEmu::S3D::Material::Material(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	//texture reference to a 0x05
	wld_fragment30 *header = (wld_fragment30*)frag_buffer;
	frag_buffer += sizeof(wld_fragment30);

	if(!header->flags)
		frag_buffer += sizeof(uint32_t) * 2;

	wld_fragment_reference *ref = (wld_fragment_reference*)frag_buffer;

	if(!header->params1 || !ref->id) {
		std::shared_ptr<TextureBrush> tb(new TextureBrush());
		std::shared_ptr<Texture> t(new Texture());
		t->GetTextureFrames().push_back("collide.dds");
		tb->GetTextures().push_back(t);
		tb->SetFlags(1);
		data = tb;
		return;
	}
	
	WLDFragment &frag = out[ref->id - 1];
	uint32_t tex_ref = 0;
	try {
		tex_ref = EQEmu::any_cast<uint32_t>(frag.data);
	}
	catch (EQEmu::bad_any_cast&) { }
	
	
	frag = out[tex_ref];
	try {
		std::shared_ptr<TextureBrush> tb = EQEmu::any_cast<std::shared_ptr<TextureBrush>>(frag.data);
		std::shared_ptr<TextureBrush> new_tb(new TextureBrush);

		*new_tb = *tb;

		if (header->params1 & (1 << 1) || header->params1 & (1 << 2) || header->params1 & (1 << 3) || header->params1 & (1 << 4))
			new_tb->SetFlags(1);
		else
			new_tb->SetFlags(0);
	
		data = new_tb;
	}
	catch (EQEmu::bad_any_cast&) { }
}

// MaterialList is 0x31
EQEmu::S3D::MaterialList::MaterialList(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment31 *header = (wld_fragment31*)frag_buffer;
	frag_buffer += sizeof(wld_fragment31);

	std::shared_ptr<TextureBrushSet> tbs(new TextureBrushSet());

	auto &ts = tbs->GetTextureSet();
	ts.resize(header->count);
	for(uint32_t i = 0; i < header->count; ++i) {
		uint32_t ref_id = *(uint32_t*)frag_buffer;
		frag_buffer += sizeof(uint32_t);
		WLDFragment &frag = out[ref_id - 1];
		try {
			std::shared_ptr<TextureBrush> tb = EQEmu::any_cast<std::shared_ptr<TextureBrush>>(frag.data);
			ts[i] = tb;
		}
		catch (EQEmu::bad_any_cast&) {}
	}

	data = tbs;
}

// TODO: VertexColors is 0x32

// TODO: VertexColorsReference is 0x33

// TODO: ParticleCloud is 0x34

// TODO: GlobalAmbientLight is 0x35

// Mesh is 0x36
EQEmu::S3D::Mesh::Mesh(std::vector<WLDFragment> &out, char *frag_buffer, uint32_t frag_length, uint32_t frag_name, char *hash, bool old) {
	wld_fragment36 *header = (wld_fragment36*)frag_buffer;
	frag_buffer += sizeof(wld_fragment36);

	float scale = 1.0f / (float)(1 << header->scale);
	float recip_255 = 1.0f / 256.0f, recip_127 = 1.0f / 127.0f;

	std::shared_ptr<Geometry> model(new Geometry());
	model->SetName((char*)&hash[-(int32_t)frag_name]);

	WLDFragment &tex_frag = out[header->frag1 - 1];
	try {
		std::shared_ptr<TextureBrushSet> tbs = EQEmu::any_cast<std::shared_ptr<TextureBrushSet>>(tex_frag.data);
		model->SetTextureBrushSet(tbs);
	}
	catch (EQEmu::bad_any_cast&) {}

	auto &verts = model->GetVertices();
	verts.resize(header->vertex_count);
	for(uint32_t i = 0; i < header->vertex_count; ++i) {
		wld_fragment36_vert *in = (wld_fragment36_vert*)frag_buffer;
		frag_buffer += sizeof(wld_fragment36_vert);

		auto &v = verts[i];
		v.pos.x = header->center_x + in->x * scale;
		v.pos.y = header->center_y + in->y * scale;
		v.pos.z = header->center_z + in->z * scale;
	}

	if(old) {
		for(uint32_t i = 0; i < header->tex_coord_count; ++i) {
			wld_fragment36_tex_coords_old *in = (wld_fragment36_tex_coords_old*)frag_buffer;
		
			if (i < model->GetVertices().size()) {
				model->GetVertices()[i].tex.x = in->u * recip_255;
				model->GetVertices()[i].tex.y = in->v * recip_255;
			}

			frag_buffer += sizeof(wld_fragment36_tex_coords_old);
		}
	} else {
		for (uint32_t i = 0; i < header->tex_coord_count; ++i) {
			wld_fragment36_tex_coords_new *in = (wld_fragment36_tex_coords_new*)frag_buffer;

			if (i < model->GetVertices().size()) {
				model->GetVertices()[i].tex.x = in->u;
				model->GetVertices()[i].tex.y = in->v;
			}

			frag_buffer += sizeof(wld_fragment36_tex_coords_new);
		}
	}

	for(uint32_t i = 0; i < header->normal_count; ++i) {
		wld_fragment36_normal *in = (wld_fragment36_normal*)frag_buffer;

		// this check is here cause there's literally zones where there's normals than verts (ssratemple for ex). Stupid I know.
		if (i < model->GetVertices().size()) {
			model->GetVertices()[i].nor.x = in->x * recip_127;
			model->GetVertices()[i].nor.y = in->y * recip_127;
			model->GetVertices()[i].nor.z = in->z * recip_127;
		}

		frag_buffer += sizeof(wld_fragment36_normal);
	}

	frag_buffer += sizeof(uint32_t) * header->color_count;

	auto &polys = model->GetPolygons();
	polys.resize(header->polygon_count);
	for(uint32_t i = 0; i < header->polygon_count; ++i) {
		wld_fragment36_poly *in = (wld_fragment36_poly*)frag_buffer;

		auto &p = polys[i];
		p.flags = in->flags;
		p.verts[0] = in->index[2];
		p.verts[1] = in->index[1];
		p.verts[2] = in->index[0];
		frag_buffer += sizeof(wld_fragment36_poly);
	}

	frag_buffer += 4 * header->size6;
	uint32_t pc = 0;
	for(uint32_t i = 0; i < header->polygon_tex_count; ++i) {
		wld_fragment36_tex_map *tm = (wld_fragment36_tex_map*)frag_buffer;

		for(uint32_t j = 0; j < tm->poly_count; ++j) {
			if(pc >= model->GetPolygons().size()) {
				break;
			}

			model->GetPolygons()[pc++].tex = tm->tex;
		}

		frag_buffer += sizeof(wld_fragment36_tex_map);
	}

	data = model;
}

// TODO: MeshAnimatedVertices is 0x37