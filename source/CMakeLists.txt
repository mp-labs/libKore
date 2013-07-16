cmake_minimum_required( VERSION 2.8.3 )

project( Kore C CXX )

set( Kore_LIBRARY           Kore )

set( Kore_VERSION_MAJOR     1 )
set( Kore_VERSION_MINOR     0 )
set( Kore_VERSION_BUILD     0 )

set( Kore_VERSION
     ${Kore_VERSION_MAJOR}.${Kore_VERSION_MINOR}.${Kore_VERSION_BUILD} )

set( Kore_VERSION_STRING "${Kore_VERSION} alpha" )

include_directories( src )

# -- Qt5 --
find_package( Qt5Core REQUIRED )

# -- Require Moving Pixel build --
include( libraries/mpbQt5 )

# -- Headers --
set( Kore_HDRS      # data
                    src/data/BlockExtension.hpp
                    src/data/BlockMacros.hpp
                    src/data/BlockSettings.hpp
                    src/data/LibraryT.hpp

                    # event
                    src/event/ErrorEvent.hpp
                    src/event/KoreEvent.hpp

                    # parallel
                    src/parallel/TaskletMacros.hpp
                    src/parallel/TaskletRunner.hpp

                    # plugin
                    src/plugin/ModuleMacros.hpp
                    src/plugin/PluginInterface.hpp

                    # serialization
                    src/serialization/KoreSerializer.hpp
                    src/serialization/TreeSerializer.hpp

                    # system
                    src/system/CPU.hpp

                    # Kore
                    src/KoreApplication.hpp
                    src/KoreMacros.hpp
                    src/KoreTypes.hpp )

# -- Mocable headers --
set( Kore_MOC_HDRS  # data
                    src/data/Block.hpp
                    src/data/Library.hpp
                    src/data/MetaBlock.hpp

                    # memory
                    src/memory/MemoryManager.hpp
                    src/memory/SimpleMemoryManager.hpp

                    # parallel
                    src/parallel/MetaTasklet.hpp
                    src/parallel/Tasklet.hpp

                    # plugin
                    src/plugin/Loadable.hpp
                    src/plugin/Module.hpp
                    src/plugin/PluginsManager.hpp

                    # Kore
                    src/KoreEngine.hpp
                    src/KoreModule.hpp )

# -- Mocced sources --
qt5_wrap_cpp( Kore_MOC_SRCS ${Kore_MOC_HDRS} )

# -- Inline code --
set( Kore_INLS      # data
                    src/data/Block.inl
                    src/data/Library.inl
                    src/data/LibraryT.inl )

# -- Source files --
set( Kore_SRCS      # data
                    src/data/Block.cpp
                    src/data/BlockExtension.cpp
                    src/data/Library.cpp
                    src/data/MetaBlock.cpp

                    # event
                    src/event/ErrorEvent.cpp
                    src/event/KoreEvent.cpp

                    # memory
                    src/memory/SimpleMemoryManager.cpp

                    # parallel
                    src/parallel/MetaTasklet.cpp
                    src/parallel/Tasklet.cpp
                    src/parallel/TaskletRunner.cpp

                    # plugin
                    src/plugin/Loadable.cpp
                    src/plugin/Module.cpp
                    src/plugin/PluginsManager.cpp

                    # serialization
                    src/serialization/KoreSerializer.cpp

                    # Kore
                    src/KoreApplication.cpp
                    src/KoreEngine.cpp
                    src/KoreModule.cpp )

if( UNIX )
    set( Kore_SRCS  ${Kore_SRCS}
                    src/system/CPU_GCC.cpp )
elseif( MSVC )
    set( Kore_SRCS  ${Kore_SRCS}
                    src/system/CPU_MSVC.cpp )
endif( UNIX )

# Target library: Kore
mpb_library_add( ${Kore_LIBRARY}
                 VERSION    ${Kore_VERSION_STRING}
                 SOURCES    ${Kore_SRCS}
                            ${Kore_MOC_SRCS}
                            ${Kore_HDRS}
                            ${Kore_MOC_HDRS} )

# Set include path
mpb_library_set_includes( ${Kore_LIBRARY} ${CMAKE_CURRENT_SOURCE_DIR}/src )
# Export
mpb_library_export( ${Kore_LIBRARY} )


# Use Qt5Core
qt5_use_modules( ${KORE_TARGET} Core )

# Mac OS specifics
if( APPLE )
    set_target_properties(
        ${KORE_TARGET} PROPERTIES
        COMPILE_DEFINITIONS "_K_BUILD_KORE;_K_MACX;${DEBUG_DEFINES}"
        COMPILE_DEFINITIONS "_KORE_VERSION=\"${Kore_VERSION_STRING}\""
        COMPILE_FLAGS "-fno-common -msse -W -Wall"
        VERSION "${Kore_VERSION}" )
# Windows specifics
elseif( WIN32 )
    # Visual Studio compiler specifics
    if( MSVC )
        set_target_properties(
            ${KORE_TARGET} PROPERTIES
            COMPILE_DEFINITIONS
            "_K_BUILD_KORE;_KORE_VERSION=\"${KORE_VERSION_STRING}\";"
            "_K_WIN32;${DEBUG_DEFINES}"
            VERSION ${Kore_VERSION} )
    # Others compilers specifics
    else( MSVC )
        set_target_properties(
            ${KORE_TARGET} PROPERTIES
            COMPILE_FLAGS -msse
            COMPILE_DEFINITIONS "_K_BUILD_KORE;_K_WIN32;${DEBUG_DEFINES};"
            COMPILE_DEFINITIONS "_KORE_VERSION=\"${KORE_VERSION_STRING}\""
            VERSION ${Kore_VERSION} )
    endif( MSVC )
# Linux specifics
elseif( UNIX )
    set_target_properties(
        ${KORE_TARGET} PROPERTIES
        COMPILE_FLAGS -msse
        COMPILE_DEFINITIONS "_K_BUILD_KORE;_K_UNIX;${DEBUG_DEFINES};"
        COMPILE_DEFINITIONS "_KORE_VERSION=\"${KORE_VERSION_STRING}\""
        VERSION ${Kore_VERSION} )
endif( APPLE )

# Documentation
find_package( Doxygen QUIET )
if( DOXYGEN_FOUND )
    set( DOXYGEN_OUTPUT ../doc/html )
    add_custom_command(
        OUTPUT ${DOXYGEN_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo_append "Building Kore Documentation..."
        COMMAND ${DOXYGEN_EXECUTABLE} ../doc/Doxyfile
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ../doc )
    add_custom_target( kore-doc DEPENDS ${DOXYGEN_OUTPUT} )
endif( DOXYGEN_FOUND )

# Tests
if( MPB_TESTS_ENABLED )
    add_subdirectory( test )
endif( MPB_TESTS_ENABLED )
