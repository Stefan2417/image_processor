set(CMAKE_CXX_STANDARD 20)

add_executable(
        image_processor
        image_processor.cpp
        parser/parser.cpp parser/parser.h parser/parser.h parser/parser.cpp bmp/bmp.h bmp/bmp.cpp graphics/graphics.h graphics/graphics.cpp filters/filters.h filters/filters.cpp exceptions/exceptions.h)
