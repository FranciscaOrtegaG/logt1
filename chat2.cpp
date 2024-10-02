#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <chrono>
#include <fstream> // tsv
#include <filesystem> // tsv

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

typedef long long ll;

const int PAGE_SIZE = 1024;
const int maxElementosPorPagina = PAGE_SIZE / sizeof(ll);

// Función de Hashing aleatorio (no determinista)
ll h(ll y) {
    std::random_device rd;
    std::mt19937_64 gen(y);
    std::uniform_int_distribution<ll> dis(0, std::numeric_limits<ll>::max());
    return dis(gen);
}

// Estructura para las métricas del experimento
struct Metrics {
    double costo_promedio;
    double porcentaje_llenado;
    ll io_count;
    ll duracion_segundos;
};

// Estructura para una página
struct Page {
    vector<ll> elementos;
    Page* next;

    Page() : next(nullptr) {}
};

// Estructura para cada entrada de la tabla de hash
struct Entry {
    Page* head; // Puntero a la primera página
    Entry() : head(nullptr) {}
};

// Clase para la tabla de hashing
class TablaHashing {
private:
    vector<Entry> table; // Tabla de hash
    ll p;               // Cantidad de páginas actuales
    ll t;               // Parámetro para determinar la expansión
    ll count;           // Total de accesos
    ll C_MAX;           // Costo promedio máximo permitido
    ll io_count;  // Contador de I/Os
    

    bool buscar_en_pagina(Page* pag, ll y) {
        io_count++;
        for (const ll& elem : pag->elementos) {
            if (elem == y) {
                return true;
            }
        }
        return false;
    }

    void insertar_en_pagina(ll idPage, ll valor) {
        if (table[idPage].head == nullptr) {
            table[idPage].head = new Page();
            io_count++;
        }

        Page* pag = table[idPage].head;
        while (pag != nullptr) {
            count++; // Contar acceso
            //io_count++;
            if (buscar_en_pagina(pag, valor)) { //hace un io count
                return; // Elemento ya existe
            }

            if (pag->elementos.size() < maxElementosPorPagina) {
                pag->elementos.push_back(valor);
                io_count++;
                return;
            }

            // Si la página está llena, ir a la siguiente
            if (pag->next == nullptr) {
                pag->next = new Page();
                io_count++;
            }
            pag = pag->next; // Mover a la siguiente página
        }
    }


public:
    TablaHashing(ll MAX_ACCESOS) : p(1), t(0), count(0), C_MAX(MAX_ACCESOS), io_count(0) {
        table.resize(p);
    }

    ~TablaHashing() {
        for (auto& entry : table) {
            Page* current = entry.head;
            while (current != nullptr) {
                Page* temp = current->next;
                delete current;
                current = temp;
            }
        }
    }

    void insert(ll valor) {
        ll k = h(valor) % (1LL << (t + 1));

        if (k < p) {
            insertar_en_pagina(k, valor);
        } else {
            insertar_en_pagina(k - (1LL << t), valor);
        }

        // Evaluar el costo promedio
        double costoPromedio = static_cast<double>(count) / p;
        if (costoPromedio > C_MAX) {
            expandir();
        }
    }

    bool buscar(ll y) {
        for (int i = 0; i < p; i++) {
            Page* pag = table[i].head;
            while (pag != nullptr) {
                if (buscar_en_pagina(pag, y)) {
                    return true;
                }
                pag = pag->next;
            }
        }
        return false;
    }

    void compactar(ll idPage) {
        Page* current = table[idPage].head;
        while (current != nullptr) {
            current->elementos.clear(); // Limpia los elementos
            current = current->next;
        }
    }

    void expandir() {
        table.push_back(Entry());
        ll p_anterior = p;
        p++;

        if (p == (1LL << (t + 1))) {
            t++;
        }

        ll idx = p-1;
        Page* pag_anterior = table[idx].head;

        // Mover elementos de la página anterior a la nueva
        Page* current = pag_anterior;
        while (current != nullptr) {
            for (ll y : current->elementos) {
                ll k = h(y) % (1LL << (t + 1));
                if (k == p_anterior) {
                    table.back().head->elementos.push_back(y);
                } else {
                    insertar_en_pagina(k, y);
                }
            }
            current = current->next;
        }

        // Compactar la página anterior
        compactar(idx);
    }

    double obtener_costo_promedio() const {
        return static_cast<double>(count) / p;
    }

    double porcentaje_llenado() const {
        ll total_elementos = 0;
        ll total_paginas = 0;
        for (const Entry& entry : table) {
            Page* pag = entry.head;
            while (pag != nullptr) {
                total_elementos += pag->elementos.size();
                total_paginas++;
                pag = pag->next;
            }
        }
        return (total_paginas == 0) ? 0 : (static_cast<double>(total_elementos) / (total_paginas * maxElementosPorPagina)) * 100.0;
    }

    ll get_io_count() const {
        return io_count;
    }


    void print() {
        cout << "Tabla Hash:\n";
        for (int i = 0; i < table.size(); i++) {
            cout << i << " -> ";
            if (table[i].head == nullptr) {
                cout << "[]\n";
                continue;
            }
            Page* pag = table[i].head;
            while (pag != nullptr) {
                for (ll elem : pag->elementos) {
                    cout << elem << " ";
                }
                pag = pag->next;
                cout << (pag ? " -> " : "");
            }
            cout << "\n";
        }
    }
};


void experimento(ll C_MAX, int N)
{
    TablaHashing t(C_MAX);

    // Generar N elementos aleatorios
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<ll> dis(1, numeric_limits<long long>::max());
    for (int i = 0; i < N; i++)
    {
        ll valor = dis(gen);
        t.insert(valor);
    }

    // Imprimir la tabla de hash
    t.print();
}
Metrics experimento2(ll C_MAX, ll N)
{
    TablaHashing t(C_MAX);

    // Generar N elementos aleatorios entre 1 y 2^63-1
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<ll> dis(1, numeric_limits<long long>::max());

    // Insertar los N elementos
    for (ll i = 0; i < N; i++)
    {
        ll valor = dis(gen);
        t.insert(valor);

        // Muestra el progreso
        if (i % (N / 10) == 0) {
            cout << "Progreso: " << (i * 100) / N << "%\n";
        }
    }

    // Resultados del experimento
    double costo_promedio = t.obtener_costo_promedio();
    double porcentaje_llenado = t.porcentaje_llenado();

    cout << "Resultados del experimento con N = " << N << " elementos:\n";
    cout << "Costo promedio real de insercion: " << costo_promedio << "\n";
    cout << "Porcentaje de llenado de las paginas: " << porcentaje_llenado << "%\n";
    cout << "Número total de I/Os: " << t.get_io_count() << "\n";


    // Resultados del experimento
    Metrics metricas;
    metricas.costo_promedio = t.obtener_costo_promedio();
    metricas.porcentaje_llenado = t.porcentaje_llenado();
    metricas.io_count = t.get_io_count();

    return metricas;
}

Metrics medirDuracionExperimento(ll C_MAX, ll N) {
    auto inicio = chrono::high_resolution_clock::now(); // Inicio del cronómetro

    // Ejecutar el experimento 2
    Metrics metricas = experimento2(C_MAX, N);

    auto fin = chrono::high_resolution_clock::now(); // Fin del cronómetro
    auto duracion = chrono::duration_cast<seconds>(fin - inicio);  // Duración en segundos

    auto minutos = chrono::duration_cast<minutes>(duracion);
    auto segundos = chrono::duration_cast<seconds>(duracion - minutos);

    cout << "Tiempo de ejecucion: " << minutos.count() << " minutos y " << segundos.count() << " segundos" << endl;
    metricas.duracion_segundos = duracion.count();
    return metricas;
}

int main() {

    // Archivo log de creación
    string filename = "experiments_data/data.tsv";
    fs::path filepath = filename;

    // Crear el directorio si no existe
    fs::path dir = filepath.parent_path();
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }

    // Abrir el archivo
    ofstream data_tsv;
    data_tsv.open(filename);
    if (!data_tsv.is_open()) {
        cerr << "Error al abrir el archivo " << filename << endl;
        return 1;
    }
    data_tsv << "i\tN\tC_MAX\tcosto_promedio\tporcentaje_llenado\tio_count\tduracion_segundos\n"; // TSV Header

    // Medir duración de experimento2
    for(int i=10; i < 15; i++){
        ll N = 1LL << i;
        ll C_MAX_values[] = {10, 100, 500, 750, 1024, 2056, 5000, 10000};
        // experimento para 10, 100, 500, 750,1024, 2056, 5000 y 10000
        for (ll C_MAX : C_MAX_values) {
            Metrics metrics = medirDuracionExperimento(C_MAX, N);
            data_tsv << i << "\t"  << N << "\t" << C_MAX << "\t" << metrics.costo_promedio << "\t" << metrics.porcentaje_llenado << "\t" << metrics.io_count << "\t" << metrics.duracion_segundos << "\n";
        }
    }
    
    data_tsv.close();
    return 0;
}