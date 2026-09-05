#include "ProcesadorEMULVA360X.h"
#include <cstdint>
#include <cstring>

// Constructor — inicializa el estado del procesador
ProcesadorEMULVA360X::ProcesadorEMULVA360X() {
    reiniciar();
}

// Reinicia todos los registros y estado del CPU
void ProcesadorEMULVA360X::reiniciar() {
    // Reinicia todos los registros de propósito general (GPR)
    memset(registros_gpr, 0, sizeof(registros_gpr));
    
    // Reinicia registros de punto flotante (FPR — VMX de PowerPC)
    memset(registros_fpr, 0, sizeof(registros_fpr));
    
    // Reinicia registros de control y estado
    pc = 0;                  // Contador de programa — dónde empieza a ejecutar
    estado = ESTADO_REINICIADO;
    ciclos_ejecutados = 0;
    modo_ejecucion = MODO_USUARIO;
}

// Inicializa y arranca el procesador
bool ProcesadorEMULVA360X::inicializar() {
    reiniciar();
    estado = ESTADO_INICIALIZADO;
    
    // Configuración inicial del CPU de Xbox 360:
    // — PowerPC Xenon, 3 núcleos, 2 hilos cada uno = 6 hilos
    numero_nucleos = 3;
    hilos_por_nucleo = 2;
    frecuencia_base = 3200000000; // 3.2 GHz
    
    estado = ESTADO_LISTO;
    return true;
}

// Ejecuta una instrucción del juego
void ProcesadorEMULVA360X::ejecutarInstruccion() {
    if (estado != ESTADO_CORRIENDO) return;
    
    // Leer la instrucción desde la memoria según el PC
    uint32_t instruccion = leerMemoriaInstruccion(pc);
    
    // Decodificar y ejecutar la instrucción de PowerPC
    decodificarYEjecutar(instruccion);
    
    // Avanzar al siguiente instrucción
    pc += 4; // Las instrucciones PowerPC son de 4 bytes
    
    ciclos_ejecutados++;
}

// Decodifica y simula la instrucción
void ProcesadorEMULVA360X::decodificarYEjecutar(uint32_t instr) {
    // Este es el corazón de la emulación:
    // Según el código de operación (primeros bits de la instrucción),
    // se ejecuta la operación correspondiente: suma, salto, carga, guardado, etc.
    
    uint8_t opcode = (instr >> 26) & 0x3F;
    
    switch (opcode) {
        case 0x0C: // Instrucción de carga de palabra
            instruccionCargarPalabra(instr);
            break;
        case 0x10: // Instrucción de guardado de palabra
            instruccionGuardarPalabra(instr);
            break;
        case 0x18: // Operaciones aritméticas
            instruccionAritmetica(instr);
            break;
        case 0x1B: // Saltos y ramificaciones
            instruccionSalto(instr);
            break;
        default:
            // Instrucción no implementada aún
            break;
    }
}

// Leer instrucción desde la memoria
uint32_t ProcesadorEMULVA360X::leerMemoriaInstruccion(uint64_t direccion) {
    // Aquí se conecta con el módulo de MemoriaRAM
    // Devuelve el valor de 4 bytes en la dirección indicada
    return memoria->leer32(direccion);
}

// Cargar valor desde memoria a registro
void ProcesadorEMULVA360X::instruccionCargarPalabra(uint32_t instr) {
    uint8_t rt = (instr >> 21) & 0x1F;   // Registro destino
    uint8_t ra = (instr >> 16) & 0x1F;   // Registro base
    int16_t desp = instr & 0xFFFF;       // Desplazamiento
    
    uint64_t direccion = registros_gpr[ra] + desp;
    registros_gpr[rt] = memoria->leer32(direccion);
}

// Guardar valor de registro a memoria
void ProcesadorEMULVA360X::instruccionGuardarPalabra(uint32_t instr) {
    uint8_t rs = (instr >> 21) & 0x1F;   // Registro origen
    uint8_t ra = (instr >> 16) & 0x1F;   // Registro base
    int16_t desp = instr & 0xFFFF;       // Desplazamiento
    
    uint64_t direccion = registros_gpr[ra] + desp;
    memoria->escribir32(direccion, registros_gpr[rs]);
}

// Operaciones aritméticas y lógicas
void ProcesadorEMULVA360X::instruccionAritmetica(uint32_t instr) {
    uint8_t rt = (instr >> 21) & 0x1F;
    uint8_t ra = (instr >> 16) & 0x1F;
    uint8_t rb = (instr >> 11) & 0x1F;
    
    // Suma básica — se amplía con más operaciones
    registros_gpr[rt] = registros_gpr[ra] + registros_gpr[rb];
}

// Saltos y cambios de flujo
void ProcesadorEMULVA360X::instruccionSalto(uint32_t instr) {
    int32_t direccion_salto = (instr & 0x03FFFFFF) << 2;
    pc = direccion_salto;
}

// Arranca la ejecución
void ProcesadorEMULVA360X::arrancar() {
    if (estado == ESTADO_LISTO || estado == ESTADO_PAUSADO) {
        estado = ESTADO_CORRIENDO;
    }
}

// Pausa la ejecución
void ProcesadorEMULVA360X::pausar() {
    if (estado == ESTADO_CORRIENDO) {
        estado = ESTADO_PAUSADO;
    }
}

// Devuelve el estado actual
int ProcesadorEMULVA360X::obtenerEstado() {
    return estado;
}

// Devuelve el contador de programa
uint64_t ProcesadorEMULVA360X::obtenerContadorPrograma() {
    return pc;
}

// Vincula el módulo de memoria
void ProcesadorEMULVA360X::conectarMemoria(MemoriaRAMEMULVA360X* mem) {
    memoria = mem;
}
