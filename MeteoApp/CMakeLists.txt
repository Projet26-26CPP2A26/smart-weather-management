cmake_minimum_required(VERSION 3.16)
project(MeteoApp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)   # compile les fichiers .ui (Qt Designer)
set(CMAKE_AUTORCC ON)   # embarque style.qss
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)
if(MSVC)
    add_compile_options(/utf-8)
endif()
add_executable(MeteoApp main.cpp mainwindow.cpp mainwindow.h mainwindow.ui
               releve_dialog.h releve_dialog.ui attestation_dialog.h
               maintenance_window.cpp maintenance_window.h maintenance_window.ui
               intervention_dialog.h intervention_dialog.ui predictive_dialog.h maintenance_db.h
               resources.qrc)
target_link_libraries(MeteoApp PRIVATE Qt6::Widgets Qt6::Sql Qt6::Charts)
