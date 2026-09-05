cmake_minimum_required(VERSION 3.22.1)
project(EMULVA360X LANGUAGES CXX C)

# Configuración estándar para Android
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Flags de compilación para ARM64 (celulares modernos)
if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -fPIC")
endif()

# Archivos fuente — TODOS los que creamos, SOLO TUYOS
set(SOURCES
    # Núcleo principal
    ProcesadorEMULVA360X.cpp
    MemoriaRAMEMULVA360X.cpp
    GraficaEMULVA360X.cpp
    ControlesEMULVA360X.cpp
    GestorJuegosEMULVA360X.cpp
    GestorCarpetasEMULVA360X.cpp
    InterfazUsuarioEMULVA360X.cpp
    SistemaGuardadosEMULVA360X.cpp
    OptimizacionesRendimientoEMULVA360X.cpp
    ModuloFirmwareEMULVA360X.cpp
)

# Archivos de cabecera
set(HEADERS
    ProcesadorEMULVA360X.h
    MemoriaRAMEMULVA360X.h
    GraficaEMULVA360X.h
    ControlesEMULVA360X.h
    GestorJuegosEMULVA360X.h
    GestorCarpetasEMULVA360X.h
    InterfazUsuarioEMULVA360X.h
    SistemaGuardadosEMULVA360X.h
    OptimizacionesRendimientoEMULVA360X.h
    ModuloFirmwareEMULVA360X.h
)

# Crear librería compartida para Android
add_library(emulva360x SHARED ${SOURCES} ${HEADERS})

# Incluir carpeta actual para encontrar los .h
target_include_directories(emulva360x PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Librerías estándar de Android
find_library(log-lib log)
find_library(android-lib android)
target_link_libraries(emulva360x ${log-lib} ${android-lib})
