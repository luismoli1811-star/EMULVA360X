#include "OptimizationsRendimiento.h"
#include "ProcesadorEMULVA360X.h"
#include "MemoriaRAMEMULVA360X.h"
#include <cstring>
#include <algorithm>

// Constructor — inicializa todo
OptimizacionesRendimiento::OptimizacionesRendimiento() {
    inicializada = false;
    modo_optimizacion = MODO_EQUILIBRADO;
    frames_segundo_objetivo = 60;
    uso_vsync = true;
    limitador_fps = true;
    tiempo_por_frame_us = 1000000 / 60; // ~16.66ms por frame
    
    // Cachés y búferes
    cache_instrucciones = nullptr;
    cache_datos = nullptr;
    tamano_cache_instr = 0;
    tamano_cache_datos = 0;
    
    // Contadores de rendimiento
    frames_renderizados = 0;
    tiempo_ultimo_frame = 0;
    fps_actual = 0;
    promedio_fps = 0.0f;
    min_fps = 999;
    max_fps = 0;
    
    // Umbrales de ahorro
    umbral_bateria_baja = 20;
    reduccion_fps_bateria = 30;
}

// Destructor — libera cachés
OptimizacionesRendimiento::~OptimizacionesRendimiento() {
    liberarCaches();
}

// — INICIALIZACIÓN —

bool OptimizacionesRendimiento::inicializar() {
    // Detectar capacidades del dispositivo
    detectarCapacidadesDispositivo();
    
    // Asignar cachés
    if (!inicializarCaches()) return false;
    
    // Aplicar modo por defecto
    aplicarModoOptimizacion(modo_optimizacion);
    
    inicializada = true;
    return true;
}

void OptimizacionesRendimiento::detectarCapacidadesDispositivo() {
    // Detectar núcleos y arquitectura
    numero_nucleos = 4; // Valor base — se ajusta al dispositivo
    arquitectura = ARQUITECTURA_ARM64; // Android = ARM64
    soporte_neon = true; // NEON = instrucciones SIMD de ARM → ¡MUY IMPORTANTE!
    
    // Resolución de pantalla
    ancho_pantalla = 1280;
    alto_pantalla = 720;
    
    // Frecuencia estimada
    frecuencia_cpu_mhz = 2400;
}

bool OptimizacionesRendimiento::inicializarCaches() {
    // Caché de instrucciones → acelera la lectura del código del juego
    tamano_cache_instr = 256 * 1024; // 256 KB
    cache_instrucciones = (uint8_t*)malloc(tamano_cache_instr);
    if (!cache_instrucciones) return false;
    memset(cache_instrucciones, 0, tamano_cache_instr);
    
    // Caché de datos → acelera lecturas/escrituras frecuentes
    tamano_cache_datos = 128 * 1024; // 128 KB
    cache_datos = (uint8_t*)malloc(tamano_cache_datos);
    if (!cache_datos) { free(cache_instrucciones); return false; }
    memset(cache_datos, 0, tamano_cache_datos);
    
    // Marcar entradas como inválidas
    memset(etiquetas_cache_instr, 0xFF, sizeof(etiquetas_cache_instr));
    memset(etiquetas_cache_datos, 0xFF, sizeof(etiquetas_cache_datos));
    
    return true;
}

// — GESTIÓN DE CACHÉS (¡EL CORAZÓN DE LA VELOCIDAD!) —

bool OptimizacionesRendimiento::buscarCacheInstruccion(uint64_t direccion, uint32_t& valor) {
    // Caché mapeada directa → muy rápida
    uint32_t indice = (direccion >> 2) & 0x3FFF; // 16k entradas
    uint64_t etiqueta = direccion >> 18;
    
    if (etiquetas_cache_instr[indice] == etiqueta) {
        uint32_t offset = (direccion & 0x3F) * 4;
        valor = *(uint32_t*)&cache_instrucciones[offset + indice * 64];
        aciertos_cache_instr++;
        return true;
    }
    
    fallos_cache_instr++;
    return false;
}

void OptimizacionesRendimiento::guardarCacheInstruccion(uint64_t direccion, uint32_t valor) {
    uint32_t indice = (direccion >> 2) & 0x3FFF;
    uint64_t etiqueta = direccion >> 18;
    uint32_t offset = (direccion & 0x3F) * 4;
    
    etiquetas_cache_instr[indice] = etiqueta;
    *(uint32_t*)&cache_instrucciones[offset + indice * 64] = valor;
}

bool OptimizacionesRendimiento::buscarCacheDato(uint64_t direccion, uint32_t& valor) {
    uint32_t indice = (direccion >> 2) & 0x1FFF;
    uint64_t etiqueta = direccion >> 15;
    
    if (etiquetas_cache_datos[indice] == etiqueta) {
        uint32_t offset = (direccion & 0x7F);
        valor = *(uint32_t*)&cache_datos[offset + indice * 128];
        aciertos_cache_datos++;
        return true;
    }
    
    fallos_cache_datos++;
    return false;
}

void OptimizacionesRendimiento::guardarCacheDato(uint64_t direccion, uint32_t valor) {
    uint32_t indice = (direccion >> 2) & 0x1FFF;
    uint64_t etiqueta = direccion >> 15;
    uint32_t offset = (direccion & 0x7F);
    
    etiquetas_cache_datos[indice] = etiqueta;
    *(uint32_t*)&cache_datos[offset + indice * 128] = valor;
}

void OptimizacionesRendimiento::limpiarCaches() {
    if (cache_instrucciones) {
        free(cache_instrucciones);
        cache_instrucciones = nullptr;
    }
    if (cache_datos) {
        free(cache_datos);
        cache_datos = nullptr;
    }
    memset(etiquetas_cache_instr, 0xFF, sizeof(etiquetas_cache_instr));
    memset(etiquetas_cache_datos, 0xFF, sizeof(etiquetas_cache_datos));
    reiniciarContadores();
}

void OptimizacionesRendimiento::invalidarCaches() {
    memset(etiquetas_cache_instr, 0xFF, sizeof(etiquetas_cache_instr));
    memset(etiquetas_cache_datos, 0xFF, sizeof(etiquetas_cache_datos));
}

// — MODOS DE RENDIMIENTO —

void OptimizacionesRendimiento::aplicarModoOptimizacion(uint32_t modo) {
    modo_optimizacion = modo;
    
    switch (modo) {
        case MODO_MAX_RENDIMIENTO:
            frames_segundo_objetivo = 60;
            limitador_fps = false;
            uso_vsync = false;
            reducir_resolucion = false;
            habilitar_dual_core = true;
            break;
            
        case MODO_EQUILIBRADO:
            frames_segundo_objetivo = 60;
            limitador_fps = true;
            uso_vsync = true;
            reducir_resolucion = false;
            habilitar_dual_core = true;
            break;
            
        case MODO_AHORRO_BATERIA:
            frames_segundo_objetivo = 30;
            limitador_fps = true;
            uso_vsync = true;
            reducir_resolucion = true;
            habilitar_dual_core = false;
            break;
    }
    
    tiempo_por_frame_us = 1000000 / frames_segundo_objetivo;
}

uint32_t OptimizacionesRendimiento::obtenerModoActual() {
    return modo_optimizacion;
}

// — CONTROL DE FPS Y SINCRONIZACIÓN —

void OptimizacionesRendimiento::iniciarFrame(uint64_t tiempo_actual_us) {
    tiempo_inicio_frame = tiempo_actual_us;
}

// Espera el tiempo necesario para mantener los FPS objetivo
void OptimizacionesRendimiento::esperarSiguienteFrame(uint64_t tiempo_actual_us) {
    if (!limitador_fps) return;
    
    uint64_t transcurrido = tiempo_actual_us - tiempo_inicio_frame;
    if (transcurrido < tiempo_por_frame_us) {
        // Quedarse esperando el tiempo restante
        uint64_t esperar = tiempo_por_frame_us - transcurrido;
        // En Android: usleep(esperar);
    }
}

void OptimizacionesRendimiento::actualizarFPS(uint64_t tiempo_actual_us) {
    frames_renderizados++;
    
    if (tiempo_actual_us - tiempo_ultimo_frame >= 1000000) { // Cada segundo
        fps_actual = frames_renderizados;
        frames_renderizados = 0;
        tiempo_ultimo_frame = tiempo_actual_us;
        
        // Actualizar estadísticas
        if (fps_actual < min_fps) min_fps = fps_actual;
        if (fps_actual > max_fps) max_fps = fps_actual;
        promedio_fps = (promedio_fps * 0.95f) + (fps_actual * 0.05f);
    }
}

uint32_t OptimizacionesRendimiento::obtenerFPS() {
    return fps_actual;
}

float OptimizacionesRendimiento::obtenerPromedioFPS() {
    return promedio_fps;
}

// — AJUSTES DINÁMICOS —

void OptimizacionesRendimiento::ajusteDinamicoPorBateria(uint32_t porcentaje_bateria) {
    if (porcentaje_bateria < umbral_bateria_baja && 
        modo_optimizacion != MODO_AHORRO_BATERIA) {
        // Batería baja → bajar ahorro automáticamente
        frames_segundo_objetivo = reduccion_fps_bateria;
        tiempo_por_frame_us = 1000000 / frames_segundo_objetivo;
    }
}

void OptimizacionesRendimiento::ajustePorTemperatura(float temperatura_c) {
    if (temperatura_c > 45.0f) {
        // Demasiado caliente → reducir carga
        if (frames_segundo_objetivo > 30) {
            frames_segundo_objetivo -= 5;
            tiempo_por_frame_us = 1000000 / frames_segundo_objetivo;
        }
    }
}

// — ESTADÍSTICAS —

void OptimizacionesRendimiento::reiniciarContadores() {
    frames_renderizados = 0;
    fps_actual = 0;
    promedio_fps = 0.0f;
    min_fps = 999;
    max_fps = 0;
    aciertos_cache_instr = 0;
    fallos_cache_instr = 0;
    aciertos_cache_datos = 0;
    fallos_cache_datos = 0;
}

uint64_t OptimizacionesRendimiento::obtenerAciertosInstrucciones() {
    return aciertos_cache_instr;
}

uint64_t OptimizacionesRendimiento::obtenerFallosInstrucciones() {
    return fallos_cache_instr;
}

float OptimizacionesRendimiento::obtenerPorcentajeAciertosInstr() {
    uint64_t total = aciertos_cache_instr + fallos_cache_instr;
    if (total == 0) return 0.0f;
    return (float)aciertos_cache_instr * 100.0f / total;
}

// — LIBERACIÓN —

void OptimizacionesRendimiento::liberarRecursos() {
    limpiarCaches();
    inicializada = false;
}
