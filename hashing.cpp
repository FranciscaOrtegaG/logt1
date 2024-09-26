typedef long long ll;
using namespace std;
#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <unordered_map>

const int PAGE_SIZE = 1024;

const int maxElementosPorPagina = PAGE_SIZE/64;

// Funcion de Hashing
ll h(ll y) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<ll> dis(0, std::numeric_limits<ll>::max());
    return dis(gen);
}

// Estructura page para las paginas
struct page{
    vector<ll> elementos;
    page* next;

    page(): next(nullptr) {};
};


// Estructura llave -> valor
struct Entry {
    unsigned long long int key;
    vector<page> listPages; // Lista de paginas
};


//////////////////////
//      Prints      //
//////////////////////

// Overload << operator para imprimir paginas
std::ostream& operator<<(std::ostream& os, const page& p) {
    os << "[";
    for (int i = 0; i < p.elementos.size(); i++) {
        os << p.elementos[i];
        if (i != p.elementos.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    
    return os;
}




// Tabla de hashing
class tablaHashing{

    private:

        // Funcion auxiliar para buscar un valor en una pagina
        bool buscar_en_pagina(page* pag, ll y){
            while (pag != nullptr) { 
                for (int i = 0; i < pag->elementos.size(); i++) { 
                    if (pag->elementos[i] == y) {
                        return true;
                    }
                }
                if (pag->next == nullptr) {
                    return false;
                    break;
                } else{ 
                    pag= pag->next;
                }
            }
            return false;
      }
       
        // Funcion auxiliar para insertar en una pagina
        void insertarEnPagina(ll idPage, ll valor){
            
            // Si la pagina esta vacia
            if (table[idPage].listPages.size() == 0){
                page newPage;
                table[idPage].listPages.push_back(newPage);
            }


            page* pag = &table[idPage].listPages[0]; //desde la primera pagina de la lista
            while (pag != nullptr && pag->next != nullptr){
                count++;
                // buscar si el elemento se encuentra en la pagina
                
                if (buscar_en_pagina(pag, valor)){
                    return;
                }
                pag = pag->next; //hasta la ultima pagina para posicionarnos
            }
            if (pag->elementos.size() < maxElementosPorPagina){ //si la ultima página no está llena
                pag->elementos.push_back(valor); //agregamos el valor
            }
            else{
                page* newPage = new page();
                newPage->elementos.push_back(valor);
                pag->next = newPage;
            }
        }

    public:

        vector<Entry> table; // Vector que representa la tabla hash
        ll p; // cantidad de paginas
        ll t; // 
        
        ll count; // total de accesos
        ll C_MAX; // Coste promedio maximo (Hiperparametro)

        // Constructor
        tablaHashing(ll MAX_ACCESOS){
            ll p = 1;
            ll t = 0;
            ll count=0; // total de accesos
            this->C_MAX = MAX_ACCESOS;
            table.resize(p);
        };
        
        // Insertar
        void insert(ll valor) {
            int k = h(valor) % (1 << (t + 1));

            // Insercion
            if (k < p) {
                insertarEnPagina(k, valor);
            }
            else {
                insertarEnPagina(k-(1<<t), valor);
            }

            // Luego de insertar
            // Se debe verificar si insercion ha provocado un aumento en el costo promedio de busqueda
            double costoPromedio = (double) count / p;   // Se calcula el nuevo coste promedio de busqueda
            if (costoPromedio > C_MAX) {
                expandir();
            }
        }
        
        // Busqueda en una tabla de hash
        bool buscar(ll y) {
            for (int i = 0; i < p; i++) {
                for (int j =0; j < table[i].listPages.size(); j++){
                    if (buscar_en_pagina(&table[i].listPages[j], y)) {
                        return true;
                    }
                }
            }
            return false;   
        }

        //bool necesita_expandir() {
          //  return table[p - (1 << t)].listPages[0].elementos.size() > maxElementosPorPagina;
        //}

        void compactar(ll idpage){
            page* currentPage = &table[idpage].listPages[0];
            while (currentPage->next != nullptr) {
                page* temp = currentPage->next;
                currentPage->next = temp->next;
                delete temp;
            }
        }


        void expandir(){
            page newPage;
            table.push_back(Entry{0, std::vector<page>{newPage}});
            ll p_anterior= p;
            p++;
            
            if (p == (1 << (t + 1))) {
                t++;
            }

            ll idx = p_anterior - (1 << t);
            page* pag_anterior = &table[idx].listPages[0];
            page* nuevaPagina = &table.back().listPages[0];
            vector<ll> elementos_paganterior;
            
            for (ll y : pag_anterior->elementos) {
                ll k = h(y) % (1 << (t + 1));
                if (k == p_anterior) {
                    nuevaPagina->elementos.push_back(y);
                } else {
                    elementos_paganterior.push_back(y);
                }
            }

            pag_anterior->elementos = elementos_paganterior;
            compactar(idx);

        }

    // Metodo para printear y debuggear de forma profesio
    void print()
    {
        cout << "Tabla Hash:\n";
        for (int i = 0; i < table.size(); i++)
        {
            Entry entry = table[i];
            cout << entry.key << " -> ";
            if (entry.listPages.size() == 0)
            {
                cout << "[]\n";
                continue;
            }
            else
            {
                for (int j = 0; j < entry.listPages.size(); j++)
                {
                    cout << entry.listPages[j] << "-";
                }
            }
            cout << "\n";
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
    
    return 0;
}