#pragma once
#include "Nodo_arbol_pico.h"

class ArbolPicos {
public:
    ArbolPicos();
    ~ArbolPicos(); 

    void insertar(double tiempo, float amplitud);
    
    void imprimirPicos() const;

private:
    NodoArbolPico* m_raiz;

    void insertarRecursivo(NodoArbolPico*& nodo, double tiempo, float amplitud);
    void destruirRecursivo(NodoArbolPico* nodo);
    void imprimirRecursivo(NodoArbolPico* nodo) const;
};