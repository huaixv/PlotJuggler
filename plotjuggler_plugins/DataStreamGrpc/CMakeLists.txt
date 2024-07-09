
include_directories( ../ )

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

SET( SRC datastream_grpc.cpp)

add_library(DataStreamGrpc SHARED ${SRC} ${UI_SRC}  )

target_link_libraries(DataStreamGrpc
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    plotjuggler_base)

install(TARGETS DataStreamGrpc DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY}  )
