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

using namespace std;

int main(int argc, char **argv) {
	eqLogInit(62);
	eqLogRegister(std::shared_ptr<EQEmu::Log::LogBase>(new EQEmu::Log::LogStdOut()));
	eqLogRegister(std::shared_ptr<EQEmu::Log::LogBase>(new EQEmu::Log::LogFile("eqx.log")));
	
	eqLogMessage(LogInfo, "eqx v0.0.1");
	
	int i = 1;
	bool ignore_collide_tex = true;
	if (argc > 2) {
		if (strcmp(argv[1], "--IncludeCollideTex") == 0) {
			ignore_collide_tex = false;
			i = 2;
		}
	}

	try {
		auto config = toml::parse_file("test.toml");
		std::string_view library_name = config["library"]["name"].value_or("");
		eqLogMessage(LogInfo, "library_name: %s", library_name);
	} catch (const toml::parse_error& err) {
		eqLogMessage(LogError, "parse test.toml: %s", err.description());
		return 1;
	}
	
	for(; i < argc; ++i) {
		parse(argv[i]);
	}
	return 0;
}


bool parse(const char* path) {
	eqLogMessage(LogInfo, "parsing %s");
	if (extractPfs(path)) return true;

	eqLogMessage(LogError, "failed to parse %s: unknown type");
	return false;
}

bool extractPfs(const char* path) {
	filesystem::path fp = filesystem::path(path);
	string ext = fp.extension().string();
	string zone_name = fp.filename().string();
	if (fp.has_stem()) zone_name = fp.stem().string();

	if (ext.compare(".s3d") > -1) {
		eqLogMessage(LogInfo, "detected as s3d pfs file");
		EQEmu::S3DLoader s3d;
		std::vector<EQEmu::S3D::WLDFragment> zone_frags;
		std::vector<EQEmu::S3D::WLDFragment> zone_object_frags;
		std::vector<EQEmu::S3D::WLDFragment> object_frags;

		EQEmu::PFS::Archive archive;
		char *image_name = "lava001.bmp";
		const std::vector<char> data = readFile(image_name);
		eqLogMessage(LogInfo, "loaded %s %zd bytes", image_name, data.size());
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
		eqLogMessage(LogInfo, "detected as eqg pfs file");
		EQEmu::EQGLoader eqg;
		std::vector<shared_ptr<EQEmu::EQG::Geometry>> eqg_models;
		std::vector<shared_ptr<EQEmu::Placeable>> eqg_placables;
		std::vector<shared_ptr<EQEmu::EQG::Region>> eqg_regions;
		std::vector<shared_ptr<EQEmu::Light>> eqg_lights;

		if (eqg.Load(zone_name, eqg_models, eqg_placables, eqg_regions, eqg_lights)) {
			eqLogMessage(LogInfo, "loaded as standard eqg");
			return true;
		}
		eqLogMessage(LogInfo, "attempting to load %s.eqg as v4 eqg", zone_name.c_str());
		EQEmu::EQG4Loader eqg4;
		shared_ptr<EQEmu::EQG::Terrain> terrain;
		if (!eqg4.Load(zone_name, terrain)) {
			eqLogMessage(LogInfo, "load eqg4 did not succeed");
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

std::vector<char> readFile(const char* filename) {	
    std::vector<char> vec;
    std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		eqLogMessage(LogError, "open %s failed: %s", filename, strerror(errno));
		return vec;
	}
    file.unsetf(std::ios::skipws);
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    vec.reserve(fileSize);

    vec.insert(vec.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());

    return vec;
}