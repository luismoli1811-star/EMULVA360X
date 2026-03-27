#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib> // Para acceder a variables del sistema Android

class GestorCarpetasEMULVA360X {
private:
    // Rutas base según estándares de Android
    std::string rutaInterna;
    std::string rutaExterna;
    const std::string NOMBRE_APP = "com.emulva360x.emulador";

    // Método para crear una carpeta individual
    void crearCarpeta(const std::string& ruta) {
        std::string comando = "mkdir -p \"" + ruta + "\"";
        system(comando.c_str());
        std::cout << "📂 Carpeta creada: " << ruta << std::endl;
    }

public:
    // Constructor: Detecta rutas y crea estructura completa
    GestorCarpetasEMULVA360X() {
        // Obtener rutas base del sistema Android
        rutaInterna = std::string(getenv("ANDROID_DATA")) + "/data/" + NOMBRE_APP + "/";
        rutaExterna = std::string(getenv("ANDROID_STORAGE")) + "/emulated/0/Xbox 360/";

        // Crear TODAS las carpetas en orden
        crearEstructuraCompleta();
        std::cout << "✅ ESTRUCTURA DE CARPETAS DEL EMULADOR CREADA" << std::endl;
    }

    // Método principal: Crea toda la jerarquía
    void crearEstructuraCompleta() {
        // --------------------------
        // CARPETAS INTERNAS (PROTEGIDAS)
        // --------------------------
        crearCarpeta(rutaInterna + "Configuracion/");
        crearCarpeta(rutaInterna + "Cache/");
        crearCarpeta(rutaInterna + "Temporal/");
        crearCarpeta(rutaInterna + "Logs/");

        // --------------------------
        // CARPETAS EXTERNAS (ACCESIBLES)
        // --------------------------
        crearCarpeta(rutaExterna);
        crearCarpeta(rutaExterna + "Juegos/");
        crearCarpeta(rutaExterna + "Firmware/");
        crearCarpeta(rutaExterna + "Guardados/");
        crearCarpeta(rutaExterna + "Capturas/");
    }

    // Obtener rutas para otros módulos
    std::string getRutaInterna() { return rutaInterna; }
    std::string getRutaExterna() { return rutaExterna; }
};
