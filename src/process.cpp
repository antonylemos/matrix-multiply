#include <iostream>
#include <vector>
#include <fstream>
#include <sys/wait.h>
#include <unistd.h>
#include <cmath>
#include <chrono>
#include <sstream>

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

void getRow(const size_t targetRow, const size_t columns, vector<int>* row, const vector<vector<int>>* matrix) {
  for (size_t column = 0; column < columns; column++) {
    int targetValue = (*matrix)[targetRow][column];
    row->push_back(targetValue);
  }
}

void getColumn(const size_t targetColumn, const size_t rows, vector<int>* column, const vector<vector<int>>* matrix) {
  for (size_t row = 0; row < rows; row++) {
    int targetValue = (*matrix)[row][targetColumn];
    column->push_back(targetValue);
  }
}

int main(int argc, char const *argv[]) {
  fstream file;

  int elementsToDivide = atoi(argv[3]);

  size_t firstMatrixRows, firstMatrixColumns, secondMatrixRows, secondMatrixColumns;

  file.open(argv[1]);
  file >> firstMatrixRows;
  file >> firstMatrixColumns;
  vector<vector<int>> firstMatrix(firstMatrixRows + 1);
  readMatrixFile(file, firstMatrix, firstMatrixRows, firstMatrixColumns);

  file.open(argv[2]);
  file >> secondMatrixRows;
  file >> secondMatrixColumns;
  vector<vector<int>> secondMatrix(secondMatrixRows + 1);
  readMatrixFile(file, secondMatrix, secondMatrixRows, secondMatrixColumns);

  size_t childrensQuantity = ceil(float(firstMatrixRows * secondMatrixColumns) / float(elementsToDivide));

  pid_t process[childrensQuantity];

  for (size_t currentChildren = 0; currentChildren < childrensQuantity; currentChildren++) {
    process[currentChildren] = fork();

    if (process[currentChildren] == 0) {
      stringstream filename;
      fstream file;

      filename << "out/matriz_process_" << currentChildren << ".txt";

      file.open(filename.str(), fstream::out);

      if (file) {
        vector<int> row, column;
        vector<string> resultMatrix;
        int currentProcess = 0;

        auto begin = chrono::high_resolution_clock::now();

        size_t initialRow = int(elementsToDivide * currentChildren / secondMatrixColumns);
        size_t initialColumn = (elementsToDivide * currentChildren) % secondMatrixColumns;

        while (currentProcess < elementsToDivide) {
          stringstream resultMatrixRow;
          int resultMatrixElementValue = 0;

          getRow(initialRow, firstMatrixColumns, &row, &firstMatrix);
          getColumn(initialColumn, secondMatrixRows, &column, &secondMatrix);

          for (size_t target = 0; target < firstMatrixColumns; target++) {
            resultMatrixElementValue += row[target] * column[target];
          }

          resultMatrixRow << "c" << initialRow + 1 << initialColumn + 1 << " " << resultMatrixElementValue << endl;
          resultMatrix.push_back(resultMatrixRow.str());

          row.clear();
          column.clear();

          initialColumn++;

          if (initialColumn >= secondMatrixColumns) {
            initialRow++;

            initialColumn = int(elementsToDivide * currentChildren / secondMatrixColumns);

            if (initialRow >= firstMatrixRows) {
              break;
            }
          }

          currentProcess++;
        }

        auto end = chrono::high_resolution_clock::now();
        auto totalTime = chrono::duration_cast<chrono::milliseconds>(end - begin).count();

        file << firstMatrixRows << " " << secondMatrixColumns << endl;

        for (size_t target = 0; target < resultMatrix.size(); target++) {
          file << resultMatrix[target];
        }

        file << totalTime << endl;

        exit(0);
      }
    }

    for (size_t currentProcess = 0; currentProcess < childrensQuantity; currentProcess++) {
      wait(NULL);
    }
  }

  return 0;
}
