add_executable(ulti_minimax main.cpp)

add_library(ulti_game rules.cpp game_manager.cpp)

target_link_libraries(ulti_minimax ulti_game)
target_include_directories(ulti_minimax PRIVATE ${PROJECT_SOURCE_DIR}/cpp/src)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/cpp/src" PREFIX "Header Files" FILES
    cpp/src/rules.hpp
    cpp/src/game_manager.hpp
)

# WASM-specific configuration
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(STATUS "Configuring for WebAssembly (WASM)")

    # Update target properties for Emscripten
    set_target_properties(ulti_minimax PROPERTIES
        OUTPUT_NAME "ulti_minimax"
        SUFFIX ".js"  # Emscripten generates .js glue code
		# LINK_FLAGS "-s WASM=1 -s EXPORTED_RUNTIME_METHODS=\"['ccall', 'cwrap', 'malloc', 'free', 'UTF8ToString', 'stringToUTF8']\""
		LINK_FLAGS "-s WASM=1 -s EXPORTED_FUNCTIONS=[\"_malloc\",\"_free\",\"_wasm_main\"] -s EXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"UTF8ToString\",\"stringToUTF8\"]"
    )
# Native build-specific properties
else()    
    message(STATUS "Configuring for native build")
    set_target_properties(ulti_minimax PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
    )
endif()