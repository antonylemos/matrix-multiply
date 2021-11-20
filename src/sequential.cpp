#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

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

void multiplyMatrices(
  vector<vector<int>> firstMatrix,
  int firstMatrixRows,
  int firstMatrixColumns,
  vector<vector<int>> secondMatrix,
  int secondMatrixRows,
  int secondMatrixColumns,
  vector<vector<int>>& resultMatrix,
  int resultMatrixRows,
  int resultMatrixColumns
) {
  for (int row = 0; row < resultMatrixRows; row++) {
    resultMatrix.push_back(vector<int>());
    for (int column = 0; column < resultMatrixColumns; column++) {
      resultMatrix[row].push_back(0);

      for (int count = 0; count < firstMatrixColumns; count++) {
        resultMatrix[row][column] += firstMatrix[row][count] * secondMatrix[count][column];
      }
    }
  }
}

void storeMatrixFile(fstream& file, vector<vector<int>> matrix, int rows, int columns, int totalTime) {
  if (file) {
    file << rows << " " << columns << endl;
  
    for (int row = 0; row < rows; row++) {
      for (int column = 0; column < columns; column++) {
        file << 'c' << row + 1 << column + 1 << ' ' << matrix[row][column] << endl;
      }
    }

    file << totalTime;

    file.close();
  } else {
    cerr << "Houve um problema ao abrir o arquivo!\n" << endl;

    exit(1);
  }
}

int main(int argc, char const *argv[]) {
  fstream file;

  vector<vector<int>> firstMatrix, secondMatrix, resultMatrix;

  int firstMatrixRows, firstMatrixColumns, secondMatrixRows, secondMatrixColumns, resultMatrixRows, resultMatrixColumns;

  file.open(argv[1]);
  file >> firstMatrixRows;
  file >> firstMatrixColumns;
  readMatrixFile(file, firstMatrix, firstMatrixRows, firstMatrixColumns);

  file.open(argv[2]);
  file >> secondMatrixRows;
  file >> secondMatrixColumns;
  readMatrixFile(file, secondMatrix, secondMatrixRows, secondMatrixColumns);

  resultMatrixRows = firstMatrixRows;
  resultMatrixColumns = secondMatrixColumns;

  auto begin = chrono::high_resolution_clock::now();

  multiplyMatrices(
    firstMatrix,
    firstMatrixRows,
    firstMatrixColumns,
    secondMatrix,
    secondMatrixRows,
    secondMatrixColumns,
    resultMatrix,
    resultMatrixRows,
    resultMatrixColumns
  );

  auto end = chrono::high_resolution_clock::now();
  auto totalTime = chrono::duration_cast<chrono::milliseconds>(end - begin).count();

  cout << "Tempo de execução: " << totalTime << "ms" << endl;

  file.open("out/matriz_sequencial.txt", fstream::out);
  storeMatrixFile(file, resultMatrix, resultMatrixRows, resultMatrixColumns, totalTime);

  return 0;
}
