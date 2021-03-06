#include "s3d_loader.h"
#include "pfs.h"
#include "wld_structs.h"
#include "safe_alloc.h"
#include "log_macros.h"

void decode_string_hash(char *str, size_t len) {
	uint8_t encarr[] = { 0x95, 0x3A, 0xC5, 0x2A, 0x95, 0x7A, 0x95, 0x6A };
	for (size_t i = 0; i < len; ++i) {
		str[i] ^= encarr[i % 8];
	}
}

EQEmu::S3DLoader::S3DLoader() {
}

EQEmu::S3DLoader::~S3DLoader() {
}

bool EQEmu::S3DLoader::ParseWLDFile(std::string file_name, std::string wld_name, std::vector<S3D::WLDFragment> &out) {
	out.clear();
	std::vector<char> buffer;
	

	EQEmu::PFS::Archive archive;
	if (!archive.Open(file_name)) {
		eqLogMessage(LogError, "%s: file not found", file_name.c_str());
		return false;
	}

	if (!archive.Get(wld_name, buffer)) {
		eqLogMessage(LogError, "%s: %s wld file not found", file_name.c_str(), wld_name.c_str());
		return false;
	}
	return ParseWLD(buffer, out);
}

bool EQEmu::S3DLoader::ParseWLD(std::vector<char> buffer, std::vector<S3D::WLDFragment> &out) {	
	size_t idx = 0;
	char *current_hash;
	bool old = false;

	SafeStructAllocParse(wld_header, header);
	
	if (header->magic != 0x54503d02) {
		eqLogMessage(LogError, "header magic of %x did not match expected 0x54503d02", header->magic);
		return false;
	}

	if (header->version == 0x00015500) {
		old = true;
	}

	SafeBufferAllocParse(current_hash, header->hash_length);
	decode_string_hash(current_hash, header->hash_length);

	out.clear();
	out.reserve(header->fragments);

	//eqLogMessage(LogInfo, "%s: parsing %s fragments (%d total)", file_name.c_str(), wld_name.c_str(), header->fragments);
	for (uint32_t i = 0; i < header->fragments; ++i) {
		SafeStructAllocParse(wld_fragment_header, frag_header);

		//printf("dispatching WLD fragment of type %x", frag_header->id);
		switch (frag_header->id) {
			case 0x03: {
				S3D::BitmapName f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x04: {
				S3D::BitmapInfo f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				  break;
			}
			case 0x05: {
				S3D::BitmapInfoReference f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x10: {
				S3D::SkeletonHierarchy f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x11: {
				S3D::SkeletonHierarchyReference f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x12: {
				S3D::TrackDefFragment f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x13: {
				S3D::TrackFragment f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x14: {
				S3D::Actor f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				 f.type = frag_header->id;
				 f.name = frag_header->name_ref;
				 out.push_back(f);
				 break;
			}
			case 0x15: {
				S3D::ActorInstance f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x1B: {
				S3D::LightDefinition f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x1C: {
				S3D::LightDefinitionReference f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x21: {
				S3D:: BspTree f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x22: {
				S3D::BspRegion f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x28: {
				S3D::PointLight f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x29: {
				S3D::Zone f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x2D: {
				S3D::MeshReference f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x30: {
				S3D::Material f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
			}
			case 0x31: {
				 S3D::MaterialList f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				 f.type = frag_header->id;
				 f.name = frag_header->name_ref;
				 out.push_back(f);
				 break;
			}
			case 0x36: {
				S3D::Mesh f(out, &buffer[idx], frag_header->size, frag_header->name_ref, current_hash, old);
				f.type = frag_header->id;
				f.name = frag_header->name_ref;

				out.push_back(f);
				break;
			}
			default:
				S3D::WLDFragment f;
				f.type = frag_header->id;
				f.name = frag_header->name_ref;
				out.push_back(f);
				break;
		}

		idx += frag_header->size - 4;
	}

	return true;
}
