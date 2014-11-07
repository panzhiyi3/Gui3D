cmake_minimum_required(VERSION 2.8)

project(Gui3D LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
find_package(Freetype REQUIRED)

set(SOURCES
    source/Common.h
    source/FontManager.cpp
    source/FontManager.h
    source/FreeTypeFont.cpp
    source/FreeTypeFont.h
    source/Gorilla.cpp
    source/Gorilla.h
    source/Gui3DButton.cpp
    source/Gui3DButton.h
    source/Gui3DCaption.cpp
    source/Gui3DCaption.h
    source/Gui3DCheckbox.cpp
    source/Gui3DCheckbox.h
    source/Gui3DCheckboxSprite.cpp
    source/Gui3DCheckboxSprite.h
    source/Gui3DCheckboxText.cpp
    source/Gui3DCheckboxText.h
    source/Gui3DClock.cpp
    source/Gui3DClock.h
    source/Gui3DCombobox.cpp
    source/Gui3DCombobox.h
    source/Gui3DConfig.h
    source/Gui3DContainer.cpp
    source/Gui3DContainer.h
    source/Gui3D.cpp
    source/Gui3DDebugPanelColors.cpp
    source/Gui3DDebugPanelColors.h
    source/Gui3DFunctorBase.h
    source/Gui3D.h
    source/Gui3DInlineSelector.cpp
    source/Gui3DInlineSelector.h
    source/Gui3DListbox.cpp
    source/Gui3DListbox.h
    source/Gui3DMemberFunction.h
    source/Gui3DMultipleElementsSelector.cpp
    source/Gui3DMultipleElementsSelector.h
    source/Gui3DPanelColors.h
    source/Gui3DPanelContainer.cpp
    source/Gui3DPanelContainer.h
    source/Gui3DPanel.cpp
    source/Gui3DPanelElementCallback.h
    source/Gui3DPanelElement.cpp
    source/Gui3DPanelElement.h
    source/Gui3DPanel.h
    source/Gui3DProgressBar.cpp
    source/Gui3DProgressBar.h
    source/Gui3DScreen.cpp
    source/Gui3DScreen.h
    source/Gui3DScreenPanel.cpp
    source/Gui3DScreenPanel.h
    source/Gui3DScrollBar.cpp
    source/Gui3DScrollBar.h
    source/Gui3DStringCursorEdit.cpp
    source/Gui3DStringCursorEdit.h
    source/Gui3DTextZone.cpp
    source/Gui3DTextZone.h
    source/Gui3DUtils.h
    source/Gui3DVerticalSelector.cpp
    source/Gui3DVerticalSelector.h
    )

option(BUILD_STATIC "Build a static library")

add_definitions(-D_GUI3D_EXPORTS)

if(BUILD_STATIC)
    add_library(Gui3D STATIC ${SOURCES})
else()
    add_library(Gui3D SHARED ${SOURCES})
endif()

target_include_directories(Gui3D PUBLIC ${OGRE_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})
