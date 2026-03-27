#include <iostream>
#include "ModuloFirmware.h"
#include "GestorCarpetasEMULVA360X.h"
#include "ProcesadorEMULVA360X.h"
#include "MemoriaRAMEMULVA360X.h"
#include "GraficaEMULVA360X.h"
#include "ControlesEMULVA360X.h"
#include "GestorJuegosEMULVA360X.h"
#include "SistemaGuardadosEMULVA360X.h"
#include "InterfazUsuario.h"

// Importamos el módulo de controles actualizado
#include "ControlesEMULVA360X.h"

int main() {
    std::cout << "══════════════════════════════════════════════════" << std::endl;
    std::cout << "          🎮 EMULVA360X - VERSIÓN FINAL          " << std::endl;
    std::cout << "══════════════════════════════════════════════════" << std::endl;

    // 1. Inicializar firmware y carpetas
    ModuloFirmware firmware;
    GestorCarpetas gestorCarpetas;
    firmware.buscarActualizacion();
    gestorCarpetas.crearEstructura();

    // 2. Inicializar componentes técnicos
    ProcesadorEMULVA360X cpu;
    MemoriaRAMEMULVA360X ram;
    GraficaEMULVA360X gpu;
    
    cpu.iniciar();
    ram.asignarEspacio(64.0f);
    gpu.configurarResolucion(1080, 1920);

    // 3. Inicializar controles CON EFECTOS
    ControlesEMULVA360X controles;
    controles.configurarBotones(); // Carga colores y posiciones
    std::cout << "\n💡 CONTROLES LISTOS - EFECTOS ACTIVADOS" << std::endl;

    // 4. Gestión de juegos y guardados
    GestorJuegosEMULVA360X gestorJuegos;
    SistemaGuardadosEMULVA360X gestorGuardados;
    gestorJuegos.escanearJuegos();
    gestorGuardados.iniciarSistema();

    // 5. Interfaz + prueba de controles
    InterfazUsuario ui;
    ui.mostrarBienvenida();

    // PRUEBA DE EFECTOS DE CONTROLES
    std::cout << "\n🔍 PRUEBA DE CONTROLES - PRESIONA COORDENADAS:" << std::endl;
    std::cout << "Ejemplo: Escribe 700 500 para probar el botón A..." << std::endl;
    
    int x, y;
    do {
        std::cout << "\nIngresa coordenadas (x y) o -1 -1 para salir: ";
        std::cin >> x >> y;
        if(x == -1 && y == -1) break;
        
        // Actualiza estado del botón y muestra efecto
        controles.actualizarEstado(x, y); 
    } while(true);

    // 6. Cierre seguro
    ui.mostrarDespedida();
    return 0;
}
