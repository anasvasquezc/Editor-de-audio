#pragma once

struct NodoArbolPico {
    double tiempoSegundo;
    float amplitud;

    NodoArbolPico* izquierdo;
    NodoArbolPico* derecho;

    NodoArbolPico(double tiempo, float amp)
        : tiempoSegundo(tiempo), amplitud(amp), izquierdo(nullptr), derecho(nullptr) {}
};