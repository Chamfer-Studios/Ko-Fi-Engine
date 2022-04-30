#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ReactPhysics3D::ReactPhysics3D" for configuration "Debug"
set_property(TARGET ReactPhysics3D::ReactPhysics3D APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ReactPhysics3D::ReactPhysics3D PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/reactphysics3d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS ReactPhysics3D::ReactPhysics3D )
list(APPEND _IMPORT_CHECK_FILES_FOR_ReactPhysics3D::ReactPhysics3D "${_IMPORT_PREFIX}/lib/reactphysics3d.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
