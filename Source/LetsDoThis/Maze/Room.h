#pragma once

#include <vector>
#include <map>
#include "RoomDataStruct.h"

class Room;

enum Direction {
  north,
  east,
  south,
  west,
};

enum DirectionVariant {
  same,
  right,
  opposite,
  left
};

inline Direction operator+(Direction dir, DirectionVariant dv) {
  return static_cast<Direction>((static_cast<int>(dir) + static_cast<int>(dv)) % 4);
}

struct Coordinates {
  int x;
  int y;
  int z;
};

inline Coordinates operator+(Coordinates c1, Coordinates c2) {
  return Coordinates{ c1.x + c2.x, c1.y + c2.y, c1.z + c2.z };
}

inline Coordinates operator+(Coordinates coords, Direction dir) {
  switch (dir) {
  case Direction::north: return coords + Coordinates{ 0, 1, 0 };
  case Direction::east: return coords + Coordinates{ 1, 0, 0 };
  case Direction::south: return coords + Coordinates{ 0, -1, 0 };
  case Direction::west: return coords + Coordinates{ -1, 0, 0 };
  default: return coords + Coordinates{ 0, 0, 0 };
  }
}

inline Coordinates operator*(Coordinates coords, Direction dir) {
  switch (dir) {
  case Direction::north: return coords;
  case Direction::east: return Coordinates{ coords.y, coords.x * -1, coords.z };
  case Direction::south: return Coordinates{ coords.x * -1, coords.y * -1, coords.z };
  case Direction::west: return Coordinates{ coords.y * -1, coords.x, coords.z };
  }
  return Coordinates{};
}

struct RoomData {
  std::vector<DirectionVariant> exits;
  Coordinates coords;
  FName name;
};

class Room {
private:
  std::map<Direction, bool> exits;
  Coordinates coords;
  FName name;

  static Direction Reverse(const Direction direction) {
    return static_cast<Direction>((direction + 2) % 4);
  }

public:
  Room* AddExit(const Direction dir) {
    this->exits[dir] = true;
    return this;
  }

  FName GetName() const {
    return this->name;
  }

  Room* SetName(const FName name) {
    this->name = name;
    return this;
  }

  Room* SetPlace(const int x, const int y, const int z) {
    this->coords.x = x;
    this->coords.y = y;
    this->coords.z = z;
    return this;
  }

  Room* SetPlace(Coordinates coords) {
    this->coords.x = coords.x;
    this->coords.y = coords.y;
    this->coords.z = coords.z;
    return this;
  }

  Coordinates GetPlace() const {
    return this->coords;
  }

  int GetPlaceX() const {
    return this->coords.x;
  }

  int GetPlaceY() const {
    return this->coords.y;
  }

  int GetPlaceZ() const {
    return this->coords.z;
  }

  Direction GetFirstExit() {
    if (this->exits.empty()) {
      // TODO log exception
      return Direction::north;
    }

    return this->exits.begin()->first;
  }

  FRoomDataStruct MakeRoomDataStruct() {
    return FRoomDataStruct {
      this->name,
      this->coords.x,
      this->coords.y,
      this->coords.z,
      0
    };
  };

};
