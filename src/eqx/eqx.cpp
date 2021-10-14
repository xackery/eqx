#include <string.h>
#include "stdio.h"
#include "eqx.h"
#include "toml.hpp"
#include <iostream>

int main(int argc, char **argv) {
	
	int i = 1;
	bool ignore_collide_tex = true;
	if (argc > 2) {
		if (strcmp(argv[1], "--IncludeCollideTex") == 0) {
			ignore_collide_tex = false;
			i = 2;
		}
	}

	for(; i < argc; ++i) {		
		if (parse(argv[i])) return 0;		
	}
	return 0;
}


bool parse(char* path) {
	printf("parsing %s\n", path);

	auto config = toml::parse_file(path);

	// get key-value pairs
	std::string_view library_name = config["library"]["name"].value_or("");
	std::string_view library_author = config["library"]["authors"][0].value_or("");
	int64_t depends_on_cpp_version = config["dependencies"]["cpp"].value_or(0);

	// modify the data
	config.insert_or_assign("alternatives", toml::array{
		"cpptoml",
		"toml11",
		"Boost.TOML"
	});

	// iterate & visit over the data
	for (auto&& [k, v] : config)
	{
		v.visit([](auto& node) noexcept
		{
			std::cout << node << "\n";
			//if constexpr (toml::is_string<decltype(node)>)
				//do_something_with_string_values(node);
		});
	}

	// re-serialize as TOML
	std::cout << config << "\n";

	// re-serialize as JSON
	std::cout << toml::json_formatter{ config } << "\n";
	return true;
}