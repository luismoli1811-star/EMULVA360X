#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Enumeradores para componentes graficos
enum class TipoGPU { XENON, CUSTOM };
enum class Resolucion { P_480P, P_720P, P_1080P, AUTO };
enum class EfectoGrafico { ANTIALIASING, SOMBREADO, TEXTURAS_ALTA, DESCONOCIDO };
enum class CalidadVideo { BAJA, MEDIA, ALTA, MAXIMA };

class GraficaEMULVA360X {
private:
    // Componentes internos de la GPU
    TipoGPU tipoGPU;
    Resolucion resolucionActual;
    CalidadVideo calidad;
    std::vector<EfectoGrafico> efectosActivos;
    bool modoPantallaCompleta;
    float escalaPantalla; // Para adaptarse a celulares

    // Metodo privado: Calcular escala segun pantalla del celular
    float calcularEscala(float resCelular, float resEmulador) {
        return resCelular / resEmulador;
    }

public:
    // Constructor
    GraficaEMULVA360X() {
        tipoGPU = TipoGPU::XENON; // Igual que Xbox 360 original
        resolucionActual = Resolucion::P_720P; // Resolucion base de Xbox 360
        calidad = CalidadVideo::MEDIA;
        modoPantallaCompleta = false;
        escalaPantalla = 1.0f;
    }

    // Iniciar GPU del emulador
    void iniciarGPU() {
        std::cout << "\n=== INICIANDO MODULO DE GRAFICA EMULVA360X ===" << std::endl;
        std::cout << "GPU: XENON - Compatible con DirectX 9.0c" << std::endl;
        std::cout << "Resolucion base: 720P (1280x720) - Igual que Xbox 360 original" << std::endl;
    }

    // Configurar resolucion
    void configurarResolucion(Resolucion nuevaRes) {
        resolucionActual = nuevaRes;
        float resX = 0, resY = 0;

        if (nuevaRes == Resolucion::P_480P) { resX = 640; resY = 480; }
        else if (nuevaRes == Resolucion::P_720P) { resX = 1280; resY = 720; }
        else if (nuevaRes == Resolucion::P_1080P) { resX = 1920; resY = 1080; }
        else if (nuevaRes == Resolucion::AUTO) {
            // Adaptar a pantalla del celular (ejemplo)
            resX = 1080; resY = 2340; // Resolucion comun en celulares
            std::cout << "Resolucion adaptada a pantalla del dispositivo!" << std::endl;
        }

        std::cout << "Resolucion configurada: " << resX << "x" << resY << std::endl;
    }

    // Activar efectos graficos
    void activarEfectos(std::vector<EfectoGrafico> efectos) {
        efectosActivos = efectos;
        std::cout << "\nEFECTOS ACTIVADOS:" << std::endl;
        for (auto ef : efectos) {
            if (ef == EfectoGrafico::ANTIALIASING) std::cout << "- Anti-Aliasing Mejorado" << std::endl;
            else if (ef == EfectoGrafico::SOMBREADO) std::cout << "- Sombreado Dinamico" << std::endl;
            else if (ef == EfectoGrafico::TEXTURAS_ALTA) std::cout << "- Texturas de Alta Calidad" << std::endl;
        }
    }

    // Mostrar estado de la GPU
    void mostrarEstadoGPU() {
        std::cout << "\n--- ESTADO GPU EMULVA360X ---" << std::endl;
        std::cout << "GPU: XENON (Xbox 360 Original)" << std::endl;
        std::cout << "Resolucion: " << resX << "x" << resY << std::endl;
        std::cout << "Calidad: " << (calidad == CalidadVideo::ALTA ? "Alta" : (calidad == CalidadVideo::MEDIA ? "Media" : "Baja")) << std::endl;
        std::cout << "Efectos activos: " << efectosActivos.size() << std::endl;
    }
};
