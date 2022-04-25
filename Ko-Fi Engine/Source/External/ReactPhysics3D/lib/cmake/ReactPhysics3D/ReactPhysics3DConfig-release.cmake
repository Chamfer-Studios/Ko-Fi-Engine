#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ReactPhysics3D::ReactPhysics3D" for configuration "Release"
set_property(TARGET ReactPhysics3D::ReactPhysics3D APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ReactPhysics3D::ReactPhysics3D PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/reactphysics3d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS ReactPhysics3D::ReactPhysics3D )
list(APPEND _IMPORT_CHECK_FILES_FOR_ReactPhysics3D::ReactPhysics3D "${_IMPORT_PREFIX}/lib/reactphysics3d.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
