#include "MemoriaRAMEMULVA360X.h"
#include <cstring>
#include <cstdlib>

// Constructor — inicializa todo
MemoriaRAMEMULVA360X::MemoriaRAMEMULVA360X() {
    tamano_total = 0;
    memoria_fisica = nullptr;
    inicializada = false;
}

// Destructor — libera la memoria al cerrar
MemoriaRAMEMULVA360X::~MemoriaRAMEMULVA360X() {
    if (memoria_fisica != nullptr) {
        liberarMemoria();
    }
}

// Inicializa la memoria de la Xbox 360 — 512 MB unificada
bool MemoriaRAMEMULVA360X::inicializar() {
    // Xbox 360 tiene 512 MB de RAM unificada (CPU + GPU comparten)
    tamano_total = 512 * 1024 * 1024; // 512 MB en bytes
    
    // Reservar el bloque de memoria
    memoria_fisica = (uint8_t*)malloc(tamano_total);
    if (memoria_fisica == nullptr) {
        return false; // No se pudo reservar memoria
    }
    
    // Poner todo en ceros al iniciar
    memset(memoria_fisica, 0, tamano_total);
    
    // Inicializar las regiones de memoria
    inicializarRegiones();
    
    inicializada = true;
    return true;
}

// Define las zonas de memoria de la consola
void MemoriaRAMEMULVA360X::inicializarRegiones() {
    // Direcciones de memoria de Xbox 360:
    // 0x00000000 - 0x1FFFFFFF → RAM física (512 MB)
    // 0x80000000 - 0x9FFFFFFF → RAM mapeada para el sistema
    // 0xFC000000 - 0xFFFFFFFF → Registros hardware / ROM del sistema
    
    region_ram_inicio = 0x00000000;
    region_ram_fin    = 0x1FFFFFFF;
    
    region_sistema_inicio = 0x80000000;
    region_sistema_fin    = 0x9FFFFFFF;
    
    region_registros_inicio = 0xFC000000;
    region_registros_fin    = 0xFFFFFFFF;
}

// Verifica que la dirección sea válida
bool MemoriaRAMEMULVA360X::direccionValida(uint64_t direccion, uint32_t tamano) {
    if (!inicializada || memoria_fisica == nullptr) return false;
    
    // Verificar que no se salga de ninguna región
    if (direccion + tamano > region_registros_fin) return false;
    
    return true;
}

// Convierte dirección de la consola → posición en nuestro bloque
uint64_t MemoriaRAMEMULVA360X::traducirDireccion(uint64_t direccion) {
    // RAM física directa
    if (direccion >= region_ram_inicio && direccion <= region_ram_fin) {
        return direccion - region_ram_inicio;
    }
    
    // RAM mapeada → apunta a la misma física
    if (direccion >= region_sistema_inicio && direccion <= region_sistema_fin) {
        return direccion - region_sistema_inicio;
    }
    
    // Registros del sistema → no en RAM, se manejan aparte
    if (direccion >= region_registros_inicio && direccion <= region_registros_fin) {
        return 0xFFFFFFFFFFFFFFFF; // Marca registros especiales
    }
    
    return 0xFFFFFFFFFFFFFFFF; // Dirección inválida
}

// — LECTURAS — Leer datos desde la memoria

uint8_t MemoriaRAMEMULVA360X::leer8(uint64_t direccion) {
    if (!direccionValida(direccion, 1)) return 0;
    uint64_t offset = traducirDireccion(direccion);
    if (offset == 0xFFFFFFFFFFFFFFFF) return 0;
    return memoria_fisica[offset];
}

uint16_t MemoriaRAMEMULVA360X::leer16(uint64_t direccion) {
    if (!direccionValida(direccion, 2)) return 0;
    uint64_t offset = traducirDireccion(direccion);
    if (offset == 0xFFFFFFFFFFFFFFFF) return 0;
    
    // Xbox 360 usa Big-Endian (orden de bytes invertido respecto a Android)
    return (uint16_t(memoria_fisica[offset]) << 8) |
            uint16_t(memoria_fisica[offset + 1]);
}

uint32_t MemoriaRAMEMULVA360X::leer32(uint64_t direccion) {
    if (!direccionValida(direccion, 4)) return 0;
    uint64_t offset = traducirDireccion(direccion);
    if (offset == 0xFFFFFFFFFFFFFFFF) return 0;
    
    // Big-Endian de PowerPC
    return (uint32_t(memoria_fisica[offset]) << 24) |
           (uint32_t(memoria_fisica[offset + 1]) << 16) |
           (uint32_t(memoria_fisica[offset + 2]) << 8) |
            uint32_t(memoria_fisica[offset + 3]);
}

uint64_t MemoriaRAMEMULVA360X::leer64(uint64_t direccion) {
    if (!direccionValida(direccion, 8)) return 0;
    uint64_t offset = traducirDireccion(direccion);
    if (offset == 0xFFFFFFFFFFFFFFFF) return 0;
    
    return (uint64_t(leer32(direccion)) << 32) | leer32(direccion + 4);
}

// — ESCRITURAS — Guardar datos en la memoria

void MemoriaRAMEMULVA360X::escribir8(uint64_t direccion, uint8_t valor) {
    if (!direccionValida(direccion, 1)) return;
    uint64_t offset = traducirDireccion(direccion);
    if (offset == 0xFFFFFFFFFFFFFFFF) return;
    memoria_fisica[offset] = valor;
}

void MemoriaRAMEMULVA360X::escribir16(uint64_t direccion, uint16_t valor) {
    if (!direccionValida(direccion, 2)) return;
    uint64_t offset = traducirDireccion(direccion);
    if (offset == 0xFFFFFFFFFFFFFFFF) return;
    
    // Big-Endian
    memoria_fisica[offset]     = (valor >> 8) & 0xFF;
    memoria_fisica[offset + 1] = valor & 0xFF;
}

void MemoriaRAMEMULVA360X::escribir32(uint64_t direccion, uint32_t valor) {
    if (!direccionValida(direccion, 4)) return;
    uint64_t offset = traducirDireccion(direccion);
    if (offset == 0xFFFFFFFFFFFFFFFF) return;
    
    // Big-Endian — ¡CLAVE para que PowerPC funcione!
    memoria_fisica[offset]     = (valor >> 24) & 0xFF;
    memoria_fisica[offset + 1] = (valor >> 16) & 0xFF;
    memoria_fisica[offset + 2] = (valor >> 8) & 0xFF;
    memoria_fisica[offset + 3] = valor & 0xFF;
}

void MemoriaRAMEMULVA360X::escribir64(uint64_t direccion, uint64_t valor) {
    escribir32(direccion,     uint32_t(valor >> 32));
    escribir32(direccion + 4, uint32_t(valor & 0xFFFFFFFF));
}

// Reinicia toda la memoria a ceros
void MemoriaRAMEMULVA360X::reiniciar() {
    if (memoria_fisica != nullptr) {
        memset(memoria_fisica, 0, tamano_total);
    }
}

// Libera la memoria al cerrar
void MemoriaRAMEMULVA360X::liberarMemoria() {
    if (memoria_fisica != nullptr) {
        free(memoria_fisica);
        memoria_fisica = nullptr;
    }
    inicializada = false;
}

// Devuelve el tamaño total
uint32_t MemoriaRAMEMULVA360X::obtenerTamano() {
    return tamano_total;
}
