#include <iostream>
#include <string>
#include <vector>
#include <android/input.h> // Para detectar toques en Android
#include <android/os/Build.h> // Para compatibilidad con versiones
#include <android/hardware/vibrator.h> // Para retroalimentación háptica

// Estructura para botones virtuales con estado visual
struct BotonVirtual {
    std::string nombre;
    int xPos; // Posición en pantalla (píxeles)
    int yPos;
    int ancho;
    int alto;
    bool presionado;
    std::string colorNormal;
    std::string colorPresionado;
    std::string icono;
};

class ControlesEMULVA360X {
private:
    std::vector<BotonVirtual> botones;
    bool modoPantallaCompleta;
    // Puntero para controlar el vibrador del celular
    AVibrator* vibrador;

    // Método para inicializar el vibrador
    void inicializarVibrador() {
        if (android_get_device_api_level() >= 26) {
            vibrador = AVibrator_getService();
            std::cout << "🔊 RETROALIMENTACIÓN HÁPTICA ACTIVADA" << std::endl;
        } else {
            std::cout << "⚠️ Retroalimentación háptica no compatible (Android < 8.0)" << std::endl;
        }
    }

    // Método para vibrar brevemente al presionar un botón
    void vibrar() {
        if (vibrador != nullptr) {
            AVibrator_vibrate(vibrador, 50); // Vibra 50 milisegundos
        }
    }

    // Método para dibujar un botón según su estado
    void dibujarBoton(const BotonVirtual& boton) {
        std::cout << "\n[" << boton.nombre << "] ";
        if (boton.presionado) {
            std::cout << "🟥 " << boton.colorPresionado << " | " << boton.icono << " (PRESIONADO)";
        } else {
            std::cout << "🟩 " << boton.colorNormal << " | " << boton.icono << " (LISTO)";
        }
        std::cout << " | Posición: (" << boton.xPos << "," << boton.yPos << ")" << std::endl;
    }

    // Método para detectar si un toque está dentro de un botón
    bool detectarToqueEnBoton(int xToque, int yToque, BotonVirtual& boton) {
        return (xToque >= boton.xPos && xToque <= boton.xPos + boton.ancho &&
                yToque >= boton.yPos && yToque <= boton.yPos + boton.alto);
    }

public:
    // Constructor: Inicializa controles con efectos
    ControlesEMULVA360X() {
        modoPantallaCompleta = true;
        inicializarVibrador();

        // Definir botones del control Xbox 360
        botones = {
            {"A", 700, 500, 80, 80, false, "VERDE", "OSCURO_VERDE", "🅰️"},
            {"B", 780, 420, 80, 80, false, "ROJO", "OSCURO_ROJO", "🅱️"},
            {"X", 620, 420, 80, 80, false, "AZUL", "OSCURO_AZUL", "❎"},
            {"Y", 700, 340, 80, 80, false, "AMARILLO", "OSCURO_AMARILLO", "🅾️"},
            {"START", 450, 420, 60, 30, false, "GRIS", "OSCURO_GRIS", "▶️"},
            {"SELECT", 350, 420, 60, 30, false, "GRIS", "OSCURO_GRIS", "⏸️"},
            {"IZQ_ARRIBA", 150, 340, 60, 60, false, "NEGRO", "GRIS", "⬆️"},
            {"IZQ_ABAJO", 150, 460, 60, 60, false, "NEGRO", "GRIS", "⬇️"},
            {"IZQ_IZQ", 90, 400, 60, 60, false, "NEGRO", "GRIS", "⬅️"},
            {"IZQ_DERECHA", 210, 400, 60, 60, false, "NEGRO", "GRIS", "➡️"}
        };
    }

    // Método para actualizar estado de los botones al detectar un toque
    void actualizarControles(int xToque, int yToque, bool toqueActivo) {
        for (auto& boton : botones) {
            if (detectarToqueEnBoton(xToque, yToque, boton)) {
                boton.presionado = toqueActivo;
                if (toqueActivo) {
                    vibrar(); // Vibra al presionar
                    std::cout << "🎮 Botón " << boton.nombre << " PRESIONADO" << std::endl;
                } else {
                    std::cout << "🎮 Botón " << boton.nombre << " SOLTADO" << std::endl;
                }
            }
        }
        dibujarTodosLosBotones();
    }

    // Método para dibujar todos los controles
    void dibujarTodosLosBotones() {
        system("clear");
        std::cout << "=====================================" << std::endl;
        std::cout << "          🎮 CONTROLES VIRTUALES        " << std::endl;
        std::cout << "=====================================" << std::endl;
        for (const auto& boton : botones) {
            dibujarBoton(boton);
        }
    }

    // Método para obtener el estado de un botón
    bool getEstadoBoton(const std::string& nombre) {
        for (const auto& boton : botones) {
            if (boton.nombre == nombre) {
                return boton.presionado;
            }
        }
        return false;
    }
};
