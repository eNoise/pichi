# - Try to find Boost-regex lib

FIND_LIBRARY( BOOST_REGEX_LIBRARIES ${Boost_LIB_PREFIX}boost_regex${_boost_MULTITHREADED}-${Boost_LIB_VERSION} )

if( BOOST_REGEX_LIBRARIES )
	message( STATUS "Found boost-regex: ${BOOST_REGEX_LIBRARIES}" )
	set( BOOST_REGEX_FOUND 1 )
else( BOOST_REGEX_LIBRARIES )
	message( STATUS "Could NOT find boost-regex" )
endif( BOOST_REGEX_LIBRARIES )
