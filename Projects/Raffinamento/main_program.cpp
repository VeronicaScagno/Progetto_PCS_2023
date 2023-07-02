#include "empty_class.hpp"
#include <iostream>
#include "Eigen/Eigen"
#include <fstream>
#include <cmath>


using namespace std;
using namespace Eigen;
using namespace ProjectLibrary;

int main()
{
    string dataset = "Test2";

    double percentuale = 0.70;

    TriangularMesh mesh;


    if(!mesh.ImportMesh(mesh, dataset, percentuale))
    {
      return 1;
    }

    unsigned int posizione_limite = floor(mesh.NumberCell2D * percentuale);

    double area_limite;

    auto attuale = mesh.Cell2DAreeOrdinate.testa;
    for(unsigned int i=0; i<posizione_limite; i++)
    {
        attuale = attuale->successivo;
    }
    area_limite = attuale->area;

    while(mesh.Cell2DAreeOrdinate.testa->area >= area_limite - 1e-24)
    {
        mesh.divideCell2(mesh.Cell2DAreeOrdinate.testa->id);
    }

    mesh.ExportMesh(mesh, dataset);
}

