cmake_minimum_required(VERSION 3.31...4.1)

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

# Find SQLite3 before third_party/botan: the imported target must be created
# in this directory so it is visible both here and in the subdirectory (with
# CMake < 4 a target created only in the subdirectory is not usable here).
find_package(SQLite3 REQUIRED)
# CMake < 4.3 defines only the legacy SQLite::SQLite3 target name; 4.3+
# defines SQLite3::SQLite3 and deprecates the legacy one.
if(NOT TARGET SQLite3::SQLite3)
  add_library(SQLite3::SQLite3 ALIAS SQLite::SQLite3)
endif()

# A fix for my broken toolchain on macOS 26 (todo).
if(APPLE)
  get_target_property(_sqlite_target SQLite3::SQLite3 ALIASED_TARGET)
  if(NOT _sqlite_target)
    set(_sqlite_target SQLite3::SQLite3)
  endif()
  get_target_property(_sqlite_inc ${_sqlite_target} INTERFACE_INCLUDE_DIRECTORIES)
  if(_sqlite_inc MATCHES "/SDKs/MacOSX[^/]*\\.sdk/usr/include")
    set_target_properties(${_sqlite_target} PROPERTIES
                          INTERFACE_INCLUDE_DIRECTORIES "")
  endif()
  unset(_sqlite_target)
  unset(_sqlite_inc)
endif()

# Add libthr on FreeBSD for Botan's OS::set_thread_name call to
# pthread_setname_np. Debug implicitly pulls it in via ASan runtime.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Third party
add_subdirectory(third_party/botan)

# Sources
add_executable(
  ${PROJECT_NAME}
  src/main.cpp
  src/config.cpp
  src/ca.cpp
  src/p11.cpp
  src/store.cpp
  src/util.cpp
  src/x509ext.cpp)

# Load-test seeder is a separate binary
add_executable(${PROJECT_NAME}-seed src/seed_main.cpp src/seed.cpp
                                    src/config.cpp src/ca.cpp src/p11.cpp
                                    src/store.cpp src/util.cpp src/x509ext.cpp)

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
  target_link_libraries(${tgt} PRIVATE botan SQLite3::SQLite3 Threads::Threads)
  # FreeBSD links libc++ (over libcxxrt) via -stdlib=libc++ alone; it has no
  # separate libc++abi.
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${tgt} PRIVATE c++ c++abi)
  endif()
  # Release binaries ship stripped. ld64 on macOS has no --strip-all; dead
  # code stripping is handled above via -dead_strip.
  if(NOT APPLE)
    target_link_options(${tgt} PRIVATE $<$<CONFIG:Release>:-Wl,--strip-all>)
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
                                     src/util.cpp src/ca.cpp src/p11.cpp
                                     src/store.cpp src/x509ext.cpp)
target_include_directories(${PROJECT_NAME}_tests
                           PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME}_tests SYSTEM
                           PRIVATE "${PROJECT_SOURCE_DIR}/third_party")
target_compile_options(${PROJECT_NAME}_tests
                       PRIVATE -stdlib=libc++ -std=c++23 -Wall -Wextra -Werror
                               ${YCA_SAN_FLAGS})
target_link_options(${PROJECT_NAME}_tests PRIVATE -stdlib=libc++
                    ${YCA_SAN_FLAGS})
target_link_libraries(${PROJECT_NAME}_tests PRIVATE botan SQLite3::SQLite3
                                                   Threads::Threads)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(${PROJECT_NAME}_tests PRIVATE c++ c++abi)
endif()

# Install: the CLI + support files. yca-seed/yca_tests are not installed;
# the Go binaries and the systemd units are handled by a separate script.
# The real CLI goes to libexec (services and yca-acme call it there); the
# `yca` and `yca-acme` on PATH are the operator wrappers running under the
# yca account.
include(GNUInstallDirs)
set(YCA_REAL
    ${CMAKE_INSTALL_FULL_LIBEXECDIR}/${PROJECT_NAME}/${PROJECT_NAME})
set(YCA_ACME_REAL ${CMAKE_INSTALL_FULL_LIBEXECDIR}/${PROJECT_NAME}/yca-acme)
# Configure-time copies for the wrapper tests (ctest).
configure_file(share/bin/${PROJECT_NAME}.in
               ${CMAKE_BINARY_DIR}/wrapper/${PROJECT_NAME} @ONLY)
configure_file(share/bin/yca-acme.in ${CMAKE_BINARY_DIR}/wrapper/yca-acme
               @ONLY)
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/${PROJECT_NAME})
# The wrappers are generated at install time and their paths to the
# real binaries follow `cmake --install --prefix`.
install(
  CODE "set(YCA_REAL \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}/${PROJECT_NAME}/${PROJECT_NAME}\")
  set(YCA_ACME_REAL \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBEXECDIR}/${PROJECT_NAME}/yca-acme\")
  configure_file(\"${CMAKE_SOURCE_DIR}/share/bin/${PROJECT_NAME}.in\"
                 \"${CMAKE_BINARY_DIR}/wrapper-install/${PROJECT_NAME}\" @ONLY)
  configure_file(\"${CMAKE_SOURCE_DIR}/share/bin/yca-acme.in\"
                 \"${CMAKE_BINARY_DIR}/wrapper-install/yca-acme\" @ONLY)")
install(PROGRAMS ${CMAKE_BINARY_DIR}/wrapper-install/${PROJECT_NAME}
                 ${CMAKE_BINARY_DIR}/wrapper-install/yca-acme
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES share/zsh-completion/_${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/zsh/site-functions)
install(FILES share/bash-completion/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions)
install(FILES share/man/${PROJECT_NAME}.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

enable_testing()
add_test(NAME unit COMMAND ${PROJECT_NAME}_tests)
add_test(NAME e2e COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e.sh
                          $<TARGET_FILE:${PROJECT_NAME}>)
add_test(NAME wrapper COMMAND sh ${CMAKE_SOURCE_DIR}/tests/wrapper.sh
                              ${CMAKE_BINARY_DIR}/wrapper/${PROJECT_NAME})
add_test(NAME wrapper-acme COMMAND sh ${CMAKE_SOURCE_DIR}/tests/wrapper-acme.sh
                                   ${CMAKE_BINARY_DIR}/wrapper/yca-acme)
# pkcs11 backend on ephemeral SoftHSM2 token
add_test(NAME e2e-hsm COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e-hsm.sh
                              $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(e2e-hsm PROPERTIES SKIP_RETURN_CODE 77)
# Go ACME frontend (acme/) driven by a real client
add_test(NAME e2e-acme COMMAND bash ${CMAKE_SOURCE_DIR}/tests/e2e-acme.sh
                               $<TARGET_FILE:${PROJECT_NAME}>)
set_tests_properties(e2e-acme PROPERTIES SKIP_RETURN_CODE 77)
# zsh completion scripts, driven through a real completion widget
add_test(NAME completion COMMAND bash ${CMAKE_SOURCE_DIR}/tests/completion.sh
                                 ${CMAKE_SOURCE_DIR}/share/zsh-completion)
set_tests_properties(completion PROPERTIES SKIP_RETURN_CODE 77)
# Completions against the actual options the binaries report
add_test(NAME completion-sync
         COMMAND bash ${CMAKE_SOURCE_DIR}/tests/completion-sync.sh
                 $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/acme
                 ${CMAKE_SOURCE_DIR}/share)
set_tests_properties(completion-sync PROPERTIES SKIP_RETURN_CODE 77)
