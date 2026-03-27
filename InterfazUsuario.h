#include <iostream>
#include <string>
#include <vector>
#include "GestorJuegosEMULVA360X.h"
#include "ModuloFirmware.h"

// Enumeradores para secciones de la interfaz
enum class SeccionUI { BIENVENIDA, MENU_PRINCIPAL, LISTA_JUEGOS, CONFIGURACION, SALIR };

class InterfazUsuario {
private:
    SeccionUI seccionActual;
    std::string tituloEmulador = "EMULVA360X";
    bool mostrarAnimaciones;

    // Método para dibujar líneas decorativas
    void dibujarLinea() {
        std::cout << "\n══════════════════════════════════════════════════" << std::endl;
    }

    // Método para mostrar pantalla de bienvenida
    void mostrarPantallaBienvenida() {
        dibujarLinea();
        std::cout << "          🎮 ¡BIENVENIDO A EMULVA360X! 🎮" << std::endl;
        std::cout << "      Versión: 2.0.0 | Firmware: v2.0.0" << std::endl;
        std::cout << "  Compatible con Android 5.0+ | Optimizado para ARM64" << std::endl;
        dibujarLinea();
        std::cout << "\n👉 Presiona cualquier tecla para continuar al menú..." << std::endl;
        // Simular espera de input del usuario
        std::cin.get();
        seccionActual = SeccionUI::MENU_PRINCIPAL;
    }

    // Método para mostrar menú principal
    void mostrarMenuPrincipal() {
        system("clear"); // Limpiar pantalla (adaptable a Android)
        dibujarLinea();
        std::cout << "                📋 MENÚ PRINCIPAL" << std::endl;
        dibujarLinea();
        std::cout << " [1] 🎮 Mis Juegos" << std::endl;
        std::cout << " [2] ⚙️ Configuración" << std::endl;
        std::cout << " [3] 🔧 Gestión de Firmware" << std::endl;
        std::cout << " [4] ❌ Salir del Emulador" << std::endl;
        dibujarLinea();
        std::cout << "Elige una opción: ";
    }

    // Método para mostrar lista de juegos con interfaz amigable
    void mostrarListaJuegos(GestorJuegosEMULVA360X& gestor) {
        system("clear");
        dibujarLinea();
        std::cout << "                🕹️ LISTA DE JUEGOS DETECTADOS" << std::endl;
        dibujarLinea();
        
        auto juegos = gestor.getListaJuegos(); // Método agregado al gestor para obtener juegos
        for (int i = 0; i < juegos.size(); i++) {
            std::cout << " [" << i+1 << "] " << juegos[i].nombre << " | " 
                      << juegos[i].formato << " | Tamaño: " << juegos[i].tamanoMB << " GB" << std::endl;
        }
        
        dibujarLinea();
        std::cout << "\n [0] Volver al menú principal" << std::endl;
        std::cout << "Elige una opción: ";
    }

public:
    // Constructor
    InterfazUsuario() {
        seccionActual = SeccionUI::BIENVENIDA;
        mostrarAnimaciones = true;
        std::cout << "🖥️ INTERFAZ DE USUARIO INICIALIZADA" << std::endl;
    }

    // Método principal para ejecutar la interfaz
    void ejecutarInterfaz(GestorJuegosEMULVA360X& gestor) {
        int opcion;
        do {
            switch(seccionActual) {
                case SeccionUI::BIENVENIDA:
                    mostrarPantallaBienvenida();
                    break;
                
                case SeccionUI::MENU_PRINCIPAL:
                    mostrarMenuPrincipal();
                    std::cin >> opcion;
                    switch(opcion) {
                        case 1: seccionActual = SeccionUI::LISTA_JUEGOS; break;
                        case 2: seccionActual = SeccionUI::CONFIGURACION; break;
                        case 3: seccionActual = SeccionUI::FIRMWARE; break;
                        case 4: seccionActual = SeccionUI::SALIR; break;
                        default: std::cout << "⚠️ Opción no válida!" << std::endl;
                    }
                    break;
                
                case SeccionUI::LISTA_JUEGOS:
                    mostrarListaJuegos(gestor);
                    std::cin >> opcion;
                    if (opcion == 0) seccionActual = SeccionUI::MENU_PRINCIPAL;
                    break;
                
                case SeccionUI::CONFIGURACION:
                    system("clear");
                    dibujarLinea();
                    std::cout << "                ⚙️ CONFIGURACIÓN DEL EMULADOR" << std::endl;
                    dibujarLinea();
                    std::cout << " [1] Ajustes de gráfica | [2] Controles virtuales" << std::endl;
                    std::cout << " [3] Optimización de rendimiento | [4] Volver" << std::endl;
                    std::cin >> opcion;
                    if (opcion == 4) seccionActual = SeccionUI::MENU_PRINCIPAL;
                    break;
                
                case SeccionUI::FIRMWARE:
                    system("clear");
                    dibujarLinea();
                    std::cout << "                🔧 GESTIÓN DE FIRMWARE" << std::endl;
                    dibujarLinea();
                    std::cout << " [1] Ver versión instalada | [2] Buscar actualizaciones" << std::endl;
                    std::cout << " [3] Reinstalar firmware | [4] Volver" << std::endl;
                    std::cin >> opcion;
                    if (opcion == 4) seccionActual = SeccionUI::MENU_PRINCIPAL;
                    break;
            }
        } while (seccionActual != SeccionUI::SALIR);

        // Pantalla de despedida
        dibujarLinea();
        std::cout << "          🎉 ¡GRACIAS POR USAR EMULVA360X!" << std::endl;
        std::cout << "            🔄 ¡Nos vemos en la próxima actualización!" << std::endl;
        dibujarLinea();
    }
};
