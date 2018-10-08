
if (NOT WIN32)
    message(FATAL "platform_win.cmake must be included only on Win32 interface")
endif()


if (NOT BOOST_ROOT)
    set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/install/msvc)
endif (NOT BOOST_ROOT)

add_definitions(-D_WINSOCKAPI_)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_HAS_ITERATOR_DEBUGGING=1 -D_ITERATOR_DEBUG_LEVEL=2 -DDEBUG)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")     # Visual studio
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
