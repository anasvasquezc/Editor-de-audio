#pragma once
#include <string>
#include <vector>
#include "Arbol_picos.h"

class PistaAudio {
private:
    std::vector<std::vector<float>> m_canales;
    int m_sampleRate;
    long long m_numFrames;
    ArbolPicos m_picosDetectados;

public:
    PistaAudio();

    bool cargarDesdeArchivo(const std::string& ruta);
    bool guardarEnArchivo(const std::string& ruta);

    int getSampleRate() const { return m_sampleRate; }
    long long getNumFrames() const { return m_numFrames; }
    int getNumCanales() const { return m_canales.size(); }
    const std::vector<std::vector<float>>& getCanales() const { return m_canales; }

    std::vector<std::vector<float>>& getCanalesEditables() { return m_canales; }

    const ArbolPicos& getPicosDetectados() const { return m_picosDetectados; }

    void setPicosDetectados(const ArbolPicos&);

    void setNumFrames(long long newNumFrames) { m_numFrames = newNumFrames; }

    PistaAudio& operator=(const PistaAudio& otra) {
        if (this != &otra) {
            m_canales = otra.m_canales;
            m_sampleRate = otra.m_sampleRate;
            m_numFrames = otra.m_numFrames;
            m_picosDetectados = ArbolPicos();
        }
        return *this;
    }

    PistaAudio(const PistaAudio& otra) {
        m_canales = otra.m_canales;
        m_sampleRate = otra.m_sampleRate;
        m_numFrames = otra.m_numFrames;
        m_picosDetectados = ArbolPicos();
    }

};