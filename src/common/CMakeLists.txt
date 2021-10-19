CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)

SET(common_sources
	compression.cpp
	eq_math.cpp
	eqg_loader.cpp
	eqg_model_loader.cpp
	eqg_v4_loader.cpp
	oriented_bounding_box.cpp
	pfs.cpp
	pfs_crc.cpp
	q3bsp/Q3BspLoader.cpp
	q3bsp/Q3BspMap.cpp
	q3bsp/Q3BspPatch.cpp
	s3d_loader.cpp
	string_util.cpp
	water_map.cpp
	water_map_v1.cpp
	water_map_v2.cpp
	wld_fragment.cpp
	event/event_loop.cpp
)

SET(common_headers
	aligned_bounding_box.h
	any.h
	compression.h
	eq_math.h
	eqemu_endian.h
	eqg_geometry.h
	eqg_invis_wall.h
	eqg_loader.h
	eqg_material.h
	eqg_model_loader.h
	eqg_region.h
	eqg_structs.h
	eqg_terrain.h
	eqg_terrain_tile.h
	eqg_v4_loader.h
	eqg_water_sheet.h
	gltf/json.hpp
	gltf/stb_image_write.h
	gltf/stb_image.h
	gltf/tiny_gltf.h
	light.h
	octree.h
	oriented_bounding_box.h
	pfs.h
	pfs_crc.h
	placeable.h
	placeable_group.h	
	q3bsp/Q3Bsp.hpp
	q3bsp/Q3BspLoader.hpp
	q3bsp/Q3BspMap.hpp
	q3bsp/BspMap.hpp
	q3bsp/Q3BspPatch.hpp
	q3bsp/Q3BspRenderHelpers.hpp
	safe_alloc.h
	s3d_bsp.h
	s3d_geometry.h
	s3d_loader.h
	s3d_skeleton_track.h
	s3d_texture.h
	s3d_texture_brush.h
	s3d_texture_brush_set.h
	string_util.h
	water_map.h
	water_map_v1.h
	water_map_v2.h
	wld_fragment_reference.h
	wld_fragment.h
	wld_structs.h
	event/background_task.h
	event/event_loop.h
	event/timer.h
	toml.hpp
)

ADD_LIBRARY(common ${common_sources} ${common_headers})


SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
