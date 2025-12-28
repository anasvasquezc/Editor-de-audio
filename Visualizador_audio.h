#pragma once
#include "Editor_logica.h"
#include <vector>

struct GLFWwindow;

class VisualizadorAudio {
public:
    VisualizadorAudio();

    void ejecutarGUI(EditorLogica& logica);

private:
    GLFWwindow* m_window;
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    bool inicializarGUI();
    void limpiarGUI();

    void DibujarPaneles(EditorLogica& logica);

    std::vector<float> m_datosTiempoY;
    std::vector<float> m_datosTiempoX;
    std::vector<float> m_datosFrecuenciaMagnitud;
    std::vector<float> m_datosFrecuenciaX;
    
    int m_canalSeleccionado;
    float m_factorVolumen;
    char m_rutaArchivo[256];
    float m_umbralPicos;

    void dibujarGraficoTiempo(const PistaAudio& pista, int canal);
    void dibujarGraficoFrecuencia(int sampleRate);
    void actualizarDatosGraficos(const PistaAudio& pista, int canal, EditorLogica& logica);
};