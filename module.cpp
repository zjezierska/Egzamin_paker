//
//  module.cpp
//  paker
//
//  Created by Zuzanna Jezierska on 11/02/2022.
//

#include <Python.h>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// PyObject -> Vector
// pyutils.cpp https://gist.github.com/rjzak/5681680
vector<int> listTupleToVector_Int(PyObject* incoming) {
    vector<int> data;
    // sprawdzamy czy mamy tuple czy list i wsadzamy elementy do wektora
    // w tym programie niby sprawdzanie czy to tuple jest raczej niepotrzebne
    if (PyTuple_Check(incoming)) {
        for(Py_ssize_t i = 0; i < PyTuple_Size(incoming); i++) {
            PyObject *value = PyTuple_GetItem(incoming, i);
            data.push_back( PyFloat_AsDouble(value) );
        }
    } else {
        if (PyList_Check(incoming)) {
            for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++) {
                PyObject *value = PyList_GetItem(incoming, i);
                data.push_back( PyFloat_AsDouble(value) );
            }
        } else {
            throw logic_error("Passed PyObject pointer was not a list or tuple!");
        }
    }
    return data;
}

// Vector -> PyObject
PyObject* vectorToList(const vector<int> data) {
    PyObject* PythList = PyList_New(data.size());
    
    // konwertujemy elementy na typ Pythona i wsadzamy do listy
    for (int i = 0; i < data.size(); i++) {
        PyObject *numer = PyLong_FromLong(data[i]);
        PyList_SET_ITEM(PythList, i, numer);
    }
    return PythList;
}

// glowna funkcja
PyObject* plecaczek(PyObject *self, PyObject *args) {
    
    PyObject* capacity;
    PyObject* weights;
    
    // Parse arguments
    if(!PyArg_ParseTuple(args, "OO", &capacity, &weights))
        return NULL;
    
    long W = PyLong_AsLong(capacity); // pojemnosc plecaka
    vector <int> wt = listTupleToVector_Int(weights); // wagi
    
    wt.insert(wt.begin(), 0);
    int n = wt.size() - 1; // ilosc rzeczy
    
    int i, j;
    int K[n+1][W+1]; // K[i][j] przechowuje jakie maksymalne wypelnienie mozna osiagnac dla i pierwszych przedmiotow oraz pojemnosci plecaka j
    
    
    // 0-1 knapsack problem with values = weights
    for (i = 0; i <= n; i++) {
        for (j = 0; j <= W; j++) {
            if(i == 0 || j == 0)
                K[i][j] = 0;
            else if (wt[i-1] <= j)
                K[i][j] = max(wt[i-1] + K[i - 1][j - wt[i - 1]], K[i - 1][j]);
            else
                K[i][j] = K[i-1][j];
        }
    }
    
    int maxi = K[n][W]; // maksymalna osiagnieta wartosc <= W to K[n][W]
    vector <int> itemy; // tu bedziemy przechowywac rzeczy, które wchodzą do plecaka, poniżej przedstawienie JEDNEJ działającej możliwości
    
    // Można też zaimplementować kod dla wielu możliwych rozwiązań, np. dla paker.pakujplecak(100, [80,15,10,5]) mamy [80,15], [80,10,5]).
    
    // Jest to znany problem: Perfect Sum Problem (Print all subsets with given sum), jednak zwiększa on znacząco trudność obliczeniową problemu https://www.geeksforgeeks.org/perfect-sum-problem-print-subsets-given-sum/
    
    for(i = n; i > 0 && maxi > 0; i--) {
        if (maxi == K[i-1][W])
            continue; // tzn że tej rzeczy nie ma w plecaku
        else {
            itemy.push_back(wt[i-1]); // ta rzecz jest w plecaku, odejmujemy jej wage od osiagnietej i szukamy dalej
            maxi = maxi - wt[i-1];
            W = W - wt[i-1];
        }
    }
    //^^^ klasyczna implementacja działającego subsetu w problemie plecakowym, zwraca jedno optymalne ustawienie, jednak pewnie jest o wiele szybsza niż przeszukiwanie wszystkich subsetów dla możliwych rozwiązań
    
    return vectorToList(itemy);
}

static PyMethodDef paker_methods[] = {
    { "pakuj_plecak", (PyCFunction)plecaczek, METH_VARARGS, nullptr },
    { nullptr, nullptr, 0, nullptr }
};

static PyModuleDef paker_module = {
    PyModuleDef_HEAD_INIT,
    "paker",                        // Module name to use with Python import statements
    "Solves 0-1 knapsack problems.",  // Module description
    0,
    paker_methods                   // Structure that defines the methods of the module
};

PyMODINIT_FUNC PyInit_paker() {
    return PyModule_Create(&paker_module);
}
