project (gloox)
cmake_minimum_required (VERSION 2.6)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
aux_source_directory(${PROJECT_SOURCE_DIR} gloox_srcs)
#include_directories (${Boost_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${BZIP2_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})
if (WIN32)
  add_definitions( -DWIN32 -D_WIN32)
endif (WIN32)
#configure_file( ${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h.unix )

set (LINK STATIC)
add_library (gloox ${LINK} ${gloox_srcs})
