cmake_minimum_required (VERSION 3.8)
add_definitions(-D_UNICODE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "")
project (AngryEngine)

set(cpp_version "cxx_std_17")

#bat build
add_custom_target(bat_build SOURCES build.bat output/data/config.xml COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/build.bat COMMENT "run build.bat")

#bat shader_compiler
file(GLOB_RECURSE  SHADER shader/*)
file(GLOB_RECURSE  SHADER_HEADER shader/header*)
list(REMOVE_ITEM SHADER ${SHADER_HEADER})
source_group(shader FILES ${SHADER})
source_group(shader_header FILES ${SHADER_HEADER})
add_custom_target(bat_shader_compiler SOURCES shader/shader_compiler.bat ${SHADER} ${SHADER_HEADER} COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/shader/shader_compiler.bat COMMENT "run shader_compiler.bat")


#lib common
add_library(lib_common SHARED common/common.h common/template_define.h common/encode.cpp common/math.cpp common/manager.cpp common/log.cpp common/timer.cpp)

set_target_properties(lib_common PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(lib_common PROPERTIES OUTPUT_NAME_DEBUG common_debug)
set_target_properties(lib_common PROPERTIES OUTPUT_NAME_RELEASE common)
target_compile_features(lib_common PRIVATE ${cpp_version})

target_link_libraries(lib_common Dbghelp)
set(DEBUG_common ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/common_debug.dll)
set(RELEASE_common ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/common.dll)
add_custom_command(TARGET lib_common POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_common}>$<$<CONFIG:release>:${RELEASE_common}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy common")


#lib ui
add_library(lib_ui SHARED ui/ui.h ui/ui.cpp)

set_target_properties(lib_ui PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(lib_ui PROPERTIES OUTPUT_NAME_DEBUG ui_debug)
set_target_properties(lib_ui PROPERTIES OUTPUT_NAME_RELEASE ui)
set_target_properties(lib_ui PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
target_compile_features(lib_ui PRIVATE ${cpp_version})

target_include_directories(lib_ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(lib_ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(lib_ui PRIVATE debug common_debug optimized common)

set(DEBUG_ui ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/ui_debug.dll)
set(RELEASE_ui ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/ui.dll)
add_custom_command(TARGET lib_ui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_ui}>$<$<CONFIG:release>:${RELEASE_ui}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy ui")


#exe TestUI
add_executable(exe_TestUI ui/ui.h ui/ui.cpp ui/test_main.cpp)

set_target_properties(exe_TestUI PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
set_target_properties(exe_TestUI PROPERTIES OUTPUT_NAME_DEBUG TestUI_debug)
set_target_properties(exe_TestUI PROPERTIES OUTPUT_NAME_RELEASE TestUI)
set_target_properties(exe_TestUI PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
target_compile_features(exe_TestUI PRIVATE ${cpp_version})

target_include_directories(exe_TestUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exe_TestUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exe_TestUI PRIVATE debug common_debug optimized common)


#lib gpu
add_library(lib_gpu SHARED gpu/vulkan_objects.h gpu/vulkan_objects.cpp gpu/gpu_objects.h gpu/gpu_objects.cpp gpu/gpu.h gpu/gpu.cpp)

set_target_properties(lib_gpu PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(lib_gpu PROPERTIES OUTPUT_NAME_DEBUG gpu_debug)
set_target_properties(lib_gpu PROPERTIES OUTPUT_NAME_RELEASE gpu)
set_target_properties(lib_gpu PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
target_compile_features(lib_gpu PRIVATE ${cpp_version})

target_include_directories(lib_gpu PRIVATE $ENV{VULKAN_SDK}/Include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(lib_gpu PRIVATE $ENV{VULKAN_SDK}/Lib ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(lib_gpu PRIVATE vulkan-1 debug common_debug optimized common)

set(DEBUG_gpu ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/gpu_debug.dll)
set(RELEASE_gpu ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/gpu.dll)
add_custom_command(TARGET lib_ui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_gpu}>$<$<CONFIG:release>:${RELEASE_gpu}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy gpu")


#exe TestGPU
add_executable(exe_TestGPU gpu/vulkan_objects.h gpu/vulkan_objects.cpp gpu/gpu_objects.h gpu/gpu_objects.cpp  gpu/gpu.h gpu/gpu.cpp gpu/test_main.cpp)

set_target_properties(exe_TestGPU PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
set_target_properties(exe_TestGPU PROPERTIES OUTPUT_NAME_DEBUG TestGPU_debug)
set_target_properties(exe_TestGPU PROPERTIES OUTPUT_NAME_RELEASE TestGPU)
set_target_properties(exe_TestGPU PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
target_compile_features(exe_TestGPU PRIVATE ${cpp_version})

target_include_directories(exe_TestGPU PRIVATE $ENV{VULKAN_SDK}/Include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exe_TestGPU PRIVATE $ENV{VULKAN_SDK}/Lib ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exe_TestGPU PRIVATE vulkan-1 debug common_debug optimized common)


#exe CodeGenerator
add_executable(exe_CodeGenerator code_generator/code_generator.cpp code_generator/generated_config_struct_enum.h)

set_target_properties(exe_CodeGenerator PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
set_target_properties(exe_CodeGenerator PROPERTIES OUTPUT_NAME_DEBUG CodeGenerator_debug)
set_target_properties(exe_CodeGenerator PROPERTIES OUTPUT_NAME_RELEASE CodeGenerator)
set_target_properties(exe_CodeGenerator PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $(SolutionDir)$(Configuration))
target_compile_features(exe_CodeGenerator PRIVATE ${cpp_version})

target_include_directories(exe_CodeGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exe_CodeGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exe_CodeGenerator PRIVATE debug common_debug optimized common)
#add_custom_command(TARGET exe_CodeGenerator POST_BUILD COMMAND cmd //C code_generator.bat COMMENT "run code_generator.bat")
#add_custom_target(exe_CodeGenerator COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/code_generator.bat COMMENT "run code_generator.bat")


#bat code_generator
add_custom_target(bat_code_generator SOURCES code_generator/code_generator.bat COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/code_generator/code_generator.bat COMMENT "run code_generator.bat")


#exe AngryEngine
set(SRC_ALGORITHM src/game_encode.cpp)
set(SRC_ALGORITHM_HEADER src/game_encode.h)
set(SRC_UI src/ui.cpp)
set(SRC_UI_HEADER src/ui.h)
set(SRC_VULKAN src/graphics.cpp src/vulkan.cpp)
set(SRC_VULKAN_HEADER src/graphics.h src/vulkan.h)

set(SRC_GAMEOBJECT  src/animation.cpp   src/axis.cpp    src/camera.cpp
                    src/cubemap.cpp src/grid.cpp
                    src/inputcontrol.cpp src/light.cpp  src/line.cpp
                    src/material.cpp    src/model.cpp   src/object.cpp
                    src/particle.cpp    src/plane.cpp   src/postprocessing.cpp
                    src/transform.cpp   src/rendersetting.cpp src/scene.cpp)

set(SRC_GAMEOBJECT_HEADER src/animation.h   src/axis.h    src/camera.h
                          src/cubemap.h src/grid.h
                          src/inputcontrol.h src/light.h  src/line.h
                          src/material.h    src/model.h   src/object.h
                          src/particle.h    src/plane.h   src/postprocessing.h
                          src/transform.h   src/rendersetting.h src/scene.h)

set(SRC_MANAGER src/game_asset.cpp  src/command.cpp src/engine.cpp
                src/global.cpp  src/objectmanager.cpp)

set(SRC_MANAGER_HEADER src/header.h        src/game_asset.h src/command.h
                       src/global.h         src/engine.h
                       src/objectmanager.h)

file(GLOB_RECURSE  SRC_MODEL output/data/models/*.gltf)

source_group(algorithm FILES ${SRC_ALGORITHM})
source_group(algorithm\\header FILES ${SRC_ALGORITHM_HEADER})
source_group(gameobject FILES ${SRC_GAMEOBJECT})
source_group(gameobject\\header FILES ${SRC_GAMEOBJECT_HEADER})
source_group(manager FILES ${SRC_MANAGER})
source_group(manager\\header FILES ${SRC_MANAGER_HEADER})
source_group(ui FILES ${SRC_UI})
source_group(ui\\header FILES ${SRC_UI_HEADER})
source_group(vulkan FILES ${SRC_VULKAN})
source_group(vulkan\\header FILES ${SRC_VULKAN_HEADER})
source_group(model FILES ${SRC_MODEL})

add_executable(exe_AngryEngine  ${SRC_ALGORITHM} ${SRC_ALGORITHM_HEADER} ${SRC_GAMEOBJECT} ${SRC_GAMEOBJECT_HEADER}
                                 ${SRC_MANAGER} ${SRC_MANAGER_HEADER} ${SRC_UI} ${SRC_UI_HEADER} ${SRC_VULKAN}
                                 ${SRC_VULKAN_HEADER} ${SRC_MODEL})

set_target_properties(exe_AngryEngine PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
set_target_properties(exe_AngryEngine PROPERTIES OUTPUT_NAME_DEBUG AngryEngine_debug)
set_target_properties(exe_AngryEngine PROPERTIES OUTPUT_NAME_RELEASE AngryEngine)
set_target_properties(exe_AngryEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
target_compile_features(exe_AngryEngine PRIVATE ${cpp_version})

target_include_directories(exe_AngryEngine PRIVATE $ENV{VULKAN_SDK}/Include ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(exe_AngryEngine PRIVATE $ENV{VULKAN_SDK}/Lib  ${CMAKE_CURRENT_SOURCE_DIR}/build)
target_link_libraries(exe_AngryEngine vulkan-1 debug common_debug optimized common)

set(DEBUG_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug/AngryEngine_debug.exe)
set(RELEASE_AngryEngine ${CMAKE_CURRENT_SOURCE_DIR}/build/Release/AngryEngine.exe)
add_custom_command(TARGET exe_AngryEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<$<CONFIG:debug>:${DEBUG_AngryEngine}>$<$<CONFIG:release>:${RELEASE_AngryEngine}>" ${CMAKE_CURRENT_SOURCE_DIR}/output COMMENT "copy AngryEngine")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT exe_AngryEngine)
