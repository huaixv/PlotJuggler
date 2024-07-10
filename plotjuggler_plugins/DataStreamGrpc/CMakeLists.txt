include_directories(../)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

QT5_WRAP_UI(UI_SRC datastream_grpc.ui)

# Find Protobuf and gRPC
option(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

find_package(gRPC CONFIG REQUIRED)

set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/perfdata.proto)

get_filename_component(perf_proto "${PROTO_FILES}" ABSOLUTE)
get_filename_component(perf_proto_path "${perf_proto}" PATH)

set(perf_proto_srcs "${perf_proto_path}/perfdata.pb.cc")
set(perf_proto_hdrs "${perf_proto_path}/perfdata.pb.h")
set(perf_grpc_srcs "${perf_proto_path}/perfdata.grpc.pb.cc")
set(perf_grpc_hdrs "${perf_proto_path}/perfdata.grpc.pb.h")

add_custom_command(
        OUTPUT "${perf_proto_srcs}" "${perf_proto_hdrs}" "${perf_grpc_srcs}" "${perf_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${perf_proto_path}"
        --cpp_out "${perf_proto_path}"
        -I "${perf_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${perf_proto}"
        DEPENDS "${perf_proto}")

include_directories("${perf_proto_path}")

set(SRC datastream_grpc.cpp
        ${perf_grpc_srcs}
        ${perf_grpc_hdrs}
        ${perf_proto_srcs}
        ${perf_proto_hdrs})

add_library(DataStreamGrpc SHARED ${SRC} ${UI_SRC})

target_link_libraries(DataStreamGrpc
        PRIVATE ${Protobuf_LIBRARIES} ${_GRPC_GRPCPP}
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Xml_LIBRARIES}
        plotjuggler_base)

install(TARGETS DataStreamGrpc DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY})
