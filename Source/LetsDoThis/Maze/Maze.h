#pragma once

#include "Room.h"
#include <iostream>
#include <map>
#include <random>
#include <string>

class Maze {
private:
  static std::mt19937 generator;
  static std::map<int, RoomData*> roomDataVariants;
  static std::map<int, Room*> grid;
  static Room* start;

  static const int SIDE = 15;
  static const int CENTER = SIDE / 2 + 1;
  static const int LINEAR_CENTER = CENTER * (SIDE + 1);
  static const int SIZE = SIDE * SIDE;

public:
  static TArray<FRoomDataStruct> generateMap() {
    Maze::initRoomLocations();
    Maze::clearGrid();
    Maze::placeStart();
    Maze::placeAllKeyRooms();
  }

  static void initRoomLocations() {
    if (Maze::roomDataVariants.size != nullptr) {
      return;
    }

    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(0, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite}, 0, 4, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(1, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite, DirectionVariant::left}, 1, 3, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(2, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite, DirectionVariant::left}, 2, 2, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(3, new RoomData{std::vector<DirectionVariant>{DirectionVariant::left}, 3, 1, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(4, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite}, 0, 3, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(5, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite, DirectionVariant::left}, 1, 2, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(6, new RoomData{std::vector<DirectionVariant>{DirectionVariant::left}, 2, 1, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(7, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite, DirectionVariant::right}, -1, 3, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(8, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite, DirectionVariant::left, DirectionVariant::right}, 0, 2, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(9, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same, DirectionVariant::left}, 1, 1, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(10, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same, DirectionVariant::left}, 2, 0, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(11, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite, DirectionVariant::right}, -1, 2, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(12, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same}, 1, 0, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(13, new RoomData{std::vector<DirectionVariant>{DirectionVariant::opposite, DirectionVariant::right}, -2, 2, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(14, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same, DirectionVariant::right}, -1, 1, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(15, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same}, 1, -1, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(16, new RoomData{std::vector<DirectionVariant>{DirectionVariant::right}, -2, 1, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(17, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same}, -1, 0, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(18, new RoomData{std::vector<DirectionVariant>{DirectionVariant::right}, -3, 1, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(19, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same, DirectionVariant::right}, -2, 0, 0}));
    Maze::roomDataVariants.insert(std::pair<int, RoomData*>(20, new RoomData{std::vector<DirectionVariant>{DirectionVariant::same}, -1, -1, 0}));
  }

  static void clearGrid() {
    Maze::grid.clear();
  }

  static void placeStart() {
    Maze::start = new Room();
    Maze::grid[LINEAR_CENTER] = Maze::start->AddExit(DirectionVariant::same)->SetName("SPAWN_CLASSD")->SetPlace(0, 0, 0);
  }

  static void placeAllKeyRooms() {
    auto prevRoom = Maze::start;
    for (const FName name : {"CR_914", "CR_173"}) {
      prevRoom = Maze::placeNextKeyRoom(prevRoom, name);
    }
  }

  static Room* placeNextKeyRoom(Room* prevRoom, FName name) {
    auto roomData = Maze::getNextRoomData();
    int nextRoomLinearID = (prevRoom->GetPlaceY() + roomData->y) * Maze::SIDE + prevRoom->GetPlaceX() + roomData->x;

    while (
      Maze::isNextRoomNotInBounds(nextRoomLinearID) ||
      Maze::isNextRoomPlaceOccupied(nextRoomLinearID)
    ) {
      roomData = Maze::getNextRoomData();
      nextRoomLinearID = (prevRoom->GetPlaceY() + roomData->y) * Maze::SIDE + prevRoom->GetPlaceX() + roomData->x;
    }

    auto nextRoom = new Room();
    auto const exitID = Maze::rollRoomExit(roomData->exits.size());
    nextRoom->AddExit(roomData->exits[exitID]);
    nextRoom->SetName(name);
    nextRoom->SetPlace(prevRoom->GetPlaceX() + roomData->x, prevRoom->GetPlaceY() + roomData->y, prevRoom->GetPlaceZ() + roomData->z);

    Maze::grid[nextRoomLinearID] = nextRoom;

    Maze::createRoute(prevRoom, nextRoom);

    return nextRoom;
  }

  static RoomData* getNextRoomData() {
    int nextRoomID = Maze::rollNextRoom();
    RoomData* nextRoomData = Maze::roomDataVariants[nextRoomID];
    return nextRoomData;
  }

  static int rollNextRoom(int seed = 1) {
    return roll(0, 20, seed);
  }

  static int rollRoomExit(int exits = 4, int seed = 1) {
    return roll(0, exits - 1, seed);
  }

  static int roll(int min = 0, int max = 99, int seed = 1) {
    std::uniform_int_distribution<int> uniform_int_distribution(min, max);
    generator.seed(seed);
    return uniform_int_distribution(generator);
  }

  static bool isNextRoomNotInBounds(int nextRoomLinearID) {
    return nextRoomLinearID >= Maze::SIZE;
  }

  static bool isNextRoomPlaceOccupied(int nextRoomLinearID) {
    return Maze::grid[nextRoomLinearID] != nullptr;
  }

  static void createRoute(Room* prevRoom, Room* nextRoom) {
    // FIXME implement createRoute
  }
};
