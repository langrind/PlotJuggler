
include_directories( ./ ../  ../../include  ../../common)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)


#QT5_WRAP_UI ( UI_SRC  ../common/selectlistdialog.ui  )


SET( SRC
    datastream_json.cpp
    ../../include/PlotJuggler/datastreamer_base.h
    )

add_library(DataStreamJSON SHARED ${SRC} ${UI_SRC}  )
target_link_libraries(DataStreamJSON  ${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Network_LIBRARIES})

if(COMPILING_WITH_CATKIN)
    install(TARGETS DataStreamJSON
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION} )
else()
    install(TARGETS DataStreamJSON DESTINATION bin  )
endif()
