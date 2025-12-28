#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <complex>
#include "Editor_logica.h"
#include "Visualizador_Audio.h"

void mostrarMenu() {
    std::cout << "Selecciona una opcion:" << std::endl;
    std::cout << "1. Cargar archivo de audio" << std::endl;
    std::cout << "2. Aplicar volumen" << std::endl;
    std::cout << "3. Deshacer" << std::endl;
    std::cout << "4. Aplicar Karaoke" << std::endl;
    std::cout << "5. Encontrar Picos" << std::endl;
    std::cout << "6. Calcular FFT" << std::endl;
    std::cout << "7. Guardar archivo" << std::endl;
    std::cout << "8. Recortar Audio" << std::endl;
    std::cout << "9. Ver visualizacion de la senal" << std::endl;
    std::cout << "10. Salir" << std::endl;
    std::cout << "Opcion: ";
}

void imprimirResultadoFFT(const std::vector<std::complex<float>>& fftData, int sampleRate) {
    std::cout << "Resultado de la FFT (Magnitud por Frecuencia):" << std::endl;
    size_t numBins = fftData.size();
    float freqPaso = (float)sampleRate / (2 * (numBins - 1));

    for (size_t i = 0; i < numBins; ++i) {
        float magnitud = std::abs(fftData[i]);
        float frecuencia = i * freqPaso;
        if (i % (numBins / 20) == 0) {
            std::cout << "  Frecuencia: " << (int)frecuencia << " Hz, Magnitud: " << magnitud << std::endl;
        }
    }
}

int main() {
    EditorLogica logica;

    std::cout << "Editor de Audio" << std::endl;

    bool salir = false;
    while (!salir) {
        mostrarMenu();
        int opcion;

        if (!(std::cin >> opcion)) {
            std::cout << "Error, ingresa un numero." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string rutaEntrada;
        std::string rutaSalida;

        switch (opcion) {
            case 1:
                std::cout << "Ingresa el nombre del archivo a cargar: ";
                std::getline(std::cin, rutaEntrada);
                logica.cargarArchivo(rutaEntrada);
                break;

            case 2: {
                float factor;
                std::cout << "Ingresa el factor de volumen (ej: 1.5 para 150%, 0.5 para 50%)";
                std::cin >> factor;
                logica.aplicarVolumen(factor);
                break;
            }
            case 3:
                std::cout << "Aplicando Deshacer" << std::endl;
                logica.deshacer();
                break;

            case 4:
                std::cout << "Aplicando Efecto Karaoke" << std::endl;
                logica.aplicarKaraoke();
                break;

            case 5: {
                float umbral;
                std::cout << "Ingresa el minimo para buscar picos (ej: 0.2): ";
                std::cin >> umbral;
                logica.analizarPicos(umbral);
                break;
            }
            case 6: {
                std::cout << "Calculando FFT" << std::endl;
                auto resultadoFFT = logica.obtenerFFT(0);
                if (!resultadoFFT.empty()) {
                    int sr = logica.getPistaActual().getSampleRate();
                    imprimirResultadoFFT(resultadoFFT, sr);
                }
                break;
            }
            case 7:
                std::cout << "Ingresa el nombre para guardar el archivo";
                std::getline(std::cin, rutaSalida);
                logica.guardarEnArchivo(rutaSalida);
                break;

            case 8: {
                double inicio, fin;
                std::cout << "Recortar Audio" << std::endl;
                std::cout << "Ingresa el segundo inicial: ";
                std::cin >> inicio;
                std::cout << "Ingresa el segundo de final: ";
                std::cin >> fin;
                logica.recortarAudio(inicio, fin);
                break;
            }

            case 9: {
                if (logica.getPistaActual().getNumFrames() == 0) {
                    std::cout << "Debes cargar un archivo de audio antes de visualizar.";
                    std::cout << std::endl;
                } else {
                    VisualizadorAudio visualizador;
                    visualizador.ejecutarGUI(logica);
                }
                break;
            }
            case 10:
                std::cout << "Fin del programa" << std::endl;
                salir = true;
                break;
            default:
                std::cout << "Opcion no valida. Intentalo de nuevo." << std::endl;
                break;
        }
    }

    return 0;
}