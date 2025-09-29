#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "JsonSchemaSerializer::JsonSchemaSerializer" for configuration "Debug"
set_property(TARGET JsonSchemaSerializer::JsonSchemaSerializer APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(JsonSchemaSerializer::JsonSchemaSerializer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libJsonSchemaSerializer.a"
  )

list(APPEND _cmake_import_check_targets JsonSchemaSerializer::JsonSchemaSerializer )
list(APPEND _cmake_import_check_files_for_JsonSchemaSerializer::JsonSchemaSerializer "${_IMPORT_PREFIX}/lib/libJsonSchemaSerializer.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
