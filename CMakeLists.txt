cmake_minimum_required(VERSION 3.20)
project(AudioVisualizador)

set(CMAKE_CXX_STANDARD 17)
set(IMGUI_SOURCES
libs/imgui-master/imgui-master/imgui.cpp
libs/imgui-master/imgui-master/imgui_draw.cpp
libs/imgui-master/imgui-master/imgui_widgets.cpp
libs/imgui-master/imgui-master/imgui_tables.cpp
libs/imgui-master/imgui-master/imgui_impl_glfw.cpp
libs/imgui-master/imgui-master/imgui_impl_opengl3.cpp
)
set(IMPLOT_SOURCES
        libs/implot-master/implot-master/implot.cpp
        libs/implot-master/implot-master/implot_items.cpp
)

find_package(SndFile CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Prueba_audio
        main.cpp
        ${IMGUI_SOURCES}
        ${IMPLOT_SOURCES}
        Pista_audio.cpp
        Arbol_picos.cpp
        Procesador_audio.cpp
        Editor_logica.cpp
        kissfft/kiss_fft.c
        kissfft/kiss_fftr.c
        Visualizador_audio.cpp
)

target_include_directories(Prueba_audio PRIVATE
        libs/imgui-master/imgui-master
        libs/implot-master/implot-master
        kissfft)

target_link_libraries(Prueba_audio PRIVATE
        glfw
        OpenGL::GL
        SndFile::sndfile
)