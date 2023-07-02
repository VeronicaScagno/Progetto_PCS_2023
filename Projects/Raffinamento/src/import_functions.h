#ifndef IMPORTFUNCTIONS_H
#define IMPORTFUNCTIONS_H

namespace Lista
{
    struct Lista
    {
        public:
            int lunghezza; // ci dice il numero di triangoli totali della mesh
            Lista(); // costruttore
            void Inserimento(unsigned int id, double area); // funzione per l'inserimento di un triangolo e la sua area
            int Estrazione(unsigned int id); // funzione per la rimozione di un triangolo e la sua area
        private:
            /// SI PUO' FARE STRUCT NELLE STRUCT?
            struct Nodo // il triangolo con l'area ad esso associata punta al triangolo con l'area immediatamente minore della sua
            {
                unsigned int id;
                double area;
                Nodo* successivo; //successivo è il puntatore al triangolo dopo
            };
            Nodo* testa; // puntatore alla testa della lista
            /*
            Nodo *temp; // nodo temporaneo
            Nodo *prev; // nodo temporaneo precedente a quello si cui stiamo iterando
            */

    };
}

#endif // IMPORTFUNCTIONS_H
