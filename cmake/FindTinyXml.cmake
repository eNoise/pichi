# - Try to find TinyXML

FIND_PATH( TINYXML_INCLUDE_DIR "tinyxml.h" )
FIND_LIBRARY( TINYXML_LIBRARIES tinyxml )

if( TINYXML_LIBRARIES AND TINYXML_INCLUDE_DIR )
	message( STATUS "Found tinyxml: ${TINYXML_LIBRARIES}" )
	set( TINYXML_FOUND 1 )
else( TINYXML_LIBRARIES AND TINYXML_INCLUDE_DIR )
	message( STATUS "Could NOT find tinyxml" )
endif( TINYXML_LIBRARIES AND TINYXML_INCLUDE_DIR )
