cmake_minimum_required(VERSION 3.10)
project(JsonSchemaSerializer VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/JsonSchemaSerializer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/JsonSchemaSerializer/src)

# Source files for the library
set(JSONSCHEMA_SOURCES
    JsonSchemaSerializer/src/JsonSchemaParser.cpp
    JsonSchemaSerializer/src/BinarySerializer.cpp
    JsonSchemaSerializer/src/JsonSchemaValidator.cpp
    JsonSchemaSerializer/src/JsonSchemaSerializer.cpp
)

# Header files for the library
set(JSONSCHEMA_HEADERS
    JsonSchemaSerializer/include/JsonSchemaTypes.h
    JsonSchemaSerializer/include/JsonSchemaSerializer.h
    JsonSchemaSerializer/src/JsonSchemaParser.h
    JsonSchemaSerializer/src/BinarySerializer.h
    JsonSchemaSerializer/src/JsonSchemaValidator.h
)

# Create the static library
add_library(JsonSchemaSerializer STATIC ${JSONSCHEMA_SOURCES} ${JSONSCHEMA_HEADERS})

# Set library properties
set_target_properties(JsonSchemaSerializer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER ${JSONSCHEMA_HEADERS}
)

# Compiler-specific options
if(MSVC)
    target_compile_definitions(JsonSchemaSerializer PRIVATE JSONSCHEMASERIALIZER_EXPORTS)
    target_compile_options(JsonSchemaSerializer PRIVATE /W4)
else()
    target_compile_options(JsonSchemaSerializer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Create unit tests executable
add_executable(JsonSchemaSerializerTests
    JsonSchemaSerializerTests/TestRunner.cpp
    JsonSchemaSerializerTests/TestJsonSchemaParser.cpp
    JsonSchemaSerializerTests/TestBinarySerializer.cpp
    JsonSchemaSerializerTests/TestJsonSchemaValidator.cpp
    JsonSchemaSerializerTests/TestJsonSchemaSerializer.cpp
)

# Link tests with the library
target_link_libraries(JsonSchemaSerializerTests JsonSchemaSerializer)

# Create example executable
add_executable(JsonSchemaSerializerExample
    example_usage.cpp
)

# Link example with the library
target_link_libraries(JsonSchemaSerializerExample JsonSchemaSerializer)

# Enable testing
enable_testing()

# Add test to CTest
add_test(NAME JsonSchemaSerializerTests COMMAND JsonSchemaSerializerTests)

# Installation rules
install(TARGETS JsonSchemaSerializer
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

install(TARGETS JsonSchemaSerializerTests JsonSchemaSerializerExample
    RUNTIME DESTINATION bin
)

# Install schema files
install(FILES
    address.schema.json
    person.schema.json
    DESTINATION share/JsonSchemaSerializer/schemas
)

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    JsonSchemaSerializerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonSchemaSerializerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/JsonSchemaSerializerConfig.cmake
    INSTALL_DESTINATION lib/cmake/JsonSchemaSerializer
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/JsonSchemaSerializerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/JsonSchemaSerializerConfigVersion.cmake
    DESTINATION lib/cmake/JsonSchemaSerializer
)

# Export targets
install(TARGETS JsonSchemaSerializer
    EXPORT JsonSchemaSerializerTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

install(EXPORT JsonSchemaSerializerTargets
    FILE JsonSchemaSerializerTargets.cmake
    NAMESPACE JsonSchemaSerializer::
    DESTINATION lib/cmake/JsonSchemaSerializer
)