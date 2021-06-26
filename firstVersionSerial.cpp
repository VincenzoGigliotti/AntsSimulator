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
int foodEat = 0;

bool** antsAllocator() {
	bool** ants = new bool* [900];
	for (int i = 0; i < 900; i++) {
		ants[i] = new bool[900];
		for (int j = 0; j < 900; j++) {
			bool a = false;
			ants[i][j] = a;
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			ants[i][j] = true;
		}
	}
	return ants;
}

void addAnts(bool** ants) {
	int cont = 0;
	for (int i = 0; i < 900; i++) {
		for (int j = 0; j < 900; j++) {
			if (!ants[i][j]) {
				ants[i][j] = true;
				cont++;
			}
			if (cont > 50)
				return;
		}
	}
}

void printAnts(bool** ants, bool** findFood, bool** findPath) {
	for (int i = 0; i < 900; i++) {
		for (int j = 0; j < 900; j++) {
			if (ants[i][j] && !findFood[i][j] && !findPath[i][j]) // formiche che non hanno trovato il cibo 
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 128, 128));
			else if (ants[i][j] && findPath[i][j] && !findFood[i][j])
				al_draw_filled_circle(i, j, 2, al_map_rgb(0, 128, 128));
			else if (ants[i][j] && !findPath[i][j] && findFood[i][j])
				al_draw_filled_circle(i, j, 2, al_map_rgb(128, 0, 0));
		}
	}
}

void dropFood(std::vector<int>& foodPosition, bool** findPath) {
	// inserisco il cibo in modo randomico e con grandezza quasi randomica.
	foodPosition.clear();
	srand(time(NULL));
	int x = rand() % 400 + 200;
	int y = rand() % 400 + 200;
	int dimension = rand() % 50 + 1;
	dimension *= 2;
	for (int i = 0; i < 900; i++) {
		for (int j = 0; j < 900; j++) {
			findPath[i][j] = false;
		}
	}
	foodPosition.push_back(x);
	foodPosition.push_back(y);
	foodPosition.push_back(dimension);
	al_draw_filled_circle(x, y * y, dimension, al_map_rgb(135, 206, 235));
}

void drawFood(std::vector<int> foodPosition) {
	al_draw_filled_circle(0, 0, antsNumber, al_map_rgb(255, 248, 220));
	al_draw_filled_circle(foodPosition[0], foodPosition[1], foodPosition[2], al_map_rgb(135, 206, 235));
	al_flip_display();
	al_rest(0.09);
}



bool** move(bool** ants, bool** &findFood, bool** &findPath, std::vector<int> &foodPosition) {
	bool moved = false;
	int cont = 0;
	bool** secondMatrix = new bool* [900];
	for (int i = 0; i < 900; i++) {
		secondMatrix[i] = new bool[900];
		for (int j = 0; j < 900; j++) {
			bool a = false;
			secondMatrix[i][j] = a;
		}
	}
	for (int i = 0; i < 900; i++) {
		for (int j = 0; j < 900; j++) {
			if (ants[i][j] && !findPath[i][j] && !findFood[i][j]) {
				if (i < 200 && j < 200 && !secondMatrix[i+3][j+3] && i %2 == 0) {
					secondMatrix[i + 3][j + 3] = true;
					ants[i][j] = false;
					if (i + 3 - foodPosition[0] <= 100 && i+3 - foodPosition[0] > -50 || j+3 - foodPosition[1] > -50 && j + 3 - foodPosition[1] <= 100) {
						findPath[i + 3][j + 3] = true;
					}
				}
				else {
					while (!moved && cont < 4) {
						int move = rand() % 2 + 1;
						switch (move) {
						case 1:
							if (i < 889 && !secondMatrix[i+3][j]) {
								secondMatrix[i + 3][j] = true;
								moved = true;
								if (i + 3 - foodPosition[0] <= 100 && i + 3 - foodPosition[0] > -50 || j - foodPosition[1] > -50 && j - foodPosition[1] <= 100) {
									findPath[i + 3][j] = true;
								}
							}
							break;
						case 2:
							if (j < 889 && !secondMatrix[i][j + 3]) {
								secondMatrix[i][j + 3] = true;
								moved = true;
								if (i - foodPosition[0] <= 100 && i - foodPosition[0] > -50 || j + 3 - foodPosition[1] > -50 && j + 3 - foodPosition[1] <= 100) {
									findPath[i][j + 3] = true;
								}
							}
							break;
						}
						cont++;
						if (cont == 4 && !moved) {
							secondMatrix[i][j] = true;
							if (i - foodPosition[0] <= 100 && i - foodPosition[0] > -50 || j - foodPosition[1] > -50 && j - foodPosition[1] <= 100) {
								findPath[i][j] = true;
							}
						}
					}
				}
				moved = false;
				cont = 0;
			}
			else if (ants[i][j] && findPath[i][j] && !findFood[i][j]) {
				if (i < foodPosition[0]) {
					secondMatrix[i + 3][j] = true;
					findPath[i + 3][j] = true;
					findPath[i][j] = false;
					if (i - foodPosition[0] > -30 && i - foodPosition[0] <= 30 && j - foodPosition[1] > -30 && j - foodPosition[1] <= 30) {
						findFood[i + 3][j] = true;
						findPath[i + 3][j] = false;
						if (foodPosition[2] > 0)
							foodPosition[2]--;
					}
				}
				else if (j < foodPosition[1]) {
					secondMatrix[i][j + 3] = true;
					findPath[i][j + 3] = true;
					findPath[i][j] = false;
					if (i - foodPosition[0] > -30 && i - foodPosition[0] <= 30 && j - foodPosition[1] > -30 && j - foodPosition[1] <= 30) {
						findFood[i][j + 3] = true;
						findPath[i][j + 3] = false;
						if (foodPosition[2] > 0)
							foodPosition[2]--;
					}
				}
				else if (j > foodPosition[1]) {
					secondMatrix[i][j - 3] = true;
					findPath[i][j - 3] = true;
					findPath[i][j] = false;
					if (i - foodPosition[0] > -30 && i - foodPosition[0] <= 30 && j - foodPosition[1] > -30 && j - foodPosition[1] <= 30) {
						findFood[i][j - 3] = true;
						findPath[i][j - 3] = false;
						if (foodPosition[2] > 0)
							foodPosition[2]--;
					}
				}
			}
			else if (ants[i][j] && findFood[i][j]) {
				if (i > 20) {
					secondMatrix[i - 3][j] = true;
					findFood[i - 3][j] = true;
					findFood[i][j] = false;
					if (i - 3 <= 20 && j <= 20) {
						findFood[i - 3][j] = false;
						foodEat++;
					}
				}
				else if (j > 20) {
					secondMatrix[i][j - 3] = true;
					findFood[i][j - 3] = true;
					findFood[i][j] = false;
					if (j - 3 <= 20 && i <= 20) {
						findFood[i][j-3] = false;
						foodEat++;
					}
				}
			}
		}
	}
	return secondMatrix;
}

int main() {
	

	bool** secondMatrix = new bool* [900];
	for (int i = 0; i < 900; i++) {
		secondMatrix[i] = new bool[900];
		for (int j = 0; j < 900; j++) {
			bool a = false;
			secondMatrix[i][j] = a;
		}
	}


	if (!al_init()) {
		std::cerr << "failed to inizialize allegro!\n";
		return -1;
	}

	al_init();
	al_install_keyboard();
	al_init_primitives_addon();
	display = al_create_display(width + 100, height + 100);

	bool** firstMatrix = antsAllocator();
	std::vector<int> foodPosition;
	bool** findPath = new bool* [900];
	for (int i = 0; i < 900; i++) {
		findPath[i] = new bool[900];
		for (int j = 0; j < 900; j++) {
			bool a = false;
			findPath[i][j] = a;
		}
	}
	dropFood(foodPosition, findPath);

	bool** findFood = new bool* [900];
	for (int i = 0; i < 900; i++) {
		findFood[i] = new bool[900];
		for (int j = 0; j < 900; j++) {
			bool a = false;
			findFood[i][j] = a;
		}
	}

	


	while (true) {
		al_clear_to_color(al_map_rgb(101, 67, 33));
		if (foodPosition[2] == 0)
			dropFood(foodPosition, findPath);
		if (foodEat %10 == 0 && foodEat > 0) {
			if (!chooseMatrix)
				addAnts(firstMatrix);
			else
				addAnts(secondMatrix);
		}
		if (!chooseMatrix) {
			secondMatrix = move(firstMatrix, findFood, findPath, foodPosition);
			printAnts(secondMatrix, findFood, findPath);
			drawFood(foodPosition);
		}
		else {
			firstMatrix = move(secondMatrix, findFood, findPath, foodPosition);
			printAnts(firstMatrix, findFood, findPath);
			drawFood(foodPosition);
		}
		if (chooseMatrix)
			chooseMatrix = false;
		else
			chooseMatrix = true;
	}
}