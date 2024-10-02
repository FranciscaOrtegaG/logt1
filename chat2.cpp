#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <chrono>
using namespace std;

typedef long long ll;

const int PAGE_SIZE = 1024;
const int maxElementosPorPagina = PAGE_SIZE / sizeof(ll);

// Función de Hashing aleatorio (no determinista)
ll h(ll y) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<ll> dis(0, std::numeric_limits<ll>::max());
    return dis(gen);
}

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

    bool buscar_en_pagina(Page* pag, ll y) {
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
        }

        Page* pag = table[idPage].head;
        while (pag != nullptr) {
            count++; // Contar acceso
            if (buscar_en_pagina(pag, valor)) {
                return; // Elemento ya existe
            }

            if (pag->elementos.size() < maxElementosPorPagina) {
                pag->elementos.push_back(valor);
                return;
            }

            // Si la página está llena, ir a la siguiente
            if (pag->next == nullptr) {
                pag->next = new Page();
            }
            pag = pag->next; // Mover a la siguiente página
        }
    }

public:
    TablaHashing(ll MAX_ACCESOS) : p(1), t(0), count(0), C_MAX(MAX_ACCESOS) {
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
void experimento2(ll C_MAX, ll N)
{
    TablaHashing t(C_MAX);

    // Generar N elementos aleatorios
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

    // Imprimir la tabla final (opcional, solo para depuración)
    // t.print();
}

void medirDuracionExperimento(void(*experimento)(ll, ll), ll C_MAX, ll N) {
    auto inicio = chrono::high_resolution_clock::now(); // Inicio del cronómetro

    // Ejecutar el experimento
    experimento(C_MAX, N);

    auto fin = chrono::high_resolution_clock::now(); // Fin del cronómetro
    chrono::duration<double> duracion = fin - inicio; // Duración en segundos

    cout << "Tiempo de ejecución: " << duracion.count() << " segundos" << endl;
}

int main() {
    ll N = 1LL << 15; // Valor de N

    // Medir duración de experimento2
    medirDuracionExperimento(experimento2, 1024, N);

    return 0;
}