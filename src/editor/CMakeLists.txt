cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME sge_editor)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

sge_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER sge/editor)
target_link_libraries(${PROJECT_NAME} PUBLIC sge_engine imgui::imgui)
