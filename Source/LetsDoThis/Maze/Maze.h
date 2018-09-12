#pragma once

#include "Array.h"
#include "Room.h"
#include "RoomDataStruct.h"
#include <map>
#include <random>

class Maze {
private:
  static std::mt19937 generator;
  static std::map<int, const RoomData*> roomDataVariants;
  static std::map<int, std::map<int, Room*>> grid;
  static std::map<int, std::map<int, int>> heightMap;
  static std::map<int, std::map<int, int>> stateMap;
  static std::map<int, std::map<int, int>> costMap;
  static std::map<int, std::map<int, std::pair<int, int>>> jumpMap;
  static Room* start;
  static std::vector<FName> keyRooms;

public:
  static TArray<FRoomDataStruct>* GenerateMap(FString seed = Maze::GenerateSeed());
  static FString GenerateSeed();
  static void SeedRandom(FString seed = Maze::GenerateSeed());
  static void InitRoomLocations();
  static void ClearDataContainers();
  static void ClearPointers();
  static void PlaceStart();
  static void InitKeyRooms();
  static void PlaceAllKeyRooms();
  static Room* PlaceNextKeyRoom(Room* prevRoom, FName name);
  static RoomData* GetNextRoomData();
  static int RollNextRoom();
  static int RollRoomExit(int exits = 4);
  static int Roll(int min = 0, int max = 99);
  static bool IsNextRoomPlaceOccupied(Coordinates coords);
  static void CreateRoute(Room* prevRoom, Room* nextRoom);
  static void AddDirections(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minX, int maxX, int minY, int maxY);
  static void AddDirectionsVertical(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minY, int maxY);
  static void AddDirectionsHorizontal(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minX, int maxX);
  static void SafeCreateCorridor(int x, int y);
  static void AddGridExits(Room* room);
  static TArray<FRoomDataStruct>* IterateGrid();
};

enum SearchState {
  checked,
  used,
  clean
};
