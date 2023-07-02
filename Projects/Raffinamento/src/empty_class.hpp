#ifndef __EMPTY_H
#define __EMPTY_H

#include <iostream>
#include "Eigen/Eigen"
#include <fstream>
#include <cmath>
#include "map"

using namespace std;
using namespace Eigen;

namespace ProjectLibrary
{

    struct Lista
    {
        public:
            int lunghezza; // ci dice il numero di triangoli totali della mesh
            Lista(); // costruttore
            void Inserimento(unsigned int id, double area); // funzione per l'inserimento di un triangolo e la sua area
            void Estrazione(unsigned int id); // funzione per la rimozione di un triangolo e la sua area
            struct Nodo // il triangolo con l'area ad esso associata punta al triangolo con l'area immediatamente minore della sua
            {
                unsigned int id;
                double area;
                Nodo* successivo; // puntatore al triangolo dopo
            };
            Nodo* testa; // puntatore alla testa della lista
    };

    struct TriangularMesh // in una struct tutti i metodi sono pubblici (accessibili da altre classi)
    {
        unsigned int NumberCell0D = 0; ///< number of Cell0D (numero di vertici che inizializzo a 0)
        std::vector<unsigned int> Cell0DId = {}; ///< Cell0D id, size 1 x NumberCell0D (id per identificare un lato specifico, vettore di int)
        std::vector<Eigen::Vector2d> Cell0DCoordinates = {}; ///< Cell0D coordinates, size 2 x NumberCell0D (x,y) (coordinate dei vertici, double)
        std::map<unsigned int, list<unsigned int>> Cell0DMarkers = {}; ///< Cell0D markers, size 1 x NumberCell0D (marker) (identifica il tipo di vertice, se interno o agli angoli ecc)

        unsigned int NumberCell1D = 0; ///< number of Cell1D
        std::vector<unsigned int> Cell1DId = {}; ///< Cell1D id, size 1 x NumberCell1D
        std::vector<Eigen::Vector2i> Cell1DVertices = {}; ///< Cell1D vertices indices, size 2 x NumberCell1D (fromId,toId)
        std::map<unsigned int, list<unsigned int>> Cell1DMarkers = {}; ///< Cell1D propertoes, size 1 x NumberCell1D (marker)
        std::vector<Eigen::Vector2i> Cell1DAdjacency = {}; // per ogni id dei lati, i due triangoli di cui fa parte

        unsigned int NumberCell2D = 0; ///< number of Cell2D
        std::vector<unsigned int> Cell2DId = {}; ///< Cell2D id, size 1 x NumberCell2D
        std::vector<array<unsigned int, 3>> Cell2DVertices = {}; ///< Cell2D Vertices indices, size 1 x NumberCell2DVertices[NumberCell2D]
        std::vector<array<unsigned int, 3>> Cell2DEdges = {}; ///< Cell2D Cell1D indices, size 1 x NumberCell2DEdges[NumberCell2D]
        Lista Cell2DAreeOrdinate = Lista();

        bool ImportMesh(TriangularMesh& mesh, string file, double percentuale);
        bool ImportCell0Ds(TriangularMesh& mesh, string file, string parentFolder, double percentuale);
        bool ImportCell1Ds(TriangularMesh& mesh, string file, string parentFolder, double percentuale);
        bool ImportCell2Ds(TriangularMesh& mesh, string file, string parentFolder, double percentuale);

        bool ExportMesh(TriangularMesh& mesh, string file);
        bool ExportCell0Ds(TriangularMesh& mesh, string file, string parentFolder);
        bool ExportCell1Ds(TriangularMesh& mesh, string file, string parentFolder);
        bool ExportCell2Ds(TriangularMesh& mesh, string file, string parentFolder);
        bool ExportParaview(TriangularMesh& mesh, string file, string parentFolder);

        void divideCell2(unsigned int id);

        // funzione usata per definire i marker dei vertici (gli unici vertici inseriti sono i punti medi)
        // usiamo i marker dei due vertici che compongono il lato a cui appartiene: se almeno uno dei due è interno, lo è anche il punto medio; se entrambi sono laterali dello stesso lato lo è anche il marker (stessa cosa se uno dei sue è un angolo e l'altro è laterale), se sono entrambi laterali ma di due lati diversi allora è interno
        unsigned int findMarkerVertices(unsigned int id_1, unsigned int id_2);

        // Descrive un oggetto che controlla una sequenza di elementi di lunghezza variabile. La sequenza viene archiviata come elenco collegato singolarmente di nodi,
        // creiamo una lista in cui memorizziamo i triangoli in ordine decrescente in base alla dimensione dell'area

        inline unsigned int findMarkerLato(unsigned int id) {
            unsigned int m;

            bool found = false;

            unsigned int markers[] = {5, 6, 7, 8};

            while(!found)
            {
                for(unsigned int i : markers)
                {
                    if(find(Cell1DMarkers[i].begin(),Cell1DMarkers[i].end(),id) != Cell1DMarkers[i].end())
                    {
                        m = i;
                        found = true;
                        break;
                    }
                }
                if(found == false)
                {
                    found = true;
                    m=0;
                }
            return m;
            }
        }

        inline unsigned int findMarkerVerticeSingolo(unsigned int id) {
            unsigned int m;

            bool found = false;

            unsigned int markers[] = {1, 2, 3, 4, 5, 6, 7, 8};
            while(!found)
            {
                for(unsigned int i : markers)
                {
                    if(find(Cell0DMarkers[i].begin(),Cell0DMarkers[i].end(),id) != Cell0DMarkers[i].end())
                    {
                        m = i;
                        found = true;
                        break;
                    }
                }
                if(found == false)
                {
                    found = true;
                    m = 0;
                }
            }

            return m;
        }

        inline array<array<double, 2>, 3> findCoordinates(unsigned int id) {

            array<array<double, 2>, 3> coordinates;
            for(unsigned int i=0; i<3; i++)
            {

                coordinates[i][0] = Cell0DCoordinates[Cell2DVertices[id][i]][0];
                coordinates[i][1] = Cell0DCoordinates[Cell2DVertices[id][i]][1];
            }

            return coordinates;
        }

    };

    inline double ComputeArea(array<array<double, 2>, 3> vertices) {
        double area = std::abs((vertices[0][0] * (vertices[1][1] - vertices[2][1]) + vertices[1][0] * (vertices[2][1] - vertices[0][1]) + vertices[2][0] * (vertices[0][1] - vertices[1][1])) / 2.0);
        return area;
    }

    // La parola chiave inline indica al compilatore di sostituire il codice nella definizione di funzione per ogni istanza di una chiamata di funzione
    inline double ComputeDistance(Vector2d coord1, Vector2d coord2){
          return (pow((pow((coord2[0] - coord1[0]),2) + pow((coord2[1] - coord1[1]),2)),0.5));
    }


}

#endif //__EMPTY_H
