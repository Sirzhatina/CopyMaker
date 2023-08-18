set(TARGET CopyMaker)

set(SOURCES 
    main.cpp
    Compress.hpp
    CopyAccomplisher.hpp 
    PathsParser.hpp)

add_executable(${TARGET} ${SOURCES})