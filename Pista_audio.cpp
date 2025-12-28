#include "Pista_audio.h"
#include <sndfile.h> // librería libsndfile
#include <iostream>
#include <vector>

PistaAudio::PistaAudio() : m_sampleRate(0), m_numFrames(0) {}
void PistaAudio::setPicosDetectados(const ArbolPicos& picos) {
    m_picosDetectados = picos;
}
bool PistaAudio::cargarDesdeArchivo(const std::string& ruta) {
    SF_INFO sfInfo;
    SNDFILE* archivo = sf_open(ruta.c_str(), SFM_READ, &sfInfo);
    if (!archivo) {
        std::cerr << "Error al abrir el archivo" << ruta << std::endl;
        return false;
    }

    this->m_sampleRate = sfInfo.samplerate;
    this->m_numFrames = sfInfo.frames;
    int numCanales = sfInfo.channels;

    std::vector<float> bufferIntercalado(m_numFrames * numCanales);
    sf_readf_float(archivo, bufferIntercalado.data(), m_numFrames);

    m_canales.clear();
    m_canales.resize(numCanales);
    for (int c = 0; c < numCanales; ++c) {
        m_canales[c].resize(m_numFrames);
    }

    for (long long i = 0; i < m_numFrames; ++i) {
        for (int c = 0; c < numCanales; ++c) {
            m_canales[c][i] = bufferIntercalado[i * numCanales + c];
        }
    }

    sf_close(archivo);
    return true;
}

bool PistaAudio::guardarEnArchivo(const std::string& ruta) {
    if (m_canales.empty() || m_numFrames == 0) {
        std::cerr << "No hay datos de audio para guardar." << std::endl;
        return false;
    }

    int numCanales = m_canales.size();

    SF_INFO sfInfo;
    sfInfo.frames = m_numFrames;
    sfInfo.samplerate = m_sampleRate;
    sfInfo.channels = numCanales;
    sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // WAV 16-bit

    SNDFILE* archivo = sf_open(ruta.c_str(), SFM_WRITE, &sfInfo);
    if (!archivo) {
        std::cerr << "Error al abrir el archivo" << ruta << std::endl;
        return false;
    }

    std::vector<float> bufferIntercalado(m_numFrames * numCanales);
    for (long long i = 0; i < m_numFrames; ++i) {
        for (int c = 0; c < numCanales; ++c) {
            bufferIntercalado[i * numCanales + c] = m_canales[c][i];
        }
    }

    sf_writef_float(archivo, bufferIntercalado.data(), m_numFrames);

    sf_close(archivo);
    return true;
}