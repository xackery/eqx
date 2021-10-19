#include <string.h>
#include <stdio.h>
#include "eqx.h"
#include "toml.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include "compression.h"
#include "log_macros.h"
#include "log_stdout.h"
#include "log_file.h"
#include <gtc/matrix_transform.hpp>
#include "s3d_loader.h"
#include "eqg_loader.h"
#include "eqg_v4_loader.h"
#include "q3bsp/Q3BspLoader.hpp"
#include <stdint.h>
#include <filesystem>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#define GLM_FORCE_RADIANS
#include <glm.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

#include "gltf/tiny_gltf.h"
using namespace tinygltf;

using namespace std;

int main(int argc, char **argv) {
	eqLogInit(62);
	eqLogRegister(shared_ptr<EQEmu::Log::LogBase>(new EQEmu::Log::LogStdOut()));
	eqLogRegister(shared_ptr<EQEmu::Log::LogBase>(new EQEmu::Log::LogFile("eqx.log")));
	
	if (argc <= 1) {
		printf("usage: eqx <target>\n");
		return 1;
	}
	eqLogMessage(LogInfo, "eqx v0.0.2");
	
	int i = 1;
	bool ignore_collide_tex = true;
	if (argc > 2) {
		if (strcmp(argv[1], "--IncludeCollideTex") == 0) {
			ignore_collide_tex = false;
			i = 2;
		}
	}
	
	FILE *f = fopen("test.toml", "w");
	if (!f) {
		eqLogMessage(LogError, "failed to create test.toml");
		return 1;
	}
	fprintf(f, "# library represents XYZ\n");
	fprintf(f, "[library]\n");
	fprintf(f, "name = \"Test\"\n");
	fclose(f);
	
	try {
		auto config = toml::parse_file("test.toml");
		const char* library_name = config["library"]["name"].value_or("");
		eqLogMessage(LogInfo, "library_name: %s", library_name);
		/*
		for (auto&& [k, v] : config) {
			cout << k << endl;
			v.visit([](auto& node) noexcept {
				cout << node << "\n";				
			});
		}*/
	} catch (const toml::parse_error& err) {
		eqLogMessage(LogError, "parse test.toml: %s", err.description());
		return 1;
	}
	
	
	for(i = 1; i < argc; ++i) {
		parse(argv[i]);
	}
	return 0;
}


void parse(const char* path) {
	if (strlen(path) < 2) return;

	eqLogMessage(LogInfo, "parsing %s", path);
	filesystem::path fp = filesystem::path(path);
	
	string zone_name = fp.filename().string();
	if (fp.has_stem()) zone_name = fp.stem().string();	
	string ext = fp.extension().string();
	if (ext == ".s3d") {
		if (filesystem::is_directory(fp)) {			
			if (fp.filename().string().find("_") != 0) {
				eqLogMessage(LogWarn, "%s: folder must have prefix _ to be compressed, skipping", path);
				return;
			}
			pfsCompress(path);
			return;
		}
		pfsExtract(path);
		return;
	}
	if (ext == ".eqg") {
		if (filesystem::is_directory(fp)) {
			pfsCompress(path);
			return;
		}
		pfsExtract(path);
		return;
	}

	if (ext == ".bsp") {
		bspToWld(path);
		return;
	}

	if (ext == ".wld") {
		wldToBsp(path);
		return;
	}

	if (ext == ".gltf") {
		gltfToWld(path);
		return;
	}

	if (ext == ".glb") {
		glbToWld(path);
		return;
	}

	eqLogMessage(LogInfo, "%s: unknown file extension, skipping", path);
	return;
}
/*
bool extractPfs(const char* path) {
	filesystem::path fp = filesystem::path(path);
	
	string zone_name = fp.filename().string();
	if (fp.has_stem()) zone_name = fp.stem().string();

	if (ext.compare(".s3d") > -1) {
		PfsExtract(path);
		eqLogMessage(LogInfo, "%s: s3d pfs file", path);
		EQEmu::S3DLoader s3d;
		vector<EQEmu::S3D::WLDFragment> zone_frags;
		vector<EQEmu::S3D::WLDFragment> zone_object_frags;
		vector<EQEmu::S3D::WLDFragment> object_frags;

		EQEmu::PFS::Archive archive;
		char *image_name = "lava001.bmp";
		const vector<char> data = readFile(image_name);
		eqLogMessage(LogInfo, "%s: loaded %s %zd bytes", path, image_name, data.size());
		if (!archive.Set(image_name, data)) {
			eqLogMessage(LogError, "failed to set %s", image_name);
		}
		if (!archive.Save("test.s3d")) {
			eqLogMessage(LogError, "save failed");
		}

		if (!s3d.ParseWLDFile(zone_name + ".s3d", zone_name + ".wld", zone_frags)) {
			eqLogMessage(LogInfo, "parse %s.s3d: %s.wld skipped", zone_name.c_str(), zone_name.c_str());
			return true;
		}

		if (!s3d.ParseWLDFile(zone_name + ".s3d", "objects.wld", zone_object_frags)) {
			eqLogMessage(LogInfo, "parse %s.s3d: objects.wld skipped", zone_name.c_str());
			return true;
		}

		if (!s3d.ParseWLDFile(zone_name + "_obj.s3d", zone_name + "_obj.wld", object_frags)) {
			eqLogMessage(LogInfo, "%s_obj.s3d: %s_obj.wld skipped", zone_name.c_str(), zone_name.c_str());
			return true;
		}
		
		eqLogMessage(LogInfo, "loaded as s3d");
		return true;
	}

	if (ext.compare(".eqg") > -1) {
		eqLogMessage(LogInfo, "%s: detected as eqg pfs file", path);
		EQEmu::EQGLoader eqg;
		vector<shared_ptr<EQEmu::EQG::Geometry>> eqg_models;
		vector<shared_ptr<EQEmu::Placeable>> eqg_placables;
		vector<shared_ptr<EQEmu::EQG::Region>> eqg_regions;
		vector<shared_ptr<EQEmu::Light>> eqg_lights;

		if (eqg.Load(zone_name, eqg_models, eqg_placables, eqg_regions, eqg_lights)) {
			eqLogMessage(LogInfo, "%s: loaded as standard eqg", path);
			return true;
		}
		EQEmu::EQG4Loader eqg4;
		shared_ptr<EQEmu::EQG::Terrain> terrain;
		if (!eqg4.Load(zone_name, terrain)) {
			eqLogMessage(LogInfo, "load eqg4 was unable to load (unsupported type)");
			return true;
		}

		eqLogMessage(LogInfo, "loaded as v4 eqg");
		return true;
	}

	if (ext.compare(".bsp") > -1) {		
		eqLogMessage(LogInfo, "%s detected as bsp q3 file", path);
		Q3BspLoader loader;
		Q3BspMap  *q3map = loader.Load(path);
		if (q3map == nullptr) {
			eqLogMessage(LogInfo, "q3map returned null, skipping");
			return true;
		}
		eqLogMessage(LogInfo, "found %zu brushes", q3map->brushes.size());
		return true;
	}
	return false;

}
*/

vector<char> readFile(const char* filename) {	
    vector<char> vec;
    ifstream file(filename, ios::binary);
	if (!file.is_open()) {
		eqLogMessage(LogError, "open %s failed: %s", filename, strerror(errno));
		return vec;
	}
    file.unsetf(ios::skipws);
    streampos fileSize;

    file.seekg(0, ios::end);
    fileSize = file.tellg();
    file.seekg(0, ios::beg);

    vec.reserve(fileSize);

    vec.insert(vec.begin(), istream_iterator<char>(file), istream_iterator<char>());
    return vec;
}

void writeFile(const char* filename, vector<char> data) {
    ofstream fout(filename, ios::out | ios::binary);
	for (auto d : data) {
		fout.write(&d, 1);
	}
	fout.close();
}

void pfsExtract(const char* path) {
	EQEmu::PFS::Archive archive;
	if (!archive.Open(path)) {
		eqLogMessage(LogError, "%s open failed", path);
		return;
	}
	vector<string> filenames;
	if (!archive.GetFilenames("", filenames)) {
		eqLogMessage(LogError, "%s: GetFilenames: failed", path);
		return;
	}
	filesystem::path fp = filesystem::path(path);
	auto outPath = fp.parent_path().string();	
	outPath.append("_" + fp.filename().string() + "/");
	filesystem::create_directories(outPath);

	int extractCount = 0;
	for (auto itr : filenames) {
		vector<char> buf;
		if (!archive.Get(itr, buf)) {
			eqLogMessage(LogError, "%s Get %s: failed", path, itr.c_str());
			return;
		}
		string outFile = outPath;
		outFile.append(itr);
		writeFile(outFile.c_str(), buf);
		extractCount++;
	}
	eqLogMessage(LogInfo, "%s extracted %d files to %s", path, extractCount, outPath.c_str());
}

void pfsCompress(const char* path) {	
	EQEmu::PFS::Archive archive;

	auto fp = filesystem::path(path);

	//printf("base path: %s\n", fp.string().c_str());
	int compressCount = 0;
	for (auto const& filepath: filesystem::directory_iterator{fp}) {
		string inPath = path;
		string filename = filepath.path().filename().string();
		//printf("loading %s and saving as %s\n", filepath.path().string().c_str(), filename.c_str());
		inPath.append("/");
		inPath.append(filename);
		const vector<char> data = readFile(filepath.path().string().c_str());
		//eqLogMessage(LogInfo, "%s: loaded %s %zd bytes", path, filepath.path().string().c_str(), data.size());
		if (!archive.Set(filename, data)) {
			eqLogMessage(LogError, "%s: set %s: failed", path, filename.c_str());
			return;
		}
		compressCount++;
	}
	string outPath = fp.string();
	outPath = outPath.substr(1);
	if (!archive.Save(outPath)) {
		eqLogMessage(LogError, "%s: save %s: failed", path, outPath.c_str());
		return;
	}

	eqLogMessage(LogInfo, "%s compressed %d files to %s", path, compressCount, outPath.c_str());
}

void bspToWld(const char* path) {
	const float q3Scale = 5;
	vector<EQEmu::S3D::WLDFragment> frags;

	Q3BspLoader loader;
	Q3BspMap  *q3map = loader.Load(path);	
	if (q3map == nullptr) {
		eqLogMessage(LogInfo, "q3map returned null, skipping");
		return;
	}
	eqLogMessage(LogInfo, "%s: found %zu vertices", path, q3map->vertices.size());

	for (auto vert: q3map->vertices) {
		printf("%0.2f, %0.2f, %0.2f\n", vert.position.x*q3Scale, vert.position.y*q3Scale, vert.position.z*q3Scale);
		return;
	}
	return;
}

void wldToBsp(const char* path) {
	const vector<char> data = readFile(path);

	EQEmu::S3DLoader s3d;
	vector<EQEmu::S3D::WLDFragment> frags;

	if (!s3d.ParseWLD(data, frags)) {
		eqLogMessage(LogInfo, "%s: failed parse, skipping", path);
		return;
	}

	eqLogMessage(LogInfo, "%s: found %d fragments", path, frags.size());
	for (auto frag:frags) {
		if (frag.type != 0x22) {
			printf("0x%x fragment, ", frag.type);
			continue;
		}
		EQEmu::S3D::Mesh &f = reinterpret_cast<EQEmu::S3D::Mesh&>(frag);
		auto model = f.GetData();
		eqLogMessage(LogInfo, "%s: first mesh fragment (0x36) vertices is %d", path, model->GetVertices().size());
		
		for (auto vert: model->GetVertices()) {
		//	printf("%0.2f, %0.2f, %0.2f\n", vert.pos.x, vert.pos.y, vert.pos.z);
			continue;
		}
	}
	return;
}

void gltfToWld(const char* path) {
	Model model;
	TinyGLTF loader;
	string err;
	string warn;
	
	
	bool resp = loader.LoadASCIIFromFile(&model, &err, &warn, path);
	//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

	if (!warn.empty()) {
		eqLogMessage(LogWarn, "%s: %s", path, warn.c_str());
	}

	if (!err.empty()) {
		eqLogMessage(LogError, "%s: %s", path, err.c_str());
		return;
	}

	if (!resp) {
		eqLogMessage(LogError, "%s: failed parse, skipping", path);
		return;
	}
	gltfModelToWld(path, model);
	return;
}


void glbToWld(const char* path) {
	Model model;
	TinyGLTF loader;
	string err;
	string warn;
	
	
	bool resp = loader.LoadBinaryFromFile(&model, &err, &warn, path);

	if (!warn.empty()) {
		eqLogMessage(LogWarn, "%s: %s", path, warn.c_str());
	}

	if (!err.empty()) {
		eqLogMessage(LogError, "%s: %s", path, err.c_str());
		return;
	}

	if (!resp) {
		eqLogMessage(LogError, "%s: failed parse, skipping", path);
		return;
	}

	gltfModelToWld(path, model);
	return;
}

void gltfModelToWld(const char *path, Model model) {
	auto wldModel(new EQEmu::S3D::Geometry());
	wldModel->SetName(path);
	for (auto node: model.nodes) {
		if (ifind(node.name, "light")) continue; //printf("%s %0.2f %0.2f %0.2f\n", node.name.c_str(), node.translation[0], node.translation[1], node.translation[2]);
		if (ifind(node.name, "camera")) continue;
		if (node.translation.size() == 0) continue;
		printf("%s node %0.2f, %0.2f, %0.2f\n", node.name.c_str(), node.translation[0], node.translation[1], node.translation[2]);
		
		if (model.meshes.size() < node.mesh) {
			eqLogMessage(LogWarn, "node %s refers to mesh %d, which is larger than meshes (%d)", node.name.c_str(), node.mesh, model.meshes.size());			
			return;
		}
		for (auto prim: model.meshes[node.mesh].primitives) {
			if (model.materials.size() < prim.material) {
				eqLogMessage(LogWarn, "node %s refers to mesh %d material %d, which is larger than materials (%d)", node.name.c_str(), node.mesh, prim.material, model.materials.size());				
				return;
			}
			printf("%s node mesh %d material %d name is %s\n", node.name.c_str(), node.mesh, prim.material, model.materials[prim.material].name.c_str());
			//prim.mode default is 4, triangles, but 0: points, 1: lines, 2: line_loop, 3: line_strip, 4: triangles, 5: triangle_strip, 6: triangle_fan			
			if (prim.mode != 4) {
				eqLogMessage(LogWarn, "node %s refers to mesh %d and stores data as mode %d, which is not yet supported", node.name.c_str(), node.mesh, prim.mode);
				return;
			}

			//https://github.com/andreasdr/tdme2/blob/f76444c2d6568fee75ed1f61514f77f73c41e7a4/src/tdme/engine/fileio/models/GLTFReader.cpp#L442
			for (auto kv = prim.attributes.begin(); kv != prim.attributes.end(); kv++) {
				printf("attr: %s\n", kv->first.c_str());
				if (kv->first._Equal("POSITION")) {
					auto accessor = model.accessors[kv->second];
					if (accessor.type == (int)TINYGLTF_TYPE_VEC3) {
						eqLogMessage(LogWarn, "node %s refers to mesh %d attr %s value %d type %d, which is not VEC3 (3)",  node.name.c_str(), node.mesh, kv->first.c_str(), kv->second, accessor.type);				
						return;
					}
					
				}
			}

			if (model.accessors.size() < prim.indices)
			{
				eqLogMessage(LogWarn, "node %s refers to mesh %d indice %d, which is larger than accessors (%d)", node.name.c_str(), node.mesh, prim.indices, model.accessors.size());
				return;
			}

			auto accessor = model.accessors[prim.indices];

			if (model.bufferViews.size() < accessor.bufferView) {
				eqLogMessage(LogWarn, "node %s refers to mesh %d indices %d bufferViews %d, which is larger than buffViews (%d)", node.name.c_str(), node.mesh, prim.indices, accessor.bufferView, model.accessors.size());				
				return;
			}
			if (accessor.type == TINYGLTF_TYPE_VEC3) {
				eqLogMessage(LogWarn, "node %s refers to mesh %d indices %d bufferViews %d type %d, which is not VEC3 (3)", node.name.c_str(), node.mesh, prim.indices, accessor.bufferView, accessor.type);				
				return;
			}

			//auto bufferView = model.bufferViews[accessor.bufferView];
			

		}

		
	}

	//wldModel->AddVert();
}

bool ifind(const string & strHaystack, const string & strNeedle) {
  auto it = search(
    strHaystack.begin(), strHaystack.end(),
    strNeedle.begin(),   strNeedle.end(),
    [](char ch1, char ch2) { return toupper(ch1) == toupper(ch2); }
  );
  return (it != strHaystack.end() );
}