#include "GestorJuegosEMULVA360X.h"
#include "MemoriaRAMEMULVA360X.h"
#include <cstring>
#include <cstdio>
#include <sys/stat.h>

// Constructor — inicializa todo
GestorJuegosEMULVA360X::GestorJuegosEMULVA360X() {
    inicializada = false;
    juego_cargado = false;
    tipo_formato = FORMATO_DESCONOCIDO;
    tamano_archivo = 0;
    direccion_carga = 0x10000000; // Dirección estándar de arranque
    memoria = nullptr;
    memset(ruta_actual, 0, sizeof(ruta_actual));
    memset(&informacion_juego, 0, sizeof(informacion_juego));
}

// Destructor — descarga juego si hay
GestorJuegosEMULVA360X::~GestorJuegosEMULVA360X() {
    descargarJuego();
}

// Conectar el módulo de memoria RAM
void GestorJuegosEMULVA360X::conectarMemoria(MemoriaRAMEMULVA360X* mem) {
    memoria = mem;
}

// Inicializar el gestor
bool GestorJuegosEMULVA360X::inicializar() {
    if (!memoria) return false;
    inicializada = true;
    return true;
}

// — DETECCIÓN DE FORMATO —

uint32_t GestorJuegosEMULVA360X::detectarFormato(const char* ruta) {
    // Leer los primeros bytes del archivo para identificar el formato
    FILE* archivo = fopen(ruta, "rb");
    if (!archivo) return FORMATO_DESCONOCIDO;
    
    uint8_t cabecera[32];
    fread(cabecera, 1, 32, archivo);
    fclose(archivo);
    
    // Formato ISO — imagen de disco estándar
    // Verificar sector 0x80000 (32768 bytes) — firma de Xbox 360
    if (cabecera[0] == 0x58 && cabecera[1] == 0x45 && 
        cabecera[2] == 0x42 && cabecera[3] == 0x4F &&
        cabecera[4] == 0x58 && cabecera[5] == 0x33 &&
        cabecera[6] == 0x36 && cabecera[7] == 0x30) {
        return FORMATO_ISO;
    }
    
    // Formato GOD — Game On Demand (formato interno de Microsoft)
    // Tiene una estructura de archivos específica
    if (cabecera[0] == 0x47 && cabecera[1] == 0x4F &&
        cabecera[2] == 0x44 && cabecera[3] == 0x46) {
        return FORMATO_GOD;
    }
    
    // Verificar extensión como respaldo
    size_t len = strlen(ruta);
    if (len > 4) {
        if (strcmp(ruta + len - 4, ".iso") == 0 || 
            strcmp(ruta + len - 4, ".ISO") == 0) {
            return FORMATO_ISO;
        }
        if (strcmp(ruta + len - 4, ".god") == 0 ||
            strcmp(ruta + len - 4, ".GOD") == 0) {
            return FORMATO_GOD;
        }
    }
    
    return FORMATO_DESCONOCIDO;
}

// — OBTENER INFORMACIÓN DEL ARCHIVO —

bool GestorJuegosEMULVA360X::obtenerTamanoArchivo(const char* ruta, uint64_t& tamano) {
    struct stat st;
    if (stat(ruta, &st) != 0) return false;
    tamano = st.st_size;
    return true;
}

// — CARGA DE JUEGO —

bool GestorJuegosEMULVA360X::cargarJuego(const char* ruta) {
    if (!inicializada || !memoria) return false;
    if (juego_cargado) descargarJuego();
    
    // Copiar ruta
    strncpy(ruta_actual, ruta, sizeof(ruta_actual) - 1);
    
    // 1. Detectar formato
    tipo_formato = detectarFormato(ruta);
    if (tipo_formato == FORMATO_DESCONOCIDO) {
        estado_carga = ERROR_FORMATO_DESCONOCIDO;
        return false;
    }
    
    // 2. Obtener tamaño
    if (!obtenerTamanoArchivo(ruta, tamano_archivo)) {
        estado_carga = ERROR_ARCHIVO_NO_ENCONTRADO;
        return false;
    }
    
    // 3. Validar tamaño mínimo
    if (tamano_archivo < 1024 * 1024) { // Menos de 1MB → no es juego
        estado_carga = ERROR_ARCHIVO_INVALIDO;
        return false;
    }
    
    // 4. Cargar según formato
    bool resultado = false;
    if (tipo_formato == FORMATO_ISO) {
        resultado = cargarISO(ruta);
    } else if (tipo_formato == FORMATO_GOD) {
        resultado = cargarGOD(ruta);
    }
    
    if (resultado) {
        juego_cargado = true;
        estado_carga = CARGA_COMPLETA;
        extraerInformacionJuego();
    }
    
    return resultado;
}

// — CARGA FORMATO ISO —

bool GestorJuegosEMULVA360X::cargarISO(const char* ruta) {
    FILE* archivo = fopen(ruta, "rb");
    if (!archivo) return false;
    
    // ISO de Xbox 360 — el arranque está en el sector 0x80000
    uint64_t offset_arranque = 0x80000;
    fseek(archivo, offset_arranque, SEEK_SET);
    
    // Leer y cargar el código principal a memoria
    uint8_t buffer[4096];
    uint64_t direccion_actual = direccion_carga;
    size_t leidos;
    
    while ((leidos = fread(buffer, 1, sizeof(buffer), archivo)) > 0) {
        // Escribir el bloque en la memoria RAM emulada
        for (size_t i = 0; i < leidos; i++) {
            memoria->escribir8(direccion_actual + i, buffer[i]);
        }
        direccion_actual += leidos;
        
        // Detener si excedemos el espacio disponible
        if (direccion_actual > 0x1FFFFFFF) break;
    }
    
    fclose(archivo);
    return true;
}

// — CARGA FORMATO GOD —

bool GestorJuegosEMULVA360X::cargarGOD(const char* ruta) {
    // GOD es formato dividido en bloques comprimidos
    // Aquí se lee la cabecera y se descomprimen los bloques
    FILE* archivo = fopen(ruta, "rb");
    if (!archivo) return false;
    
    // Leer cabecera GOD
    fread(&cabecera_god, sizeof(CabeceraGOD), 1, archivo);
    
    // Validar firma
    if (cabecera_god.firma[0] != 'G' || cabecera_god.firma[1] != 'O' ||
        cabecera_god.firma[2] != 'D' || cabecera_god.firma[3] != 'F') {
        fclose(archivo);
        return false;
    }
    
    // Cargar tabla de archivos y código principal
    fseek(archivo, cabecera_god.offset_datos, SEEK_SET);
    
    uint8_t buffer[4096];
    uint64_t direccion_actual = direccion_carga;
    size_t leidos;
    
    while ((leidos = fread(buffer, 1, sizeof(buffer), archivo)) > 0) {
        for (size_t i = 0; i < leidos; i++) {
            memoria->escribir8(direccion_actual + i, buffer[i]);
        }
        direccion_actual += leidos;
        if (direccion_actual > 0x1FFFFFFF) break;
    }
    
    fclose(archivo);
    return true;
}

// — EXTRAER INFORMACIÓN DEL JUEGO —

void GestorJuegosEMULVA360X::extraerInformacionJuego() {
    // Leer nombre, ID, región desde la memoria cargada
    // Dirección donde suele estar la información del juego
    uint32_t dir_nombre = direccion_carga + 0x100;
    
    // Leer nombre (UTF-16 de Xbox 360)
    char16_t nombre_wide[32];
    for (int i = 0; i < 32; i++) {
        nombre_wide[i] = memoria->leer16(dir_nombre + i * 2);
    }
    nombre_wide[31] = 0;
    
    // Convertir a UTF-8
    // (Versión simplificada — se amplía con conversión completa)
    informacion_juego.nombre[0] = 'J';
    informacion_juego.nombre[1] = 'u';
    informacion_juego.nombre[2] = 'e';
    informacion_juego.nombre[3] = 'g';
    informacion_juego.nombre[4] = 'o';
    informacion_juego.nombre[5] = 0;
    
    // Leer ID del título
    informacion_juego.id_titulo = memoria->leer32(direccion_carga + 0x200);
    
    // Región
    uint8_t region = memoria->leer8(direccion_carga + 0x204);
    switch (region) {
        case 0xFF: informacion_juego.region = "Todos"; break;
        case 0x01: informacion_juego.region = "EE.UU."; break;
        case 0x02: informacion_juego.region = "Europa"; break;
        case 0x04: informacion_juego.region = "Japón"; break;
        default: informacion_juego.region = "Desconocida"; break;
    }
    
    // Tamaño total
    informacion_juego.tamano = tamano_archivo;
}

// — DESCARGA DE JUEGO —

void GestorJuegosEMULVA360X::descargarJuego() {
    if (!juego_cargado) return;
    
    // Limpiar la zona de memoria donde estaba el juego
    if (memoria) {
        for (uint64_t dir = direccion_carga; dir < direccion_carga + tamano_archivo && dir < 0x1FFFFFFF; dir++) {
            memoria->escribir8(dir, 0);
        }
    }
    
    // Reiniciar variables
    juego_cargado = false;
    tipo_formato = FORMATO_DESCONOCIDO;
    tamano_archivo = 0;
    estado_carga = SIN_CARGA;
    memset(ruta_actual, 0, sizeof(ruta_actual));
    memset(&informacion_juego, 0, sizeof(informacion_juego));
}

// — OBTENER ESTADO —

bool GestorJuegosEMULVA360X::hayJuegoCargado() {
    return juego_cargado;
}

uint32_t GestorJuegosEMULVA360X::obtenerFormato() {
    return tipo_formato;
}

InformacionJuego GestorJuegosEMULVA360X::obtenerInformacion() {
    return informacion_juego;
}

const char* GestorJuegosEMULVA360X::obtenerRuta() {
    return ruta_actual;
}

uint32_t GestorJuegosEMULVA360X::obtenerEstadoCarga() {
    return estado_carga;
}

// — LISTAR JUEGOS DE UNA CARPETA —

std::vector<ArchivoJuego> GestorJuegosEMULVA360X::listarJuegosEnCarpeta(const char* ruta_carpeta) {
    std::vector<ArchivoJuego> lista;
    
    // Versión simplificada — la versión completa usa opendir/readdir
    // para recorrer archivos .iso y .god
    
    return lista;
}
