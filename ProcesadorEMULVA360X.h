#include <iostream>
#include <string>
#include <vector>

// Enumeradores para el procesador
enum class ArquitecturaCPU { POWERPC_XENON, EMULADO, OPTIMIZADO };
enum class NivelProcesamiento { BAJO, MEDIO, ALTO, MAXIMO };
enum class EstadoCPU { INACTIVO, ACTIVO, SOBRECARGADO };

class ProcesadorEMULVA360X {
private:
    ArquitecturaCPU arquitectura;
    NivelProcesamiento nivel;
    EstadoCPU estado;
    int nucleosSimulados;
    float velocidadGHz;
    std::vector<std::string> instruccionesProcesadas;

public:
    // Constructor
    ProcesadorEMULVA360X() {
        arquitectura = ArquitecturaCPU::POWERPC_XENON;
        nivel = NivelProcesamiento::MEDIO;
        estado = EstadoCPU::INACTIVO;
        nucleosSimulados = 3;
        velocidadGHz = 3.2f;
        instruccionesProcesadas = {"Cargar juego", "Ejecutar instrucción", "Sincronizar memoria"};
    }

    // Método para iniciar el procesador
    void iniciarProcesador() {
        estado = EstadoCPU::ACTIVO;
        std::cout << "\n=== PROCESADOR EMULVA360X INICIADO ===" << std::endl;
        std::cout << "Arquitectura: PowerPC Xenon | Núcleos: " << nucleosSimulados << std::endl;
        std::cout << "Velocidad: " << velocidadGHz << "GHz" << std::endl;
    }

    // Método para procesar instrucciones
    void procesarInstrucciones() {
        std::cout << "\n--- PROCESANDO INSTRUCCIONES ---" << std::endl;
        for (std::string instr : instruccionesProcesadas) {
            std::cout << "Instrucción: " << instr << std::endl;
        }
    }
};
