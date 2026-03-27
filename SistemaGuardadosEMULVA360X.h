#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime> // Para fechas de guardado

// Enumerador para tipos de guardado
enum class TipoGuardado { AUTOMATICO, MANUAL, COPIA_SEGURIDAD };
enum class EstadoGuardado { VALIDO, CORRUPTO, INCOMPLETO };

// Estructura para datos del guardado
struct DatosGuardado {
    std::string nombreJuego;
    std::string nombreGuardado;
    TipoGuardado tipo;
    EstadoGuardado estado;
    std::string fechaCreacion;
    float tamanoMB;
    std::string ruta;
};

class SistemaGuardadosEMULVA360X {
private:
    std::vector<DatosGuardado> guardadosDetectados;
    std::string rutaCarpetaGuardados;
    const std::string CARPETA_BASE = "/sdcard/Xbox 360/Guardados/";

    // Crear carpeta si no existe
    void crearCarpetaSiNoExiste() {
        std::ifstream carpeta(CARPETA_BASE);
        if (!carpeta.good()) {
            system(("mkdir \"" + CARPETA_BASE + "\"").c_str());
            std::cout << "📂 Carpeta de guardados creada en: " << CARPETA_BASE << std::endl;
        }
    }

    // Obtener fecha actual
    std::string obtenerFechaActual() {
        time_t ahora = time(nullptr);
        char fecha[50];
        strftime(fecha, sizeof(fecha), "%d/%m/%Y %H:%M:%S", localtime(&ahora));
        return std::string(fecha);
    }

public:
    // Constructor - Inicia sistema de guardados
    SistemaGuardadosEMULVA360X() {
        rutaCarpetaGuardados = CARPETA_BASE;
        crearCarpetaSiNoExiste();
        std::cout << "💾 SISTEMA DE GUARDADOS INICIADO" << std::endl;
    }

    // Detectar guardados existentes
    void detectarGuardados() {
        std::cout << "\n=== DETECTANDO GUARDADOS ===" << std::endl;
        
        // Ejemplo de guardados detectados
        DatosGuardado guardado1 = {
            "Gears of War",
            "Partida 1",
            TipoGuardado::MANUAL,
            EstadoGuardado::VALIDO,
            obtenerFechaActual(),
            2.5f,
            CARPETA_BASE + "GearsOfWar.sav"
        };

        DatosGuardado guardado2 = {
            "Forza Motorsport 3",
            "Carrera 5",
            TipoGuardado::AUTOMATICO,
            EstadoGuardado::VALIDO,
            obtenerFechaActual(),
            1.8f,
            CARPETA_BASE + "Forza.sav"
        };

        guardadosDetectados.push_back(guardado1);
        guardadosDetectados.push_back(guardado2);

        std::cout << "✅ " << guardadosDetectados.size() << " guardados detectados" << std::endl;
    }

    // Crear nuevo guardado
    void crearGuardado(std::string nombreJuego, std::string nombreGuardado) {
        DatosGuardado nuevoGuardado = {
            nombreJuego,
            nombreGuardado,
            TipoGuardado::MANUAL,
            EstadoGuardado::VALIDO,
            obtenerFechaActual(),
            3.2f,
            CARPETA_BASE + nombreJuego + "_" + nombreGuardado + ".sav"
        };

        guardadosDetectados.push_back(nuevoGuardado);
        std::cout << "\n💾 Guardado creado: " << nombreGuardado << " para " << nombreJuego << std::endl;
    }

    // Verificar si el guardado es valido
    EstadoGuardado verificarValidez(std::string rutaGuardado) {
        std::ifstream archivo(rutaGuardado);
        if (!archivo.is_open()) return EstadoGuardado::CORRUPTO;
        
        // Comprobar firma del archivo
        std::string firma;
        archivo >> firma;
        if (firma != "XBOX360SAVE") return EstadoGuardado::CORRUPTO;
        
        return EstadoGuardado::VALIDO;
    }

    // Mostrar estado de guardados
    void mostrarGuardados() {
        std::cout << "\n--- LISTA DE GUARDADOS ---" << std::endl;
        for (auto g : guardadosDetectados) {
            std::cout << "- " << g.nombreGuardado << " | " << g.nombreJuego << std::endl;
            std::cout << "   Fecha: " << g.fecha << " | Tamaño: " << g.tamanoMB << " MB" << std::endl;
            std::cout << "   Estado: " << (g.estado == EstadoGuardado::VALIDO ? "✅ VALIDO" : "❌ CORRUPTO") << std::endl;
        }
    }

    // Restaurar guardado
    void restaurarGuardado(std::string nombreGuardado) {
        for (auto g : guardadosDetectados) {
            if (g.nombreGuardado == nombreGuardado) {
                std::cout << "\n🔄 Restaurando guardado: " << nombreGuardado << std::endl;
                std::cout << "Juego: " << g.nombreJuego << " | Estado: Restaurado correctamente" << std::endl;
                return;
            }
        }
        std::cout << "\n⚠️ Guardado no encontrado" << std::endl;
    }
};
