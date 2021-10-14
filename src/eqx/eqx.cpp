#include <string.h>
#include "stdio.h"
#include "eqx.h"
#include "toml.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include "compression.h"
#include "log_macros.h"
#include <gtc/matrix_transform.hpp>
#include "s3d_loader.h"
#include "eqg_loader.h"
#include "eqg_v4_loader.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#define GLM_FORCE_RADIANS
#include <glm.hpp>

using namespace std;

int main(int argc, char **argv) {
	printf("eqx v0.0.1\n");
	
	int i = 1;
	bool ignore_collide_tex = true;
	if (argc > 2) {
		if (strcmp(argv[1], "--IncludeCollideTex") == 0) {
			ignore_collide_tex = false;
			i = 2;
		}
	}

	for(; i < argc; ++i) {
		parse(argv[i]);
	}
	return 0;
}


bool parse(char* path) {
	printf("[INF] parsing %s\n", path);
	if (extractPfs(path)) return true;

	printf("[ERR] failed to parse %s: unknown type\n", path);
	return false;
}

bool extractPfs(char* path) {
	filesystem::path fp = filesystem::path(path);
	string ext = fp.extension();
	string zone_name = fp.filename();	
	if (fp.has_stem()) zone_name = fp.stem().string();

	if (ext.compare(".s3d") > -1) {
		printf("[INF] detected as s3d pfs file\n");
		EQEmu::S3DLoader s3d;
		std::vector<EQEmu::S3D::WLDFragment> zone_frags;
		std::vector<EQEmu::S3D::WLDFragment> zone_object_frags;
		std::vector<EQEmu::S3D::WLDFragment> object_frags;

		if (!s3d.ParseWLDFile(zone_name + ".s3d", zone_name + ".wld", zone_frags)) {
			printf("[INF] parse %s.s3d: %s.wld skipped\n", zone_name.c_str(), zone_name.c_str());
			return true;
		}

		if (!s3d.ParseWLDFile(zone_name + ".s3d", "objects.wld", zone_object_frags)) {
			printf("[INF] parse %s.s3d: objects.wld skipped\n", zone_name.c_str());
			return true;
		}

		if (!s3d.ParseWLDFile(zone_name + "_obj.s3d", zone_name + "_obj.wld", object_frags)) {
			printf("[INF] %s_obj.s3d: %s_obj.wld skipped\n", zone_name.c_str(), zone_name.c_str());
			return true;
		}
		printf("[INF] loaded as s3d\n");
		return true;
	}

	if (ext.compare(".eqg") > -1) {
		printf("[INF] detected as eqg pfs file\n");
		EQEmu::EQGLoader eqg;
		std::vector<std::shared_ptr<EQEmu::EQG::Geometry>> eqg_models;
		std::vector<std::shared_ptr<EQEmu::Placeable>> eqg_placables;
		std::vector<std::shared_ptr<EQEmu::EQG::Region>> eqg_regions;
		std::vector<std::shared_ptr<EQEmu::Light>> eqg_lights;

		if (eqg.Load(zone_name, eqg_models, eqg_placables, eqg_regions, eqg_lights)) {
			printf("[INF] loaded as standard eqg\n");
			return true;
		}
		printf("[INF] attempting to load %s.eqg as v4 eqg\n", zone_name.c_str());
		EQEmu::EQG4Loader eqg4;
		std::shared_ptr<EQEmu::EQG::Terrain> terrain;
		if (!eqg4.Load(zone_name, terrain)) {
			printf("[INF] load eqg4 did not succeed\n");
			return true;
		}

		printf("[INF] loaded as v4 eqg\n");
		return true;
	}

	
	return false;

}
