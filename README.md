# EMULVA360X

EMULVA360X - Emulador de Xbox 360 para Android, desarrollado en C++.
Compatible solo con formatos ISO y GOD (backups legales requeridos). Proyecto de código abierto enfocado en eficiencia y simplicidad.

## 📁 Archivos del núcleo
Todos los módulos desarrollados desde cero para esta versión:

- **ModuloFirmwareEMULVA360X.cpp** — Inicialización del sistema y emulación de firmware
- **ProcesadorEMULVA360X.cpp** — Emulación del procesador PowerPC
- **MemoriaRAMEMULVA360X.cpp** — Gestión de memoria 512MB + Big-Endian
- **GraficaEMULVA360X.cpp** — Emulación del GPU Xenos y renderizado
- **ControlesEMULVA360X.cpp** — Mandos, controles táctiles y vibración
- **GestorJuegosEMULVA360X.cpp** — Carga y detección de formatos ISO y GOD
- **GestorCarpetasEMULVA360X.cpp** — Estructura de carpetas y organización de archivos
- **InterfazUsuarioEMULVA360X.cpp** — Menús, pantallas y navegación
- **SistemaGuardadosEMULVA360X.cpp** — Partidas guardadas y estados rápidos
- **OptimizacionesRendimientoEMULVA360X.cpp** — Cachés, FPS y ajustes de rendimiento

## 📦 Compilación
- Plataforma objetivo: Android (ARM64-v8a)
- Lenguaje: C++20
- Sistema de compilación: CMakeLists.txt (incluido)

## 📋 Características
- ✅ Formatos soportados: .ISO, .GOD
- ✅ Controles táctiles integrados
- ✅ Hasta 4 mandos simultáneos
- ✅ Estados rápidos y partidas guardadas
- ✅ Modos de rendimiento: Máximo / Equilibrado / Ahorro de batería
- ✅ Interfaz en español
- ![EMULVA360X Logo](https://github.com/luismoli1811-star/EMULVA360X/blob/main/logo.jpg)
- 
