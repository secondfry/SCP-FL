#pragma once

#include "Maze.h"

#include "Array.h"
#include "Engine.h"
#include "Room.h"
#include "RoomDataStruct.h"

#include <deque>
#include <map>
#include <random>
#include <string>

std::mt19937 Maze::generator;
std::map<int, RoomData*> Maze::roomDataVariants;
std::map<int, std::map<int, Room*>> Maze::grid;
std::map<int, std::map<int, int>> Maze::heightMap;
std::map<int, std::map<int, int>> Maze::stateMap;
std::map<int, std::map<int, int>> Maze::costMap;
std::map<int, std::map<int, std::pair<int, int>>> Maze::jumpMap;
Room* Maze::start;

TArray<FRoomDataStruct> Maze::generateMap() {
  Maze::initRoomLocations();
  Maze::clearGrid();
  Maze::placeStart();
  Maze::placeAllKeyRooms();

  return TArray<FRoomDataStruct>{ };
}

void Maze::initRoomLocations() {
  if (!Maze::roomDataVariants.empty()) {
    return;
  }

  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(0, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite }, Coordinates{ 0, 4, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(1, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::left }, Coordinates{ 1, 3, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(2, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::left }, Coordinates{ 2, 2, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(3, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::left }, Coordinates{ 3, 1, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(4, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite }, Coordinates{ 0, 3, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(5, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::left }, Coordinates{ 1, 2, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(6, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::left }, Coordinates{ 2, 1, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(7, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::right }, Coordinates{ -1, 3, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(8, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::left, DirectionVariant::right }, Coordinates{ 0, 2, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(9, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same, DirectionVariant::left }, Coordinates{ 1, 1, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(10, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same, DirectionVariant::left }, Coordinates{ 2, 0, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(11, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::right }, Coordinates{ -1, 2, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(12, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ 1, 0, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(13, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::right }, Coordinates{ -2, 2, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(14, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same, DirectionVariant::right }, Coordinates{ -1, 1, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(15, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ 1, -1, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(16, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::right }, Coordinates{ -2, 1, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(17, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ -1, 0, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(18, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::right }, Coordinates{ -3, 1, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(19, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same, DirectionVariant::right }, Coordinates{ -2, 0, 0 } }));
  Maze::roomDataVariants.insert(std::pair<int, RoomData*>(20, new RoomData{ std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ -1, -1, 0 } }));
}

void Maze::clearGrid() {
  Maze::grid.clear();
}

void Maze::placeStart() {
  Maze::start = new Room();
  Maze::grid[0][0] = Maze::start->AddExit(Direction::north)->SetName("SPAWN_CLASSD")->SetPlace(0, 0, 0);
  Maze::heightMap[0][0] = 57;
}

void Maze::placeAllKeyRooms() {
  auto prevRoom = Maze::start;
  for (const FName name : { "CR_914", "CR_173", "ROOM1", "ROOM2" }) {
    prevRoom = Maze::placeNextKeyRoom(prevRoom, name);
  }
}

Room* Maze::placeNextKeyRoom(Room* prevRoom, FName name) {
  auto roomData = Maze::getNextRoomData();

  while (
    Maze::isNextRoomPlaceOccupied(prevRoom->GetPlaceX() + roomData->coords.x, prevRoom->GetPlaceY() + roomData->coords.y)
  ) {
    roomData = Maze::getNextRoomData();
  }

  auto nextRoom = new Room();
  auto const exitID = Maze::rollRoomExit(roomData->exits.size());
  nextRoom->AddExit(prevRoom->GetFirstExit() + roomData->exits[exitID]);
  nextRoom->SetName(name);
  nextRoom->SetPlace(prevRoom->GetPlaceX() + roomData->coords.x, prevRoom->GetPlaceY() + roomData->coords.y, prevRoom->GetPlaceZ() + roomData->coords.z);

  Maze::grid[nextRoom->GetPlaceY()][nextRoom->GetPlaceX()] = nextRoom;

  Maze::createRoute(prevRoom, nextRoom);

  return nextRoom;
}

RoomData* Maze::getNextRoomData() {
  auto const nextRoomID = Maze::rollNextRoom();
  RoomData* nextRoomData = Maze::roomDataVariants[nextRoomID];
  return nextRoomData;
}

int Maze::rollNextRoom(int seed) {
  return roll(0, 20, seed);
}

int Maze::rollRoomExit(int exits, int seed) {
  return roll(0, exits - 1, seed);
}

int Maze::roll(int min, int max, int seed) {
  const std::uniform_int_distribution<int> uniform_int_distribution(min, max);
  generator.seed(seed);
  return uniform_int_distribution(generator);
}

bool Maze::isNextRoomPlaceOccupied(int x, int y) {
  return Maze::grid[y][x] != nullptr;
}

void Maze::createRoute(Room* keyStart, Room* keyFinish) {
  // Actually route from "connection" rooms, not from key rooms themselves
  Room* start = keyStart->CreateAdjacent(keyStart->GetFirstExit());
  Room* finish = keyFinish->CreateAdjacent(keyFinish->GetFirstExit());

  // Widen search for reasons
  const int minY = start->GetPlaceY() > finish->GetPlaceY() ? finish->GetPlaceY() - 1 : start->GetPlaceY() - 1;
  const int maxY = start->GetPlaceY() > finish->GetPlaceY() ? start->GetPlaceY() + 1 : finish->GetPlaceY() + 1;
  const int minX = start->GetPlaceX() > finish->GetPlaceX() ? finish->GetPlaceX() - 1 : start->GetPlaceX() - 1;
  const int maxX = start->GetPlaceX() > finish->GetPlaceX() ? start->GetPlaceX() + 1 : finish->GetPlaceX() + 1;

  // Start and finish are free by default
  Maze::heightMap[start->GetPlaceY()][start->GetPlaceX()] = 0;
  Maze::heightMap[finish->GetPlaceY()][finish->GetPlaceX()] = 0;

  // Set/reset height and state
  for (int j = minY; j < maxY; j++) {
    for (int i = minX; i < maxX; i++) {
      if (!Maze::heightMap[j][i]) {
        Maze::heightMap[j][i] = Maze::roll(1, 5);
      }

      Maze::stateMap[j][i] = SearchState::clean;
      Maze::costMap[j][i] = INT_MAX;
      Maze::jumpMap[j][i] = std::pair<int, int>{ INT_MIN, INT_MIN };
    }
  }

  // Start is always used in path
  Maze::stateMap[start->GetPlaceY()][start->GetPlaceX()] = SearchState::used;
  Maze::costMap[start->GetPlaceY()][start->GetPlaceX()] = 0;

  // Queue to read from
  std::deque<std::pair<int, int>> queue;
  queue.emplace_back(std::pair<int, int>{ start->GetPlaceY(), start->GetPlaceX() });

  // Choose step
  std::vector<std::pair<int, int>> directions;
  
  // Levit's magic
  while (!queue.empty()) {
    std::pair<int, int> coord = queue.front();
    queue.pop_front();
    Maze::stateMap[coord.first][coord.second] = SearchState::checked;

    directions.clear();
    Maze::AddDirections(directions, coord, minX, maxX, minY, maxY);

    for (std::pair<int, int> to : directions) {
      int length = Maze::heightMap[to.first][to.second];
      if (Maze::costMap[to.first][to.second] <= Maze::costMap[coord.first][coord.second] + length) {
        continue;
      }

      Maze::costMap[to.first][to.second] = Maze::costMap[coord.first][coord.second] + length;
      if (Maze::stateMap[to.first][to.second] == SearchState::clean) {
        queue.emplace_back(to);
      } else if (Maze::stateMap[to.first][to.second] == SearchState::checked) {
        queue.emplace_front(to);
      }

      Maze::jumpMap[to.first][to.second] = coord;
      Maze::stateMap[to.first][to.second] = SearchState::used;
    }
  }

  if (Maze::jumpMap[finish->GetPlaceY()][finish->GetPlaceX()] == std::pair<int, int> {INT_MIN, INT_MIN}) {
    // TODO trigger event instead of panic message
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "FUCK THIS SHIT");
  } else {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "IT WORKS");

    std::vector<std::pair<int, int>> path;
    for (
      std::pair<int, int> jump = { finish->GetPlaceY(), finish->GetPlaceX() }; 
      jump != std::pair<int, int> {INT_MIN, INT_MIN};
      jump = Maze::jumpMap[jump.first][jump.second]
    ) {
      path.push_back(jump);
    }
    std::reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); ++i) {
      Maze::heightMap[path[i].first][path[i].second] = 0;
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, (std::string("X: ") + std::to_string(path[i].second) + std::string(" Y: ") + std::to_string(path[i].first)).c_str());
    }
  }
  
}

void Maze::AddDirections(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minX, int maxX, int minY, int maxY) {
  Maze::AddDirectionsVertical(directions, coords, minY, maxY);
  Maze::AddDirectionsHorizontal(directions, coords, minX, maxX);
}

void Maze::AddDirectionsVertical(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minY, int maxY) {
  if (minY == maxY) {
    return;
  }

  if (coords.first > minY) {
    directions.push_back(std::pair<int, int> { coords.first - 1, coords.second });
  }

  if (coords.first < maxY) {
    directions.push_back(std::pair<int, int> { coords.first + 1, coords.second });
  }
}

void Maze::AddDirectionsHorizontal(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minX, int maxX) {
  if (minX == maxX) {
    return;
  }

  if (coords.second > minX) {
    directions.push_back(std::pair<int, int> { coords.first, coords.second - 1 });
  }

  if (coords.second < maxX) {
    directions.push_back(std::pair<int, int> { coords.first, coords.second + 1 });
  }
}
