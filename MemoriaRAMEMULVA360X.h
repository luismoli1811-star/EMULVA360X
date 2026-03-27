#include <iostream>
#include <string>
#include <vector>

// Enumeradores para la memoria
enum class TipoMemoria { GDDR3, DDR2, DDR3, EMULADA };
enum class CapacidadMemoria { MB_256, MB_512, GB_1, GB_2 };
enum class EstadoMemoria { INACTIVA, ACTIVA, SOBRECARGADA };

class MemoriaRAMEMULVA360X {
private:
    TipoMemoria tipo;
    CapacidadMemoria capacidad;
    EstadoMemoria estado;
    float capacidadTotalGB;
    float capacidadUsadaGB;
    std::vector<std::string> datosAlmacenados;

public:
    // Constructor
    MemoriaRAMEMULVA360X() {
        tipo = TipoMemoria::GDDR3; // Igual que la Xbox 360 original
        capacidad = CapacidadMemoria::MB_512; // Capacidad real de la Xbox 360
        estado = EstadoMemoria::INACTIVA;
        capacidadTotalGB = 512.0f / 1024.0f; // Convertir MB a GB
        capacidadUsadaGB = 0.0f;
        datosAlmacenados = {"Instrucciones CPU", "Texturas preliminares", "Configuración sistema"};
    }

    // Método para iniciar la memoria
    void iniciarMemoria() {
        estado = EstadoMemoria::ACTIVA;
        std::cout << "\n=== MEMORIA RAM EMULVA360X INICIADA ===" << std::endl;
        std::cout << "Tipo: " << (tipo == TipoMemoria::GDDR3 ? "GDDR3" : "EMULADA") << std::endl;
        std::cout << "Capacidad: " << capacidadTotalGB << " GB" << std::endl;
    }

    // Método para asignar datos
    void asignarDatos(std::string dato, float tamanoMB) {
        if (capacidadUsadaGB + (tamanoMB / 1024.0f) <= capacidadTotalGB) {
            capacidadUsadaGB += tamanoMB / 1024.0f;
            datosAlmacenados.push_back(dato);
            std::cout << "Asignados " << tamanoMB << " MB a: " << dato << std::endl;
        } else {
            std::cout << "⚠️ NO HAY MEMORIA SUFICIENTE PARA: " << dato << std::endl;
        }
    }

    // Mostrar estado de la memoria
    void mostrarEstado() {
        std::cout << "\n--- ESTADO MEMORIA ---" << std::endl;
        std::cout << "Usada: " << capacidadUsadaGB << " GB / " << capacidadTotalGB << " GB" << std::endl;
        std::cout << "Datos almacenados: " << datosAlmacenados.size() << " elementos" << std::endl;
    }
};
