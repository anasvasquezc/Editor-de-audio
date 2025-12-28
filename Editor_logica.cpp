#include "Editor_logica.h"
#include "Procesador_audio.h"
#include <iostream>
#include <algorithm>

EditorLogica::EditorLogica() {}

void EditorLogica::guardarEstadoParaUndo() {
    m_historialUndo.push(m_pistaActual);
}

void EditorLogica::cargarArchivo(const std::string& ruta) {
    if (m_pistaActual.cargarDesdeArchivo(ruta)) {
        m_historialUndo = std::stack<PistaAudio>();
        m_pistaActual.setPicosDetectados(ArbolPicos());
        std::cout << "Archivo cargado" << std::endl;
        std::cout << "Tasa de Muestreo: " << m_pistaActual.getSampleRate() << " Hz" << std::endl;
    } else {
        std::cerr << "No se pudo cargar el archivo." << std::endl;
    }
}

void EditorLogica::guardarEnArchivo(const std::string& ruta) {
    if (m_pistaActual.guardarEnArchivo(ruta)) {
        std::cout << "Archivo guardado en " << ruta << std::endl;
    } else {
        std::cerr << "No se pudo guardar el archivo." << std::endl;
    }
}

void EditorLogica::aplicarVolumen(float factor) {
    if (m_pistaActual.getNumFrames() == 0) {
        std::cerr << "No hay archivo cargado." << std::endl;
        return;
    }
    guardarEstadoParaUndo();
    ProcesadorAudio::aplicarVolumen(m_pistaActual, factor);
    std::cout << "Volumen aplicado." << std::endl;
}

void EditorLogica::aplicarKaraoke() {
    if (m_pistaActual.getNumFrames() == 0) {
        std::cerr << "No hay archivo cargado." << std::endl;
        return;
    }
    guardarEstadoParaUndo();
    ProcesadorAudio::aplicarKaraoke(m_pistaActual);
    std::cout << "Efecto karaoke aplicado." << std::endl;
}

void EditorLogica::deshacer() {
    if (!m_historialUndo.empty()) {
        m_pistaActual = m_historialUndo.top();
        m_historialUndo.pop();
        std::cout << "Operacion completada." << std::endl;
    } else {
        std::cout << "No hay nada que deshacer." << std::endl;
    }
}
void EditorLogica::recortarAudio(double tiempoInicio, double tiempoFin) {
    if (m_pistaActual.getNumFrames() == 0) {
        std::cerr << "No hay archivo cargado." << std::endl;
        return;
    }
    guardarEstadoParaUndo();

    ProcesadorAudio::recortar(m_pistaActual, tiempoInicio, tiempoFin);

    std::cout << "Audio recortado." << std::endl;
}

void EditorLogica::analizarPicos(float umbral) {
    if (m_pistaActual.getNumFrames() == 0) {
        std::cerr << "No hay archivo cargado." << std::endl;
        return;
    }
    ArbolPicos nuevosPicos = ProcesadorAudio::encontrarPicos(m_pistaActual, umbral);
    m_pistaActual.setPicosDetectados(nuevosPicos);

    std::cout << "Analisis de picos completado:" << std::endl;
    m_pistaActual.getPicosDetectados().imprimirPicos();
}

std::vector<std::complex<float>> EditorLogica::obtenerFFT(int canal) {
    if (m_pistaActual.getNumCanales() <= canal || m_pistaActual.getNumFrames() == 0) {
        return {};
    }

    const auto& datosCanal = m_pistaActual.getCanales()[canal];
    int sr = m_pistaActual.getSampleRate();

    size_t inicio = sr;
    size_t tamanoFFT = 4096;

    if (datosCanal.size() < inicio + tamanoFFT) {
        size_t tamanoReal = std::min((size_t)tamanoFFT, datosCanal.size());
        std::vector<float> segmento(datosCanal.begin(), datosCanal.begin() + tamanoReal);
        segmento.resize(tamanoFFT, 0.0f);
        return ProcesadorAudio::calcularFFT(segmento);
    }

    std::vector<float> segmento(datosCanal.begin() + inicio, datosCanal.begin() + inicio + tamanoFFT);

    return ProcesadorAudio::calcularFFT(segmento);
}