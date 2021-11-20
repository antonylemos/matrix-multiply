#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <pthread.h>

using namespace std;

typedef struct args_thread {
  size_t children;
  size_t firstMatrixRows;
  size_t firstMatrixColumns;
  size_t secondMatrixRows;
  size_t secondMatrixColumns;
  int threadsQuantity;
  vector<vector<int>> firstMatrix;
  vector<vector<int>> secondMatrix;
} thargs_t;

void readMatrixFile(fstream& file, vector<vector<int>>& matrix, int rows, int columns) {
  if (file) {
    int currentValue = 0;

    for (int row = 0; row < rows; row++) {
      matrix.push_back(vector<int>());
      for (int column = 0; column < columns; column++) {
        file >> currentValue;
        matrix[row].push_back(currentValue);
      }
    }

    file.close();
  } else {
    cerr << "Houve um problema ao abrir o arquivo!\n" << endl;

    exit(1);
  }
}

void getRow(int targetRow, int columns, vector<int>* row, vector<vector<int>>* matrix) {
  for (int column = 0; column < columns; column++) {
    int targetValue = (*matrix)[targetRow][column];
    row->push_back(targetValue);
  }
}

void getColumn(int targetColumn, int rows, vector<int>* column, vector<vector<int>>* matrix) {
  for (int row = 0; row < rows; row++) {
    int targetValue = (*matrix)[row][targetColumn];
    column->push_back(targetValue);
  }
}

void* multiplyMatricesByThread(void *args) {
  args_thread *argsThread = ((thargs_t*) args);

  stringstream filename;
  fstream file;

  filename << "out/matriz_thread_" << argsThread->children << ".txt";

  file.open(filename.str(), fstream::out);

  if (file) {
    vector<int> row, column;
    vector<string> resultMatrix;
    int currentThread = 0;

    auto begin = chrono::high_resolution_clock::now();

    size_t initialRow = int(argsThread->threadsQuantity * argsThread->children / argsThread->secondMatrixColumns);
    size_t initialColumn = (argsThread->threadsQuantity * argsThread->children) % argsThread->secondMatrixColumns;

    while (currentThread < argsThread->threadsQuantity) {
      stringstream resultMatrixRow;
      int resultMatrixElementValue = 0;

      getRow(initialRow, argsThread->firstMatrixColumns, &row, &argsThread->firstMatrix);
      getColumn(initialColumn, argsThread->secondMatrixRows, &column, &argsThread->secondMatrix);

      for (size_t target = 0; target < argsThread->firstMatrixColumns; target++) {
        resultMatrixElementValue += row[target] * column[target];
      }

      resultMatrixRow << "c" << initialRow + 1 << initialColumn + 1 << " " << resultMatrixElementValue << endl;
      resultMatrix.push_back(resultMatrixRow.str());

      row.clear();
      column.clear();

      initialColumn++;

      if (initialColumn >= argsThread->secondMatrixColumns) {
        initialRow++;

        initialColumn = int(argsThread->threadsQuantity * argsThread->children / argsThread->secondMatrixColumns);

        if (initialRow >= argsThread->firstMatrixRows) {
          break;
        }
      }

      currentThread++;
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto totalTime = chrono::duration_cast<chrono::milliseconds>(end - begin).count();

    file << argsThread->firstMatrixRows << " " << argsThread->secondMatrixColumns << endl;

    for (size_t target = 0; target < resultMatrix.size(); target++) {
      file << resultMatrix[target];
    }

    file << totalTime << endl;

    pthread_exit(NULL);
  } else {
    cerr << "Houve um problema ao abrir o arquivo!\n" << endl;

    exit(1);
  }
}

int main(int argc, char const *argv[]) {
  fstream file;

  int threadsQuantity = atoi(argv[3]);

  vector<vector<int>> firstMatrix, secondMatrix, resultMatrix;

  int firstMatrixRows, firstMatrixColumns, secondMatrixRows, secondMatrixColumns;

  file.open(argv[1]);
  file >> firstMatrixRows;
  file >> firstMatrixColumns;
  readMatrixFile(file, firstMatrix, firstMatrixRows, firstMatrixColumns);

  file.open(argv[2]);
  file >> secondMatrixRows;
  file >> secondMatrixColumns;
  readMatrixFile(file, secondMatrix, secondMatrixRows, secondMatrixColumns);

  int childrensQuantity = ceil(float(firstMatrixRows * secondMatrixColumns) / float(threadsQuantity));

  pthread_t threads[childrensQuantity];
  thargs_t argsThread[childrensQuantity];
  void *resultFromThread;

  for (int currentChildren = 0; currentChildren < childrensQuantity; currentChildren++) {
    argsThread[currentChildren].children = currentChildren;
    argsThread[currentChildren].threadsQuantity = threadsQuantity;
    argsThread[currentChildren].firstMatrix = firstMatrix;
    argsThread[currentChildren].firstMatrixRows = firstMatrixRows;
    argsThread[currentChildren].firstMatrixColumns = firstMatrixColumns;
    argsThread[currentChildren].secondMatrix = secondMatrix;
    argsThread[currentChildren].secondMatrixRows = secondMatrixRows;
    argsThread[currentChildren].secondMatrixColumns = secondMatrixColumns;

    pthread_create(&threads[currentChildren], NULL, multiplyMatricesByThread, &argsThread[currentChildren]);
  }

  for (int currentChildren = 0; currentChildren < childrensQuantity; currentChildren++) {
    pthread_join(threads[currentChildren], &resultFromThread);
  }

  return 0;
}
