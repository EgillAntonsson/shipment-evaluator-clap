cmake_minimum_required(VERSION 3.16)
project(shipment_evaluator_clap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT APPLE)
    message(FATAL_ERROR "The embedded Cocoa GUI currently requires macOS.")
endif()

enable_language(OBJCXX)
set(CMAKE_OBJCXX_STANDARD 17)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

find_path(CLAP_INCLUDE_DIR clap/clap.h
    HINTS "$ENV{CLAP_PATH}"
    PATH_SUFFIXES include
)

if(NOT CLAP_INCLUDE_DIR)
    message(FATAL_ERROR
        "CLAP headers not found. Set CLAP_PATH to the CLAP checkout or "
        "CLAP_INCLUDE_DIR to the directory containing clap/clap.h."
    )
endif()

add_library(shipment_evaluator MODULE
    src/plugin.cpp
    src/ShipmentEvaluatorPlugin.cpp
    src/ShipmentEvaluatorGui.mm
)

target_include_directories(shipment_evaluator PRIVATE
    include
    "${CLAP_INCLUDE_DIR}"
)

set_source_files_properties(src/ShipmentEvaluatorGui.mm PROPERTIES COMPILE_FLAGS "-fobjc-arc")
target_link_libraries(shipment_evaluator PRIVATE "-framework AppKit")

set_target_properties(shipment_evaluator PROPERTIES
    OUTPUT_NAME "shipment_evaluator"
    PREFIX ""
    BUNDLE TRUE
    BUNDLE_EXTENSION "clap"
    MACOSX_BUNDLE_GUI_IDENTIFIER "rocks.egill.shipment-evaluator"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ClapInfo.plist.in"
)

add_custom_command(TARGET shipment_evaluator POST_BUILD
    COMMAND codesign --force --deep --sign - "$<TARGET_BUNDLE_DIR:shipment_evaluator>"
    VERBATIM
)