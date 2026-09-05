#include "GraficaEMULVA360X.h"
#include "MemoriaRAMEMULVA360X.h"
#include <cstring>
#include <cstdlib>

// Constructor — inicializa todo
GraficaEMULVA360X::GraficaEMULVA360X() {
    inicializada = false;
    memoria = nullptr;
    ancho_pantalla = 1280;
    alto_pantalla = 720;
    buffer_imagen = nullptr;
    estado_registros = nullptr;
}

// Destructor — libera recursos
GraficaEMULVA360X::~GraficaEMULVA360X() {
    liberarRecursos();
}

// Inicializa el GPU — basado en el procesador gráfico Xenos de Xbox 360
bool GraficaEMULVA360X::inicializar(MemoriaRAMEMULVA360X* mem) {
    memoria = mem;
    
    // Reservar buffer de pantalla — formato RGBA (4 bytes por píxel)
    tamano_buffer = ancho_pantalla * alto_pantalla * 4;
    buffer_imagen = (uint8_t*)malloc(tamano_buffer);
    if (buffer_imagen == nullptr) return false;
    memset(buffer_imagen, 0, tamano_buffer);
    
    // Inicializar registros del GPU
    estado_registros = new uint32_t[REGISTROS_CANTIDAD];
    memset(estado_registros, 0, REGISTROS_CANTIDAD * sizeof(uint32_t));
    
    // Configurar regiones de memoria de la GPU
    inicializarRegionesGrafica();
    
    inicializada = true;
    return true;
}

// Define las regiones de memoria gráfica
void GraficaEMULVA360X::inicializarRegionesGrafica() {
    // Xbox 360 Xenos GPU — regiones de memoria mapeada
    registro_base_comandos = 0x7C000000;  // Registro de comandos
    registro_base_vertices = 0x7C001000;  // Datos de vértices
    registro_base_texturas = 0x7C002000;  // Datos de texturas
    registro_base_render = 0x7C003000;   // Destino de renderizado
}

// Conectar con la memoria RAM del sistema
void GraficaEMULVA360X::conectarMemoria(MemoriaRAMEMULVA360X* mem) {
    memoria = mem;
}

// Escribir un registro del GPU
void GraficaEMULVA360X::escribirRegistro(uint32_t direccion, uint32_t valor) {
    if (!inicializada || estado_registros == nullptr) return;
    
    uint32_t indice = (direccion & 0x00000FFF) / 4;
    if (indice < REGISTROS_CANTIDAD) {
        estado_registros[indice] = valor;
        procesarRegistroCambiado(indice, valor);
    }
}

// Leer un registro del GPU
uint32_t GraficaEMULVA360X::leerRegistro(uint32_t direccion) {
    if (!inicializada || estado_registros == nullptr) return 0;
    
    uint32_t indice = (direccion & 0x00000FFF) / 4;
    if (indice < REGISTROS_CANTIDAD) {
        return estado_registros[indice];
    }
    return 0;
}

// Procesa cuando un registro cambia — aquí se ejecutan las órdenes del juego
void GraficaEMULVA360X::procesarRegistroCambiado(uint32_t indice, uint32_t valor) {
    switch (indice) {
        case REGISTRO_COMANDO:
            ejecutarComandoGrafico(valor);
            break;
        case REGISTRO_ANCHO:
            ancho_pantalla = valor;
            break;
        case REGISTRO_ALTO:
            alto_pantalla = valor;
            break;
        case REGISTRO_LIMPIAR:
            limpiarPantalla();
            break;
        case REGISTRO_DIBUJAR:
            dibujarPrimitivas();
            break;
        default:
            // Otros registros — se amplían según necesidad
            break;
    }
}

// Ejecuta un comando gráfico enviado por el juego
void GraficaEMULVA360X::ejecutarComandoGrafico(uint32_t comando) {
    switch (comando) {
        case COMANDO_INICIAR_FRAME:
            limpiarPantalla();
            break;
        case COMANDO_FIN_FRAME:
            // Listo para mostrar en pantalla
            frame_listo = true;
            break;
        case COMANDO_CARGA_TEXTURA:
            cargarTexturaDesdeMemoria();
            break;
        case COMANDO_DIBUJAR_TRIANGULOS:
            dibujarTriangulos();
            break;
        default:
            break;
    }
}

// Pone toda la pantalla de un color (limpia el frame anterior)
void GraficaEMULVA360X::limpiarPantalla() {
    if (!buffer_imagen) return;
    
    // Color de limpieza — se puede cambiar desde los registros
    uint8_t r = (color_limpieza >> 16) & 0xFF;
    uint8_t g = (color_limpieza >> 8) & 0xFF;
    uint8_t b = color_limpieza & 0xFF;
    uint8_t a = 0xFF; // Opacidad total
    
    // Llenar todo el buffer
    for (uint32_t i = 0; i < ancho_pantalla * alto_pantalla * 4; i += 4) {
        buffer_imagen[i]     = r;
        buffer_imagen[i + 1] = g;
        buffer_imagen[i + 2] = b;
        buffer_imagen[i + 3] = a;
    }
}

// Dibuja triángulos — la base de todo gráfico 3D
void GraficaEMULVA360X::dibujarTriangulos() {
    // Leer vértices desde la memoria RAM compartida
    // Cada vértice: X, Y, Z, R, G, B, A, U, V
    // Formato estándar del GPU Xenos
    
    uint32_t direccion_vertices = estado_registros[REGISTRO_DIRECCION_VERTICES];
    uint32_t cantidad = estado_registros[REGISTRO_CANTIDAD_VERTICES];
    
    // Aquí se procesan los vértices y se rasterizan al buffer
    // (Versión simplificada — se amplía con transformaciones)
    
    for (uint32_t i = 0; i < cantidad; i += 3) {
        // Leer 3 vértices = 1 triángulo
        // Proyectar a coordenadas de pantalla
        // Rasterizar al buffer_imagen
    }
}

// Carga texturas desde la memoria de la consola
void GraficaEMULVA360X::cargarTexturaDesdeMemoria() {
    uint32_t direccion = estado_registros[REGISTRO_DIR_TEXTURA];
    uint32_t ancho = estado_registros[REGISTRO_ANCHO_TEXTURA];
    uint32_t alto = estado_registros[REGISTRO_ALTO_TEXTURA];
    
    // Leer datos de textura desde la memoria RAM
    // Formatos soportados: DXT1, DXT3, DXT5 — estándar de Xbox 360
}

// Avanza al siguiente frame — lo llama el bucle principal
void GraficaEMULVA360X::actualizarFrame() {
    if (!inicializada) return;
    
    // Indicar al juego que el frame terminó
    estado_registros[REGISTRO_VBLANK] = 1; // Señal de sincronización vertical
    frame_listo = false;
}

// Devuelve el buffer de imagen para mostrar en pantalla
uint8_t* GraficaEMULVA360X::obtenerBufferPantalla() {
    return buffer_imagen;
}

// Cambiar resolución
void GraficaEMULVA360X::establecerResolucion(uint32_t ancho, uint32_t alto) {
    ancho_pantalla = ancho;
    alto_pantalla = alto;
    
    // Reasignar buffer con nuevo tamaño
    if (buffer_imagen) free(buffer_imagen);
    tamano_buffer = ancho * alto * 4;
    buffer_imagen = (uint8_t*)malloc(tamano_buffer);
}

// Liberar todo al cerrar
void GraficaEMULVA360X::liberarRecursos() {
    if (buffer_imagen) {
        free(buffer_imagen);
        buffer_imagen = nullptr;
    }
    if (estado_registros) {
        delete[] estado_registros;
        estado_registros = nullptr;
    }
    inicializada = false;
}
