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
int foodFind = 0;
int foodNowFind = 0;
int foodEat = 0;

// mpi settings
int rank;
const int root = 0;
int numProces;
int cont = 0;

// Game settings
const int onlyAntsX = 1;
const int onlyAntsY = 2;
const int findPathX = 3;
const int findPathY = 4;
const int findFoodX = 5;
const int findFoodY = 6;


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
	int cont = 0;
	while (antsNumber > cont) {
		for (int i = 0; i < 200; i += 5) {
			for (int j = 0; j < 200; j += 5) {
				int randomN = rand() % 5;
				if (randomN == 1) {
					ants[i][j] = 1;
					cont++;
				}
			}
		}
	}
	antsNumber += 10;

}

void printAnts(int** ants) {
	for (int i = 0; i < 900; i++) {
		for (int j = 0; j < 900; j++) {
			if (ants[i][j] == 1) // only ants
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 128, 128));
			if (ants[i][j] == 2) // only ants
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 128, 128));
			else if (ants[i][j] == 3) // ants that have found the path
				al_draw_filled_circle(i, j, 2, al_map_rgb(0, 128, 0));
			else if (ants[i][j] == 4) // ants that have found the path
				al_draw_filled_circle(i, j, 2, al_map_rgb(0, 128, 0));
			else if (ants[i][j] == 5) // ants that have found the food
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 0, 0));
			else if (ants[i][j] == 6) // ants that have found the food
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 0, 0));
			
		}
	}
}
	
void dropFood(int* foodPosition) {
	srand(time(NULL));
	int x = rand() % 500 + 300;
	int y = rand() % 400 + 300;
	int dimension = rand() % 50 + 30;
	dimension *= 2;
	foodPosition[0] = x;
	foodPosition[1] = y;
	foodPosition[2] = dimension;
	al_draw_filled_circle(x, y * y, dimension, al_map_rgb(135, 206, 235));
}

void drawFood(int* foodPosition) {
	al_draw_filled_circle(0, 0, antsNumber, al_map_rgb(255, 248, 220));
	if (foodPosition[2] - foodFind > 1)
		al_draw_filled_circle(foodPosition[0], foodPosition[1], foodPosition[2] - foodFind, al_map_rgb(135, 206, 235));
	else
		al_draw_filled_circle(foodPosition[0], foodPosition[1], foodPosition[2], al_map_rgb(135, 206, 235));
}

int move(int** subMatrix, int x, int y, int* upperVector, int* lowerVector, int* foodPosition) {
	bool path = false;
	if (subMatrix[x][y] == onlyAntsX || subMatrix[x][y] == onlyAntsY) {
		if ((x + ((900 / numProces) * rank)) + 1 - foodPosition[0] <= 100 && (x + (900 / numProces) * rank) + 1 - foodPosition[0] > -50 || y + 1 - foodPosition[1] > -50 && y + 1 - foodPosition[1] <= 100)
			return 3;
		return 0;
	}
	
	else if (subMatrix[x][y] == 0) {
		if (x > 0) {
			if (subMatrix[x - 1][y] == onlyAntsY)	
				return onlyAntsX;
			if (subMatrix[x - 1][y] == findPathY && (x + ((900 / numProces) * rank)) + 1 < foodPosition[0])
				return findPathX;
			else if (subMatrix[x - 1][y] == findPathY && (x + ((900 / numProces) * rank)) + 1 >= foodPosition[0]) {
				if ((x + (900 / numProces) * rank) - foodPosition[0] > -30 && (x + (900 / numProces) * rank) - foodPosition[0] <= 30 && y - foodPosition[1] > -30 && y - foodPosition[1] <= 30 && foodPosition[2] - foodFind > 0) {
					foodNowFind++;
					return 6;
				}
				else
					return -2;
			}
		}
		else if (upperVector[y] == onlyAntsY || upperVector[y] == findPathY) {
			if (upperVector[y] == onlyAntsY)
				return onlyAntsX;
			if (upperVector[y] == findPathY && (x + ((900 / numProces) * rank)) + 1 < foodPosition[0])
				return findPathX;
			else if (upperVector[y] == findPathY && (x + ((900 / numProces) * rank)) + 1 >= foodPosition[0]) {
				if ((x + (900 / numProces) * rank) - foodPosition[0] > -30 && (x + (900 / numProces) * rank) - foodPosition[0] <= 30 && y - foodPosition[1] > -30 && y - foodPosition[1] <= 30 && foodPosition[2] - foodFind > 0) {
					foodNowFind++;
					return 6;
				}
				else
					return -2;
			}
		}
		if (x < 900 / numProces - 1) {
			if (subMatrix[x + 1][y] == findFoodY && (x + ((900 / numProces) * rank)) + 1 > 20)
				return findFoodY;
			else if (subMatrix[x + 1][y] == findFoodY && (x + ((900 / numProces) * rank)) + 1 <= 20)
				if ((x + (900 / numProces) * rank) <= 21 && y <= 21) {
					foodEat++;
					return onlyAntsX;
				}
				else
					return -4;

		}
		else if (lowerVector[y] == findFoodY) {
			if (lowerVector[y] == findFoodY && (x + ((900 / numProces) * rank)) + 1 > 20)
				return findFoodX;
			else if (lowerVector[y] == findFoodY && (x + ((900 / numProces) * rank)) + 1 <= 20)
				if ((x + (900 / numProces) * rank) <= 21 && y <= 21) {
					return onlyAntsX;
				}
				else
					return -4;
		}
		if (y > 0) {
			if (subMatrix[x][y - 1] == onlyAntsX)
				return onlyAntsY;
			if (subMatrix[x][y - 1] == findPathX && y + 1 < foodPosition[1] && (x + ((900 / numProces) * rank)) < foodPosition[0])
				return findPathY;
			else if (subMatrix[x][y - 1] == findPathX && y + 1 >= foodPosition[1]) {
				if ((x + (900 / numProces) * rank) - foodPosition[0] > -30 && (x + (900 / numProces) * rank) - foodPosition[0] <= 30 && y - foodPosition[1] > -30 && y - foodPosition[1] <= 30 && foodPosition[2] - foodFind > 0) {
					foodNowFind++;
					return 5;
				}
				else
					return -1;
			}
		}
		if (y < 900 - 1) {
			if (subMatrix[x][y + 1] == findFoodX && y - 1 > 20)
				return findFoodX;
			if (subMatrix[x][y + 1] == findFoodX && y - 1 <= 20)
				return findFoodY;
			else if (subMatrix[x][y + 1] == findFoodX && y + 1 <= 20)
				if ((x + (900 / numProces) * rank) <= 21 && y <= 21) {
					foodEat++;
					return onlyAntsY;
				}
				else
					return findFoodY;
		}

	}
	return 0;
}

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);
	MPI_Status status;
	
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProces);

	if (900 % numProces != 0 && rank == root) {
		std::cout << "Error: usare un divisore di 900 come numeri di processi.\n";
		MPI_Abort(MPI_COMM_WORLD, -1);
	}


	if (!al_init()) {
		std::cerr << "failed to inizialize allegro!\n";
		return -1;
	}

	int** cells = nullptr;
	int** secondMatrix = matrixAllocation(900 / numProces, 900);
	int** firstMatrix = matrixAllocation(900 / numProces, 900);
	if (rank == root) {
		al_init();
		al_install_keyboard();
		al_init_primitives_addon();
		display = al_create_display(width + 100, height + 100);
		cells = matrixAllocation(900, 900);
		antsAllocator(cells);
	}

	int* foodPosition = new int[3];

	MPI_Bcast(foodPosition, 3, MPI_INT, root, MPI_COMM_WORLD);

	int** findFood = matrixAllocation(900 / numProces, 900);
	MPI_Barrier(MPI_COMM_WORLD);
	int* upperVector = (int*)malloc(sizeof(int) * 900);
	int* lowerVector = (int*)malloc(sizeof(int) * 900);
	
	if (rank == root)
		MPI_Scatter(&cells[0][0], 900 * 900 / numProces, MPI_INT, &firstMatrix[0][0], 900 * 900 / numProces, MPI_INT, root, MPI_COMM_WORLD);
	else
		MPI_Scatter(NULL, 0, NULL, &firstMatrix[0][0], 900 * 900 / numProces, MPI_INT, root, MPI_COMM_WORLD);
	
	
	while (true) {
		MPI_Request r;
		if (rank != root) {
			MPI_Send(&foodNowFind, 1, MPI_INT, root, 1, MPI_COMM_WORLD);
			foodNowFind = 0;
		}
		else {
			int tmp = 0;
			foodNowFind = 0;
			for (int i = 1; i < numProces - 1; i++) {
				MPI_Recv(&tmp, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
				foodFind += tmp;
			}
		}
		MPI_Bcast(&foodFind, 1, MPI_INT, root, MPI_COMM_WORLD);

		if (rank == root && foodEat > 50) {
			if (!chooseMatrix)
				addAnts(firstMatrix);
			else
				addAnts(secondMatrix);
			foodEat = 0;
		}
		if (foodPosition[2] - foodFind <= 0) {
			foodFind = 0;
			for (int i = 0; i < 900 / numProces; i++) {
				for (int j = 0; j < 900; j++) {
					if (firstMatrix[i][j] == findPathX || firstMatrix[i][j] == findPathY || secondMatrix[i][j] == findPathX || secondMatrix[i][j] == findPathY) {
						firstMatrix[i][j] = onlyAntsX;
						secondMatrix[i][j] = onlyAntsX;
					}
				}
			}
			if (rank == 0) {
				dropFood(foodPosition);
			}
			MPI_Bcast(foodPosition, 3, MPI_INT, root, MPI_COMM_WORLD);
			
		}
		
		
		if (!chooseMatrix) {
			if (rank == root)
				al_clear_to_color(al_map_rgb(101, 67, 33));

			if (rank != root) //send the lowerVector to the previous process
				MPI_Isend(&firstMatrix[0][0], 900, MPI_INT, rank - 1, 11, MPI_COMM_WORLD, &r);

			if (rank != numProces - 1) //recieve the lowerVector, or fill it with 0
				MPI_Recv(&lowerVector[0], 900, MPI_INT, rank + 1, 11, MPI_COMM_WORLD, &status);
			else
				fillVector(lowerVector, 900, 0);
			if (rank != numProces - 1) //send the upperVector to the next process
				MPI_Isend(&firstMatrix[900 / numProces - 1][0], 900, MPI_INT, rank + 1, 44, MPI_COMM_WORLD, &r);

			if (rank != root) //recieve the upperVector, or fill it with 0
				MPI_Recv(&upperVector[0], 900, MPI_INT, rank - 1, 44, MPI_COMM_WORLD, &status);
			else
				fillVector(upperVector, 900, 0);


			// compute the new submatrix for each processor
			for (int i = 0; i < 900 / numProces; i++)	
				for (int j = 0; j < 900; j++) {
					secondMatrix[i][j] = move(firstMatrix, i, j, upperVector, lowerVector, foodPosition);
					if (secondMatrix[i][j] == 3) {
						firstMatrix[i][j] = 0;
					}
					if (secondMatrix[i][j] == 4) {
						firstMatrix[i][j] = 0;
					}
					if (secondMatrix[i][j] == -1) {
						secondMatrix[i][j - 1] = 4;
						secondMatrix[i][j] = 0;
					}
					if (secondMatrix[i][j] == -2) {
						secondMatrix[i - 1][j] = 3;
						secondMatrix[i][j] = 0;
					}
					if (firstMatrix[i][j] == -3) {
						firstMatrix[i + 1][j] = 6;
						firstMatrix[i][j] = 0;
					}
					if (firstMatrix[i][j] == -4) {
						firstMatrix[i][j + 1] = 5;
						firstMatrix[i][j] = 0;
					}
				}
			
			MPI_Barrier(MPI_COMM_WORLD);
			// the new matrix is stored into the root, overwriting the old one
			if (rank == root)
				MPI_Gather(&secondMatrix[0][0], 900 / numProces * 900 , MPI_INT, &cells[0][0], 900 * 900 / numProces, MPI_INT, root, MPI_COMM_WORLD);
			else
				MPI_Gather(&secondMatrix[0][0], 900 / numProces * 900 , MPI_INT, NULL, 0, MPI_INT, root, MPI_COMM_WORLD);
		} 
		else {
			if (rank == 0) {
				al_clear_to_color(al_map_rgb(101, 67, 33));
			}

			if (rank != root) // send the lowerVector to the previous process
				MPI_Isend(&secondMatrix[0][0], 900, MPI_INT, rank - 1, 22, MPI_COMM_WORLD, &r);

			if (rank != numProces - 1) // recieve the lowerVector, or fill it with 0
				MPI_Recv(&lowerVector[0], 900, MPI_INT, rank + 1, 22, MPI_COMM_WORLD, &status);
			else
				fillVector(lowerVector, 900, 0);

			if (rank != numProces - 1) // send the upperVector to the next process
				MPI_Isend(&secondMatrix[900 / numProces - 1][0], 900, MPI_INT, rank + 1, 33, MPI_COMM_WORLD, &r);

			if (rank != root) // recieve the upperVector, or fill it with 0
				MPI_Recv(&upperVector[0], 900, MPI_INT, rank - 1, 33, MPI_COMM_WORLD, &status);
			else
				fillVector(upperVector, 900, 0);


			// compute the new submatrix for each processor
			for (int i = 0; i < 900 / numProces; i++)
				for (int j = 0; j < 900; j++) {
					firstMatrix[i][j] = move(secondMatrix, i, j, upperVector, lowerVector, foodPosition);
					if (firstMatrix[i][j] == 3) {
						secondMatrix[i][j] = 0;
					}
					if (firstMatrix[i][j] == 4) {
						secondMatrix[i][j] = 0;
					}
					if (firstMatrix[i][j] == -1) {
						firstMatrix[i][j - 1] = 4;
						firstMatrix[i][j] = 0;
					}
					if (firstMatrix[i][j] == -2) {
						firstMatrix[i - 1][j] = 3;
						firstMatrix[i][j] = 0;
					}
					if (firstMatrix[i][j] == -3) {
						firstMatrix[i + 1][j] = 6;
						firstMatrix[i][j] = 0;
					}
					if (firstMatrix[i][j] == -4) {
						firstMatrix[i][j + 1] = 5;
						firstMatrix[i][j] = 0;
					}

				}
			
			MPI_Barrier(MPI_COMM_WORLD);
			// the new matrix is stored into the root, overwriting the old one
			if (rank == root)
				MPI_Gather(&firstMatrix[0][0], 900 / numProces * 900, MPI_INT, &cells[0][0], 900 / numProces * 900 , MPI_INT, root, MPI_COMM_WORLD);
			else
				MPI_Gather(&firstMatrix[0][0], 900 / numProces * 900 , MPI_INT, NULL, 0, MPI_INT, root, MPI_COMM_WORLD);
			
		}
		
		chooseMatrix = !chooseMatrix;
		if (rank == 0) {
			drawFood(foodPosition);
			printAnts(cells);
			al_flip_display();
			
		}
	}

	
	MPI_Finalize();
	return 0;
}