#pragma once

#include "Maze.h"

#include "Array.h"
#include "Engine.h"
#include "Room.h"
#include "RoomDataStruct.h"

#include <algorithm>
#include <chrono>
#include <deque>
#include <iomanip>
#include <map>
#include <random>
#include <sstream>
#include <string>

std::mt19937 Maze::generator;
int Maze::roomDataVariantsCount = 0;
std::map<int, const RoomData*> Maze::roomDataVariants;
std::map<int, std::map<int, Room*>> Maze::grid;
std::map<int, std::map<int, int>> Maze::heightMap;
std::map<int, std::map<int, int>> Maze::stateMap;
std::map<int, std::map<int, int>> Maze::costMap;
std::map<int, std::map<int, std::pair<int, int>>> Maze::jumpMap;
Room* Maze::start = nullptr;
std::vector<FName> Maze::keyRooms;

TArray<FRoomDataStruct>* Maze::GenerateMap(FString seed) {
  // Done once
  Maze::InitRoomLocations();

  // Done each time
  Maze::SeedRandom(seed);
  Maze::ClearDataContainers();
  Maze::ClearPointers();
  Maze::InitKeyRooms();
  Maze::PlaceStart();
  Maze::PlaceAllKeyRooms();

  return Maze::IterateGrid();
}

FString Maze::GenerateSeed() {
  unsigned HEARTHSTONE = std::chrono::system_clock::now().time_since_epoch().count();
  Maze::generator.seed(HEARTHSTONE);
  std::stringstream stream;
  for (int i = 0; i < 16; i++) {
    stream << std::hex << Maze::Roll(0, 15);
  }
  return stream.str().c_str();
}

void Maze::SeedRandom(FString seed) {
  std::string str = TCHAR_TO_UTF8(*seed);
  std::seed_seq seq(str.begin(), str.end());
  Maze::generator.seed(seq);
}

void Maze::InitRoomLocations() {
  if (Maze::roomDataVariantsCount != 0) {
    return;
  }

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite }, Coordinates{ -1, 3, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite }, Coordinates{ 0, 3, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite }, Coordinates{ 1, 3, 0 });

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::right }, Coordinates{ -2, 0, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::right }, Coordinates{ -2, 1, 0 });

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::left }, Coordinates{ 2, 0, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::left }, Coordinates{ 2, 1, 0 });

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ -1, -1, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ -1, 0, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ 1, 0, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::same }, Coordinates{ 1, -1, 0 });

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::same, DirectionVariant::right }, Coordinates{ -1, 1, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::same, DirectionVariant::left }, Coordinates{ 1, 1, 0 });

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::right }, Coordinates{ -1, 3, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::right }, Coordinates{ -2, 3, 0 });

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::left }, Coordinates{ 1, 3, 0 });
  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::left }, Coordinates{ 2, 3, 0 });

  Maze::InitRoomLocation(std::vector<DirectionVariant>{ DirectionVariant::opposite, DirectionVariant::left, DirectionVariant::right }, Coordinates{ 0, 2, 0 });
}

void Maze::InitRoomLocation(std::vector<DirectionVariant> dvs, Coordinates coords) {
  Maze::roomDataVariants.insert(std::pair<int, const RoomData*>(Maze::roomDataVariantsCount++, new RoomData{ dvs, coords }));
}

void Maze::ClearDataContainers() {
  Maze::grid.clear();
  Maze::heightMap.clear();
  Maze::stateMap.clear();
  Maze::costMap.clear();
  Maze::jumpMap.clear();
  Maze::keyRooms.clear();
}

void Maze::ClearPointers() {
  if (Maze::start != nullptr) {
    delete Maze::start;
  }

  Maze::start = nullptr;
}

void Maze::PlaceStart() {
  Maze::start = new Room();
  Maze::grid[0][0] = Maze::start->AddExit(Direction::north)->SetName("SPAWN_CLASSD")->SetPlace(0, 0, 0);
  Maze::heightMap[0][0] = 57;
}

void Maze::InitKeyRooms() {
  for (const FName name : { "CR_914", "CR_173", "ROOM1", "ROOM2" }) {
    Maze::keyRooms.push_back(name);
  }

  std::shuffle(std::begin(Maze::keyRooms), std::end(Maze::keyRooms), Maze::generator);
}

void Maze::PlaceAllKeyRooms() {
  auto prevRoom = Maze::start;
  for (const FName name : Maze::keyRooms) {
    prevRoom = Maze::PlaceNextKeyRoom(prevRoom, name);
  }
}

Room* Maze::PlaceNextKeyRoom(Room* prevRoom, FName name) {
  Direction exitDirection;
  Coordinates nextRoomCoordinates;

  do {
    auto roomData = Maze::GetNextRoomData();
    roomData->coords = roomData->coords * prevRoom->GetFirstExit();
    nextRoomCoordinates = prevRoom->GetPlace() + roomData->coords;

    const auto exitID = Maze::RollRoomExit(roomData->exits.size());
    exitDirection = prevRoom->GetFirstExit() + roomData->exits[exitID];
  } while (
    Maze::IsNextRoomPlaceOccupied(nextRoomCoordinates) ||
    Maze::IsNextRoomPlaceOccupied(nextRoomCoordinates + exitDirection)
  );

  auto nextRoom = new Room();
  nextRoom->AddExit(exitDirection);
  nextRoom->SetName(name);
  nextRoom->SetPlace(nextRoomCoordinates);

  Maze::grid[nextRoom->GetPlaceY()][nextRoom->GetPlaceX()] = nextRoom;
  Maze::heightMap[nextRoom->GetPlaceY()][nextRoom->GetPlaceX()] = 57;

#ifdef SF_DEBUG_MAZE
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, (std::string("NEXT X: ") + std::to_string(nextRoom->GetPlaceX()) + std::string(" Y: ") + std::to_string(nextRoom->GetPlaceY())).c_str());
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, (std::string("NEXT EXIT: ") + std::to_string(nextRoom->GetFirstExit())).c_str());
#endif

  Maze::CreateRoute(prevRoom, nextRoom);

  return nextRoom;
}

RoomData* Maze::GetNextRoomData() {
  const auto nextRoomID = Maze::RollNextRoom();
  const RoomData* nextRoomData = Maze::roomDataVariants[nextRoomID];
  auto ret = new RoomData(*nextRoomData);
  return ret;
}

int Maze::RollNextRoom() {
  return Maze::Roll(0, Maze::roomDataVariantsCount - 1);
}

int Maze::RollRoomExit(int exits) {
  return Maze::Roll(0, exits - 1);
}

int Maze::Roll(int min, int max) {
  const std::uniform_int_distribution<int> uniform_int_distribution(min, max);
  return uniform_int_distribution(generator);
}

bool Maze::IsNextRoomPlaceOccupied(Coordinates coords) {
  int x = coords.x;
  int y = coords.y;

  if (Maze::grid[y][x] != nullptr) {
    auto occupied = Maze::grid[y][x];
  }

  return Maze::grid[y][x] != nullptr;
}

void Maze::CreateRoute(Room* keyStart, Room* keyFinish) {
  // Actually route from "connection" rooms, not from key rooms themselves
  Room* start = new Room();
  start->SetPlace(keyStart->GetPlace() + keyStart->GetFirstExit());
  Room* finish = new Room();
  finish->SetPlace(keyFinish->GetPlace() + keyFinish->GetFirstExit());

  // Widen search for reasons
  const int minY = start->GetPlaceY() > finish->GetPlaceY() ? finish->GetPlaceY() - 2 : start->GetPlaceY() - 2;
  const int maxY = start->GetPlaceY() > finish->GetPlaceY() ? start->GetPlaceY() + 2 : finish->GetPlaceY() + 2;
  const int minX = start->GetPlaceX() > finish->GetPlaceX() ? finish->GetPlaceX() - 2 : start->GetPlaceX() - 2;
  const int maxX = start->GetPlaceX() > finish->GetPlaceX() ? start->GetPlaceX() + 2 : finish->GetPlaceX() + 2;

  // Start and finish are free by default
  Maze::heightMap[start->GetPlaceY()][start->GetPlaceX()] = 0;
  Maze::heightMap[finish->GetPlaceY()][finish->GetPlaceX()] = 0;

  // Set/reset height and state
  for (int j = minY; j < maxY; j++) {
    for (int i = minX; i < maxX; i++) {
      if (!Maze::heightMap[j][i]) {
        Maze::heightMap[j][i] = Maze::Roll(1, 5);
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
    const std::pair<int, int> coord = queue.front();
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

  if (Maze::jumpMap[finish->GetPlaceY()][finish->GetPlaceX()] == std::pair<int, int>{ INT_MIN, INT_MIN }) {
    // TODO trigger event instead of panic message
#ifdef SF_DEBUG_MAZE
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "FUCK THIS SHIT");
#endif
  } else {
#ifdef SF_DEBUG_MAZE
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "IT WORKS");
#endif

    std::vector<std::pair<int, int>> path;
    for (
      std::pair<int, int> jump = { finish->GetPlaceY(), finish->GetPlaceX() };
      jump != std::pair<int, int>{ INT_MIN, INT_MIN };
      jump = Maze::jumpMap[jump.first][jump.second]
    ) {
      path.push_back(jump);
    }
    std::reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); ++i) {
      Maze::heightMap[path[i].first][path[i].second] = 0;
      Maze::SafeCreateCorridor(path[i].second, path[i].first);
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
    directions.emplace_back(coords.first - 1, coords.second);
  }

  if (coords.first < maxY) {
    directions.emplace_back(coords.first + 1, coords.second);
  }
}

void Maze::AddDirectionsHorizontal(std::vector<std::pair<int, int>>& directions, std::pair<int, int> coords, int minX, int maxX) {
  if (minX == maxX) {
    return;
  }

  if (coords.second > minX) {
    directions.emplace_back(coords.first, coords.second - 1);
  }

  if (coords.second < maxX) {
    directions.emplace_back(coords.first, coords.second + 1);
  }
}

void Maze::SafeCreateCorridor(int x, int y) {
  if (Maze::grid[y][x] != nullptr) {
    return;
  }

  Maze::grid[y][x] = (new Room())->SetPlace(x, y, 0);
}

TArray<FRoomDataStruct>* Maze::IterateGrid() {
  auto ret = new TArray<FRoomDataStruct>();

  for (auto& row : Maze::grid) {
    for (auto& roomPair : row.second) {
      auto room = roomPair.second;

      if (room == nullptr) {
        // TODO DEPLOY SECURITY ROBOTS PENETRATE NULL POINTER ACCESS
        // WONTFIX?
#ifdef SF_DEBUG_MAZE
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "NULL POINTER ACCESS SOMETHING IS TERRIBLY WRONG");
#endif
        continue;
      }

      Maze::AddGridExits(room);
      ret->Add(room->MakeRoomDataStruct());
    }
  }

  return ret;
}

void Maze::AddGridExits(Room* room) {
  if (Maze::grid[room->GetPlaceY() + 1][room->GetPlaceX()] != nullptr) {
    room->AddExit(Direction::north);
  }

  if (Maze::grid[room->GetPlaceY() - 1][room->GetPlaceX()] != nullptr) {
    room->AddExit(Direction::south);
  }

  if (Maze::grid[room->GetPlaceY()][room->GetPlaceX() + 1] != nullptr) {
    room->AddExit(Direction::east);
  }

  if (Maze::grid[room->GetPlaceY()][room->GetPlaceX() - 1] != nullptr) {
    room->AddExit(Direction::west);
  }
}
