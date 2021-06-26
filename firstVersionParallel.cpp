#include <iostream>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "mpi.h"

// Allegro settings
ALLEGRO_DISPLAY* display = nullptr;
int width = 900;
int height = 900;
int antsNumber = 10;
bool chooseMatrix = false;
int foodEat = 0;

// mpi settings
int rank;
const int root = 0;
int numProc;
int cont = 0;

// Game settings
const int onlyAnts = 1;
const int findPath = 2;
const int findFood = 3;



void fillVector(int* vector, int dimension, int value) {
	for (int i = 0; i < dimension; i++)
		vector[i] = value;
}

int** matrixAllocation(int N, int M) {
	int* tmp = (int*)malloc(sizeof(int) * N * M);
	int** mat = (int**)malloc(sizeof(int*) * N);
	for (int i = 0; i < N; i++) {
		mat[i] = &(tmp[M * i]);
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			mat[i][j] = 0;
		}
	}
	return mat;
}


void antsAllocator(int** matrix) {
	for (int i = 0; i < 100; i += 5) {
		for (int j = 0; j < 100; j += 5) {
			int randomN = rand() % 5;
			if (randomN == 1)
				matrix[i][j] = 1;
		}
	}
}

void addAnts(int** ants) {
	for (int i = 0; i < 200; i += 5) {
		for (int j = 0; j < 200; j += 5) {
			int randomN = rand() % 5;
			if (randomN == 1) {
				ants[i][j] = 1;
			}
		}
	}

}

void printAnts(int** ants) {
	for (int i = 0; i < 900; i++) {
		for (int j = 0; j < 900; j++) {
			if (ants[i][j] == 1) // only ants
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 128, 128));
			else if (ants[i][j] == 2) // ants that have found the path
				al_draw_filled_circle(i, j, 2, al_map_rgb(0, 128, 0));
			else if (ants[i][j] == 3) // ants that have found the food
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 0, 0));
		}
	}
}

void dropFood(int* foodPosition) {
	srand(time(NULL));
	int x = rand() % 400 + 200;
	int y = rand() % 400 + 200;
	int dimension = rand() % 50 + 30;
	dimension *= 2;
	foodPosition[0] = x;
	foodPosition[1] = y;
	foodPosition[2] = dimension;
	//al_draw_filled_circle(x, y * y, 5, al_map_rgb(135, 206, 235));
}

void drawFood(int* foodPosition) {
	al_draw_filled_circle(0, 0, antsNumber, al_map_rgb(255, 248, 220));

	al_draw_filled_circle(foodPosition[0], foodPosition[1], foodPosition[2], al_map_rgb(135, 206, 235));
}




int** move(int** ants, int *foodPosition, int *upperVector, int *lowerVector) {
	bool moved = false;
	int cont = 0;
	int** secondMatrix = matrixAllocation(900 / numProc, 900);
	for (int i = 0; i < 900 / numProc; i++) {
		for (int j = 0; j < 900; j++) {
			if (ants[i][j] == onlyAnts) {
				if ((i + (900 / numProc) * rank) < 900 && j < 900 && i % 2 == 0) {
					secondMatrix[i + 1][j + 1] = onlyAnts;
					if ((i + ((900 / numProc) * rank)) + 1 - foodPosition[0] <= 100 && (i + (900 / numProc) * rank) + 1 - foodPosition[0] > -50 || j + 1 - foodPosition[1] > -50 && j + 1 - foodPosition[1] <= 100) {
						secondMatrix[i + 1][j + 1] = findPath;
					}
				}
				else {
					while (!moved && cont < 4) {
						int move = rand() % 2 + 1;
						switch (move) {
						case 1:
							if ((i + 1 < 900 / numProc) || (i + 1 == 900 / numProc)) {
								if (i + 1 < 900 / numProc && secondMatrix[i + 1][j] == 0)
									secondMatrix[i + 1][j] = onlyAnts;
								moved = true;
								if ((i + (900 / numProc) * rank) + 1 - foodPosition[0] <= 100 && (i + (900 / numProc) * rank) + 1 - foodPosition[0] > -50 || j - foodPosition[1] > -50 && j - foodPosition[1] <= 100) {
									secondMatrix[i + 1][j] = findPath;
								}
							}
							break;
						case 2:
							if (j < 900 && secondMatrix[i][j + 1] == 0) {
								secondMatrix[i][j + 1] = onlyAnts;
								moved = true;
								if ((i + (900 / numProc) * rank) - foodPosition[0] <= 100 && (i + (900 / numProc) * rank) - foodPosition[0] > -50 || j + 1 - foodPosition[1] > -50 && j + 1 - foodPosition[1] <= 100) {
									secondMatrix[i][j + 1] = findPath;
								}
							}
							break;
						}
						cont++;
						if (cont == 4 && !moved) {
							secondMatrix[i][j] = onlyAnts;
							if ((i + (900 / numProc) * rank) - foodPosition[0] <= 100 && (i + (900 / numProc) * rank) - foodPosition[0] > -50 || j - foodPosition[1] > -50 && j - foodPosition[1] <= 100) {
								secondMatrix[i][j] = findFood;
								foodPosition[2]--;
							}
						}
					}
				}
				moved = false;
				cont = 0;
			}
			else if (ants[i][j] == findPath) {
				if ((i + (900 / numProc) * rank) < foodPosition[0]) {
					if (i + 1 < 900 / numProc) {
						secondMatrix[i + 1][j] = findPath;
						secondMatrix[i][j] = 0;
					}
					if ((i + (900 / numProc) * rank) - foodPosition[0] > -30 && (i + (900 / numProc) * rank) - foodPosition[0] <= 30 && j - foodPosition[1] > -30 && j - foodPosition[1] <= 30) {
						if (i + 1 < 900 / numProc) {
							secondMatrix[i + 1][j] = findFood;
							secondMatrix[i + 1][j] = 0;
						}
						if (foodPosition[2] > 0)
							foodPosition[2]--;
					}
				}
				else if (j < foodPosition[1]) {
					secondMatrix[i][j + 1] = findPath;
					secondMatrix[i][j] = 0;
					if ((i + (900 / numProc) * rank) - foodPosition[0] > -30 && (i + (900 / numProc) * rank) - foodPosition[0] <= 30 && j - foodPosition[1] > -30 && j - foodPosition[1] <= 30) {
						secondMatrix[i][j + 1] = findFood;
						secondMatrix[i][j] = 0;
						if (foodPosition[2] > 0)
							foodPosition[2]--;
					}
				}
				else if (j > foodPosition[1]) {
					secondMatrix[i][j - 1] = findPath;
					secondMatrix[i][j] = 0;
					if ((i + (900 / numProc) * rank) - foodPosition[0] > -30 && (i + (900 / numProc) * rank) - foodPosition[0] <= 30 && j - foodPosition[1] > -30 && j - foodPosition[1] <= 30) {
						secondMatrix[i][j - 1] = findFood;
						secondMatrix[i][j] = 0;
						if (foodPosition[2] > 0)
							foodPosition[2]--;
					}
				}
			}
			else if (ants[i][j] == findFood) {
				if (rank == root && i - 1 >= 20 && j >= 20 || rank != root && i - 1 >= 0) {
					if (rank == root && i - 1 >= 20 && j >= 20) {
						secondMatrix[i - 1][j] = findFood;
						secondMatrix[i][j] = 0;
					}
					if (rank != root && i - 1 >= 0) {
						secondMatrix[i - 1][j] = findFood;
						secondMatrix[i][j] = 0;
					}
					if (rank == root && i - 1 <= 20 && j <= 20) {
						secondMatrix[i - 1][j] = onlyAnts;
						secondMatrix[i][j] = 0;
						foodEat++;
					}
				}
				else if ((j > 0 && rank != root) || rank == root && j >= 20) {
					if (rank == root && j - 1 > 10) {
						secondMatrix[i][j - 1] = findFood;
						secondMatrix[i][j] = 0;
					}
					else if (j - 1 > 0) {
						secondMatrix[i][j - 1] = findFood;
						secondMatrix[i][j] = 0;
					}
					if (rank == root && i <= 20 && j - 1 <= 20) {
						secondMatrix[i][j - 1] = onlyAnts;
						secondMatrix[i][j] = 0;
						foodEat++;
					}
				}
			}
		}
	}
	return secondMatrix;
}




int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);

	if (900 % numProc != 0 && rank == root) {
		std::cout << "Error: usare un divisore di 900 come numeri di processi.\n";
		MPI_Abort(MPI_COMM_WORLD, -1);
	}


	if (!al_init()) {
		std::cerr << "failed to inizialize allegro!\n";
		return -1;
	}
	int* foodPosition = new int[3];
	int** cells = nullptr;
	int** secondMatrix = matrixAllocation(900 / numProc, 900);
	int** firstMatrix = matrixAllocation(900 / numProc, 900);
	if (rank == root) {
		al_init();
		al_install_keyboard();
		al_init_primitives_addon();
		display = al_create_display(width + 100, height + 100);
		cells = matrixAllocation(900, 900);
		antsAllocator(cells);
		dropFood(foodPosition);
	}

	
	

	int** findPath = matrixAllocation(900 / numProc, 900);
	
	MPI_Bcast(foodPosition, 3, MPI_INT, root, MPI_COMM_WORLD);
	

	
	int* upperVector = (int*)malloc(sizeof(int) * 900);
	int* lowerVector = (int*)malloc(sizeof(int) * 900);

	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == root)
		MPI_Scatter(&cells[0][0], 900 * 900 / numProc, MPI_INT, &firstMatrix[0][0], 900 * 900 / numProc, MPI_INT, root, MPI_COMM_WORLD);
	else
		MPI_Scatter(NULL, 0, NULL, &firstMatrix[0][0], 900 * 900 / numProc, MPI_INT, root, MPI_COMM_WORLD);


	int cont = 0;
	while (true) {
		MPI_Request r;

		if (!chooseMatrix) {
			if (rank == 0) {
				al_clear_to_color(al_map_rgb(101, 67, 33));
			}

			// compute the new submatrix for each processor
			secondMatrix = move(firstMatrix, foodPosition, lowerVector, upperVector);

			if (rank != numProc - 1)
				MPI_Isend(&secondMatrix[900 / numProc -1][0], 900, MPI_INT, rank + 1, 22, MPI_COMM_WORLD, &r);

			if (rank != root)
				MPI_Recv(&upperVector[0], 900, MPI_INT, rank - 1, 22, MPI_COMM_WORLD, &status);

			if (rank != root)
				MPI_Isend(&secondMatrix[0][0], 900, MPI_INT, rank - 1, 22, MPI_COMM_WORLD, &r);

			if (rank != numProc - 1)
				MPI_Recv(&lowerVector[0], 900, MPI_INT, rank + 1, 22, MPI_COMM_WORLD, &status);


			if (rank != root) {
				for (int i = 0; i < 900; i++) {
					secondMatrix[0][i] = upperVector[i];
				}
			}

			if (rank != numProc - 1) {
				for (int i = 0; i < 900; i++) {
					secondMatrix[900 / numProc - 1][i] = lowerVector[i];
				}
			}

			MPI_Barrier(MPI_COMM_WORLD);
			// the new matrix is stored into the root, overwriting the old one
			if (rank == root)
				MPI_Gather(&secondMatrix[0][0], 900 / numProc * 900, MPI_INT, &cells[0][0], 900 * 900 / numProc, MPI_INT, root, MPI_COMM_WORLD);
			else
				MPI_Gather(&secondMatrix[0][0], 900 / numProc * 900, MPI_INT, NULL, 0, MPI_INT, root, MPI_COMM_WORLD);
		}
		else {
			if (rank == 0) {
				al_clear_to_color(al_map_rgb(101, 67, 33));
			}


			// compute the new submatrix for each processor
			firstMatrix = move(secondMatrix, foodPosition, lowerVector, upperVector);
			
			if (rank != numProc - 1)
				MPI_Isend(&firstMatrix[900 / numProc - 1][0], 900, MPI_INT, rank + 1, 22, MPI_COMM_WORLD, &r);

			if (rank != root)
				MPI_Recv(&upperVector[0], 900, MPI_INT, rank - 1, 22, MPI_COMM_WORLD, &status);

			if (rank != root)
				MPI_Isend(&firstMatrix[0][0], 900, MPI_INT, rank - 1, 22, MPI_COMM_WORLD, &r);

			if (rank != numProc - 1)
				MPI_Recv(&lowerVector[0], 900, MPI_INT, rank + 1, 22, MPI_COMM_WORLD, &status);


			if (rank != root) {
				for (int i = 0; i < 900; i++) {
					firstMatrix[0][i] = upperVector[i];
				}
			}

			if (rank != numProc - 1) {
				for (int i = 0; i < 900; i++) {
					firstMatrix[900 / numProc - 1][i] = lowerVector[i];
				}
			}

			MPI_Barrier(MPI_COMM_WORLD);
			// the new matrix is stored into the root, overwriting the old one
			if (rank == root)
				MPI_Gather(&firstMatrix[0][0], 900 / numProc * 900, MPI_INT, &cells[0][0], 900 / numProc * 900, MPI_INT, root, MPI_COMM_WORLD);
			else
				MPI_Gather(&firstMatrix[0][0], 900 / numProc * 900, MPI_INT, NULL, 0, MPI_INT, root, MPI_COMM_WORLD);

		}

		chooseMatrix = !chooseMatrix;
		if (rank == root) {
			drawFood(foodPosition);
			printAnts(cells);
			al_flip_display();

		}
		cont++;
	}


	MPI_Finalize();
	return 0;
}