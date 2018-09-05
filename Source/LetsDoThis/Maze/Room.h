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

struct RoomData {
  std::vector<DirectionVariant> exits;
  int x;
  int y;
  int z;
  FName name;
};

class Room {
private:
  std::map<Direction, Room*> neighbours;
  RoomData* data;

  static Direction Reverse(const Direction direction) {
    switch (direction) {
      case Direction::north:
        return Direction::south;
      case Direction::east:
        return Direction::west;
      case Direction::south:
        return Direction::north;
      case Direction::west:
        return Direction::east;
      default:
        throw;
    }
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
  Room* CreateAdjacent(const Direction direction) {
    Room* ret = new Room();
    this->SetAdjacent(direction, ret);
    ret->SetOpposite(direction, this);
    return ret;
  }

  bool HasAdjacent(const Direction direction) {
    Room* adjacent = this->GetAdjacent(direction);
    return adjacent != nullptr;
  }

  Room* AddExit(const DirectionVariant dv) {
    this->data->exits.push_back(dv);
    return this;
  }

  Room* SetName(const FName name) {
    this->data->name = name;
    return this;
  }

  Room* SetPlace(const int x, const int y, const int z) {
    this->data->x = x;
    this->data->y = y;
    this->data->z = z;
    return this;
  }

  int GetPlaceX() const {
    return this->data->x;
  }

  int GetPlaceY() const {
    return this->data->y;
  }

  int GetPlaceZ() const {
    return this->data->z;
  }

};
