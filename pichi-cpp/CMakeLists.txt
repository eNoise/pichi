cmake_minimum_required (VERSION 2.6)
PROJECT(pichi)

set (VERSION "0.6.0")

LINK_LIBRARIES(gloox)
LINK_LIBRARIES(pthread)
LINK_LIBRARIES(sqlite3)
LINK_LIBRARIES(tinyxml)
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

