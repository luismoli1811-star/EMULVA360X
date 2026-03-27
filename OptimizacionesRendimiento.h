#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime> // Para medir tiempos de ejecución
#include <cstdlib> // Para obtener datos del hardware

// Enumeradores para tipos de optimización
enum class TipoOptimizacion { AJUSTE_AUTO, REDUCCION_GPU, REDUCCION_CPU, LIMITADOR_FPS };
enum class NivelHardware { BAJO, MEDIO, ALTO, PREMIUM };
enum class EstadoOptimizacion { ACTIVA, DESACTIVADA, EN_PROCESO };

// Estructura para datos del hardware
struct DatosDispositivo {
    std::string modeloCelular;
    float ramTotalGB;
    float almacenamientoGB;
    int nucleosCPU;
    NivelHardware nivel;
};

// Clase principal de optimizaciones
class OptimizacionesRendimiento {
private:
    // Datos del dispositivo detectados automáticamente
    DatosDispositivo dispositivo;
    std::vector<TipoOptimizacion> optimizacionesActivas;
    bool modoEco;
    float umbralTemperatura; // Límite para evitar sobrecalentamiento
    int limiteFPS; // Máximo FPS permitido
    float factorEscalaGrafica;

    // Método privado: Detectar nivel de hardware
    void detectarNivelHardware() {
        // Simula lectura de datos reales del celular
        dispositivo.modeloCelular = std::string(getenv("MODEL
