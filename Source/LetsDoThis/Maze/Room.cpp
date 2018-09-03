enum Direction {
  north,
  east,
  south,
  west,
};

class Room {

private:
  Room* north = nullptr;
  Room* east = nullptr;
  Room* south = nullptr;
  Room* west = nullptr;

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
    switch (direction) {
      case Direction::north:
        return this->north;
      case Direction::east:
        return this->east;
      case Direction::south:
        return this->south;
      case Direction::west:
        return this->west;
      default:
        throw;
    }
  }

  Room* GetOpposite(const Direction direction) const {
    return this->GetAdjacent(Room::Reverse(direction));
  }

  Room* SetAdjacent(const Direction direction, Room room, bool const force = false) {
    Room* adjacent = this->GetAdjacent(direction);

    if (adjacent != nullptr && !force) {
      throw;
    }

    *adjacent = room;
    return this;
  }

  Room* SetOpposite(const Direction direction, Room room, bool const force = false) {
    return this->SetAdjacent(Room::Reverse(direction), room, force);
  }

public:
  Room() = default;

  Room CreateAdjacent(const Direction direction) {
    Room ret = Room();
    this->SetAdjacent(direction, ret);
    ret.SetOpposite(direction, *this);
    return ret;
  }

  bool HasAdjacent(const Direction direction) const {
    Room* adjacent = this->GetAdjacent(direction);
    return adjacent != nullptr;
  }

};
