#include "GestorCarpetasEMULVA360X.h"
#include <cstring>
#include <sys/stat.h>
#include <sys/dir.h>
#include <cstdio>

// Constructor — inicializa rutas
GestorCarpetasEMULVA360X::GestorCarpetasEMULVA360X() {
    inicializada = false;
    memset(ruta_base, 0, sizeof(ruta_base));
    memset(ruta_juegos, 0, sizeof(ruta_juegos));
    memset(ruta_guardados, 0, sizeof(ruta_guardados));
    memset(ruta_configuracion, 0, sizeof(ruta_configuracion));
    memset(ruta_capturas, 0, sizeof(ruta_capturas));
    memset(ruta_logs, 0, sizeof(ruta_logs));
    memset(ruta_temporales, 0, sizeof(ruta_temporales));
}

// Inicializar rutas según el sistema
bool GestorCarpetasEMULVA360X::inicializar(const char* ruta_raiz) {
    // Ruta base — en Android suele ser /storage/emulated/0/Android/data/[paquete]/files/
    strncpy(ruta_base, ruta_raiz, sizeof(ruta_base) - 1);
    size_t len = strlen(ruta_base);
    if (len > 0 && ruta_base[len - 1] != '/') {
        strcat(ruta_base, "/");
    }
    
    // Construir rutas de cada carpeta
    snprintf(ruta_juegos,       sizeof(ruta_juegos),       "%sROMs/", ruta_base);
    snprintf(ruta_guardados,    sizeof(ruta_guardados),    "%sSaves/", ruta_base);
    snprintf(ruta_configuracion,sizeof(ruta_configuracion),"%sConfig/", ruta_base);
    snprintf(ruta_capturas,     sizeof(ruta_capturas),     "%sScreenshots/", ruta_base);
    snprintf(ruta_logs,         sizeof(ruta_logs),         "%sLogs/", ruta_base);
    snprintf(ruta_temporales,   sizeof(ruta_temporales),   "%sTemp/", ruta_base);
    
    // Crear todas las carpetas
    if (!crearCarpetas()) {
        return false;
    }
    
    inicializada = true;
    return true;
}

// — CREACIÓN DE CARPETAS —

bool GestorCarpetasEMULVA360X::crearCarpeta(const char* ruta) {
    struct stat st;
    // ¿Ya existe?
    if (stat(ruta, &st) == 0) {
        if (S_ISDIR(st.st_mode)) return true; // Ya es carpeta → OK
        return false; // Existe pero no es carpeta → error
    }
    // Crear carpeta con permisos completos
    if (mkdir(ruta, 0755) != 0) {
        return false;
    }
    return true;
}

bool GestorCarpetasEMULVA360X::crearCarpetas() {
    bool ok = true;
    
    // Carpeta base
    if (!crearCarpeta(ruta_base)) ok = false;
    
    // Subcarpetas
    if (!crearCarpeta(ruta_juegos))        ok = false;
    if (!crearCarpeta(ruta_guardados))     ok = false;
    if (!crearCarpeta(ruta_configuracion)) ok = false;
    if (!crearCarpeta(ruta_capturas))      ok = false;
    if (!crearCarpeta(ruta_logs))          ok = false;
    if (!crearCarpeta(ruta_temporales))    ok = false;
    
    return ok;
}

// — VERIFICACIÓN Y ESTADO —

bool GestorCarpetasEMULVA360X::existeCarpeta(const char* ruta) {
    struct stat st;
    if (stat(ruta, &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

bool GestorCarpetasEMULVA360X::todasCarpetasExisten() {
    return existeCarpeta(ruta_juegos) &&
           existeCarpeta(ruta_guardados) &&
           existeCarpeta(ruta_configuracion) &&
           existeCarpeta(ruta_capturas) &&
           existeCarpeta(ruta_logs) &&
           existeCarpeta(ruta_temporales);
}

// — OBTENER RUTAS —

const char* GestorCarpetasEMULVA360X::obtenerRutaBase() {
    return ruta_base;
}

const char* GestorCarpetasEMULVA360X::obtenerRutaJuegos() {
    return ruta_juegos;
}

const char* GestorCarpetasEMULVA360X::obtenerRutaGuardados() {
    return ruta_guardados;
}

const char* GestorCarpetasEMULVA360X::obtenerRutaConfiguracion() {
    return ruta_configuracion;
}

const char* GestorCarpetasEMULVA360X::obtenerRutaCapturas() {
    return ruta_capturas;
}

const char* GestorCarpetasEMULVA360X::obtenerRutaLogs() {
    return ruta_logs;
}

const char* GestorCarpetasEMULVA360X::obtenerRutaTemporales() {
    return ruta_temporales;
}

// — LISTAR ARCHIVOS POR EXTENSIÓN —

std::vector<ArchivoEnCarpeta> GestorCarpetasEMULVA360X::listarArchivos(const char* ruta_carpeta, const char* filtro_extension) {
    std::vector<ArchivoEnCarpeta> lista;
    DIR* dir = opendir(ruta_carpeta);
    if (!dir) return lista;
    
    struct dirent* entrada;
    while ((entrada = readdir(dir)) != nullptr) {
        // Saltar carpetas . y ..
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }
        
        // Construir ruta completa
        char ruta_completa[512];
        snprintf(ruta_completa, sizeof(ruta_completa), "%s%s", ruta_carpeta, entrada->d_name);
        
        struct stat st;
        if (stat(ruta_completa, &st) != 0) continue;
        
        // Si es carpeta, incluir sin filtro
        if (S_ISDIR(st.st_mode)) {
            ArchivoEnCarpeta elem;
            strncpy(elem.nombre, entrada->d_name, sizeof(elem.nombre) - 1);
            strncpy(elem.ruta_completa, ruta_completa, sizeof(elem.ruta_completa) - 1);
            elem.es_carpeta = true;
            elem.tamano = 0;
            elem.fecha_modificacion = st.st_mtime;
            lista.push_back(elem);
            continue;
        }
        
        // Si es archivo, verificar extensión
        if (filtro_extension != nullptr && strlen(filtro_extension) > 0) {
            const char* ext = strrchr(entrada->d_name, '.');
            if (!ext || strcasecmp(ext, filtro_extension) != 0) {
                continue; // No coincide la extensión
            }
        }
        
        // Agregar archivo a la lista
        ArchivoEnCarpeta elem;
        strncpy(elem.nombre, entrada->d_name, sizeof(elem.nombre) - 1);
        strncpy(elem.ruta_completa, ruta_completa, sizeof(elem.ruta_completa) - 1);
        elem.es_carpeta = false;
        elem.tamano = st.st_size;
        elem.fecha_modificacion = st.st_mtime;
        lista.push_back(elem);
    }
    
    closedir(dir);
    return lista;
}

// Métodos específicos para cada tipo
std::vector<ArchivoEnCarpeta> GestorCarpetasEMULVA360X::listarJuegos() {
    // Devolver .iso y .god — se puede ampliar para ambos formatos
    auto lista_iso = listarArchivos(ruta_juegos, ".iso");
    auto lista_god = listarArchivos(ruta_juegos, ".god");
    
    // Combinar las dos listas
    std::vector<ArchivoEnCarpeta> resultado;
    resultado.reserve(lista_iso.size() + lista_god.size());
    resultado.insert(resultado.end(), lista_iso.begin(), lista_iso.end());
    resultado.insert(resultado.end(), lista_god.begin(), lista_god.end());
    
    return resultado;
}

std::vector<ArchivoEnCarpeta> GestorCarpetasEMULVA360X::listarGuardados() {
    return listarArchivos(ruta_guardados, ".sav");
}

// — ELIMINAR Y LIMPIAR —

bool GestorCarpetasEMULVA360X::eliminarArchivo(const char* ruta_archivo) {
    struct stat st;
    if (stat(ruta_archivo, &st) != 0) return false;
    if (S_ISDIR(st.st_mode)) return false; // No borrar carpetas con este método
    return remove(ruta_archivo) == 0;
}

uint32_t GestorCarpetasEMULVA360X::limpiarTemporales() {
    auto lista = listarArchivos(ruta_temporales, nullptr);
    uint32_t borrados = 0;
    
    for (auto& archivo : lista) {
        if (!archivo.es_carpeta) {
            if (eliminarArchivo(archivo.ruta_completa)) {
                borrados++;
            }
        }
    }
    return borrados;
}

// — TAMAÑO Y ESPACIO —

uint64_t GestorCarpetasEMULVA360X::obtenerTamanoCarpeta(const char* ruta) {
    auto lista = listarArchivos(ruta, nullptr);
    uint64_t total = 0;
    
    for (auto& elem : lista) {
        if (!elem.es_carpeta) {
            total += elem.tamano;
        }
    }
    return total;
}

uint64_t GestorCarpetasEMULVA360X::obtenerTamanoJuegos() {
    return obtenerTamanoCarpeta(ruta_juegos);
}

// — RUTA PERSONALIZADA —

void GestorCarpetasEMULVA360X::establecerRutaJuegosPersonalizada(const char* nueva_ruta) {
    strncpy(ruta_juegos, nueva_ruta, sizeof(ruta_juegos) - 1);
    // Asegurar que termine con barra
    size_t len = strlen(ruta_juegos);
    if (len > 0 && ruta_juegos[len - 1] != '/') {
        strcat(ruta_juegos, "/");
    }
}
