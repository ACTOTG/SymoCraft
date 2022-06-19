# Install script for directory: E:/Coding/SymoCraft/vendor/FastNoise2

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SymoCraft")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/msys64/clang64/bin/llvm-objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/Coding/SymoCraft/cmake-build-deubg-clang/vendor/FastNoise2/src/libFastNoiseD.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/FastSIMD" TYPE FILE FILES
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastSIMD/FastSIMD.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastSIMD/FastSIMD_Config.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastSIMD/FastSIMD_Export.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastSIMD/FunctionList.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastSIMD/InlInclude.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastSIMD/SIMDTypeList.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/FastNoise" TYPE FILE FILES
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/FastNoise.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/FastNoise_C.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/FastNoise_Config.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/FastNoise_Export.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Metadata.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/SmartNode.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/FastNoise/Generators" TYPE FILE FILES
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/BasicGenerators.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Blends.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Cellular.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/DomainWarp.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/DomainWarpFractal.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Fractal.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Generator.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Modifiers.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Perlin.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Simplex.h"
    "E:/Coding/SymoCraft/vendor/FastNoise2/src/../include/FastNoise/Generators/Value.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FastNoise2" TYPE FILE FILES
    "E:/Coding/SymoCraft/cmake-build-deubg-clang/vendor/FastNoise2/generated/FastNoise2Config.cmake"
    "E:/Coding/SymoCraft/cmake-build-deubg-clang/vendor/FastNoise2/generated/FastNoise2ConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FastNoise2/FastNoise2Targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FastNoise2/FastNoise2Targets.cmake"
         "E:/Coding/SymoCraft/cmake-build-deubg-clang/vendor/FastNoise2/CMakeFiles/Export/lib/cmake/FastNoise2/FastNoise2Targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FastNoise2/FastNoise2Targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/FastNoise2/FastNoise2Targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FastNoise2" TYPE FILE FILES "E:/Coding/SymoCraft/cmake-build-deubg-clang/vendor/FastNoise2/CMakeFiles/Export/lib/cmake/FastNoise2/FastNoise2Targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/FastNoise2" TYPE FILE FILES "E:/Coding/SymoCraft/cmake-build-deubg-clang/vendor/FastNoise2/CMakeFiles/Export/lib/cmake/FastNoise2/FastNoise2Targets-debug.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/Coding/SymoCraft/cmake-build-deubg-clang/vendor/FastNoise2/src/cmake_install.cmake")

endif()

