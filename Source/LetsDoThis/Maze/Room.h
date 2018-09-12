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
  return Coordinates{ };
}

inline bool operator==(Coordinates c1, Coordinates c2) {
  return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z;
}

struct RoomData {
  std::vector<DirectionVariant> exits;
  Coordinates coords;
  FName name;
};

enum RoomType {
  Corridor,
  Turn,
  TIntersection,
  XIntersection,
  Key,
  DeadEnd
};

class Room {
private:
  std::map<Direction, bool> exits;
  Coordinates coords;
  FName name;
  bool isKeyRoom;
  RoomType roomType;
  int yaw;

public:
  static Direction Reverse(const Direction direction) {
    return static_cast<Direction>((direction + 2) % 4);
  }

  Room* AddExit(const Direction dir) {
    this->exits[dir] = true;
    return this;
  }

  std::map<Direction, bool> GetExits() const {
    return this->exits;
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
    this->DecideRoomType();
    this->DecideName();
    this->DecideYaw();

    /**
     * Well this major fuck up
     * I never knew that in UE4 X-axis is vertical and Y-axis is horizontal
     */
    return FRoomDataStruct{
      this->name,
      this->coords.y,
      this->coords.x,
      this->coords.z,
      this->yaw
    };
  };

  void DecideRoomType() {
    if (this->isKeyRoom) {
      this->roomType = RoomType::Key;
      return;
    }

    const int size = this->exits.size();
    if (size == 2) {
      int sum = 0;
      for (auto& elem : this->exits) {
        sum += elem.first;
      }

      if (sum % 2 == 0) {
        this->roomType = RoomType::Corridor;
        return;
      }

      this->roomType = RoomType::Turn;
      return;
    }

    switch (this->exits.size()) {
      case 4:
        this->roomType = RoomType::XIntersection;
        return;
      case 3:
        this->roomType = RoomType::TIntersection;
        return;
      default:
        // TODO log exception
        this->roomType = RoomType::XIntersection;
        return;
    }
  }

  void DecideName() {
    // Key rooms should have their names set upon creation
    if (this->isKeyRoom) {
      return;
    }

    FString name = "Corridor_LCZ_";
    name += this->GetRoomTypeString();
    this->name = FName(*name);
  }

  void DecideYaw() {
    if (this->isKeyRoom) {
      this->yaw = this->GetFirstExit() * 90;
      return;
    }

    int magicSum = 0;
    int magicMul = 1;
    for (auto& elem : this->exits) {
      magicSum += elem.first;
      magicMul *= elem.first;
    }
    const int magicNumber = magicSum + magicMul;

    switch (this->roomType) {
      case RoomType::Corridor:
        this->yaw = this->GetFirstExit() * 90 % 180;
        return;
      case RoomType::Turn:
        switch (magicNumber) {
            // 0 + 1 + 0 * 1
          case 1:
            this->yaw = Direction::north * 90;
            return;
            // 1 + 2 + 1 * 2
          case 5:
            this->yaw = Direction::east * 90;
            return;
            // 2 + 3 + 2 * 3
          case 11:
            this->yaw = Direction::south * 90;
            return;
            // 3 + 0 + 3 * 0
          case 3:
            this->yaw = Direction::west * 90;
            return;
          default:
            // TODO log exception
            this->yaw = 0;
            return;
        }
        break;
      case RoomType::TIntersection:
        switch (magicNumber) {
            // 0 + 1 + 2 + 0 * 1 * 2
          case 3:
            this->yaw = Direction::north * 90;
            return;
            // 1 + 2 + 3 + 1 * 2 * 3
          case 12:
            this->yaw = Direction::east * 90;
            return;
            // 2 + 3 + 0 + 2 * 3 * 0
          case 5:
            this->yaw = Direction::south * 90;
            return;
            // 3 + 0 + 1 + 3 * 0 * 1
          case 4:
            this->yaw = Direction::west * 90;
            return;
          default:
            // TODO log exception
            this->yaw = 0;
            return;
        }
        break;
      case RoomType::XIntersection:
        this->yaw = Direction::north * 90;
        return;
      default:
        // TODO log exception
        this->yaw = 0;
        return;
    }
  }

  FString GetRoomTypeString() const {
    switch (this->roomType) {
      case RoomType::Corridor: return "I";
      case RoomType::Turn: return "R";
      case RoomType::TIntersection: return "T";
      case RoomType::XIntersection: return "X";
      default: return "X";
    }
  }

  bool IsKeyRoom() const {
    return this->isKeyRoom;
  }

  Room* IsKeyRoom(bool isKeyRoom) {
    this->isKeyRoom = isKeyRoom;
    return this;
  }

};
