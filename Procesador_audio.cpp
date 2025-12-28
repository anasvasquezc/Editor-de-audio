#include "Procesador_audio.h"
#include "kiss_fftr.h" // librería: kissfft
#include <cmath>
#include <iostream>
#include <vector>

void ProcesadorAudio::aplicarVolumen(PistaAudio& pista, float factor) {
    if (pista.getNumFrames() == 0) return;
    for (auto& canal : pista.getCanalesEditables()) {
        for (float& muestra : canal) {
            muestra *= factor;
        }
    }
}

void ProcesadorAudio::aplicarKaraoke(PistaAudio& pista) {
    if (pista.getNumCanales() != 2) {
        std::cerr << "Karaoke solo funciona en pistas de dos canales." << std::endl;
        return;
    }
    auto& canales = pista.getCanalesEditables();
    for (long long i = 0; i < pista.getNumFrames(); ++i) {
        float muestraMono = canales[0][i] - canales[1][i];
        canales[0][i] = muestraMono;
        canales[1][i] = muestraMono;
    }
}
void ProcesadorAudio::recortar(PistaAudio& pista, double tiempoInicio, double tiempoFin) {
    int sr = pista.getSampleRate();
    long long numFramesAntiguo = pista.getNumFrames();

    long long inicioSample = (long long)(tiempoInicio * sr);
    long long finSample = (long long)(tiempoFin * sr);

    if (inicioSample < 0) inicioSample = 0;
    if (finSample > numFramesAntiguo) finSample = numFramesAntiguo;

    if (inicioSample >= finSample || tiempoInicio >= tiempoFin) {
        std::cerr << "Rango de tiempo inválido." << std::endl;
        return;
    }

    std::vector<std::vector<float>> canalesNuevos;
    std::vector<std::vector<float>>& canalesAntiguos = pista.getCanalesEditables();

    for (const auto& canal : canalesAntiguos) {
        auto inicioIter = canal.begin() + inicioSample;
        auto finIter = canal.begin() + finSample;

        canalesNuevos.push_back(std::vector<float>(inicioIter, finIter));
    }

    canalesAntiguos = std::move(canalesNuevos);

    long long nuevosFrames = finSample - inicioSample;
    pista.setNumFrames(nuevosFrames);
}

std::vector<std::complex<float>> ProcesadorAudio::calcularFFT(const std::vector<float>& canal) {
    int nfft = canal.size();
    if (nfft == 0) {
        return {};
    }
    int nfft_out = (nfft / 2) + 1;

    kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 0, nullptr, nullptr);
    if (cfg == nullptr) {
        std::cerr << "Error al configurar kissfft" << std::endl;
        return {};
    }

    std::vector<kiss_fft_cpx> espectro(nfft_out);
    kiss_fftr(cfg, canal.data(), espectro.data());
    kiss_fftr_free(cfg);

    std::vector<std::complex<float>> resultado;
    resultado.reserve(nfft_out);
    for (const auto& cpx : espectro) {
        resultado.push_back(std::complex<float>(cpx.r, cpx.i));
    }
    return resultado;
}

ArbolPicos ProcesadorAudio::encontrarPicos(const PistaAudio& pista, float umbral) {
    ArbolPicos arbol;
    if (pista.getCanales().empty()) {
        return arbol;
    }
    const auto& canalCero = pista.getCanales()[0];
    for (long long i = 1; i < pista.getNumFrames() - 1; ++i) {
        float muestra = std::abs(canalCero[i]);
          if (muestra > umbral && muestra > std::abs(canalCero[i-1]) && muestra >= std::abs(canalCero[i+1])) {double tiempo = (double)i / pista.getSampleRate();
            arbol.insertar(tiempo, muestra);
        }
    }
    return arbol;
}