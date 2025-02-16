
add_definitions(-DQMAPCONTROL_LIBRARY)

include_directories(
        .
        ${Qt5Widgets_INCLUDE_DIRS}
)

file(GLOB HEADERS QMapControl/src/QMapControl/*.h)
file(GLOB SOURCES QMapControl/src/QMapControl/*.cpp)

if (NOT APPLE)
    set(GDAL_LIBRARY gdal)
else()
    set(GDAL_LIBRARY GDAL::GDAL)
endif()

#if (WIN32)
#    link_directories(${CMAKE_SOURCE_DIR}/install/msvc/lib)
#    set(GDAL_LIBRARY gdal_i)
#endif (WIN32)

set(LIBRARIES
        ${GDAL_LIBRARY}
        )

add_library(QMapControl SHARED
        ${SOURCES} ${HEADERS}
        ${UIS} ${QRCS}
        )

target_link_libraries(QMapControl
        ${LIBRARIES}
        Qt5::Widgets
        Qt5::Network
        )

target_include_directories(QMapControl
        PRIVATE
        ${GDAL_INCLUDE_DIRS}
        )

# Set target directory
install(TARGETS QMapControl
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT gui)
