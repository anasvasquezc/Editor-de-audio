#include "Arbol_picos.h"
#include <iostream>

ArbolPicos::ArbolPicos() : m_raiz(nullptr) {}

ArbolPicos::~ArbolPicos() {
    destruirRecursivo(m_raiz);
}

void ArbolPicos::insertar(double tiempo, float amplitud) {
    insertarRecursivo(m_raiz, tiempo, amplitud);
}

void ArbolPicos::imprimirPicos() const {
    if (m_raiz == nullptr) {
        std::cout << "No se encontraron picos." << std::endl;
    } else {
        std::cout << "Picos encontrados (Tiempo, Amplitud)" << std::endl;
        imprimirRecursivo(m_raiz);
    }
}

void ArbolPicos::insertarRecursivo(NodoArbolPico*& nodo, double tiempo, float amplitud) {
    if (nodo == nullptr) {
        nodo = new NodoArbolPico(tiempo, amplitud);
    }
    else if (tiempo < nodo->tiempoSegundo) {
        insertarRecursivo(nodo->izquierdo, tiempo, amplitud);
    }
    else {
        insertarRecursivo(nodo->derecho, tiempo, amplitud);
    }
}

void ArbolPicos::destruirRecursivo(NodoArbolPico* nodo) {
    if (nodo != nullptr) {
        destruirRecursivo(nodo->izquierdo);
        destruirRecursivo(nodo->derecho);
        delete nodo;
    }
}

void ArbolPicos::imprimirRecursivo(NodoArbolPico* nodo) const {
    if (nodo != nullptr) {
        imprimirRecursivo(nodo->izquierdo);
        std::cout << "  - " << nodo->tiempoSegundo << "s, " << nodo->amplitud << std::endl;
        imprimirRecursivo(nodo->derecho);
    }
}