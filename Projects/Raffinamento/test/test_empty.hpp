#ifndef __TEST_EMPTY_H
#define __TEST_EMPTY_H

#include <gtest/gtest.h>
#include "iostream"

#include "empty_class.hpp"

using namespace testing;
using namespace std;
using namespace ProjectLibrary;

// Nelle funzioni di Import prese dall'esercitazione 4 sono già presenti test che verifichino che ci sia coerenza tra i vertici associati a un lato e tra i vertici e i lati associati a un triangolo.
// Un possibile test da fare è ripetere queste verifiche con la mesh finale
/*Template
TEST(TestEmpty, TestEmpty)
{
  ProjectLibrary::Empty empty;
  ASSERT_NO_THROW(empty.Show());
}
*/

TEST(TestEmpty_Class, TestComputeArea)
{
    array<array<double, 2>, 3> vertices = {0,0, 0,1, 1,0.5};
    double area = ComputeArea(vertices);
    double risultato = 0.5;
    EXPECT_EQ(area, risultato);
}

TEST(TestEmpty_Class, TestfindMarkerVerticeSingolo)
{
    TriangularMesh mesh;
    mesh.ImportMesh(mesh, "Test1", 0.7);

    unsigned int markerV1 = mesh.findMarkerVerticeSingolo(4); // marker di vertice INTERNO
    unsigned int expectedV1 = 0;
    EXPECT_EQ(markerV1, expectedV1);

    unsigned int markerV2 = mesh.findMarkerVerticeSingolo(0); // marker di vertice SULL'ANGOLO
    unsigned int expectedV2 = 1;
    EXPECT_EQ(markerV2, expectedV2);

    unsigned int markerV3 = mesh.findMarkerVerticeSingolo(18); // marker di vertice SUL LATO
    unsigned int expectedV3 = 6;
    EXPECT_EQ(markerV3, expectedV3);

}

TEST(TestEmpty_Class, TestfindMarkerLato)
{
    TriangularMesh mesh;
    mesh.ImportMesh(mesh, "Test1", 0.7);

    unsigned int markerL1 = mesh.findMarkerLato(0); // marker di lato INTERNO
    unsigned int expectedL1 = 0;
    EXPECT_EQ(markerL1, expectedL1);

    unsigned int markerL2 = mesh.findMarkerLato(68); // marker di lato SUL BORDO
    unsigned int expectedL2 = 6;
    EXPECT_EQ(markerL2, expectedL2);
}

TEST(TestEmpty_Class, TestfindMarkerVertices)
{
    TriangularMesh mesh;
    mesh.ImportMesh(mesh, "Test1",0.7);

    unsigned int marker1 = mesh.findMarkerVertices(6,13); // marker del punto medio tra due vertici sullo stesso lato
    unsigned int expected1 = 5;
    EXPECT_EQ(marker1, expected1);

    unsigned int marker2 = mesh.findMarkerVertices(4,9); // marker del punto medio tra due vertici interni
    unsigned int expected2 = 0;
    EXPECT_EQ(marker2, expected2);
}

TEST(TestEmpty_Class, TestComputeDistance)
{
    Vector2d coord1 = {0,0};
    Vector2d coord2 = {3,4};
    double distance = ComputeDistance(coord1,coord2);
    double calcolo = 5;
    EXPECT_EQ(distance, calcolo);

}

TEST(TestEmpty_Class, TestDivideCell2)
{
    TriangularMesh mesh;
    mesh.ImportMesh(mesh, "TestVerifica", 0.6);
    mesh.divideCell2(3);

    array <unsigned int,3> risultato1 = mesh.Cell2DVertices[3];
    array <unsigned int,3> expected1 = {3,6,4};
    EXPECT_EQ(risultato1, expected1);

    array <unsigned int,3> risultato2 = mesh.Cell2DEdges[3];
    array <unsigned int,3> expected2 = {3,10,9};
    EXPECT_EQ(risultato2, expected2);
}

TEST(TestEmpty_Class, TestAreaLimite)
{

    TriangularMesh mesh;
    mesh.ImportMesh(mesh, "Test1", 0.7);

    unsigned int posizione_limite = floor(mesh.NumberCell2D * 0.7); // Andremo a diminuire fino al primo 60% delle aree iniziali dei triangoli della mesh
    //double area_limite = ComputeArea(mesh.Cell2DAreeOrdinate[posizione_limite]);

    double area_limite = 0.0;

    auto attuale = mesh.Cell2DAreeOrdinate.testa->successivo;
    for(unsigned int i=0; i<posizione_limite; i++)
    {
        attuale = attuale->successivo;
    }
    area_limite = attuale->area;

    while(mesh.Cell2DAreeOrdinate.testa->area >= area_limite)
    {
        mesh.divideCell2(mesh.Cell2DAreeOrdinate.testa->id);
    }

    double area_ottenuta = mesh.Cell2DAreeOrdinate.testa->area;
    EXPECT_LT(area_ottenuta, area_limite);

}





#endif // __TEST_EMPTY_H
