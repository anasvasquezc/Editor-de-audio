#pragma once
#include "Pista_audio.h"
#include <stack>
#include <string>
#include <vector>
#include <complex>

class EditorLogica {
private:
    PistaAudio m_pistaActual;
    std::stack<PistaAudio> m_historialUndo;

    void guardarEstadoParaUndo();

public:
    EditorLogica();
    void recortarAudio(double tiempoInicio, double tiempoFin);
    void cargarArchivo(const std::string& ruta);
    void guardarEnArchivo(const std::string& ruta);
    void aplicarVolumen(float factor);
    void aplicarKaraoke();
    void deshacer();
    void analizarPicos(float umbral);
    std::vector<std::complex<float>> obtenerFFT(int canal);

    const PistaAudio& getPistaActual() const { return m_pistaActual; }
};