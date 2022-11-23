#pragma once

#include "dungeon.h"

#include <cstddef>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

class DungeonSolver {
public:
    DungeonSolver(Room* starting_room) : starting_room_(starting_room) {
    }

    Room* Solve() {
        Room* final_room_maybe = Dfs(starting_room_);

        while (!final_room_maybe->IsFinal() && !closed_doors_.empty()) {
            int64_t doors_num = closed_doors_.size();
            Door* door = closed_doors_.front();
            while (doors_num--) {
                door = closed_doors_.front();
                for (auto key : keys_) {
                    if (door->TryOpen(key)) {
                        break;
                    }
                }
                closed_doors_.pop_front();
                if (door->IsOpen()) {
                    if (!used_rooms_.contains(door->GoThrough())) {
                        break;
                    }
                } else {
                    closed_doors_.push_back(door);
                }
            }
            if (!door->IsOpen()) {
                break;
            }
            final_room_maybe = Dfs(door->GoThrough());
        }

        return final_room_maybe->IsFinal() ? final_room_maybe : nullptr;
    }

private:
    Room* starting_room_;
    std::unordered_set<Room*> used_rooms_;
    std::vector<std::string> keys_;
    std::deque<Door*> closed_doors_;

    Room* Dfs(Room* cur_room) {
        if (cur_room->IsFinal()) {
            return cur_room;
        }

        used_rooms_.insert(cur_room);

        for (size_t key_index = 0; key_index < cur_room->NumKeys(); ++key_index) {
            keys_.push_back(cur_room->GetKey(key_index));
        }

        for (size_t door_index = 0; door_index < cur_room->NumDoors(); ++door_index) {
            Door* door = cur_room->GetDoor(door_index);
            if (door->IsOpen()) {
                if (!used_rooms_.contains(door->GoThrough())) {
                    Room* room = Dfs(door->GoThrough());
                    if (room->IsFinal()) {
                        return room;
                    }
                }
            } else {
                closed_doors_.push_back(door);
            }
        }

        return cur_room;
    }
};

Room* FindFinalRoom(Room* starting_room) {
    return DungeonSolver(starting_room).Solve();
}
