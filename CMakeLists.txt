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
target_include_directories(gliderpro PRIVATE Headers)
target_link_libraries(gliderpro comctl32 dsound dxguid msimg32 winmm)
target_sources(
  gliderpro PRIVATE
  Headers/About.h
  Headers/Audio.h
  Headers/ByteIO.h
  Headers/DialogUtils.h
  Headers/DynamicMaps.h
  Headers/Environ.h
  Headers/Externs.h
  Headers/GameOver.h
  Headers/GliderDefines.h
  Headers/GliderProtos.h
  Headers/GliderStructs.h
  Headers/GliderVars.h
  Headers/House.h
  Headers/Macintosh.h
  Headers/MacTypes.h
  Headers/MainWindow.h
  Headers/Map.h
  Headers/Marquee.h
  Headers/ObjectEdit.h
  Headers/Objects.h
  Headers/Play.h
  Headers/Player.h
  Headers/RectUtils.h
  Headers/ResourceIDs.h
  Headers/Room.h
  Headers/RoomGraphics.h
  Headers/RubberBands.h
  Headers/Scoreboard.h
  Headers/StructIO.h
  Headers/Tools.h
  Headers/Utilities.h
  Headers/WinAPI.h
  Resources/gliderpro.rc
  Sources/About.c
  Sources/AnimCursor.c
  Sources/AppleEvents.c
  Sources/Audio.c
  Sources/Banner.c
  Sources/ByteIO.c
  Sources/ColorUtils.c
  Sources/Coordinates.c
  Sources/DebugUtilities.c
  Sources/DialogUtils.c
  Sources/DynamicMaps.c
  Sources/Dynamics.c
  Sources/Dynamics2.c
  Sources/Dynamics3.c
  Sources/Environ.c
  Sources/Events.c
  Sources/FileError.c
  Sources/GameOver.c
  Sources/Grease.c
  Sources/HighScores.c
  Sources/House.c
  Sources/HouseInfo.c
  Sources/HouseIO.c
  Sources/HouseLegal.c
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
  Sources/ObjectDraw2.c
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
  Sources/StructuresInit2.c
  Sources/Tools.c
  Sources/Transit.c
  Sources/Transitions.c
  Sources/Triggers.c
  Sources/Trip.c
  Sources/Utilities.c
  Sources/Validate.c
  Sources/WindowUtils.c
)
