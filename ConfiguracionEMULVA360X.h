#include <iostream>
#include <string>
#include <vector>

// Enumeradores para opciones específicas
enum class NivelRendimiento { BAJO = 1, MEDIO = 2, ALTO = 3, MAXIMO = 4 };
enum class ResolucionPantalla { RES_720P = 1, RES_1080P = 2, RES_2K = 3, RES_NATIVA = 4 };
enum class CalidadTexturas { BAJA = 1, MEDIA = 2, ALTA = 3, ULTRA = 4 };
enum class BackendGrafico { VULKAN = 1, OPENGL_ES = 2, METAL = 3 };
enum class CalidadAudio { MONO = 1, ESTEREO = 2, SURROUND_51 = 3, SURROUND_71 = 4 };
enum class TipoControl { VIRTUAL = 1, BLUETOOTH = 2, USB = 3, MANDO_XBOX = 4 };

// Clase para la configuración de EMULVA360X
class ConfiguracionEMULVA360X {
private:
    // Configuración general
    std::string nombreUsuario;
    NivelRendimiento rendimientoGlobal;
    std::string rutaJuegos;
    std::string rutaGuardados;
    bool modoOscuro;
    bool inicioRapido;

    // Configuración gráficos
    BackendGrafico backendGrafico;
    ResolucionPantalla resolucion;
    CalidadTexturas calidadTexturas;
    bool activarSombreado;
    bool activarAntiAliasing;
    int nivelAntiAliasing;
    bool modoPantallaCompleta;
    int brillo;
    int contraste;

    // Configuración audio
    CalidadAudio calidadAudio;
    float volumenMaestro;
    float volumenJuego;
    float volumenMusica;
    float volumenEfectos;
    bool efectosEspaciales;
    bool reducirRetrasoAudio;

    // Configuración entrada
    TipoControl controlPrincipal;
    bool invertirEjes;
    int sensibilidadMando;
    bool vibracionActivada;
    int intensidadVibracion;
    std::vector<std::string> controlesPersonalizados;

    // Configuración red
    bool conexionXboxLive;
    std::string servidorPersonalizado;
    int puertoConexion;
    bool compresionDatos;
    bool modoOffline;

    // Configuración guardado
    bool guardadoAutomatico;
    int intervaloGuardado;
    int numeroSlotsGuardado;
    bool sincronizarNube;
    std::string rutaNube;

public:
    // Constructor
    ConfiguracionEMULVA360X() {
        // Valores iniciales
        nombreUsuario = "UsuarioEMULVA";
        rendimientoGlobal = NivelRendimiento::MEDIO;
        rutaJuegos = "/Almacenamiento/EMULVA360X/Juegos/";
        rutaGuardados = "/Almacenamiento/EMULVA360X/Guardados/";
        modoOscuro = true;
        inicioRapido = false;

        // Gráficos iniciales
        backendGrafico = BackendGrafico::VULKAN;
        resolucion = ResolucionPantalla::RES_720P;
        calidadTexturas = CalidadTexturas::MEDIA;
        activarSombreado = true;
        activarAntiAliasing = true;
        nivelAntiAliasing = 2;
        modoPantallaCompleta = true;
        brillo = 80;
        contraste = 70;

        // Audio inicial
        calidadAudio = CalidadAudio::ESTEREO;
        volumenMaestro = 0.8f;
        volumenJuego = 0.9f;
        volumenMusica = 0.7f;
        volumenEfectos = 0.9f;
        efectosEspaciales = false;
        reducirRetrasoAudio = true;

        // Entrada inicial
        controlPrincipal = TipoControl::VIRTUAL;
        invertirEjes = false;
        sensibilidadMando = 5;
        vibracionActivada = true;
        intensidadVibracion = 60;
        controlesPersonalizados = {"A = Saltar", "B = Disparar", "X = Interactuar"};

        // Red inicial
        conexionXboxLive = false;
        servidorPersonalizado = "emulva360x.servidorlocal";
        puertoConexion = 3000;
        compresionDatos = true;
        modoOffline = true;

        // Guardado inicial
        guardadoAutomatico = true;
        intervaloGuardado = 15;
        numeroSlotsGuardado = 10;
        sincronizarNube = false;
        rutaNube = "/Almacenamiento/EMULVA360X/Nube/";
    }

    // Método para mostrar configuración completa
    void mostrarConfiguracion() {
        std::cout << "\n=====================================" << std::endl;
        std::cout << "===== CONFIGURACIÓN EMULVA360X ======" << std::endl;
        std::cout << "=====================================\n" << std::endl;

        std::cout << "--- GENERAL ---" << std::endl;
        std::cout << "Usuario: " << nombreUsuario << std::endl;
        std::cout << "Rendimiento: " << static_cast<int>(rendimientoGlobal) << std::endl;
        std::cout << "Ruta juegos: " << rutaJuegos << std::endl;
        std::cout << "Ruta guardados: " << rutaGuardados << "\n" << std::endl;

        std::cout << "--- GRÁFICOS ---" << std::endl;
        std::cout << "Backend: " << static_cast<int>(backendGrafico) << std::endl;
        std::cout << "Resolución: " << static_cast<int>(resolucion) << std::endl;
        std::cout << "Texturas: " << static_cast<int>(calidadTexturas) << "\n" << std::endl;

        std::cout << "--- AUDIO ---" << std::endl;
        std::cout << "Calidad: " << static_cast<int>(calidadAudio) << std::endl;
        std::cout << "Volumen maestro: " << volumenMaestro * 100 << "%" << "\n" << std::endl;

        // ... y así sucesivamente con el resto de secciones
    }
};
