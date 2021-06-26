#include <iostream>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

// Allegro settings
ALLEGRO_DISPLAY* display = nullptr;
int width = 900;
int height = 900;
int antsNumber = 10;
bool chooseMatrix = false;
int foodFind = 0;
int foodEat = 0;


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
	int x = rand() % 700 + 300;
	int y = rand() % 700 + 300;
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
		if (x + 1 - foodPosition[0] <= 100 && x + 1 - foodPosition[0] > -50 || y + 1 - foodPosition[1] > -50 && y + 1 - foodPosition[1] <= 100)
			return 3;
		return 0;
	}

	else if (subMatrix[x][y] == 0) {
		if (x > 0) {
			if (subMatrix[x - 1][y] == onlyAntsY)
				return onlyAntsX;
			if (subMatrix[x - 1][y] == findPathY && x + 1 < foodPosition[0])
				return findPathX;
			else if (subMatrix[x - 1][y] == findPathY && x + 1 >= foodPosition[0]) {
				if (x - foodPosition[0] > -30 && x - foodPosition[0] <= 30 && y - foodPosition[1] > -30 && y - foodPosition[1] <= 30 && foodPosition[2] - foodFind > 0) {
					foodFind++;
					return 6;
				}
				else
					return -2;
			}
		}
		
		if (x < 900 - 1) {
			if (subMatrix[x + 1][y] == findFoodY && x + 1 > 20)
				return findFoodY;
			else if (subMatrix[x + 1][y] == findFoodY && x + 1 <= 20)
				if (x + 900 <= 21 && y <= 21) {
					foodEat++;
					return onlyAntsX;
				}
				else
					return -4;

		}
		
		if (y > 0) {
			if (subMatrix[x][y - 1] == onlyAntsX)
				return onlyAntsY;
			if (subMatrix[x][y - 1] == findPathX && y + 1 < foodPosition[1] && x < foodPosition[0])
				return findPathY;
			else if (subMatrix[x][y - 1] == findPathX && y + 1 >= foodPosition[1]) {
				if (x - foodPosition[0] > -30 && x - foodPosition[0] <= 30 && y - foodPosition[1] > -30 && y - foodPosition[1] <= 30 && foodPosition[2] - foodFind > 0) {
					foodFind++;
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
				if (x <= 21 && y <= 21) {
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
	int** cells = nullptr;
	int** secondMatrix = matrixAllocation(900, 900);
	int** firstMatrix = matrixAllocation(900, 900);
	al_init();
	al_install_keyboard();
	al_init_primitives_addon();
	display = al_create_display(width + 100, height + 100);
	cells = matrixAllocation(900, 900);
	antsAllocator(cells);
		if (!al_init()) {
			std::cerr << "failed to inizialize allegro!\n";
			return -1;
		}

	
	
	
	

	int* foodPosition = new int[3];


	int** findFood = matrixAllocation(900, 900);
	int* upperVector = (int*)malloc(sizeof(int) * 900);
	int* lowerVector = (int*)malloc(sizeof(int) * 900);


	while (true) {

		if (foodEat > 50) {
			if (!chooseMatrix)
				addAnts(firstMatrix);
			else
				addAnts(secondMatrix);
			foodEat = 0;
		}
		if (foodPosition[2] - foodFind <= 0) {
			foodFind = 0;
			for (int i = 0; i < 900; i++) {
				for (int j = 0; j < 900; j++) {
					if (firstMatrix[i][j] == findPathX || firstMatrix[i][j] == findPathY || secondMatrix[i][j] == findPathX || secondMatrix[i][j] == findPathY) {
						firstMatrix[i][j] = onlyAntsX;
						secondMatrix[i][j] = onlyAntsX;
					}
				}
			}
		

		}

		if (!chooseMatrix) {
			for (int i = 0; i < 900; i++) {
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
			}
		}

		else {
			al_clear_to_color(al_map_rgb(101, 67, 33));
			for (int i = 0; i < 900; i++)
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
		}

		chooseMatrix = !chooseMatrix;
		drawFood(foodPosition);
		printAnts(cells);
		al_flip_display();

		
	}


	
	return 0;
}