project(core VERSION 0.1 LANGUAGES CXX)


file(GLOB PROJECT_SOURCES "*.cpp" "*.h" "*.hpp" "*.cc" "*.c" "*.ui" "data/*.cpp" "data/*.h" "data/*.hpp" "data/*.cc" "data/*.c" "data/*.ui" "plots/*.cpp" "plots/*.h" "plots/*.hpp" "plots/*.cc" "plots/*.c" "plots/*.ui")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
qt_add_library(core
MANUAL_FINALIZATION
${PROJECT_SOURCES}
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET core APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
if(ANDROID)
add_library(core SHARED
${PROJECT_SOURCES}
)
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else()
add_library(core
${PROJECT_SOURCES}
)
endif()
endif()

target_link_libraries(core PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# target_link_libraries(core PRIVATE core)
target_link_libraries(core PRIVATE qcustomplot) 

set_target_properties(core PROPERTIES
MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

target_sources(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/plots/graph_settings_model.h)
target_include_directories(core INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/core_autogen/include)

if(QT_VERSION_MAJOR EQUAL 6)
qt_finalize_target(core)
endif()
