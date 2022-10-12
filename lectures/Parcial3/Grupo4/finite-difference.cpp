// Linear boundary-value problem

// Sea una ecuación del tipo: y'' = P(x) y' + Q(x) y + R(x)

#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <fstream>

#include "finite-difference.h"

// --------------------------------
// --------- CONSTRUCTOR ----------
// --------------------------------
FiniteDifferences::FiniteDifferences()
{

    this->setN();
    this->setA();
    this->setB();
    this->setAlpha();
    this->setBeta();
}

// --------------------------------
// ------------ SET ---------------
// --------------------------------

void FiniteDifferences::setN()
{
    std::cout << "Ingresa N:" << std::endl;
    this->N = this->verificarDatos();
}

void FiniteDifferences::setA()
{
    std::cout << "Ingresa A:" << std::endl;
    this->a = this->verificarDatos();
}

void FiniteDifferences::setB()
{
    std::cout << "Ingresa B:" << std::endl;
    this->b = this->verificarDatos();
}

void FiniteDifferences::setAlpha()
{
    std::cout << "Ingresa Alpha:" << std::endl;
    this->alpha = this->verificarDatos();
}

void FiniteDifferences::setBeta()
{
    std::cout << "Ingresa Beta:" << std::endl;
    this->beta = this->verificarDatos();
}

std::vector<double> FiniteDifferences::getX()
{
    return vectX;
}

// --------------------------------
// --------- FUNCTIONS ------------
// --------------------------------

float FiniteDifferences::p(float x)
{
    return (-1 * 2) / x;
}

float FiniteDifferences::q(float x)
{
    return 2 / pow(x, 2);
}

float FiniteDifferences::r(float x)
{
    return sin(log(x)) / pow(x, 2);
}

// --------------------------------
// ----- CENTERED DIFFERENCE ------
// --------------------------------

void FiniteDifferences::centeredDifference(std::vector<double> &vectA, std::vector<double> &vectB,
                                           std::vector<double> &vectC, std::vector<double> &vectD,
                                           std::vector<double> &vectX)
{

    // step size
    float h = (b - a) / (N + 1);

    // mesh points
    float x = a + h;
    vectX.push_back(a);
    vectX.push_back(x);

    std::cout << " ------ Definir  X ------ " << std::endl;
    std::cout << "x1=" << x << ", h=" << h << std::endl;

    // First Element
    vectA.push_back(2 + pow(h, 2) * this->q(x));
    vectB.push_back(-1 + (h / 2) * this->p(x));
    vectC.push_back(0);
    vectD.push_back(-1 * pow(h, 2) * this->r(x) + (1 + (h / 2) * this->p(x)) * alpha);

    // From 2 to N-1
    for (size_t i = 2; i <= N - 1; i++)
    {
        x = a + i * h;
        vectX.push_back(x);
        std::cout << "x" << i << "=" << x << std::endl;

        vectA.push_back(2 + pow(h, 2) * this->q(x));
        vectB.push_back(-1 + (h / 2) * this->p(x));
        vectC.push_back(-1 - (h / 2) * this->p(x));
        vectD.push_back(-1 * pow(h, 2) * this->r(x));
    }

    // Last element
    x = b - h;
    vectX.push_back(x);
    vectX.push_back(b);
    std::cout << "x" << N << "=" << x << std::endl;

    vectA.push_back(2 + pow(h, 2) * this->q(x));
    vectB.push_back(0);
    vectC.push_back(-1 - (h / 2) * this->p(x));
    vectD.push_back(-1 * pow(h, 2) * this->r(x) + (1 - (h / 2) * this->p(x)) * beta);

    this->vectX = vectX;
}

// --------------------------------------
// ----- TRIDIAGONAL LINEAR SYSTEM ------
// --------------------------------------
std::vector<double> FiniteDifferences::tridiagonaLinearSystem()
{

    std::vector<double> vectA;
    std::vector<double> vectB;
    std::vector<double> vectC;
    std::vector<double> vectD;
    std::vector<double> vectX;

    centeredDifference(vectA, vectB, vectC, vectD, vectX);

    std::vector<double> vectL;
    std::vector<double> vectU;
    std::vector<double> vectZ;

    // First element
    vectL.push_back(vectA.at(0));
    vectU.push_back(vectB.at(0) / vectL.at(0));
    vectZ.push_back(vectD.at(0) / vectL.at(0));

    // Elements From 1 to N (positions from 2 to N-1 in Pseudocode)
    // W has element 0
    for (size_t i = 1; i < N - 1; i++)
    {
        vectL.push_back(vectA.at(i) - vectC.at(i) * vectU.at(i - 1));
        vectU.push_back(vectB.at(i) / vectL.at(i));
        vectZ.push_back((vectD.at(i) - vectC.at(i) * vectZ.at(i - 1)) / vectL.at(i));
    }

    // Last element N-1 (N in Pseudocode)
    vectL.push_back(vectA.at(N - 1) - vectC.at(N - 1) * vectU.at(N - 2)); // A(8), C(8), U(7)
    vectU.push_back(0);
    vectZ.push_back((vectD.at(N - 1) - vectC.at(N - 1) * vectZ.at(N - 2)) / vectL.at(N - 1));

    std::cout << " ------ Definir  W ------ " << std::endl;

    std::vector<double> vectW(N + 2); // se agregan 2: W(O), W(N+1)

    // First position
    vectW.at(0) = alpha;

    // Last position
    vectW.at(N + 1) = beta; // W(10)

    // Position N in W and position N in Z in Pseudocode
    vectW.at(N) = vectZ.at(N - 1); // W(9) , Z(8)

    // From N-1 to 1 for W, From N-2 to 0 in U and Z
    for (size_t i = N - 1; i >= 1; i--) // va de 8 A 1
    {
        // N para Z y para U es Z(9), U(9), pero lo que busco es Z(8)
        vectW.at(i) = vectZ.at(i - 1) - vectU.at(i - 1) * vectW.at(i + 1);
    }

    return vectW;
}

// --------------------------------
// -------- VERIFY DATA -----------
// --------------------------------
double FiniteDifferences::verificarDatos()
{

    double num = 0;
    std::cin >> num;

    while (std::cin.fail())
    {
        std::cout << "Debes ingresar un número." << std::endl;
        std::cin.clear();
        std::cin >> num;
    }

    return num;
}

// --------------------------------
// -------- SAVE - PRINT -----------
// --------------------------------
void FiniteDifferences::saveSolutions(std::vector<double> k, std::vector<double> x)
{

    std::ofstream MyFile("solucion.txt");

    for (size_t i = 0; i < k.size(); i++)
    {
        std::cout << "w" << i << "=" << k.at(i) << std::endl;
        MyFile << x.at(i) << "          " << k.at(i) << std::endl;
    }
}