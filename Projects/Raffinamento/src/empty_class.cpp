#include "empty_class.hpp"
#include <filesystem>
#include <string>
#include <windows.h>

namespace ProjectLibrary
{
    // costruttore
    Lista::Lista()
    {
        testa = NULL;
        lunghezza = 0;
    }

    void Lista::Inserimento(unsigned int id, double area)
    {
        Nodo* nuovo = new Nodo();
        nuovo->area = area;
        nuovo->id = id;
        nuovo->successivo = nullptr; // il successivo è un puntatore nullo

        if(lunghezza == 0)
        {
            testa = nuovo;
            lunghezza += 1;
        }
        else if(testa->area < area + 1e-24)
        {
            Nodo* temp = testa;
            testa = nuovo;
            nuovo->successivo = temp;
            lunghezza += 1;
        }
        else if(lunghezza == 1)
        {
            testa->successivo = nuovo;
            lunghezza +=1;
        }
        else
        {
            Nodo* prev = testa;
            Nodo* temp = testa->successivo;
            unsigned int i = 1;
            while(temp->area >= area - 1e-24 && i<lunghezza-1)
            {
                prev = prev->successivo;
                temp = temp->successivo;
                i++;
            }

            if(i == lunghezza - 1 && temp->area >= area - 1e-24)
            {
                temp->successivo = nuovo;
                lunghezza += 1;
            }
            else
            {
                nuovo->successivo = temp;
                prev->successivo = nuovo;
                lunghezza += 1;
            }
        }
    }

    void Lista::Estrazione(unsigned int id)
    {
        Nodo* prev = testa;
        if(prev->id == id)
        {
            testa = testa->successivo;
            delete prev;
            lunghezza -= 1;
        }
        else
        {
            Nodo* temp = testa;
            if(temp->successivo != nullptr)
                temp = temp->successivo;
            while(temp->id != id && temp->successivo != nullptr)
            {
                prev = prev->successivo;
                temp = temp->successivo;
            }
            if(temp->successivo == nullptr)
            {
                cerr << "Triangle " << id << " is no longer in the list!" << endl;
            }
            prev->successivo = temp->successivo;
            delete temp;
            lunghezza -= 1;
        }

    }

    bool TriangularMesh::ImportMesh(TriangularMesh& mesh, string filename, double percentuale)
    {

        // Andiamo a scegliere il path corretto per il Dataset che viene dato in input alla nostra funzione di Import
        std::string filePath;

        char buffer[MAX_PATH];
        DWORD length = GetModuleFileName(NULL, buffer, MAX_PATH);
        if (length != 0) {
            filePath = std::string(buffer, length);
        }

        std::string parentFolder = filePath;

            for (int i = 0; i < 2; ++i) {
                size_t lastSeparator = parentFolder.find_last_of("/\\");
                if (lastSeparator != std::string::npos) {
                    parentFolder = parentFolder.substr(0, lastSeparator);
                }
            }

      if(!ImportCell0Ds(mesh,filename, parentFolder, percentuale))
      {
        return false;
      }
      else
      {
        //cout << "Cell0D marker:" << endl;
        //for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            //cout << "key:\t" << it -> first << "\t values:"; // restituisce la chiave spazio valore di tutti i punti tranne quelli interni (ossia con chiave 0)
          //for(const unsigned int id : it -> second) // per ogni chiave stampata vado a stampare i valori (più vertici hanno stessa chiave)
            //cout << "\t" << id;

          //cout << endl; // passo alla chiave dopo una volta che ho stampato tutti i valori aventi quella chiave
        }
      }

      if(!ImportCell1Ds(mesh, filename, parentFolder, percentuale))
      {
        return false;
      }
      /*
      else
      {
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
          cout << "key:\t" << it -> first << "\t values:";
          for(const unsigned int id : it -> second)
            cout << "\t" << id;

          cout << endl;
        }
      }
      */

      if(!ImportCell2Ds(mesh, filename, parentFolder, percentuale))
      {
        return false;
      }
      else
      {
        // il test si occupa di trovare l'origine e la fine di ogni lato
        // Test:
        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
          array<unsigned int, 3> edges = mesh.Cell2DEdges[c]; //definiamo un array per i lati di ogni cella in Cell2Ds

          for(unsigned int e = 0; e < 3; e++) // cicliamo nelle tre dimensioni
          {
             const unsigned int origin = mesh.Cell1DVertices[edges[e]][0]; // prendiamo l'origine del lato come il primo vertice del lato corrispondente in Cell1Ds
             const unsigned int end = mesh.Cell1DVertices[edges[e]][1]; // prendiamo la fine del lato come il secondo vertice

             auto findOrigin = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), origin);
             if(findOrigin == mesh.Cell2DVertices[c].end())
             {
               cerr << "Wrong mesh" << endl;
               return 2;
             }

             auto findEnd = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), end);
             if(findEnd == mesh.Cell2DVertices[c].end())
             {
               cerr << "Wrong mesh" << endl;
               return 3;
             }

             //cout << "c: " << c << ", origin: " << *findOrigin << ", end: " << *findEnd << endl;

          }
        }
      }

      return true;
    }

    bool TriangularMesh::ImportCell0Ds(TriangularMesh& mesh, string filename, string parentFolder, double percentuale)
    {
      // leggiamo il contenuto dal file con uno stream
      ifstream file;
      file.open(parentFolder + "\\Raffinamento\\Dataset\\" + filename + "\\Cell0Ds.csv");
      if(file.fail())
        return false;

      list<string> listLines; // creiamo una lista di stringhe in cui mettiamo ogni riga
      string line;
      while (getline(file, line))
        listLines.push_back(line); // aggiungiamo la riga al fondo della lista con push_back
      file.close();

      listLines.pop_front(); // elimino il primo elemento (era la riga che ci dice cosa abbiamo)

      mesh.NumberCell0D = listLines.size(); // possiamo ora definire il numero di celle (sarà uguale al numero di righe)

      if (mesh.NumberCell0D == 0) // controllo generico che sia stato importato qualcosa
      {
        cerr << "There is no cell 0D" << endl;
        return false;
      }

      mesh.Cell0DId.reserve(floor(mesh.NumberCell0D*(8*percentuale)));
      mesh.Cell0DCoordinates.reserve(floor(mesh.NumberCell0D*(8*percentuale)));

      for (const string& line : listLines)
      {
        istringstream converter(line); // converte in stringa

        // memorizzo gli id, marker e le coordinate
        unsigned int id;
        unsigned int marker;
        Vector2d coord;

        converter >>  id >> marker >> coord(0) >> coord(1); // inserisco i dati

        mesh.Cell0DId.push_back(id); // mettiamo nella nostra mesh Cell0Id l'Id dei nostri vertici
        mesh.Cell0DCoordinates.push_back(coord); // mettiamo le coordinate

        // controllo se il marker esiste o meno e lo inserisco nei marker della mesh
        if( marker != 0) // se il marker non è 0 (non ci interessano i punti interni)...
        {
          if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
            mesh.Cell0DMarkers.insert({marker, {id}}); // inseriamo il marker con l'id se esiste già il marker
          else
            mesh.Cell0DMarkers[marker].push_back(id); // se no inseriamo il marker nuovo
        }
      }
      file.close();
      return true;
    }

    bool TriangularMesh::ImportCell1Ds(TriangularMesh& mesh, string filename, string parentFolder, double percentuale)
    {
      ifstream file;
      file.open(parentFolder + "\\Raffinamento\\Dataset\\" + filename + "\\Cell1Ds.csv");
      if(file.fail())
        return false;

      list<string> listLines;
      string line;
      while (getline(file, line))
        listLines.push_back(line);

      listLines.pop_front();

      mesh.NumberCell1D = listLines.size();

      if (mesh.NumberCell1D == 0)
      {
        cerr << "There is no cell 1D" << endl;
        return false;
      }

      mesh.Cell1DId.reserve(floor(mesh.NumberCell1D*(10*percentuale)));
      mesh.Cell1DVertices.reserve(floor(mesh.NumberCell1D*(10*percentuale)));
      mesh.Cell1DAdjacency.reserve(floor(mesh.NumberCell1D*(10*percentuale)));

      for (const string& line : listLines)
      {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices; // rispetto a Cell0D mettiamo i vertici
        Vector2i adiacenze = {-1, -1}; // definiamo un vettore nullo in cui andremo a inserire gli id dei triangoli adiacenti a ogni lato

        converter >>  id >> marker >> vertices(0) >> vertices(1); // inseriamo i dati

        // aggiungiamo per la mesh della Cell1Ds
        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);
        mesh.Cell1DAdjacency.push_back(adiacenze);

        if( marker != 0)
        {
          if (mesh.Cell1DMarkers.find(marker) == mesh.Cell1DMarkers.end())
            mesh.Cell1DMarkers.insert({marker, {id}});
          else
            mesh.Cell1DMarkers[marker].push_back(id);
        }
      }

      file.close();

      return true;
    }

    // dobbiamo modificare ImportCell2Ds per aggiungere le adiacenze e le aree
    bool TriangularMesh::ImportCell2Ds(TriangularMesh& mesh, string filename, string parentFolder, double percentuale)
    {
      ifstream file;
      file.open(parentFolder + "\\Raffinamento\\Dataset\\" + filename + "\\Cell2Ds.csv");

      if(file.fail())
        return false;

      list<string> listLines;
      string line;
      while (getline(file, line))
        listLines.push_back(line);

      listLines.pop_front();

      mesh.NumberCell2D = listLines.size();

      if (mesh.NumberCell2D == 0)
      {
        cerr << "There is no cell 2D" << endl;
        return false;
      }

      // adesso abbiamo i vertici e i lati
      mesh.Cell2DId.reserve(floor(mesh.NumberCell2D*(6*percentuale)));
      mesh.Cell2DVertices.reserve(floor(mesh.NumberCell2D*(6*percentuale)));
      mesh.Cell2DEdges.reserve(floor(mesh.NumberCell2D*(6*percentuale)));

      for (const string& line : listLines)
      {
        istringstream converter(line);

        unsigned int id;
        array<unsigned int, 3> vertices; // dichiariamo un array di dimensione 3 per i vertici
        array<unsigned int, 3> edges; // dichiariamo un array di dimensione 3 per i lati

        // adesso, siccome abbiamo degli array, aggiungiamo i vertici e i lati con dei cicli
        converter >>  id;
        for(unsigned int i = 0; i < 3; i++)
          converter >> vertices[i];
        for(unsigned int i = 0; i < 3; i++)
          converter >> edges[i];

        array<array<double,2>, 3> coordinates;

        for(unsigned int i=0; i<3; i++)
        {
            coordinates[i][0] = mesh.Cell0DCoordinates[vertices[i]][0]; // CONTROLLARE SE FARE CON = O CON <<
            coordinates[i][1] = mesh.Cell0DCoordinates[vertices[i]][1];
        }

        for(unsigned int i = 0; i < 3; i++)
        {
            int searchElement = edges[i];

            if(mesh.Cell1DAdjacency[searchElement] == Eigen::Vector2i(-1, -1))
                mesh.Cell1DAdjacency[searchElement][0] = id;
            else
                mesh.Cell1DAdjacency[searchElement][1] = id;
        }



        mesh.Cell2DId.push_back(id);
        mesh.Cell2DVertices.push_back(vertices);
        mesh.Cell2DEdges.push_back(edges);

        mesh.Cell2DAreeOrdinate.Inserimento(id, ComputeArea(coordinates));

      }

      file.close();
      return true;
    }

    // funzione usata per individuare i marker dei punti medi (punti inseriti)
    unsigned int TriangularMesh::findMarkerVertices(unsigned int id_1, unsigned int id_2){
        unsigned int m1 = findMarkerVerticeSingolo(id_1);
        unsigned int m2 = findMarkerVerticeSingolo(id_2);

        // unsigned int m_lati[] = {5, 6, 7, 8};
        // unsigned int m_angoli[] = {1, 2, 3, 4};

        unsigned int marker; // marker del punto medio da assegnare
        if (m1 == 0 || m2 == 0)
            marker = 0;
        else if (m1 == m2 && (m1 == 5 || m1 == 6 || m1 == 7 || m1 == 8)) // entrambi sullo stesso lato
            marker = m1;
        // angolo in basso a sinistra
        else if (m1 == 1 || m2 ==1 )
        {
            if(m1+m2 == 6)
                marker = 5;
            else if (m1+m2 == 9)
                marker = 8;
            else
                marker = 0; // ho angolo e un vertice interno

        }
        else if (m1+m2 != 13 && (abs(static_cast<int>(m1-m2)) == 3 || abs(static_cast<int>(m1-m2)) == 4)) // ho un angolo con uno dei due lati
        {
            if(m1 > m2) // sfrutto il fatto che i marker dei lati sono sempre maggiori
                marker = m1;
            else
                marker = m2;
        }
        else
            marker = 0;

        return marker;
    }


    void TriangularMesh::divideCell2(unsigned int id){

        Cell2DAreeOrdinate.Estrazione(id);


        // memorizzeremo tutti i dati del primo triangolo e del secondo (l'adiacente al primo sul lato più lungo) partendo dall'id
        array<unsigned int, 3>* vertices_1 = new array<unsigned int, 3>;

        array<unsigned int, 3>* edge_id_1 = new array<unsigned int, 3>;
        array<unsigned int, 3>* edge_coord1_1 = new array<unsigned int, 3>; // id dell'origine dei lati
        array<unsigned int, 3>* edge_coord2_1 = new array<unsigned int, 3>; // id della fine dei lati

        for(unsigned int i=0; i<3; i++)
        {
            (*vertices_1)[i] = Cell2DVertices[id][i];
            (*edge_id_1)[i] = Cell2DEdges[id][i];
        }
        for(unsigned int i=0; i<3; i++)
        {
            (*edge_coord1_1)[i] = Cell1DVertices[(*edge_id_1)[i]][0];
            (*edge_coord2_1)[i] = Cell1DVertices[(*edge_id_1)[i]][1];
        }

        unsigned int* lato_max = new unsigned int;
        double* lunghezza = new double;
        *lunghezza = 0.0;

        for(unsigned int i=0; i<3; i++)
        {
            if(ComputeDistance(Cell0DCoordinates[(*edge_coord1_1)[i]],Cell0DCoordinates[(*edge_coord2_1)[i]]) - *lunghezza > 1e-12)
            {
                *lato_max = (*edge_id_1)[i];
                *lunghezza = ComputeDistance(Cell0DCoordinates[(*edge_coord1_1)[i]],Cell0DCoordinates[(*edge_coord2_1)[i]]);
            }
        }

        // nelle adiacenze cerco il triangolo adiacente al lato max trovato
        // Controlliamo di non essere su un bordo, ovvero se il marker del lato_max è 5, 6, 7 o 8
        bool* d = new bool;
        *d = true;

        if(Cell1DAdjacency[*lato_max][1] == -1)
            *d = false; // lato non è interno


        // Voglio solo lavorare sul secondo triangolo se possibile ma, per poter lavorare sui dati in diverse parti del codice, devo prima dichiarare le variabili globalmente e poi inizializzarle

        unsigned int* tri_adiacente = new unsigned int;
        array<unsigned int, 3>* vertices_2 = new array<unsigned int, 3>;

        array<unsigned int, 3>* edge_id_2 = new array<unsigned int, 3>;
        array<unsigned int, 3>* edge_coord1_2 = new array<unsigned int, 3>; // ascisse dei vertici
        array<unsigned int, 3>* edge_coord2_2 = new array<unsigned int, 3>; // ordinate dei vertici

        unsigned int* vertice_opposto_2 = new unsigned int; // il vertice opposto al lato_max nel secondo triangolo
        unsigned int* indice_vertice_per_antiorario_2 = new unsigned int; // memorizzo l'indice per mantenere il senso antiorario di vertici e lati nel secondo triangolo e nel secondo triangolo nuovo

        if(*d)
        {
            if(Cell1DAdjacency[*lato_max][0] != id)
                *tri_adiacente = Cell1DAdjacency[*lato_max][0];
            else
                *tri_adiacente = Cell1DAdjacency[*lato_max][1];

            Cell2DAreeOrdinate.Estrazione(*tri_adiacente);

            // salvo tutte le informazioni inerenti al triangolo adiacente

            for(unsigned int i=0; i<3; i++)
            {
                (*vertices_2)[i] = Cell2DVertices[*tri_adiacente][i];
                (*edge_id_2)[i] = Cell2DEdges[*tri_adiacente][i];
                (*edge_coord1_2)[i] = Cell1DVertices[(*edge_id_2)[i]][0];
                (*edge_coord2_2)[i] = Cell1DVertices[(*edge_id_2)[i]][1];
            }

            for(unsigned int i=0; i<3; i++)
            {
                if((*vertices_2)[i] != Cell1DVertices[*lato_max][0] && (*vertices_2)[i] != Cell1DVertices [*lato_max][1])
                {
                    *vertice_opposto_2 = (*vertices_2)[i];
                    if(i != 2) // se siamo al fondo del vettore, dovremo tornare all'indice 0
                        *indice_vertice_per_antiorario_2 = i+1;
                    else
                        *indice_vertice_per_antiorario_2 = 0;
                    break;
                }
            }
        }

        // cerchiamo il vertice opposto al lato massimo

        unsigned int* vertice_opposto = new unsigned int;
        unsigned int* indice_vertice_per_antiorario = new unsigned int;
        // memorizziamo l'indice dove prendiamo il vertice opposto in Cell2DVertices in modo da mantenere l'ordine in senso antiorario dei vertici
         for(unsigned int i=0; i<3; i++)
         {
             if ((*vertices_1)[i] != Cell1DVertices[*lato_max][0] && (*vertices_1)[i] != Cell1DVertices[*lato_max][1])
             {
                 *vertice_opposto = (*vertices_1)[i];
                 if(i != 2) // se siamo al fondo del vettore, dovremo tornare all'indice 0
                     *indice_vertice_per_antiorario = i+1;
                 else
                     *indice_vertice_per_antiorario = 0;
                 break;
             }
         }


        unsigned int* primo_vertice = new unsigned int;
        *primo_vertice = Cell2DVertices[id][*indice_vertice_per_antiorario]; // uno dei due vertici adiacenti al lato massimo è scelto come primo vertice
        unsigned int* secondo_vertice = new unsigned int;
        if(*indice_vertice_per_antiorario == 2) // se siamo al fondo del vettore, dovremo tornare all'indice 0
            *secondo_vertice = Cell2DVertices[id][0];
        else
            *secondo_vertice = Cell2DVertices[id][*indice_vertice_per_antiorario + 1];


        // cerco il punto medio del lato massimo, è un nuovo vertice

        unsigned int* punto_medio = new unsigned int;
        *punto_medio = NumberCell0D;
        NumberCell0D += 1;
        Vector2d* temp_coord = new Vector2d;

        (*temp_coord)[0] = (Cell0DCoordinates[*primo_vertice][0] + Cell0DCoordinates[*secondo_vertice][0])/2;
        (*temp_coord)[1] = (Cell0DCoordinates[*primo_vertice][1] + Cell0DCoordinates[*secondo_vertice][1])/2;

        Cell0DId.push_back(*punto_medio);
        Cell0DCoordinates.push_back(*temp_coord);

        unsigned int* marker_punto_medio = new unsigned int;
        *marker_punto_medio = findMarkerVertices(*primo_vertice, *secondo_vertice);
        if(*marker_punto_medio!=0)
            Cell0DMarkers[*marker_punto_medio].push_back(*punto_medio);

        unsigned int* lato_nuovo = new unsigned int;
        *lato_nuovo = NumberCell1D;
        NumberCell1D += 1;

        Vector2i* vertici_lato_nuovo = new Vector2i;
        (*vertici_lato_nuovo)(0) = *vertice_opposto;
        (*vertici_lato_nuovo)(1) = *punto_medio;

        Cell1DId.push_back(*lato_nuovo);
        Cell1DVertices.push_back(*vertici_lato_nuovo);

        for(unsigned int i=0; i<2; i++)
        {
            // aggiustiamo i vertici del lato massimo che abbiamo diviso in due;
            // per coerenza della base dati, teniamo lo stesso id sul lato e su uno dei vertici (quindi cambiamo solo un vertice)
            if(Cell1DVertices[*lato_max][0] == *primo_vertice)
                Cell1DVertices[*lato_max][1] = *punto_medio;
            else
                Cell1DVertices[*lato_max][0] = *punto_medio;
        }

        // definiamo l'altra metà del lato massimo, con primo vertice il punto medio
        unsigned int* lato_diviso = new unsigned int;
        *lato_diviso = NumberCell1D;
        NumberCell1D += 1;

        Vector2i* vertici_lato_diviso = new Vector2i;
        (*vertici_lato_diviso)(0) = *punto_medio;
        (*vertici_lato_diviso)(1) = *secondo_vertice;

        unsigned int* m_lato_diviso = new unsigned int;
        *m_lato_diviso = findMarkerLato(*lato_max);
        if(*m_lato_diviso!=0)
            Cell1DMarkers[*m_lato_diviso].push_back(*lato_diviso);


        Cell1DId.push_back(*lato_diviso);
        Cell1DVertices.push_back(*vertici_lato_diviso);



        // ridefiniamo il triangolo uno rispecificandone i lati e i vertici (ovvero con id dato in input alla funzione stessa)
        // dobbiamo sostituire a secondo_vertice il punto_medio
        for(unsigned int i=0; i<3; i++)
        {
            if(Cell2DVertices[id][i] == *secondo_vertice)
            {
                Cell2DVertices[id][i] = *punto_medio;
                break;
            }
        }



        // ora ridefiniamo uno dei due lati

        unsigned int* lato_da_escludere = new unsigned int;
        unsigned int* lato_da_includere = new unsigned int;
        for(unsigned int i=0; i<3; i++)
        {
            if(Cell2DEdges[id][i] != *lato_max && (Cell1DVertices[Cell2DEdges[id][i]][0] == *primo_vertice || Cell1DVertices[Cell2DEdges[id][i]][1] == *primo_vertice))
                *lato_da_includere = Cell2DEdges[id][i];
        }

        for(unsigned int i=0; i<3; i++)
        {
            if(Cell2DEdges[id][i] != *lato_max && Cell2DEdges[id][i] != *lato_da_includere)
            {
                *lato_da_escludere = Cell2DEdges[id][i]; // memorizzo il lato da escludere perchè dovrò poi includerlo nel triangolo nuovo
                Cell2DEdges[id][i] = *lato_nuovo;
                break;
            }
        }

        array<array<double, 2>, 3>* coordinates_1 = new array<array<double, 2>, 3>;

        *coordinates_1 = findCoordinates(id);

        double* area_1 = new double;
        *area_1 = ComputeArea(*coordinates_1);
        Cell2DAreeOrdinate.Inserimento(id, *area_1);

        // definiamo il nuovo triangolo

        unsigned int* triangolo_nuovo = new unsigned int;
        *triangolo_nuovo = NumberCell2D;
        NumberCell2D += 1;

        for(unsigned int i = 0; i<2; i++)
        {
            if(Cell1DAdjacency[*lato_da_escludere][i] == id)
            {
                Cell1DAdjacency[*lato_da_escludere][i] = *triangolo_nuovo;
            }
        }

        array<unsigned int, 3>* vertici_triangolo_nuovo = new array<unsigned int, 3>;
        (*vertici_triangolo_nuovo)[0] = *punto_medio;
        (*vertici_triangolo_nuovo)[1] = *secondo_vertice;
        (*vertici_triangolo_nuovo)[2] = *vertice_opposto;


        array<unsigned int, 3>* lati_triangolo_nuovo = new array<unsigned int, 3>;
        (*lati_triangolo_nuovo)[0] = *lato_diviso;
        (*lati_triangolo_nuovo)[1] = *lato_da_escludere;
        (*lati_triangolo_nuovo)[2] = *lato_nuovo;


        Vector2i* adiacenze_lato_nuovo = new Vector2i;
        (*adiacenze_lato_nuovo)(0) = id;
        (*adiacenze_lato_nuovo)(1) = *triangolo_nuovo;
        Cell1DAdjacency.push_back(*adiacenze_lato_nuovo);

        Cell2DId.push_back(*triangolo_nuovo);
        Cell2DVertices.push_back(*vertici_triangolo_nuovo);
        Cell2DEdges.push_back(*lati_triangolo_nuovo);


        array<array<double, 2>, 3>* coordinates_2 = new array<array<double, 2>, 3>;

        *coordinates_2 = findCoordinates(*triangolo_nuovo);

        double* area_2 = new double;
        *area_2 = ComputeArea(*coordinates_2);

        Cell2DAreeOrdinate.Inserimento(*triangolo_nuovo, *area_2);

        //Aggiungiamo le aree nella lista di aree in modo da poterle considerare

        /// Abbiamo finito di dividere e ridefinire il triangolo iniziale.
        /// Adesso dividiamo il triangolo adiacente
        if(*d)
        {
            unsigned int* lato_nuovo_2 = new unsigned int;
            *lato_nuovo_2 = NumberCell1D;
            NumberCell1D += 1;

            Vector2i* vertici_lato_nuovo_2 = new Vector2i;
            (*vertici_lato_nuovo_2)[0] = *vertice_opposto_2;
            (*vertici_lato_nuovo_2)[1] = *punto_medio;
            Cell1DId.push_back(*lato_nuovo_2);
            Cell1DVertices.push_back(*vertici_lato_nuovo_2);


            unsigned int* lato_da_escludere_2 = new unsigned int;
            for(unsigned int i=0; i<3;i++)
            {
                unsigned int* temp_lato = new unsigned int;
                *temp_lato = (*edge_id_2)[i];
                array<unsigned int, 2>* temp_vertici = new array<unsigned int, 2>;
                *temp_vertici = {(*edge_coord1_2)[i],(*edge_coord2_2)[i]};
                //if(temp_lato != lato_max && (temp_vertici[0] == primo_vertice || temp_vertici[1] == primo_vertice))
                //        lato_da_includere_2 = temp_lato;
                if(*temp_lato != *lato_max && (*temp_vertici)[0] != *primo_vertice && (*temp_vertici)[1] != *primo_vertice)
                        *lato_da_escludere_2 = *temp_lato;
            }


            for(unsigned int i=0;i<3;i++)
            {
                if(Cell2DVertices[*tri_adiacente][i] == *secondo_vertice)
                {
                    Cell2DVertices[*tri_adiacente][i] = *punto_medio;
                    break;
                }
            }

            for(unsigned int i=0;i<3;i++)
            {
                if(Cell2DEdges[*tri_adiacente][i] == *lato_da_escludere_2)
                {
                    Cell2DEdges[*tri_adiacente][i] = *lato_nuovo_2;
                    break;
                }
            }

            array<array<double, 2>, 3>* coordinates_3 = new array<array<double, 2>, 3>;

            *coordinates_3 = findCoordinates(*tri_adiacente);

            double* area_3 = new double;
            *area_3 = ComputeArea(*coordinates_3);

            Cell2DAreeOrdinate.Inserimento(*tri_adiacente, *area_3);

            unsigned int* triangolo_nuovo_2 = new unsigned int;
            *triangolo_nuovo_2 = NumberCell2D;
            NumberCell2D += 1;

            for(unsigned int i = 0; i<2; i++)
            {
                if(Cell1DAdjacency[*lato_da_escludere_2][i] == *tri_adiacente)
                    Cell1DAdjacency[*lato_da_escludere_2][i] = *triangolo_nuovo_2;
            }

            array<unsigned int, 3>* vertici_triangolo_nuovo_2 = new array<unsigned int, 3>;
            *vertici_triangolo_nuovo_2 = {*punto_medio, *vertice_opposto_2, *secondo_vertice};

            array<unsigned int, 3>* lati_triangolo_nuovo_2 = new array<unsigned int, 3>;
            *lati_triangolo_nuovo_2 = {*lato_nuovo_2, *lato_da_escludere_2, *lato_diviso};


            Cell2DId.push_back(*triangolo_nuovo_2);
            Cell2DVertices.push_back(*vertici_triangolo_nuovo_2);
            Cell2DEdges.push_back(*lati_triangolo_nuovo_2);


            array<array<double, 2>, 3>* coordinates_4 = new array<array<double, 2>, 3>;

            *coordinates_4 = findCoordinates(*triangolo_nuovo_2);

            double* area_4 = new double;
            *area_4 = ComputeArea(*coordinates_4);

            Cell2DAreeOrdinate.Inserimento(*triangolo_nuovo_2, *area_4);

            Vector2i* adiacenze_lato_diviso = new Vector2i;
            (*adiacenze_lato_diviso)[0] = *triangolo_nuovo;
            (*adiacenze_lato_diviso)[1] = *triangolo_nuovo_2;
            Cell1DAdjacency.push_back(*adiacenze_lato_diviso);

            Vector2i* adiacenze_lato_nuovo_2 = new Vector2i;
            (*adiacenze_lato_nuovo_2)[0] = *tri_adiacente;
            (*adiacenze_lato_nuovo_2)[1] = *triangolo_nuovo_2;
            Cell1DAdjacency.push_back(*adiacenze_lato_nuovo_2);

        }

        if(!(*d))
        {
            Vector2i* adiacenze_lato_diviso = new Vector2i;
            (*adiacenze_lato_diviso)[0] = *triangolo_nuovo;
            (*adiacenze_lato_diviso)[1] = -1;
            Cell1DAdjacency.push_back(*adiacenze_lato_diviso);
        }

    }

    bool TriangularMesh::ExportMesh(TriangularMesh& mesh, string file){
        // Andiamo a scegliere il path corretto per il Dataset che viene dato in input alla nostra funzione di Import
        std::string filePath;

        char buffer[MAX_PATH];
        DWORD length = GetModuleFileName(NULL, buffer, MAX_PATH);
        if (length != 0) {
            filePath = std::string(buffer, length);
        }

        std::string parentFolder = filePath;

            for (int i = 0; i < 2; ++i) {
                size_t lastSeparator = parentFolder.find_last_of("/\\");
                if (lastSeparator != std::string::npos) {
                    parentFolder = parentFolder.substr(0, lastSeparator);
                }
            }
        if(!mesh.ExportCell0Ds(mesh, file, parentFolder))
            return false;
        if(!mesh.ExportCell1Ds(mesh, file, parentFolder))
            return false;
        if(!mesh.ExportCell2Ds(mesh, file, parentFolder))
            return false;
        if(!mesh.ExportParaview(mesh, file, parentFolder))
            return false;

        return true;
    }

    bool TriangularMesh::ExportCell0Ds(TriangularMesh& mesh, string file, string parentFolder){

        std::ofstream outputFile(parentFolder + "\\Raffinamento\\Dataset\\" + file + "\\ExportCell0Ds.csv");
        if (!outputFile.is_open()) {
            cerr << "Failed to open the file." << endl;
            return false;
        }
        outputFile << "id marker x y" << "\n"; // Vogliamo cominciare a scrivere dalla seconda riga in poi

        for(unsigned int i=0; i<mesh.NumberCell0D; i++)
        {
            outputFile << mesh.Cell0DId[i] << " " << mesh.findMarkerVerticeSingolo(mesh.Cell0DId[i]) << " " ;
            outputFile << mesh.Cell0DCoordinates[mesh.Cell0DId[i]][0] << " " << mesh.Cell0DCoordinates[mesh.Cell0DId[i]][1] << "\n";
        }

        outputFile.close();

        return true;
    }

    bool TriangularMesh::ExportCell1Ds(TriangularMesh& mesh, string file, string parentFolder){

        std::ofstream outputFile(parentFolder + "\\Raffinamento\\Dataset\\" + file + "\\ExportCell1Ds.csv");
        if (!outputFile.is_open()) {
            cerr << "Failed to open the file." << endl;
            return false;
        }
        outputFile << "Id Marker Origin End" << "\n"; // Vogliamo cominciare a scrivere dalla seconda riga in poi

        for(unsigned int i=0; i<mesh.NumberCell1D; i++)
        {
            outputFile << mesh.Cell1DId[i] << " " << mesh.findMarkerLato(mesh.Cell1DId[i]) << " ";
            outputFile << mesh.Cell1DVertices[mesh.Cell1DId[i]][0] << " " << mesh.Cell1DVertices[mesh.Cell1DId[i]][1] << "\n";
        }

        outputFile.close();

        return true;
    }

    bool TriangularMesh::ExportCell2Ds(TriangularMesh& mesh, string file, string parentFolder){

        std::ofstream outputFile(parentFolder + "\\Raffinamento\\Dataset\\" + file + "\\ExportCell2Ds.csv");
        if (!outputFile.is_open()) {
            cerr << "Failed to open the file." << endl;
            return false;
        }
        outputFile << "Id Vertices Edges" << "\n"; // Vogliamo cominciare a scrivere dalla seconda riga in poi

        for(unsigned int i=0; i<mesh.NumberCell2D; i++)
        {
            outputFile << mesh.Cell2DId[i] << " ";

            outputFile << mesh.Cell2DVertices[mesh.Cell2DId[i]][0] << " " << mesh.Cell2DVertices[mesh.Cell2DId[i]][1] << " " << mesh.Cell2DVertices[mesh.Cell2DId[i]][2] << " ";
            outputFile << mesh.Cell2DEdges[mesh.Cell2DId[i]][0] << " " << mesh.Cell2DEdges[mesh.Cell2DId[i]][1] << " " << mesh.Cell2DEdges[mesh.Cell2DId[i]][2] << "\n";
        }

        outputFile.close();

        return true;
    }
    bool TriangularMesh::ExportParaview(TriangularMesh& mesh, string file, string parentFolder){
        std::ofstream outputFile(parentFolder + "\\Raffinamento\\Dataset\\" + file + "\\ExportParaview.csv");
        if (!outputFile.is_open()) {
            cerr << "Failed to open the file." << endl;
            return false;
        }

        outputFile << "Id Marker Origin End X-origin Y-origin X-end Y-end" << "\n";

        for(unsigned int i=0; i<mesh.NumberCell1D; i++)
        {
            outputFile << mesh.Cell1DId[i] << " " << mesh.findMarkerLato(mesh.Cell1DId[i]) << " ";
            outputFile << mesh.Cell1DVertices[mesh.Cell1DId[i]][0] << " " << mesh.Cell1DVertices[mesh.Cell1DId[i]][1] << " ";
            outputFile << mesh.Cell0DCoordinates[mesh.Cell1DVertices[mesh.Cell1DId[i]][0]][0] << " " << mesh.Cell0DCoordinates[mesh.Cell1DVertices[mesh.Cell1DId[i]][0]][1] << " " ;
            outputFile << mesh.Cell0DCoordinates[mesh.Cell1DVertices[mesh.Cell1DId[i]][1]][0] << " " << mesh.Cell0DCoordinates[mesh.Cell1DVertices[mesh.Cell1DId[i]][1]][1] << "\n";
        }
        outputFile.close();
    }

}
