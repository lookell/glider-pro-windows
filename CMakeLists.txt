if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-tree builds are not recommended. Create a separate build directory.")
endif()

cmake_minimum_required(VERSION 3.15)
project(gliderpro)

if(NOT WIN32)
  message(FATAL_ERROR "This program must be compiled for Windows")
endif()

# remove the annoying copyright banner when the resource compiler runs
set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /nologo")

# disable the built-in manifest generation; we provide our own
if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifest:no")
endif()

# enable utf-8 support for source files
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
endif()

add_executable(gliderpro)
set_target_properties(
  gliderpro PROPERTIES
  WIN32_EXECUTABLE TRUE
)
target_include_directories(gliderpro PRIVATE Headers miniz)
target_link_libraries(gliderpro comctl32 dsound dxguid msimg32 winmm)
target_sources(
  gliderpro PRIVATE
  # miniz source files
  miniz/miniz.h
  miniz/miniz.c
  # Glider PRO source files
  Headers/About.h
  Headers/AnimCursor.h
  Headers/Audio.h
  Headers/Banner.h
  Headers/ByteIO.h
  Headers/ColorUtils.h
  Headers/Coordinates.h
  Headers/DialogUtils.h
  Headers/DynamicMaps.h
  Headers/Dynamics.h
  Headers/Environ.h
  Headers/Events.h
  Headers/FileError.h
  Headers/FrameTimer.h
  Headers/GameOver.h
  Headers/GliderDefines.h
  Headers/GliderStructs.h
  Headers/Grease.h
  Headers/HighScores.h
  Headers/House.h
  Headers/HouseIO.h
  Headers/HouseInfo.h
  Headers/HouseLegal.h
  Headers/ImageIO.h
  Headers/Input.h
  Headers/Interactions.h
  Headers/InterfaceInit.h
  Headers/Link.h
  Headers/Macintosh.h
  Headers/MacTypes.h
  Headers/Main.h
  Headers/MainWindow.h
  Headers/Map.h
  Headers/Marquee.h
  Headers/Menu.h
  Headers/Modes.h
  Headers/Music.h
  Headers/ObjectAdd.h
  Headers/ObjectDraw.h
  Headers/ObjectDrawAll.h
  Headers/ObjectEdit.h
  Headers/ObjectInfo.h
  Headers/ObjectRects.h
  Headers/Objects.h
  Headers/Play.h
  Headers/Player.h
  Headers/Prefs.h
  Headers/RectUtils.h
  Headers/Render.h
  Headers/ResourceIDs.h
  Headers/ResourceLoader.h
  Headers/Room.h
  Headers/RoomGraphics.h
  Headers/RoomInfo.h
  Headers/RubberBands.h
  Headers/SavedGames.h
  Headers/Scoreboard.h
  Headers/Scrap.h
  Headers/SelectHouse.h
  Headers/Settings.h
  Headers/Sound.h
  Headers/StringUtils.h
  Headers/StructIO.h
  Headers/StructuresInit.h
  Headers/Tools.h
  Headers/Transit.h
  Headers/Transitions.h
  Headers/Triggers.h
  Headers/Trip.h
  Headers/Utilities.h
  Headers/WinAPI.h
  Headers/WindowUtils.h
  Resources/gliderpro.rc
  Sources/About.c
  Sources/AnimCursor.c
  Sources/Audio.c
  Sources/Banner.c
  Sources/ByteIO.c
  Sources/ColorUtils.c
  Sources/Coordinates.c
  Sources/DialogUtils.c
  Sources/DynamicMaps.c
  Sources/Dynamics.c
  Sources/Environ.c
  Sources/Events.c
  Sources/FileError.c
  Sources/FrameTimer.c
  Sources/GameOver.c
  Sources/Grease.c
  Sources/HighScores.c
  Sources/House.c
  Sources/HouseInfo.c
  Sources/HouseIO.c
  Sources/HouseLegal.c
  Sources/ImageIO.c
  Sources/Input.c
  Sources/Interactions.c
  Sources/InterfaceInit.c
  Sources/Link.c
  Sources/Macintosh.c
  Sources/Main.c
  Sources/MainWindow.c
  Sources/Map.c
  Sources/Marquee.c
  Sources/Menu.c
  Sources/Modes.c
  Sources/Music.c
  Sources/ObjectAdd.c
  Sources/ObjectDraw.c
  Sources/ObjectDrawAll.c
  Sources/ObjectEdit.c
  Sources/ObjectInfo.c
  Sources/ObjectRects.c
  Sources/Objects.c
  Sources/Play.c
  Sources/Player.c
  Sources/Prefs.c
  Sources/RectUtils.c
  Sources/Render.c
  Sources/ResourceLoader.c
  Sources/Room.c
  Sources/RoomGraphics.c
  Sources/RoomInfo.c
  Sources/RubberBands.c
  Sources/SavedGames.c
  Sources/Scoreboard.c
  Sources/Scrap.c
  Sources/SelectHouse.c
  Sources/Settings.c
  Sources/Sound.c
  Sources/StringUtils.c
  Sources/StructIO.c
  Sources/StructuresInit.c
  Sources/Tools.c
  Sources/Transit.c
  Sources/Transitions.c
  Sources/Triggers.c
  Sources/Trip.c
  Sources/Utilities.c
  Sources/WindowUtils.c
  # Generated zipped game resources
  Mermaid.dat
)

# Script that sets up the generation of the Mermaid.dat file
include("${CMAKE_SOURCE_DIR}/Resources/Mermaid.cmake")
