cmake_minimum_required (VERSION 2.6)
PROJECT(pichi)
set (PROJECT_NAME_GLOBAL pichi)
set (VERSION "0.7.0dev")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (CheckIncludeFile)
#include (CTest)

if(WIN32)
  set(PROJECT_BINARY ${PROJECT_NAME_GLOBAL}.exe)
else(WIN32)
  set(PROJECT_BINARY ${PROJECT_NAME_GLOBAL})
endif(WIN32)

execute_process(COMMAND ${PROJECT_SOURCE_DIR}/getrevision.sh
		OUTPUT_VARIABLE GIT_REVISION 
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
execute_process(COMMAND git describe
		OUTPUT_VARIABLE GIT_DESCRIBE 
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
add_definitions(-DGIT_REVISION="${GIT_REVISION}")
add_definitions(-DGIT_DESCRIBE="${GIT_DESCRIBE}")
add_definitions(-DGIT_VERSION="${VERSION}")

if (NOT MSVC)
    #enable c++0X
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    
    #enable win32 defs
    if (WIN32)
      add_definitions(-DWIN32 -D_WIN32)
    endif (WIN32)

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
option (LOCAL_LUA "Use local lua lib" OFF)
option (WITH_LUA "Lua scripting" ON)
option (WITH_TESTS "Enable testing" ON)
if (WITH_TESTS)
   enable_testing()
   add_definitions(-DWITH_TESTS)
endif(WITH_TESTS)

# iteration macro
macro(get_test_num_postfix test)
    if(testnum_${test})
      math(EXPR testnum ${testnum_${test}}+1)
    else(testnum_${test})
      set(testnum 1)
    endif(testnum_${test})
    set(testnum_${test} ${testnum})
    
    if(${testnum} GREATER 1)
	set(testpostfix _${testnum})
    else(${testnum} GREATER 1)
	set(testpostfix "")
    endif(${testnum} GREATER 1)
endmacro(get_test_num_postfix)

# Tests macro
macro(do_test test)
  set(argcount 0)
  foreach(arg ${ARGN})
     math(EXPR argcount ${argcount}+1)
     if(argcount EQUAL 1)
      set(runarg ${arg})
     endif(argcount EQUAL 1)
     if(argcount EQUAL 2)
      set(result ${arg})
     endif(argcount EQUAL 2)
  endforeach(arg ${ARGN})

  get_test_num_postfix(${test})
  if(runarg)
    add_test(pichitest_${test}${testpostfix} ${PROJECT_BINARY} -t ${test} ${runarg})
  else(runarg)
    add_test(pichitest_${test}${testpostfix} ${PROJECT_BINARY} -t ${test})
  endif(runarg)
  if(result)
    set_tests_properties(pichitest_${test}${testpostfix} PROPERTIES PASS_REGULAR_EXPRESSION ${result})
  endif(result)
endmacro(do_test)
macro(do_test_fail test)
  set(argcount 0)
  foreach(arg ${ARGN})
     math(EXPR argcount ${argcount}+1)
     if(argcount EQUAL 1)
      set(runarg ${arg})
     endif(argcount EQUAL 1)
     if(argcount EQUAL 2)
      set(result ${arg})
     endif(argcount EQUAL 2)
  endforeach(arg ${ARGN})

  get_test_num_postfix(${test})
  if(runarg)
    add_test(pichitest_${test}${testpostfix} ${PROJECT_BINARY} -t ${test} ${runarg})
  else(runarg)
    add_test(pichitest_${test}${testpostfix} ${PROJECT_BINARY} -t ${test})
  endif(runarg)
  if(result)
    set_tests_properties(pichitest_${test}${testpostfix} PROPERTIES PASS_REGULAR_EXPRESSION ${result})
  endif(result)
  set_tests_properties(pichitest_${test}${testpostfix} PROPERTIES WILL_FAIL TRUE)
endmacro(do_test_fail)


CHECK_INCLUDE_FILE (time.h HAVE_TIME_H)
CHECK_INCLUDE_FILE (dirent.h HAVE_DIRENT_H)
CHECK_INCLUDE_FILE (sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE (sys/stat.h HAVE_SYS_STAT_H)
if(WIN32)
	CHECK_INCLUDE_FILE (direct.h HAVE_DIRECT_H)
endif(WIN32)

find_package (Threads REQUIRED)
find_package (Sqlite3 REQUIRED)
if(WIN32)
    find_package (Boost REQUIRED COMPONENTS regex-gcc46 program_options-gcc46)
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

if (WITH_LUA)
  if (LOCAL_LUA)
      add_subdirectory(luasrc)
      include_directories ("${PROJECT_SOURCE_DIR}/luasrc")
      set (LUA_LIBRARIES "lua")
  else (LOCAL_LUA)
      find_package(Lua51 REQUIRED)
      include_directories(${LUA_INCLUDE_DIR})
  endif (LOCAL_LUA)
  add_definitions(-DWITH_LUA)
endif (WITH_LUA)


set(PICHI_CPP 	src/tests.cpp   
		src/pichiexception.cpp	
		src/pichievent.cpp
		src/log.cpp
		src/pichicurl.cpp
		src/lexemebuilder.cpp
		src/languages.cpp    
		src/xmlsimple.cpp 
		src/commandbase.cpp
		src/pichioptions.cpp
		src/pichimessage.cpp
		src/commandhandler.cpp
		src/pichiconfig.cpp
		src/helper.cpp
		src/pichi.cpp
		src/sqlobject.cpp
		src/sqlite.cpp
		src/pichidbpatcher.cpp
		src/pichicore.cpp
		src/luamanager.cpp
		src/luamap.cpp
		src/luapichi.cpp
		
		src/main.cpp
)

set(LINK_LIBS	${CMAKE_THREAD_LIBS_INIT}
		${GLOOX_LIBRARIES}
		${TINYXML_LIBRARIES}
		${SQLITE3_LIBRARIES}
		${Boost_LIBRARIES}
		${CURL_LIBRARIES}
)
if (WITH_LUA)
	set(LINK_LIBS ${LINK_LIBS} ${LUA_LIBRARIES})
endif (WITH_LUA)

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
add_definitions(-DPICHI_INSTALLED_DIR="${CLIENT_DATA_DIR}/")


install (TARGETS pichi DESTINATION ${BIN_DIR})
if(WIN32)
  install (FILES pichi.xml DESTINATION ${CLIENT_DATA_DIR})
else(WIN32)
  install (FILES pichi.xml DESTINATION "${CLIENT_DATA_DIR}/config")
endif(WIN32)
install (DIRECTORY languages/ DESTINATION "${CLIENT_DATA_DIR}/languages")
install (DIRECTORY lua/ DESTINATION "${CLIENT_DATA_DIR}/lua")

if(WITH_TESTS)
  do_test(regexp_split)

  do_test(helper_explode)

  do_test(helper_implode)

  do_test(helper_splitbysize)

  # Testing non-exist file
  do_test_fail(helper_file_exist "dsdfhsdfkjsf")
  # Testing existed file
  do_test(helper_file_exist "${PROJECT_SOURCE_DIR}/pichi.xml")
  # Testing non-exist dir
  do_test_fail(helper_file_exist "/var/www/asasdsads")
  # Testing existed dir
  do_test(helper_file_exist "${PROJECT_SOURCE_DIR}")

  do_test(helper_microtime)

  do_test(helper_createdirectory)

  do_test(helper_removedirectory)
endif(WITH_TESTS)