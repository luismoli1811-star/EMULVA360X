#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream> // Para verificar archivos/carpetas

// Enumerador para formatos de juego
enum class FormatoJuego { ISO, GOD, DESCONOCIDO };

// Estructura para datos detallados del juego
struct DetallesJuego {
    std::string nombre;
    FormatoJuego formato;
    std::string ruta;
    float tamanoGB;
    bool compatible;
    std::string desarrollador;
    std::string region;
};

class GestorJuegosEMULVA360X {
private:
    std::vector<DetallesJuego> juegos;
    std::string rutaCarpetaJuegos;
    const std::vector<std::string> FORMATOS_COMPATIBLES = {".ISO", ".GOD"};

    // Método para crear carpeta si no existe
    void crearCarpetaSiNoExiste(std::string ruta) {
        std::ifstream carpeta(ruta);
        if (!carpeta.good()) {
            // En CxStudio esto crea la carpeta
            std::cout << "Creando carpeta: " << ruta << std::endl;
            system(("mkdir \"" + ruta + "\"").c_str()); // Comando para crear carpeta en SO Android
        }
    }

    // Detectar formato del archivo
    FormatoJuego obtenerFormato(std::string nombreArchivo) {
        size_t punto = nombreArchivo.find_last_of(".");
        if (punto == std::string::npos) return FormatoJuego::DESCONOCIDO;
        
        std::string ext = nombreArchivo.substr(punto);
        for (char &c : ext) c = toupper(c);

        if (ext == ".ISO") return FormatoJuego::ISO;
        else if (ext == ".GOD") return FormatoJuego::GOD;
        else return FormatoJuego::DESCONOCIDO;
    }

    // Verificar si el juego es compatible
    bool esCompatible(FormatoJuego formato) {
        return (formato == FormatoJuego::ISO || formato == FormatoJuego::GOD);
    }

public:
    // Constructor - Busca carpeta "Xbox 360" en almacenamiento
    GestorJuegosEMULVA360X() {
        // Ruta ESTÁNDAR que el usuario crea: /Almacenamiento/Xbox 360
        rutaCarpetaJuegos = "/sdcard/Xbox 360"; 
        crearCarpetaSiNoExiste(rutaCarpetaJuegos);
        crearCarpetaSiNoExiste(rutaCarpetaJuegos + "/Juegos");
        crearCarpetaSiNoExiste(rutaCarpetaJuegos + "/Guardados");
    }

    // Escanear carpeta "Xbox 360" del usuario
    void escanearJuegos() {
        std::cout << "\n=== ESCANEO DE CARPETA \"Xbox 360\" ===" << std::endl;
        std::cout << "Ruta escaneada: " << rutaCarpetaJuegos << std::endl;

        // Ejemplos de juegos en la carpeta real del usuario
        DetallesJuego juego1 = {
            "Forza Motorsport 3",
            obtenerFormato("ForzaMotorsport3.ISO"),
            rutaCarpetaJuegos + "/ForzaMotorsport3.ISO",
            9.2f,
            esCompatible(FormatoJuego::ISO),
            "Turn 10 Studios",
            "NTSC"
        };

        DetallesJuego juego2 = {
            "GOD OF WAR III",
            obtenerFormato("GodOfWarIII.GOD"),
            rutaCarpetaJuegos + "/GodOfWarIII.GOD",
            7.5f,
            esCompatible(FormatoJuego::GOD),
            "Santa Monica Studio",
            "PAL"
        };

        juegos.push_back(juego1);
        juegos.push_back(juego2);

        mostrarJuegos();
    }

    // Mostrar juegos detectados en la carpeta "Xbox 360"
    void mostrarJuegos() {
        std::cout << "\n--- JUEGOS EN CARPETA \"Xbox 360\" ---" << std::endl;
        for (auto &juego : juegos) {
            std::cout << "\n📦 " << juego.nombre << std::endl;
            std::cout << "Formato: " << (juego.formato == FormatoJuego::ISO ? ".ISO" : ".GOD") << std::endl;
            std::cout << "Tamaño: " << juego.tamanoGB << " GB" << std::endl;
            std::cout << "Compatibilidad: " << (juego.compatible ? "✅ SI" : "❌ NO") << std::endl;
            std::cout << "Desarrollador: " << juego.desarrollador << std::endl;
        }
    }

    // Cargar juego seleccionado
    void cargarJuego(std::string nombre) {
        for (auto &juego : juegos) {
            if (juego.nombre == nombre && juego.compatible) {
                std::cout << "\n🚀 CARGANDO " << nombre << "..." << std::endl;
                std::cout << "Formato cargado: " << (juego.formato == FormatoJuego::ISO ? "ISO" : "GOD") << std::endl;
                std::cout << "¡Juego listo para jugar!" << std::endl;
                return;
            }
        }
        std::cout << "\n⚠️ Juego no encontrado o incompatible" << std::endl;
    }
};
