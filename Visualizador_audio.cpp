#include "Visualizador_Audio.h"
#include "implot.h"
#include "imgui.h"
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

VisualizadorAudio::VisualizadorAudio() : 
    m_window(nullptr), // Inicializar el puntero a nullptr
    m_canalSeleccionado(0),
    m_factorVolumen(1.0f),
    m_umbralPicos(0.5f)
{
    m_rutaArchivo[0] = '\0';
}

bool VisualizadorAudio::inicializarGUI() {
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW.\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Visualizador de Audio", NULL, NULL);
    if (m_window == NULL) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext(); 
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); 
    
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330"); 
    
    return true;
}

void VisualizadorAudio::limpiarGUI() {
    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

void VisualizadorAudio::ejecutarGUI(EditorLogica& logica) {
    if (!inicializarGUI()) {
        return;
    }

    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();

        // 1. Iniciar el frame de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        DibujarPaneles(logica);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(m_window);
    }

    limpiarGUI();
    std::cout << "Se cerro la ventana de visualizacion.\n";
}


void VisualizadorAudio::DibujarPaneles(EditorLogica& logica) {
    ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin("Panel de Visualizacion de Audio");
    
    const PistaAudio& pista = logica.getPistaActual();
    
    if (pista.getNumFrames() > 0) {
        // Selector de canal
        if (pista.getNumCanales() > 1) {
            ImGui::Text("Seleccionar Canal:");
            ImGui::RadioButton("Izquierdo (0)", &m_canalSeleccionado, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Derecho (1)", &m_canalSeleccionado, 1);
        } else {
             ImGui::Text("Pista Mono (Canal 0)");
             m_canalSeleccionado = 0;
        }

        // 1. Prepara los datos (Tiempo y FFT)
        actualizarDatosGraficos(pista, m_canalSeleccionado, logica);
        
        // 2. Dibuja las gráficas
        dibujarGraficoTiempo(pista, m_canalSeleccionado);
        ImGui::Separator();
        dibujarGraficoFrecuencia(pista.getSampleRate());
        
    } else {
        ImGui::Text("Carga un archivo para visualizar las gráficas.");
    }
    
    ImGui::End();
}

void VisualizadorAudio::actualizarDatosGraficos(const PistaAudio& pista, int canal, EditorLogica& logica) {
    if (pista.getNumFrames() == 0 || pista.getNumCanales() <= canal) {
        m_datosTiempoY.clear();
        m_datosTiempoX.clear();
        m_datosFrecuenciaMagnitud.clear();
        m_datosFrecuenciaX.clear();
        return;
    }

    const auto& datosCanal = pista.getCanales()[canal];
    long long numFrames = pista.getNumFrames();
    int sampleRate = pista.getSampleRate();
    
    size_t segmentoFrames = (size_t)numFrames;

    m_datosTiempoY.resize(segmentoFrames);
    m_datosTiempoX.resize(segmentoFrames);
    
    for (size_t i = 0; i < segmentoFrames; ++i) {
        m_datosTiempoY[i] = datosCanal[i];
        m_datosTiempoX[i] = (float)i / sampleRate;
    }

    if (m_datosFrecuenciaMagnitud.empty() || ImGui::IsItemDeactivatedAfterEdit() || ImGui::Button("Recalcular FFT")) {
        auto resultadoFFT = logica.obtenerFFT(canal);

        if (!resultadoFFT.empty()) {
            size_t numBins = resultadoFFT.size();
            m_datosFrecuenciaMagnitud.resize(numBins);
            m_datosFrecuenciaX.resize(numBins);

            float freqPaso = (float)sampleRate / (float)(2 * (numBins - 1));

            for (size_t i = 0; i < numBins; ++i) {
                m_datosFrecuenciaMagnitud[i] = std::abs(resultadoFFT[i]);
                m_datosFrecuenciaX[i] = i * freqPaso;
            }
        }
    }
}


void VisualizadorAudio::dibujarGraficoTiempo(const PistaAudio& pista, int canal) {
    if (m_datosTiempoY.empty()) return;
    
    std::string titulo = "1. Dominio del Tiempo (Amplitud) | Canal " + std::to_string(canal);
    
    if (ImPlot::BeginPlot(titulo.c_str(), ImVec2(-1, 300))) {
        
        ImPlot::SetupAxes("Tiempo (s)", "Amplitud Normalizada", ImPlotAxisFlags_None, ImPlotAxisFlags_RangeFit);
        ImPlot::SetupAxesLimits(0, m_datosTiempoX.back(), -1.0, 1.0, ImPlotCond_Always);

        // Dibuja la señal de audio
        ImPlot::PlotLine("Forma de Onda", 
                         m_datosTiempoX.data(), 
                         m_datosTiempoY.data(), 
                         m_datosTiempoY.size());

        ImPlot::EndPlot();
    }
}


void VisualizadorAudio::dibujarGraficoFrecuencia(int sampleRate) {
    if (m_datosFrecuenciaMagnitud.empty()) return;

    std::string titulo = "2. Dominio de la Frecuencia (FFT) | Max Freq: " + std::to_string(sampleRate / 2) + " Hz";

    if (ImPlot::BeginPlot(titulo.c_str(), ImVec2(-1, 300))) {
        
        ImPlot::SetupAxes("Frecuencia (Hz)", "Magnitud", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_RangeFit);
        
        ImPlot::PlotStems("Espectro",
                          m_datosFrecuenciaX.data(), 
                          m_datosFrecuenciaMagnitud.data(), 
                          m_datosFrecuenciaMagnitud.size());
        
        ImPlot::EndPlot();
    }
}