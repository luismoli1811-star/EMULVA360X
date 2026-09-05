#include "InterfazUsuario.h"
#include "GestorJuegosEMULVA360X.h"
#include "ControlesEMULVA360X.h"
#include <cstring>
#include <vector>

// Constructor — inicializa todo
InterfazUsuario::InterfazUsuario() {
    inicializada = false;
    pantalla_actual = PANTALLA_INICIO;
    menu_seleccionado = 0;
    elemento_seleccionado = 0;
    total_elementos = 0;
    modo_pantalla_completa = true;
    memset(texto_cabecera, 0, sizeof(texto_cabecera));
    memset(texto_estado, 0, sizeof(texto_estado));
    lista_elementos.clear();
    gestor_juegos = nullptr;
    controles = nullptr;
}

// Destructor — limpia todo
InterfazUsuario::~InterfazUsuario() {
    lista_elementos.clear();
}

// Conectar los módulos necesarios
void InterfazUsuario::conectarModulos(
    GestorJuegosEMULVA360X* juegos,
    ControlesEMULVA360X* ctrl) {
    gestor_juegos = juegos;
    controles = ctrl;
}

// Inicializar la interfaz
bool InterfazUsuario::inicializar() {
    // Configurar dimensiones según resolución
    ancho_pantalla = 1280;
    alto_pantalla = 720;
    
    // Tamaños de fuente y colores
    color_fondo = 0xFF1A1A1A;      // Negro oscuro
    color_texto = 0xFFFFFFFF;      // Blanco
    color_seleccion = 0xFFFF8800;  // Naranja (color Xbox)
    color_cabecera = 0xFF2D2D2D;   // Gris oscuro
    
    // Cargar pantalla de inicio
    cambiarPantalla(PANTALLA_INICIO);
    
    inicializada = true;
    return true;
}

// — GESTIÓN DE PANTALLAS —

void InterfazUsuario::cambiarPantalla(uint32_t nueva_pantalla) {
    pantalla_actual = nueva_pantalla;
    menu_seleccionado = 0;
    elemento_seleccionado = 0;
    lista_elementos.clear();
    
    switch (pantalla_actual) {
        case PANTALLA_INICIO:
            strcpy(texto_cabecera, "EMULVA360X — Emulador Xbox 360");
            cargarMenuPrincipal();
            break;
        case PANTALLA_CARGAR_JUEGO:
            strcpy(texto_cabecera, "Seleccionar Juego");
            cargarListaJuegos();
            break;
        case PANTALLA_CONFIGURACION:
            strcpy(texto_cabecera, "Configuración");
            cargarMenuConfiguracion();
            break;
        case PANTALLA_CONTROLES:
            strcpy(texto_cabecera, "Controles");
            cargarMenuControles();
            break;
        case PANTALLA_SOBRE:
            strcpy(texto_cabecera, "Acerca de");
            cargarPantallaAcercaDe();
            break;
        case PANTALLA_JUEGO_CARGADO:
            strcpy(texto_cabecera, "Juego — En Ejecución");
            break;
    }
}

// — CARGA DE MENÚS —

void InterfazUsuario::cargarMenuPrincipal() {
    agregarElemento("Cargar Juego", PANTALLA_CARGAR_JUEGO);
    agregarElemento("Configuración", PANTALLA_CONFIGURACION);
    agregarElemento("Controles", PANTALLA_CONTROLES);
    agregarElemento("Acerca de", PANTALLA_SOBRE);
    agregarElemento("Salir", 0);
    total_elementos = (uint32_t)lista_elementos.size();
}

void InterfazUsuario::cargarListaJuegos() {
    if (!gestor_juegos) {
        agregarElemento("Error: Gestor de juegos no disponible", 0);
        return;
    }
    
    // Obtener lista de juegos desde el gestor
    agregarElemento("← Volver", PANTALLA_INICIO);
    agregarElemento("🔄 Actualizar lista", 1);
    
    // Aquí se llenaría con los juegos reales encontrados
    // Simulación hasta que se conecte completamente
    agregarElemento("Ejemplo: Juego 1.iso", 2);
    agregarElemento("Ejemplo: Juego 2.god", 3);
    
    total_elementos = (uint32_t)lista_elementos.size();
}

void InterfazUsuario::cargarMenuConfiguracion() {
    agregarElemento("Resolución: 1280x720", 1);
    agregarElemento("Pantalla completa: Sí", 2);
    agregarElemento("Vsync: Activado", 3);
    agregarElemento("Audio: Activado", 4);
    agregarElemento("Ruta de juegos: /storage/emulated/0/ROMs/", 5);
    agregarElemento("← Volver", PANTALLA_INICIO);
    total_elementos = (uint32_t)lista_elementos.size();
}

void InterfazUsuario::cargarMenuControles() {
    agregarElemento("Mando 1: Conectado", 1);
    agregarElemento("Mando 2: Desconectado", 2);
    agregarElemento("Mando 3: Desconectado", 3);
    agregarElemento("Mando 4: Desconectado", 4);
    agregarElemento("Vibración: Activada", 5);
    agregarElemento("← Volver", PANTALLA_INICIO);
    total_elementos = (uint32_t)lista_elementos.size();
}

void InterfazUsuario::cargarPantallaAcercaDe() {
    agregarElemento("EMULVA360X — Emulador Xbox 360 para Android", 0);
    agregarElemento("Versión: 1.0.0 (Experimental)", 0);
    agregarElemento("", 0);
    agregarElemento("Creado desde cero — código propio", 0);
    agregarElemento("Formatos soportados: ISO, GOD", 0);
    agregarElemento("", 0);
    agregarElemento("← Volver", PANTALLA_INICIO);
    total_elementos = (uint32_t)lista_elementos.size();
}

// — ELEMENTOS DE LISTA —

void InterfazUsuario::agregarElemento(const char* texto, uint32_t id_accion) {
    ElementoMenu elem;
    strncpy(elem.texto, texto, sizeof(elem.texto) - 1);
    elem.texto[sizeof(elem.texto) - 1] = 0;
    elem.id_accion = id_accion;
    lista_elementos.push_back(elem);
}

void InterfazUsuario::limpiarLista() {
    lista_elementos.clear();
    elemento_seleccionado = 0;
    total_elementos = 0;
}

// — NAVEGACIÓN —

void InterfazUsuario::navegarArriba() {
    if (elemento_seleccionado > 0) {
        elemento_seleccionado--;
    }
}

void InterfazUsuario::navegarAbajo() {
    if (elemento_seleccionado < total_elementos - 1) {
        elemento_seleccionado++;
    }
}

uint32_t InterfazUsuario::seleccionarElemento() {
    if (elemento_seleccionado >= lista_elementos.size()) return 0;
    return lista_elementos[elemento_seleccionado].id_accion;
}

void InterfazUsuario::volverAtras() {
    if (pantalla_actual != PANTALLA_INICIO) {
        cambiarPantalla(PANTALLA_INICIO);
    }
}

// — ACTUALIZACIÓN Y RENDERIZADO —

void InterfazUsuario::actualizar() {
    if (!inicializada) return;
    
    // Actualizar barra de estado según contexto
    char estado[128];
   
