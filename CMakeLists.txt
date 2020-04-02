if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-tree builds are not recommended.")
endif()

cmake_minimum_required(VERSION 3.15)
project(gliderpro)

if(NOT WIN32)
  message(FATAL_ERROR "This program must be compiled for Windows")
endif()

# remove the annoying copyright banner when the resource compiler runs
set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /nologo")

add_executable(gliderpro)
target_include_directories(gliderpro PRIVATE Headers)
target_sources(
  gliderpro PRIVATE
  # resources
  Resources/gliderpro.manifest
  Resources/gliderpro.rc
  # added headers
  Headers/ByteIO.h
  Headers/Macintosh.h
  Headers/WinAPI.h
  # added sources
  Sources/ByteIO.c
  Sources/GliderStructs.c
  Sources/Macintosh.c
  # original headers
  Headers/About.h
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
  Headers/MainWindow.h
  Headers/Map.h
  Headers/Marquee.h
  Headers/ObjectEdit.h
  Headers/Objects.h
  Headers/Play.h
  Headers/Player.h
  Headers/RectUtils.h
  Headers/Room.h
  Headers/RoomGraphics.h
  Headers/RubberBands.h
  Headers/Scoreboard.h
  Headers/Tools.h
  Headers/Utilities.h
  # original sources
  Sources/About.c
  Sources/AnimCursor.c
  Sources/AppleEvents.c
  Sources/Banner.c
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
