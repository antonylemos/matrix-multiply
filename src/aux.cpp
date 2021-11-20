#include <iostream>
#include <fstream>

using namespace std;

void generateMatrix(ofstream& file, int rows, int columns) {
  file << rows << " " << columns << endl;

  for (int row = 0; row < rows; row++) {
    for (int column = 0; column < columns; column++) {
      bool isSecondToLastColumn = column < (columns - 1);

      file << (rand() % 10 + 1) << (isSecondToLastColumn ? ' ' : '\n');
    }
  }

  file.close();
}

int main(int argc, char const *argv[]) {
  int n1, m1, n2, m2;
  n1 = atoi(argv[1]);
  m1 = atoi(argv[2]);
  n2 = atoi(argv[3]);
  m2 = atoi(argv[4]);

  srand((int) time(0));

  ofstream file("out/matriz_1.txt");
  generateMatrix(file, n1, m1);

  file.open("out/matriz_2.txt");
  generateMatrix(file, n2, m2);

  return 0;
}
