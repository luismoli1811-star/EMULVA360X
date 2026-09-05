#include "ControlesEMULVA360X.h"
#include <cstring>
#include <algorithm>

// Constructor — inicializa todo
ControlesEMULVA360X::ControlesEMULVA360X() {
    inicializada = false;
    cantidad_mandos = 4; // Xbox 360 soporta hasta 4 mandos
    memset(estado_mandos, 0, sizeof(estado_mandos));
    memset(mando_conectado, 0, sizeof(mando_conectado));
}

// Inicializa el sistema de controles
bool ControlesEMULVA360X::inicializar() {
    // Estado por defecto: solo el mando 1 conectado
    mando_conectado[0] = true;
    mando_conectado[1] = false;
    mando_conectado[2] = false;
    mando_conectado[3] = false;
    
    // Reiniciar todos los estados
    reiniciarTodosMandos();
    
    inicializada = true;
    return true;
}

// Reinicia el estado de un mando
void ControlesEMULVA360X::reiniciarMando(uint8_t indice) {
    if (indice >= cantidad_mandos) return;
    
    EstadoMando& m = estado_mandos[indice];
    
    // Botones digitales → todos sueltos
    m.botones = 0;
    
    // Gatillos → sin presionar (0 = sin presionar, 255 = al máximo)
    m.gatillo_L = 0;
    m.gatillo_R = 0;
    
    // Sticks → centro (eje X: -32768 a 32767, Y: -32768 a 32767)
    m.stick_L_X = 0;
    m.stick_L_Y = 0;
    m.stick_R_X = 0;
    m.stick_R_Y = 0;
    
    // D-Pad → centro
    m.cruceta = CRUCETA_CENTRO;
}

// Reinicia todos los mandos
void ControlesEMULVA360X::reiniciarTodosMandos() {
    for (uint8_t i = 0; i < cantidad_mandos; i++) {
        reiniciarMando(i);
    }
}

// Conectar o desconectar un mando
void ControlesEMULVA360X::conectarMando(uint8_t indice, bool conectado) {
    if (indice >= cantidad_mandos) return;
    mando_conectado[indice] = conectado;
    if (!conectado) reiniciarMando(indice);
}

// — BOTONES DIGITALES —

void ControlesEMULVA360X::presionarBoton(uint8_t indice, uint32_t boton) {
    if (!mando_conectado[indice]) return;
    estado_mandos[indice].botones |= boton;
}

void ControlesEMULVA360X::soltarBoton(uint8_t indice, uint32_t boton) {
    if (!mando_conectado[indice]) return;
    estado_mandos[indice].botones &= ~boton;
}

bool ControlesEMULVA360X::botonPresionado(uint8_t indice, uint32_t boton) {
    if (!mando_conectado[indice]) return false;
    return (estado_mandos[indice].botones & boton) != 0;
}

// — GATILLOS ANALÓGICOS —

void ControlesEMULVA360X::establecerGatillo(uint8_t indice, bool izquierdo, uint8_t valor) {
    if (!mando_conectado[indice]) return;
    valor = std::max((uint8_t)0, std::min(valor, (uint8_t)255));
    if (izquierdo) {
        estado_mandos[indice].gatillo_L = valor;
    } else {
        estado_mandos[indice].gatillo_R = valor;
    }
}

uint8_t ControlesEMULVA360X::obtenerGatillo(uint8_t indice, bool izquierdo) {
    if (!mando_conectado[indice]) return 0;
    return izquierdo ? estado_mandos[indice].gatillo_L : estado_mandos[indice].gatillo_R;
}

// — STICKS ANALÓGICOS —

void ControlesEMULVA360X::establecerStick(uint8_t indice, bool izquierdo, int16_t x, int16_t y) {
    if (!mando_conectado[indice]) return;
    
    // Limitar rango (-32768 a 32767)
    x = std::max(-32768, std::min(x, 32767));
    y = std::max(-32768, std::min(y, 32767));
    
    if (izquierdo) {
        estado_mandos[indice].stick_L_X = x;
        estado_mandos[indice].stick_L_Y = y;
    } else {
        estado_mandos[indice].stick_R_X = x;
        estado_mandos[indice].stick_R_Y = y;
    }
}

void ControlesEMULVA360X::obtenerStick(uint8_t indice, bool izquierdo, int16_t& x, int16_t& y) {
    if (!mando_conectado[indice]) {
        x = 0; y = 0;
        return;
    }
    if (izquierdo) {
        x = estado_mandos[indice].stick_L_X;
        y = estado_mandos[indice].stick_L_Y;
    } else {
        x = estado_mandos[indice].stick_R_X;
        y = estado_mandos[indice].stick_R_Y;
    }
}

// — CRUCETA —

void ControlesEMULVA360X::establecerCruceta(uint8_t indice, uint8_t direccion) {
    if (!mando_conectado[indice]) return;
    estado_mandos[indice].cruceta = direccion;
}

uint8_t ControlesEMULVA360X::obtenerCruceta(uint8_t indice) {
    if (!mando_conectado[indice]) return CRUCETA_CENTRO;
    return estado_mandos[indice].cruceta;
}

// — OBTENER ESTADO COMPLETO (para enviar al juego) —

EstadoMando ControlesEMULVA360X::obtenerEstadoMando(uint8_t indice) {
    if (indice >= cantidad_mandos || !mando_conectado[indice]) {
        EstadoMando vacio;
        memset(&vacio, 0, sizeof(vacio));
        return vacio;
    }
    return estado_mandos[indice];
}

// — MAPEO DE TECLADO / TÁCTIL DE ANDROID —

void ControlesEMULVA360X::mapearToqueAndroid(uint8_t indice, int id_toque, float x, float y, bool presionando) {
    // Convierte toques de pantalla táctil a botones del mando
    // Se llama desde la interfaz de Kotlin cuando el usuario toca la pantalla
    
    if (!mando_conectado[indice]) return;
    
    // Áreas de la pantalla → botones del mando
    // Lado izquierdo → stick izquierdo / cruceta
    // Lado derecho → botones A B X Y
    // Esquinas → gatillos, botones atrás, guía
    
    if (presionando) {
        // Botón A — abajo a la derecha
        if (x > 0.7f && y > 0.7f) presionarBoton(indice, BOTON_A);
        // Botón B — izquierda de A
        else if (x > 0.6f && x < 0.7f && y > 0.7f) presionarBoton(indice, BOTON_B);
        // Botón X — arriba de A
        else if (x > 0.7f && y > 0.55f && y < 0.7f) presionarBoton(indice, BOTON_X);
        // Botón Y — arriba de B
        else if (x > 0.6f && x < 0.7f && y > 0.55f && y < 0.7f) presionarBoton(indice, BOTON_Y);
        // Gatillo R — esquina superior derecha
        else if (x > 0.85f && y < 0.25f) establecerGatillo(indice, false, 255);
        // Gatillo L — esquina superior izquierda
        else if (x < 0.15f && y < 0.25f) establecerGatillo(indice, true, 255);
    } else {
        // Al soltar → soltar todos los botones asociados a ese toque
        soltarBoton(indice, BOTON_A | BOTON_B | BOTON_X | BOTON_Y);
        establecerGatillo(indice, true, 0);
        establecerGatillo(indice, false, 0);
    }
}

// — VIBRACIÓN —

void ControlesEMULVA360X::establecerVibracion(uint8_t indice, uint8_t velocidad_izq, uint8_t velocidad_der) {
    if (indice >= cantidad_mandos || !mando_conectado[indice]) return;
    
    // Guardar valores para que la interfaz de Android los use
    vibracion_izquierda[indice] = velocidad_izq;
    vibracion_derecha[indice] = velocidad_der;
    
    // Aquí la capa de Android activaría el vibrador del celular
    // según estos valores
}

// — ACTUALIZAR (se llama cada frame) —

void ControlesEMULVA360X::actualizar() {
    // Preparar el estado de todos los mandos para el siguiente frame
    // Limpia cambios temporales si es necesario
    for (uint8_t i = 0; i < cantidad_mandos; i++) {
        if (!mando_conectado[i]) continue;
        // El estado se mantiene hasta que se cambie explícitamente
    }
}
