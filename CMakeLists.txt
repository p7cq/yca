cmake_minimum_required(VERSION 4.1)

file(STRINGS VERSION YCA_VERSION LIMIT_COUNT 1)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS VERSION)
project(yca VERSION ${YCA_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LLDB_EXPORT_ALL_SYMBOLS "ON")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(YCA_COV "")
set(YCA_COV_LINK "")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(YCA_COV -fprofile-instr-generate -fcoverage-mapping)
  set(YCA_COV_LINK -fprofile-instr-generate)
endif()
set(YCA_WARN_FLAGS
    -Wall
    -Werror
    -Wextra
    -Wignored-qualifiers
    -Wconversion
    -Wshadow
    -Wold-style-cast
    -Wnull-dereference
    -Wpedantic
    -Wformat=2)
set(YCA_SAN_FLAGS "")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(YCA_SAN_FLAGS "-fsanitize=address;-fno-omit-frame-pointer")
endif()

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET
      "11.0"
      CACHE STRING "min macOS version")
  add_link_options(-Wl,-dead_strip)
  # list(APPEND CMAKE_BUILD_RPATH "/opt/homebrew/opt/openssl@3/lib")
  set(CMAKE_INSTALL_RPATH "${CMAKE_BUILD_RPATH}")
endif()

# Third party
add_subdirectory(third_party/botan)

# Sources
add_executable(
  ${PROJECT_NAME}
  src/main.cpp
  src/config.cpp
  src/ca.cpp
  src/p11.cpp
  src/util.cpp)

# Load-test seeder is a separate binary
add_executable(${PROJECT_NAME}-seed src/seed_main.cpp src/seed.cpp
                                    src/config.cpp src/ca.cpp src/p11.cpp
                                    src/util.cpp)

find_package(SQLite3 REQUIRED)

foreach(tgt IN ITEMS ${PROJECT_NAME} ${PROJECT_NAME}-seed)
  target_compile_definitions(${tgt} PRIVATE APP_VERSION="${PROJECT_VERSION}")
  target_include_directories(${tgt} PRIVATE "${PROJECT_SOURCE_DIR}/src")
  target_include_directories(${tgt} SYSTEM
                             PRIVATE "${PROJECT_SOURCE_DIR}/third_party")
  target_compile_options(${tgt} PRIVATE -stdlib=libc++ -std=c++23
                                        ${YCA_WARN_FLAGS} ${YCA_SAN_FLAGS}
                                        ${YCA_COV})
  target_link_options(${tgt} PRIVATE -stdlib=libc++ ${YCA_SAN_FLAGS}
                      ${YCA_COV_LINK})
  target_link_libraries(${tgt} PRIVATE botan SQLite3::SQLite3)
  if(UNIX AND NOT APPLE)
    target_link_libraries(${tgt} PRIVATE c++ c++abi)
  endif()
endforeach()

add_custom_target(
  copy-compile-commands ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json)

# Unit tests (doctest). Reduced warning set: doctest's macros are not clean under
# the project's strict pedantic flags; the modules under test are (they also
# build in the main target). doctest.h is on -isystem via third_party.
add_executable(${PROJECT_NAME}_tests tests/unit_test.cpp src/config.cpp
                                     src/util.cpp src/ca.cpp src/p11.cpp)
target_include_directories(${PROJECT_NAME}_tests
                           PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME}_tests SYSTEM
                           PRIVATE "${PROJECT_SOURCE_DIR}/third_party")
target_compile_options(${PROJECT_NAME}_tests
                       PRIVATE -stdlib=libc++ -std=c++23 -Wall -Wextra -Werror
                               ${YCA_SAN_FLAGS})
target_link_options(${PROJECT_NAME}_tests PRIVATE -stdlib=libc++
                    ${YCA_SAN_FLAGS})
target_link_libraries(${PROJECT_NAME}_tests PRIVATE botan SQLite3::SQLite3)
if(UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME}_tests PRIVATE c++ c++abi)
endif()

# Install: the CLI + support files. yca-seed/yca_tests are not installed;
# the Go binaries and the systemd units are handled by a separate script.
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES share/zsh-completion/_${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/zsh/site-functions)
install(FILES share/man/${PROJECT_NAME}.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

enable_testing()
add_test(NAME unit COMMAND ${PROJECT_NAME}_tests)
add_test(NAME e2e COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e.sh
                          $<TARGET_FILE:${PROJECT_NAME}>)
# pkcs11 backend on ephemeral SoftHSM2 token
add_test(NAME e2e-hsm COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e-hsm.sh
                              $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(e2e-hsm PROPERTIES SKIP_RETURN_CODE 77)
# Go OCSP responder (responder/) against a temp PKI
add_test(NAME e2e-ocsp COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e-ocsp.sh
                               $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(e2e-ocsp PROPERTIES SKIP_RETURN_CODE 77)
# Go ACME frontend (acme/) driven by a real client
add_test(NAME e2e-acme COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e-acme.sh
                               $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(e2e-acme PROPERTIES SKIP_RETURN_CODE 77)
