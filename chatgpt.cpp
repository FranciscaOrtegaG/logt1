#include <vector>
using namespace std;

const int pageSize = 1024;

// Estructura principal de la página
struct Pagina {
    vector<int> elementos;  // Contiene los elementos de la página
    Pagina* rebalse;        // Puntero a una página de rebalse si la actual se llena

    Pagina() : rebalse(nullptr) {}  // Constructor inicializa sin rebalse
};

// Clase que gestiona el archivo dinámico de páginas
class ArchivoDinamico {
    vector<Pagina*> paginas;  // Vector de punteros a páginas
    int p;                    // Número actual de páginas
    int t;                    // Parámetro t que controla el rango de páginas
    int maxElementosPorPagina; // Máximo de elementos por página

    // Constructor inicializa con una página y t = 0
    ArchivoDinamico(int maxElementos) : p(1), t(0), maxElementosPorPagina(maxElementos) {
        paginas.push_back(new Pagina());
    }

    // Función hash (ejemplo simple)
    int hash(int y) {
        return y; // Asume hash identidad para simplificar
    }

    // Función para insertar un elemento
    void insertar(int y) {
        int k = hash(y) % (1 << (t + 1)); // k ← h(y) mod 2^(t+1)

        if (k < p) {
            // Insertar en la página k o en su rebalse
            insertarEnPagina(k, y);
        } else {
            // Insertar en la página k - 2^t (que sí existe)
            insertarEnPagina(k - (1 << t), y);
        }

        // Verificar si se necesita una expansión
        if (seRequiereExpansion()) {
            expandirPagina(p - (1 << t)); // Expandir la página p - 2^t
            p++; // Actualizar número de páginas
            if (p == (1 << (t + 1))) {
                t++; // Si p llega a 2^(t+1), incrementar t
            }
        }
    }

    // Insertar un elemento en una página (manejo de rebalse)
    void insertarEnPagina(int idx, int y) {
        Pagina* pag = paginas[idx];
        while (pag->rebalse != nullptr) {
            pag = pag->rebalse;
        }
        if (pag->elementos.size() < maxElementosPorPagina) {
            pag->elementos.push_back(y);
        } else {
            pag->rebalse = new Pagina();
            pag->rebalse->elementos.push_back(y);
        }
    }

    // Función que determina si se necesita una expansión
    bool seRequiereExpansion() {
        return paginas[p - (1 << t)]->elementos.size() > maxElementosPorPagina;
    }

    // Expansión de la página p - 2^t
    void expandirPagina(int idx) {
        Pagina* pag = paginas[idx];
        Pagina* nuevaPagina = new Pagina();
        paginas.push_back(nuevaPagina); // Agregar nueva página al final

        // Redistribuir elementos entre la página idx y la nueva página
        vector<int> nuevosElementos;
        for (int y : pag->elementos) {
            int nuevoIndice = hash(y) % (1 << (t + 1));
            if (nuevoIndice == p) {
                nuevaPagina->elementos.push_back(y);
            } else {
                nuevosElementos.push_back(y);
            }
        }
        pag->elementos = nuevosElementos;

        // Compactar y eliminar páginas de rebalse si ya no son necesarias
        compactarPaginas(idx);
    }

    // Compactar las páginas y eliminar rebalses innecesarios
    void compactarPaginas(int idx) {
        Pagina* pag = paginas[idx];
        while (pag->rebalse != nullptr) {
            Pagina* temp = pag->rebalse;
            pag->rebalse = temp->rebalse;
            delete temp;
        }
    }
};
