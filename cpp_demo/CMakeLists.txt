
include_directories(${CMAKE_CURRENT_BINARY_DIR})
LIST(APPEND LIBS ${EVENT_LIB})

#add_definitions(-g -fpermissive -std=c++11 -Wliteral-suffix)

IF(MSVC)
    include_directories(../wincompat)
    FIND_LIBRARY(PCRE_LIB pcre)
    IF(PCRE_LIB)
        MESSAGE(STATUS "Found pcre: ${PCRE_LIB}")
        LIST(APPEND LIBS ${PCRE_LIB})
    ELSE()
        MESSAGE(STATUS "pcre not found: http_server won't work")
    ENDIF()
    FIND_LIBRARY(PCREPOSIX_LIB pcreposix)
    IF(PCREPOSIX_LIB)
        MESSAGE(STATUS "Found pcreposix: ${PCREPOSIX_LIB}")
        LIST(APPEND LIBS ${PCREPOSIX_LIB})
    ELSE()
        MESSAGE(STATUS "pcreposix not found: http_server won't work")
    ENDIF()
    LIST(APPEND LIBS ws2_32)
    LIST(APPEND LIBS iphlpapi)
#  LIST(APPEND LIBS ${GETOPT_LIB})
ENDIF()

add_executable(cpp_demo main.cpp quic_com.h 
    quic_client_core.cpp quic_client_core.h
    quic_server_core.cpp quic_server_core.h
    libev/ev.c libev/ev.h
    public/log.cpp public/log.h
    public/socket_util.cpp public/socket_util.h)
set_target_properties(cpp_demo PROPERTIES LINKER_LANGUAGE CXX)
TARGET_LINK_LIBRARIES(cpp_demo ${LIBS})