PROJECT(pichi)
LINK_LIBRARIES(gloox)
LINK_LIBRARIES(pthread)
LINK_LIBRARIES(sqlite3)
LINK_LIBRARIES(tinyxml)
LINK_LIBRARIES(boost_regex)
ADD_EXECUTABLE(pichi src/commandbase.cpp src/pichioptions.cpp src/lastmessage.cpp src/commandhandler.cpp src/pichiconfig.cpp src/system.cpp src/core.cpp src/sqlite.cpp src/pichicore.cpp main.cpp)

