#include "SistemaGuardadosEMULVA360X.h"
#include "GestorCarpetasEMULVA360X.h"
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
#include <time.h>

// Constructor — inicializa todo
SistemaGuardadosEMULVA360X::SistemaGuardadosEMULVA360X() {
    inicializada = false;
    gestor_carpetas = nullptr;
    juego_activo_id = 0;
    cantidad_guardados = 0;
    memset(perfil_activo.nombre, 0, sizeof(perfil_activo.nombre));
    perfil_activo.id = 0;
}

// Conectar el gestor de carpetas
void SistemaGuardadosEMULVA360X::conectarGestorCarpetas(GestorCarpetasEMULVA360X* gc) {
    gestor_carpetas = gc;
}

// Inicializar el sistema
bool SistemaGuardadosEMULVA360X::inicializar() {
    if (!gestor_carpetas) return false;
    
    // Crear carpeta de guardados si no existe
    const char* ruta_guardados = gestor_carpetas->obtenerRutaGuardados();
    crearCarpetaSiNoExiste(ruta_guardados);
    
    // Cargar perfil por defecto
    strcpy(perfil_activo.nombre, "Jugador");
    perfil_activo.id = 1;
    
    // Escaneo inicial de guardados
    escanearGuardados();
    
    inicializada = true;
    return true;
}

// — GESTIÓN DE PERFILES —

void SistemaGuardadosEMULVA360X::cambiarPerfil(uint32_t id_perfil, const char* nombre) {
    perfil_activo.id = id_perfil;
    strncpy(perfil_activo.nombre, nombre, sizeof(perfil_activo.nombre) - 1);
    perfil_activo.nombre[sizeof(perfil_activo.nombre) - 1] = 0;
}

PerfilUsuario SistemaGuardadosEMULVA360X::obtenerPerfilActivo() {
    return perfil_activo;
}

// — FORMATO DE ARCHIVO DE GUARDADO —

// Cabecera del archivo .sav — compatible con Xbox 360
struct CabeceraGuardado {
    char firma[4];      // "XSAV" → identificación del formato
    uint32_t version;   // Versión del formato
    uint32_t id_juego;  // ID del juego que creó el guardado
    uint32_t id_perfil; // Perfil del jugador
    char nombre_juego[32]; // Nombre del juego
    char descripcion[64];  // Descripción del guardado
    uint64_t tamano_datos; // Tamaño de los datos del juego
    uint64_t fecha_creacion; // Timestamp Unix
    uint64_t fecha_ultima_modificacion;
    uint8_t icono[256]; // Miniatura del guardado (16x16 RGBA)
};

// — CREAR RUTA DE ARCHIVO DE GUARDADO —

void SistemaGuardadosEMULVA360X::construirRutaGuardado(char* ruta, size_t tamano_ruta, uint32_t id_juego, uint32_t ranura) {
    const char* base = gestor_carpetas->obtenerRutaGuardados();
    snprintf(ruta, tamano_ruta, "%s%08X_%d.sav", base, id_juego, ranura);
}

// — GUARDAR PARTIDA —

bool SistemaGuardadosEMULVA360X::guardarPartida(uint32_t id_juego, uint32_t ranura, 
                                                   const char* nombre_juego, 
                                                   const char* descripcion,
                                                   void* datos, uint64_t tamano_datos) {
    if (!inicializada || !gestor_carpetas) return false;
    
    char ruta[512];
    construirRutaGuardado(ruta, sizeof(ruta), id_juego, ranura);
    
    FILE* archivo = fopen(ruta, "wb");
    if (!archivo) return false;
    
    // Escribir cabecera
    CabeceraGuardado cabecera;
    memset(&cabecera, 0, sizeof(cabecera));
    
    cabecera.firma[0] = 'X';
    cabecera.firma[1] = 'S';
    cabecera.firma[2] = 'A';
    cabecera.firma[3] = 'V';
    cabecera.version = 1;
    cabecera.id_juego = id_juego;
    cabecera.id_perfil = perfil_activo.id;
    strncpy(cabecera.nombre_juego, nombre_juego, sizeof(cabecera.nombre_juego) - 1);
    strncpy(cabecera.descripcion, descripcion, sizeof(cabecera.descripcion) - 1);
    cabecera.tamano_datos = tamano_datos;
    cabecera.fecha_creacion = time(nullptr);
    cabecera.fecha_ultima_modificacion = cabecera.fecha_creacion;
    
    // Escribir cabecera
    fwrite(&cabecera, sizeof(cabecera), 1, archivo);
    
    // Escribir datos del juego
    if (datos != nullptr && tamano_datos > 0) {
        fwrite(datos, 1, tamano_datos, archivo);
    }
    
    fclose(archivo);
    
    // Actualizar lista
    escanearGuardados();
    return true;
}

// — CARGAR PARTIDA —

bool SistemaGuardadosEMULVA360X::cargarPartida(uint32_t id_juego, uint32_t ranura,
                                                 void* buffer_salida, 
                                                 uint64_t tamano_buffer,
                                                 InfoGuardado* info) {
    if (!inicializada || !gestor_carpetas) return false;
    
    char ruta[512];
    construirRutaGuardado(ruta, sizeof(ruta), id_juego, ranura);
    
    FILE* archivo = fopen(ruta, "rb");
    if (!archivo) return false;
    
    // Leer y validar cabecera
    CabeceraGuardado cabecera;
    if (fread(&cabecera, sizeof(cabecera), 1, archivo) != 1) {
        fclose(archivo);
        return false;
    }
    
    // Verificar firma
    if (cabecera.firma[0] != 'X' || cabecera.firma[1] != 'S' ||
        cabecera.firma[2] != 'A' || cabecera.firma[3] != 'V') {
        fclose(archivo);
        return false;
    }
    
    // Devolver información si se pide
    if (info != nullptr) {
        info->id_juego = cabecera.id_juego;
        info->id_perfil = cabecera.id_perfil;
        strncpy(info->nombre_juego, cabecera.nombre_juego, sizeof(info->nombre_juego) - 1);
        strncpy(info->descripcion, cabecera.descripcion, sizeof(info->descripcion) - 1);
        info->tamano_datos = cabecera.tamano_datos;
        info->fecha_creacion = cabecera.fecha_creacion;
        info->fecha_modificacion = cabecera.fecha_ultima_modificacion;
        info->ranura = ranura;
    }
    
    // Leer datos del juego
    if (buffer_salida != nullptr && cabecera.tamano_datos > 0) {
        uint64_t leer = (tamano_buffer < cabecera.tamano_datos) ? tamano_buffer : cabecera.tamano_datos;
        fread(buffer_salida, 1, leer, archivo);
    }
    
    fclose(archivo);
    return true;
}

// — ESTADO RÁPIDO (guardado instantáneo) —

bool SistemaGuardadosEMULVA360X::guardarEstadoRapido(uint32_t id_juego, void* datos, uint64_t tamano) {
    // Ranura 0 = estado rápido especial
    return guardarPartida(id_juego, 0, "Estado Rápido", "Guardado automático — Estado completo de la consola", datos, tamano);
}

bool SistemaGuardadosEMULVA360X::cargarEstadoRapido(uint32_t id_juego, void* buffer, uint64_t tamano) {
    InfoGuardado info;
    if (!cargarPartida(id_juego, 0, buffer, tamano, &info)) return false;
    return true;
}

// — LISTAR GUARDADOS —

std::vector<InfoGuardado> SistemaGuardadosEMULVA360X::obtenerListaGuardados(uint32_t id_juego) {
    std::vector<InfoGuardado> lista;
    const char* ruta = gestor_carpetas->obtenerRutaGuardados();
    
    // Escanear ranuras del 1 al 
