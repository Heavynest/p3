//
// Created by xiao- on 2017/7/1 0001.
//

#ifndef P3_SIMULATION_H
#define P3_SIMULATION_H

#include <fstream>
#define cout out_____
static std::ofstream out_____("test.txt");

#include "world_type.h"
class toolessargument{};
class negativeround{};
class fileopen{};
class toomanyspecies{};
class toomanyprograms{};
class errorinstruction{};
class illegalheight{};
class illegalwidth{};
class illegalterrain{};
class toomanycreatures{};
class unknownspecies{};
class unknowndir{};
class outsideboundry{};
class wrongability{};
class occupied{};
class lakeerror{};
opcode_t findopcode(const string &s);
terrain_t findterrain(const char t);
direction_t  finddir(const string d);
point_t INFECTjudge(const creature_t &creature, const grid_t &grid);
void infect(creature_t&creature,grid_t &grid);
void hop(grid_t &grid, creature_t &creature);

bool ifempty(creature_t &creature, grid_t &grid);
bool ifenemy(creature_t &creature, grid_t &grid);
bool initWorld(world_t &world, const string &speciesFile,
               const string &worldFile);
bool ifsame(creature_t &creature, grid_t &grid);
bool ifwall(creature_t &creature, grid_t &grid);
void Simulate(world_t &world, int round, string v);
// MODIFIES: world
//
// EFFECTS: Initialize "world" given the species summary file
// "speciesFile" and the world description file
// "worldFile". This initializes all the components of
// "world". Returns true if initialization is successful.
bool simulateCreature(creature_t &creature, grid_t &grid, bool verbose);
// REQUIRES: creature is inside the grid.
//
// MODIFIES: creature, grid, cout.
//
// EFFECTS: Simulate one turn of "creature" and update the creature,
// the infected creature, and the grid if necessary.
// The creature programID is always updated. The function
// also prints to the stdout the procedure. If verbose is
// true, it prints more information.
void printGrid(const grid_t &grid);
// MODIFIES: cout.
//
// EFFECTS: print a grid representation of the creature world.
point_t adjacentPoint(point_t pt, direction_t dir);
// EFFECTS: Returns a point that results from moving one square
// in the direction "dir" from the point "pt".
direction_t leftFrom(direction_t dir);
// EFFECTS: Returns the direction that results from turning
// left from the given direction "dir".
direction_t rightFrom(direction_t dir);
// EFFECTS: Returns the direction that results from turning
// right from the given direction "dir".
instruction_t getInstruction(const creature_t &creature);
// EFFECTS: Returns the current instruction of "creature".
creature_t *getCreature(const grid_t &grid, point_t location);
// REQUIRES: location is inside the grid.
//
// EFFECTS: Returns a pointer to the creature at "location" in "grid".
#endif //P3_SIMULATION_H
