#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

const int w = 800;
const int h = 600;
const int V = 25;

mutex mtx; //Synchronisation of output

struct Point {
    int x, y;
};

enum HouseType { CAPITALLY, WOODEN };

struct House {
    int id;
    Point position;
    Point target;
    bool isMoving;
    HouseType type;

    House(int id, Point pos, Point tgt, HouseType type) : id(id), position(pos), target(tgt), isMoving(true), type(type) {}

    void move() {
        if (!isMoving) return;

        int dx = target.x - position.x;
        int dy = target.y - position.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= V) {
            position = target;
            isMoving = false;
        }
        else {
            position.x += V * dx / distance;
            position.y += V * dy / distance;
        }
    }
};

Point generateRandomPoint(int x_min, int x_max, int y_min, int y_max) {
    return { x_min + rand() % (x_max - x_min + 1), y_min + rand() % (y_max - y_min + 1) };
}

void simulateHouse(House& house) {
    while (house.isMoving) {
        house.move();

        //Temp info
        lock_guard<mutex> guard(mtx);
        cout << (house.type == CAPITALLY ? "Capitally" : "Wooden") << " house " << house.id
            << " at (" << house.position.x << ", " << house.position.y << "), target: ("
            << house.target.x << ", " << house.target.y << "), isMoving: " << house.isMoving << "\n";

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    //Finish of moving
    lock_guard<mutex> guard(mtx);
    cout << (house.type == CAPITALLY ? "Capitally" : "Wooden") << " house " << house.id << " has stopped moving.\n";
}

int main() {
    srand(time(0));

    vector<House> houses;

    //Capitally
    for (int i = 0; i < 2; ++i) {
        Point start = generateRandomPoint(0, w / 2, 0, h / 2);
        Point target = generateRandomPoint(0, w / 2, 0, h / 2);
        houses.emplace_back(i, start, target, CAPITALLY);
    }

    //Wooden
    for (int i = 0; i < 2; ++i) {
        Point start = generateRandomPoint(w / 2, w, h / 2, h);
        Point target = generateRandomPoint(w / 2, w, h / 2, h);
        houses.emplace_back(i + 2, start, target, WOODEN);
    }

    vector<thread> threads;
    for (auto& house : houses) {
        threads.emplace_back(simulateHouse, ref(house));
    }

    //Ending of working of threads
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
