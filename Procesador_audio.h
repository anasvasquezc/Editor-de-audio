#pragma once
#include "Pista_audio.h"
#include "Arbol_picos.h"
#include <vector>
#include <complex>

class ProcesadorAudio {
public:
    static void aplicarVolumen(PistaAudio& pista, float factor);
    static void aplicarKaraoke(PistaAudio& pista);
    static void recortar(PistaAudio& pista, double tiempoInicio, double tiempoFin);

    static std::vector<std::complex<float>> calcularFFT(const std::vector<float>& canal);


    static ArbolPicos encontrarPicos(const PistaAudio& pista, float umbral);
};