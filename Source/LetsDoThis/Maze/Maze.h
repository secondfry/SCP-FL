#pragma once

#include "Room.h"
#include <iostream>
#include <random>
#include <string>

class Maze {
private:
  Room* start;
  static std::mt19937 generator;
   
public:
  Maze() {
    this->start = new Room(1);
  }

  static int rollNextRoom(int seed = 1) {
    std::uniform_int_distribution<int> uniform_int_distribution(0, 20);
    generator.seed(seed);
    return uniform_int_distribution(generator);
  }

  static int rollRoomExit(int exits = 4, int seed = 1) {
    std::uniform_int_distribution<int> uniform_int_distribution(0, exits - 1);
    generator.seed(seed);
    return uniform_int_distribution(generator);
  }
};
