#include <iostream>
#include <chrono>
#include <thread>
#include "FastDelegate.h"

using namespace fastdelegate;
using namespace std;


class Spaceship {
public:
	int x, y, vi, vj;
	Spaceship(int x, int y);
	void OnCollision();
};

Spaceship::Spaceship(int x, int y) {
	this->x = x;
	this->y = y;
	this->vi = 1;
	this->vj = 0;
}

void Spaceship::OnCollision() {
	this->vi *= -1;
	this->vj *= -1;
}


class Asteroid {
public:
	int x, y, radius;
	Asteroid(int x, int y, int radius);
};

Asteroid::Asteroid(int x, int y, int radius) {
	this->x = x;
	this->y = y;
	this->radius = radius;
}


class Buffer {
public:
	int width, height;
	char* screen;
	Buffer(int width, int height, Spaceship* spaceship, Asteroid* asteroid);
	void Draw();
	void Update();
private:
	FastDelegate0<> collisionDelegate;
	Spaceship* spaceship;
	Asteroid* asteroid;
};

Buffer::Buffer(int width, int height, Spaceship* spaceship, Asteroid* asteroid) {
	this->spaceship = spaceship;
	this->asteroid = asteroid;
	this->width = width;
	this->height = height;
	this->screen = (char*)malloc((width * height * sizeof(char)) + 1);
	this->screen[(width * height * sizeof(char))] = '\0';
	//collisionDelegate = MakeDelegate(spaceship, &Spaceship::OnCollision);
	collisionDelegate = MakeDelegate(this->spaceship, &Spaceship::OnCollision);
}

void Buffer::Draw() {
	printf("%s", this->screen);
}

void Buffer::Update() {
	memset(this->screen, ' ', this->height * this->width);

	for (int y = 0; y < this->height; y++) {

		if (y == 0 || y == (this->height-1)) {
			
			for (int x = 0; x < this->width; x++) {
				this->screen[(y * this->width) + x] = '#';
			}

		} else {

			//for (int x = 1; x < this->width; x++) {
			//	this->screen[(y * this->width) + x] = ' ';
			//}

			this->screen[(y * this->width)] = '#';
			this->screen[(y * this->width) + this->width-1] = '#';
		}
	}

	for (int x = (asteroid->x - asteroid->radius); x < (asteroid->x + asteroid->radius); x++) {
		for (int y = (asteroid->y - asteroid->radius); y < (asteroid->y + asteroid->radius); y++) {
			if (((x - asteroid->x)*(x - asteroid->x) + (y - asteroid->y)*(y - asteroid->y)) < (asteroid->radius*asteroid->radius)) {
				this->screen[(y * this->width) + x] = '#';
			}
		}
	}

	if ((((spaceship->x + spaceship->vi) - asteroid->x)*((spaceship->x + spaceship->vi) - asteroid->x) +
		 ((spaceship->y + spaceship->vj) - asteroid->y)*((spaceship->y + spaceship->vj) - asteroid->y)) <
		 (asteroid->radius*asteroid->radius)) {
		
		// raise collision event
		collisionDelegate();

	} else {

		spaceship->x += spaceship->vi;
		spaceship->y += spaceship->vj;
	}

	this->screen[(spaceship->y * this->width) + spaceship->x] = '@';
}

int main() {
	
	Spaceship spaceship(2, 10);
	Asteroid asteroid(60, 10, 5);
	Buffer buffer(80, 24, &spaceship, &asteroid);

	for (int tick = 0; tick < 100; tick++) {
		//cout << "Tick " << tick << endl;
		buffer.Update();
		buffer.Draw();
		this_thread::sleep_for(chrono::milliseconds(100));
	}

	cout << "Press any key to exit..." << endl;
	cin.get();

	return 0;
}
