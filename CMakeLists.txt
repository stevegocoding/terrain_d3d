cmake_minimum_required(VERSION 2.6.2)
project(TerrainDemo)

set(CMAKE_MODULE_PATH 
  "${PROJECT_SOURCE_DIR}/CMake"
  "${PROJECT_SOURCE_DIR}/CMake/Utils" 
  "${PROJECT_SOURCE_DIR}/CMake/Packages"
)

# set the working directory
set(WORKING_DIR ${PROJECT_BINARY_DIR})

# Header files 
set (SOURCE_FILES
        # base code
        src/Prerequisites.h
        src/Camera.h 
        src/Camera.cpp
        src/Frustum.h
        src/Frustum.cpp
        src/RenderDevice.h
        src/RenderDevice.cpp
        src/DDreamMap.h
        src/DDreamMap.cpp
        src/Texture.h
        src/Texture.cpp
        src/Image.h
        src/Image.cpp
        src/Timer.h
        src/VertexIndexBuffer.h
        src/VertexIndexBuffer.cpp
        #src/vfxXMLParser.h
        #src/vfxXMLParser.cpp

        # app code
        src/TerrainDemo.h
        src/TerrainDemo.cpp
        src/DemoApp.h
        src/DemoApp.cpp
        src/ITTerrain.h
        src/ITTerrain.cpp
        src/ITQTreeTile9X9IndexData.h
        src/ITQTreeTile9X9IndexTable.h
        src/ITQTreeTile9x9IndexTable.cpp
        src/skybox.h
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Set target properties
set(CMAKE_DEBUG_POSTFIX "_d")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows")


# Specify build path (under /build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WORKING_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WORKING_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WORKING_DIR}/bin")

add_executable(TerrainDemo ${SOURCE_FILES})









