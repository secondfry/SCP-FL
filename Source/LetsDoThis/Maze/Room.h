#pragma once

#include <vector>

enum Direction {
  north,
  east,
  south,
  west,
};

class Room {

private:
  std::vector<Room*> neighbours;
  int exits = 4;

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

  Room* GetAdjacent(const Direction direction) const {
    return this->neighbours[direction];
  }

  Room* GetOpposite(const Direction direction) const {
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
  Room(int exits) : exits(exits) {
    this->neighbours.resize(4);
  }

  Room() : Room(4) {}

  Room* CreateAdjacent(const Direction direction) {
    Room* ret = new Room();
    this->SetAdjacent(direction, ret);
    ret->SetOpposite(direction, this);
    return ret;
  }

  bool HasAdjacent(const Direction direction) const {
    Room* adjacent = this->GetAdjacent(direction);
    return adjacent != nullptr;
  }

  int GetExits() const { return exits; }
  Room* SetExits(int exits) {
    this->exits = exits;
    return this;
  }

};
