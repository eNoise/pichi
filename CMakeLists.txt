cmake_minimum_required (VERSION 2.6)
PROJECT(pichi)
set (PROJECT_NAME_GLOBAL pichi)
set (VERSION "0.6.1")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (CheckIncludeFile)

if (NOT MSVC)
    if (CMAKE_CROSSCOMPILING)
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -enable-auto-import")
    endif (CMAKE_CROSSCOMPILING)
    if (NOT APPLE)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--as-needed")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-needed")
    endif (NOT APPLE)
    #set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -pipe -DNDEBUG")
    #set (CMAKE_CXX_FLAGS_RELEASE     "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG -pipe")
    #set (CMAKE_CXX_FLAGS_DEBUG       "${CMAKE_CXX_FLAGS_DEBUG} -UNDEBUG -g3 -pipe")
endif (NOT MSVC)

option (LOCAL_GLOOX "Use local gloox lib" OFF)
option (LOCAL_TINYXML "Use local tinyxml lib" OFF)

CHECK_INCLUDE_FILE (time.h HAVE_TIME_H)
CHECK_INCLUDE_FILE (stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILE (sys/stat.h HAVE_SYS_STAT_H)

find_package (Sqlite3 REQUIRED)
if(WIN32)
    find_package (Boost REQUIRED COMPONENTS regex-gcc45 program_options-gcc45)
else(WIN32)
    find_package (Boost REQUIRED COMPONENTS regex program_options)
endif(WIN32)
find_package (CURL REQUIRED)

if (LOCAL_GLOOX)
  add_subdirectory(gloox)
  include_directories ("${PROJECT_SOURCE_DIR}")
  set (GLOOX_LIBRARIES "gloox")
else (LOCAL_GLOOX)
  find_package (Gloox REQUIRED)
  include_directories(${GLOOX_INCLUDE_DIR})
endif (LOCAL_GLOOX)

if (LOCAL_TINYXML)
  add_subdirectory(tinyxml)
  include_directories ("${PROJECT_SOURCE_DIR}/tinyxml")
  set (TINYXML_LIBRARIES "tinyxml")
else (LOCAL_TINYXML)
  find_package (TinyXml REQUIRED)
  include_directories(${TINYXML_INCLUDE_DIR})
endif (LOCAL_TINYXML)

set(PICHI_CPP 	src/pichiexception.cpp	
		src/pichievent.cpp
		src/log.cpp
		src/pichicurl.cpp
		src/lexemes.cpp
		src/languages.cpp    
		src/xmlsimple.cpp 
		src/commandbase.cpp
		src/pichioptions.cpp
		src/pichimessage.cpp
		src/commandhandler.cpp
		src/pichiconfig.cpp
		src/system.cpp
		src/core.cpp
		src/sqlite.cpp
		src/pichidbpatcher.cpp
		src/pichicore.cpp
		src/pichi.cpp
)

set(LINK_LIBS	pthread
		${GLOOX_LIBRARIES}
		${TINYXML_LIBRARIES}
		${SQLITE3_LIBRARIES}
		${Boost_LIBRARIES}
		${CURL_LIBRARIES}
)

#gloox fix
if(WIN32)
	set(LINK_LIBS ${LINK_LIBS} 
		      dnsapi
		      secur32
		      ws2_32
	)
endif(WIN32)

if(MINGW AND WIN32)
  ADD_CUSTOM_COMMAND (OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/pichi_rc.o"
            COMMAND i686-pc-mingw32-windres
            -I"${CMAKE_CURRENT_SOURCE_DIR}/"
            -i "${CMAKE_CURRENT_SOURCE_DIR}/pichi.rc"
            -o "${CMAKE_CURRENT_BINARY_DIR}/pichi_rc.o"
            )
  set(PICHI_CPP ${PICHI_CPP} "${CMAKE_CURRENT_BINARY_DIR}/pichi_rc.o")
endif(MINGW AND WIN32)

include_directories( 
			${SQLITE3_INCLUDE_DIR}
			${Boost_INCLUDE_DIRS}
			${CURL_INCLUDE_DIRS}
		   )
ADD_EXECUTABLE(pichi ${PICHI_CPP})
target_link_libraries (pichi ${LINK_LIBS})

if(WIN32)
  set (BIN_DIR .)
else(WIN32)
  set (BIN_DIR bin)
endif(WIN32)

if (NOT SHARE_DIR)
  set (SHARE_DIR "share")
endif (NOT SHARE_DIR)

if (APPLE)
  set (CLIENT_DATA_DIR "${PROJECT_NAME_GLOBAL}.app")
elseif (WIN32)
  set (CLIENT_DATA_DIR ".")
else (APPLE)
  set (CLIENT_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${SHARE_DIR}/${PROJECT_NAME_GLOBAL}")
endif (APPLE)


install (TARGETS pichi DESTINATION ${BIN_DIR})
if(WIN32)
  install (FILES pichi.xml DESTINATION ${CLIENT_DATA_DIR})
else(WIN32)
  install (FILES pichi.xml DESTINATION "${CLIENT_DATA_DIR}/config")
endif(WIN32)
install (DIRECTORY languages/ DESTINATION "${CLIENT_DATA_DIR}/languages")

