#include <iostream>
#include <vector>
#include <random>
#include <unordered_map>
#include <functional> // Para std::hash

using namespace std;

typedef long long ll;

const int PAGE_SIZE = 1024;
const int maxElementosPorPagina = PAGE_SIZE / 64;

// Función de Hashing aleatoreo
ll h(ll y) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<ll> dis(0, std::numeric_limits<ll>::max());
    return dis(gen);
}

// Estructura para una página
struct page {
    vector<ll> elementos;
    page* next;

    page() : next(nullptr) {}
};

// Estructura para cada entrada de la tabla de hash
struct Entry {
    page* head; // Puntero a la primera página
    Entry() : head(nullptr) {}
};

// Sobrecarga del operador << para imprimir una página
ostream& operator<<(ostream& os, const page& p) {
    os << "[";
    for (size_t i = 0; i < p.elementos.size(); i++) {
        os << p.elementos[i];
        if (i != p.elementos.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// Clase para la tabla de hashing
class tablaHashing {
private:
    // Función auxiliar para buscar un valor en una página
    bool buscar_en_pagina(page* pag, ll y) {
        for (const ll& elem : pag->elementos) {
            if (elem == y) {
                return true;
            }
        }
        return false;
    }

    // Función auxiliar para insertar un valor en una página
    void insertarEnPagina(ll idPage, ll valor) {
        // Si la página está vacía, crear una nueva
        if (table[idPage].head == nullptr) {
            table[idPage].head = new page();
        }

        page* pag = table[idPage].head;
        while (pag != nullptr) {
            count++; // Incrementar el contador de accesos

            // Verificar si el elemento ya existe
            if (buscar_en_pagina(pag, valor)) {
                return; // Elemento ya existe, no se inserta
            }

            // Si es la última página, salir del bucle para insertar
            if (pag->next == nullptr) {
                break;
            }
            pag = pag->next; // Moverse a la siguiente página
        }

        // Insertar el valor en la última página
        if (pag->elementos.size() < maxElementosPorPagina) {
            pag->elementos.push_back(valor);
        } else {
            // Si la página está llena, crear una nueva página
            page* newPage = new page();
            newPage->elementos.push_back(valor);
            pag->next = newPage;
        }
    }

public:
    vector<Entry> table; // Vector que representa la tabla de hash
    ll p;               // Número de buckets actuales
    ll t;               // Parámetro para determinar cuándo expandir
    ll count;           // Total de accesos
    ll C_MAX;           // Costo promedio máximo permitido

    // Constructor
    tablaHashing(ll MAX_ACCESOS) {
        p = 1;
        t = 0;
        count = 0;
        C_MAX = MAX_ACCESOS;
        table.resize(p);
    }

    // Destructor para liberar memoria
    ~tablaHashing() {
        for (auto& entry : table) {
            page* current = entry.head;
            while (current != nullptr) {
                page* temp = current->next;
                delete current;
                current = temp;
            }
        }
    }

    // Método para insertar un valor en la tabla de hash
    void insert(ll valor) {
        ll k = h(valor) % (1LL << (t + 1));

        // Inserción en el bucket correspondiente
        if (k < p) {
            insertarEnPagina(k, valor);
        } else {
            insertarEnPagina(k - (1LL << t), valor);
        }

        // Calcular el costo promedio y verificar si es necesario expandir
        double costoPromedio = static_cast<double>(count) / p;
        if (costoPromedio > C_MAX) {
            expandir();
        }
    }

    // Método para buscar un valor en la tabla de hash
    bool buscar(ll y) {
        for (int i = 0; i < p; i++) {
            page* pag = table[i].head;
            while (pag != nullptr) {
                if (buscar_en_pagina(pag, y)) {
                    return true;
                }
                pag = pag->next;
            }
        }
        return false;
    }

    // Función para compactar las páginas de un bucket después de la expansión
    void compactar(ll idpage) {
        if (table[idpage].head == nullptr) {
            return;
        }
        page* currentPage = table[idpage].head->next;
        table[idpage].head->next = nullptr;
        while (currentPage != nullptr) {
            page* temp = currentPage->next;
            delete currentPage;
            currentPage = temp;
        }
    }

    // Método para expandir la tabla de hash
    void expandir() {
        // Añadir un nuevo bucket
        table.push_back(Entry());
        ll p_anterior = p;
        p++;

        // Verificar si es necesario incrementar 't'
        if (p == (1LL << (t + 1))) {
            t++;
        }

        ll idx = p_anterior - (1LL << t);
        page* pag_anterior = table[idx].head;
        page* nuevaPagina = table.back().head;
        if (nuevaPagina == nullptr) {
            nuevaPagina = new page();
            table.back().head = nuevaPagina;
        }

        // Rehash de los elementos del bucket anterior
        page* current = pag_anterior;
        while (current != nullptr) {
            auto it = current->elementos.begin();
            while (it != current->elementos.end()) {
                ll y = *it;
                ll k = h(y) % (1LL << (t + 1));
                if (k == p_anterior) {
                    nuevaPagina->elementos.push_back(y);
                    it = current->elementos.erase(it);
                } else {
                    ++it;
                }
            }
            current = current->next;
        }

        // Compactar las páginas del bucket anterior
        compactar(idx);
    }

    // Método para imprimir y depurar la tabla de hash
    void print() {
        cout << "Tabla Hash:\n";
        for (int i = 0; i < table.size(); i++) {
            cout << i << " -> ";
            if (table[i].head == nullptr) {
                cout << "[]\n";
                continue;
            } else {
                page* pag = table[i].head;
                while (pag != nullptr) {
                    cout << *pag;
                    if (pag->next != nullptr) {
                        cout << " -> ";
                    }
                    pag = pag->next;
                }
                cout << "\n";
            }
        }
    }
};

int main() {

    tablaHashing t(100);

    // Insertar
    t.insert(10);
    t.print();

    bool b = t.buscar(10);
    cout << b << endl;
    
    t.insert(10);
    t.insert(20);
    t.insert(30);
    t.insert(40);
    t.insert(50);
    t.insert(10); // Intento de insertar un duplicado

    // Imprimir la tabla de hash
    t.print();

    // Buscar elementos
    bool b1 = t.buscar(10);
    bool b2 = t.buscar(60);
    cout << "Buscar 10: " << (b1 ? "Encontrado" : "No encontrado") << endl;
    cout << "Buscar 60: " << (b2 ? "Encontrado" : "No encontrado") << endl;

    return 0;
}
