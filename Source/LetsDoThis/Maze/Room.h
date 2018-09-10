#pragma once

#include <vector>
#include <map>

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
  std::map<Direction, Room*> neighbours;
  std::vector<Direction> exits;
  Coordinates coords;
  FName name;

  static Direction Reverse(const Direction direction) {
    return static_cast<Direction>((direction + 2) % 4);
  }

  Room* GetAdjacent(const Direction direction) {
    return this->neighbours[direction];
  }

  Room* GetOpposite(const Direction direction) {
    return this->GetAdjacent(Room::Reverse(direction));
  }

  Room* SetAdjacent(const Direction direction, Room* room, bool const force = false) {
    this->neighbours[direction] = room;
    return this;
  }

  Room* SetOpposite(const Direction direction, Room* room, bool const force = false) {
    return this->SetAdjacent(Room::Reverse(direction), room, force);
  }

public:
  static Coordinates DirectionToCoordinates(const Direction direction) {
    switch (direction) {
    case Direction::north: return Coordinates{ 0, 1, 0 };
    case Direction::east: return Coordinates{ 1, 0, 0 };
    case Direction::south: return Coordinates{ 0, -1, 0 };
    case Direction::west: return Coordinates{ -1, 0, 0 };
    default: return Coordinates{ 0, 0, 0 };
    }
  }

  Room* CreateAdjacent(const Direction direction) {
    Room* ret = new Room();
    Coordinates shift = Room::DirectionToCoordinates(direction);
    ret->SetPlace(this->coords.x + shift.x, this->coords.y + shift.y, this->coords.z + shift.z);
    this->SetAdjacent(direction, ret);
    ret->SetOpposite(direction, this);
    return ret;
  }

  bool HasAdjacent(const Direction direction) {
    Room* adjacent = this->GetAdjacent(direction);
    return adjacent != nullptr;
  }

  Room* AddExit(const Direction dir) {
    this->exits.push_back(dir);
    return this;
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

    return this->exits[0];
  }

};
