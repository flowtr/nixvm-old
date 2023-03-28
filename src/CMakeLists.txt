file(GLOB NIX_PARSER_SOURCES *.cpp)
add_executable(nix-parser ${NIX_PARSER_SOURCES})
target_include_directories(
	nix-parser
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)

install(
	TARGETS nix-parser
	EXPORT nix-parser-targets
	RUNTIME DESTINATION bin
)
