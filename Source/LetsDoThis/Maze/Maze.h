#pragma once

#include "Array.h"
#include "Room.h"
#include "RoomDataStruct.h"
#include <map>
#include <random>

class Maze {
private:
  static std::mt19937 generator;
  static std::map<int, RoomData*> roomDataVariants;
  static std::map<int, std::map<int, Room*>> grid;
  static std::map<int, std::map<int, int>> heightMap;
  static std::map<int, std::map<int, int>> stateMap;
  static std::map<int, std::map<int, int>> costMap;
  static std::map<int, std::map<int, std::pair<int, int>>> Maze::jumpMap;
  static Room* start;

public:
  static TArray<FRoomDataStruct> generateMap();
  static void initRoomLocations();
  static void clearGrid();
  static void placeStart();
  static void placeAllKeyRooms();
  static Room* placeNextKeyRoom(Room* prevRoom, FName name);
  static RoomData* getNextRoomData();
  static int rollNextRoom(int seed = 1);
  static int rollRoomExit(int exits = 4, int seed = 1);
  static int roll(int min = 0, int max = 99, int seed = 1);
  static bool isNextRoomNotInBounds(int nextRoomLinearID);
  static bool isNextRoomPlaceOccupied(int x, int y);
  static void createRoute(Room* prevRoom, Room* nextRoom);
  static void Maze::AddDirections(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minX, int maxX, int minY, int maxY);
  static void AddDirectionsVertical(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minY, int maxY);
  static void AddDirectionsHorizontal(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minX, int maxX);
};

enum SearchState {
  checked,
  used,
  clean
};
