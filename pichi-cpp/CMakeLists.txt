cmake_minimum_required (VERSION 2.6)
PROJECT(pichi)
set (VERSION "0.6.0")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (CheckIncludeFile)

option (LOCAL_GLOOX "Use local gloox lib" OFF)
option (LOCAL_TINYXML "Use local tinyxml lib" OFF)

CHECK_INCLUDE_FILE (time.h HAVE_TIME_H)
CHECK_INCLUDE_FILE (sys/stat.h HAVE_SYS_STAT_H)

find_package (Sqlite3 REQUIRED)
find_package (Boost REQUIRED)
#find_package (Pthread REQUIRED)

#if (LOCAL_GLOOX)
#  add_subdirectory(gloox)
#  set (GLOOX_INCLUDE_DIR "./")
#  set (GLOOX_LIBRARIES "gloox")
#else (LOCAL_GLOOX)
  find_package (Gloox REQUIRED)
#endif (LOCAL_GLOOX)

if (LOCAL_TINYXML)
  add_subdirectory(tinyxml)
  include_directories ("${PROJECT_SOURCE_DIR}/tinyxml")
  set (TINYXML_LIBRARIES "tinyxml")
else (LOCAL_TINYXML)
  find_package (TinyXml REQUIRED)
endif (LOCAL_TINYXML)

LINK_LIBRARIES(pthread)
LINK_LIBRARIES(boost_regex)

ADD_EXECUTABLE(pichi 	src/log.cpp
			src/lexemes.cpp
			src/languages.cpp    
			src/xmlsimple.cpp 
			src/commandbase.cpp
			src/pichioptions.cpp
			src/lastmessage.cpp
			src/commandhandler.cpp
			src/pichiconfig.cpp
			src/system.cpp
			src/core.cpp
			src/sqlite.cpp
			src/pichicore.cpp
			main.cpp)
target_link_libraries (pichi ${GLOOX_LIBRARIES} ${TINYXML_LIBRARIES} ${SQLITE3_LIBRARIES})
