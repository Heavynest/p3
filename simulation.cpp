//
// Created by xiao- on 2017/7/1 0001.
//

#include<iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include<cstdlib>
#include"simulation.h"

using namespace std;


ability_t findability(creature_t &creature, const string a)
{

    if (a == "f") return FLY;
    if (a == "a") return ARCH;
    cout << "Error: Creature (" << creature.species->name << ' ' << directName[creature.direction] << ' ' << creature.location.r
         << ' ' << creature.location.c << ") has an invalid ability " << a << '!' << endl;
    throw wrongability();
}

opcode_t findopcode(const string &s)
{
    if (s == "hop") return HOP;
    else if (s == "left")return LEFT;
    else if (s == "right")return RIGHT;
    else if (s == "infect")return INFECT;
    else if (s == "ifempty")return IFEMPTY;
    else if (s == "ifenemy")return IFENEMY;
    else if (s == "ifsame")return IFSAME;
    else if (s == "ifwall")return IFWALL;
    else if (s == "go") return GO;
    else
    {
        cout << "Error: Instruction " << s << " is not recognized!" << endl;
        throw errorinstruction();
    }
}

terrain_t findterrain(const char t)
{
    if (t == 'P') return PLAIN;
    if (t == 'L') return LAKE;
    if (t == 'F') return FOREST;
    if (t == 'H') return HILL;
}

direction_t finddir(const string d)
{
    if (d == "east")return EAST;
    if (d == "west") return WEST;
    if (d == "south") return SOUTH;
    if (d == "north") return NORTH;
    cout << "Error: Direction " << d << "is not recognized!" << endl;
    throw unknowndir();
}

species_t *getspecies(world_t &world, const string name)
{
    int sum = world.numSpecies;
    for (int i = 0; i < sum; i++)
    {
        if (world.species[i].name == name)
            return &world.species[i];
    }
    cout << "Error: Species " << name << "not found!" << endl;
    throw unknownspecies();
}


void printGrid(const grid_t &grid)
{
    int h = grid.height;
    int w = grid.width;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (grid.squares[i][j] == NULL)
            {
                cout << "____ ";
            } else
            {
                cout << grid.squares[i][j]->species->name.substr(0, 2) << "_"
                     << directShortName[grid.squares[i][j]->direction] << " ";
            }
        }
        cout << endl;
    }

}

direction_t rightFrom(direction_t dir)
{
    switch (dir)
    {
        case (EAST):
            dir = SOUTH;
            break;
        case (WEST):
            dir = NORTH;
            break;
        case (NORTH):
            dir = EAST;
            break;
        case (SOUTH):
            dir = WEST;
            break;
    }
    return dir;
}

direction_t leftFrom(direction_t dir)
{
    switch (dir)
    {
        case (EAST):
            dir = NORTH;
            break;
        case (WEST):
            dir = SOUTH;
            break;
        case (NORTH):
            dir = WEST;
            break;
        case (SOUTH):
            dir = EAST;
            break;
    }
    return dir;
}

instruction_t getInstruction(const creature_t &creature)
{
    int cmd = creature.programID;
    return creature.species->program[cmd];
}

creature_t *getCreature(const grid_t &grid, point_t location)
{
    return grid.squares[location.r][location.c];
}

void hop(grid_t &grid, creature_t &creature)
{
    int &r = creature.location.r;
    int &c = creature.location.c;
    switch (creature.direction)
    {
        case (EAST):
            if (!(c + 1 >= grid.width || grid.squares[r][c + 1] != NULL || ifwall(creature, grid)))
            {
                grid.squares[r][c] = NULL;
                c++;
                grid.squares[r][c] = &creature;
                creature.hillActive = 0;
            }
            break;
        case (WEST):
            if (!(c - 1 < 0 || grid.squares[r][c - 1] != NULL || ifwall(creature, grid)))
            {
                grid.squares[r][c] = NULL;
                c--;
                grid.squares[r][c] = &creature;
                creature.hillActive = 0;
            }
            break;
        case (SOUTH):
            if (!(r + 1 >= grid.width || grid.squares[r + 1][c] != NULL || ifwall(creature, grid)))
            {
                grid.squares[r][c] = NULL;
                r++;
                grid.squares[r][c] = &creature;
                creature.hillActive = 0;
            }
            break;
        case (NORTH):
            if (!(r - 1 < 0 || grid.squares[r - 1][c] != NULL || ifwall(creature, grid)))
            {
                grid.squares[r][c] = NULL;
                r--;
                grid.squares[r][c] = &creature;
                creature.hillActive = 0;
            }
            break;
    }


}

point_t INFECTjudge(const creature_t &creature, const grid_t &grid)
{
    int r = creature.location.r;
    int c = creature.location.c;
    if (creature.direction == EAST)
    {
        for (int i = c; i < grid.width; i++)
        {
            if (grid.squares[r][i] != NULL && grid.squares[r][i]->species != grid.squares[r][c]->species)
            {
                point_t t = {r, i};
                return t;
            }
        }
    } else if (creature.direction == WEST)
    {
        for (int i = c; i >= 0; i--)
        {
            if (grid.squares[r][i] != NULL && grid.squares[r][i]->species != grid.squares[r][c]->species)
            {
                point_t t = {r, i};
                return t;
            }
        }

    } else if (creature.direction == NORTH)
    {
        for (int i = r; i >= 0; i--)
        {
            if (grid.squares[i][c] != NULL && grid.squares[i][c]->species != grid.squares[r][c]->species)
            {
                point_t t = {i, c};
                return t;
            }
        }

    } else
    {
        for (int i = r; i < grid.height; i++)
        {
            if (grid.squares[i][c] != NULL && grid.squares[i][c]->species != grid.squares[r][c]->species)
            {
                point_t t = {i, c};
                return t;
            }
        }
    }
    point_t t = {r, c};
    return t;
}

void infect(creature_t &creature, grid_t &grid)
{
    point_t t = creature.location;
    if (creature.ability[ARCH] != 1)
    {

        if (creature.direction == EAST)
        {
            if (!(t.c + 1 >= grid.width || grid.terrain[t.r][t.c + 1] == FOREST ||
                  grid.squares[t.r][t.c + 1] == NULL || grid.squares[t.r][t.c + 1]->species == creature.species))
            {
                grid.squares[t.r][t.c + 1]->species = creature.species;
                grid.squares[t.r][t.c + 1]->programID = 0;
            }
        }
        if (creature.direction == WEST)
        {
            if (!(t.c - 1 < 0 || grid.terrain[t.r][t.c - 1] == FOREST ||
                  grid.squares[t.r][t.c - 1] == NULL || grid.squares[t.r][t.c - 1]->species == creature.species))
            {
                grid.squares[t.r][t.c - 1]->species = creature.species;
                grid.squares[t.r][t.c - 1]->programID = 0;
            }
        }
        if (creature.direction == NORTH)
        {
            if (!(t.r - 1 < 0 || grid.terrain[t.r - 1][t.c] == FOREST ||
                  grid.squares[t.r - 1][t.c] == NULL || grid.squares[t.r - 1][t.c]->species == creature.species))
            {
                grid.squares[t.r - 1][t.c]->species = creature.species;
                grid.squares[t.r - 1][t.c]->programID = 0;
            }
        }
        if (creature.direction == SOUTH)
        {
            if (!(t.r + 1 >= grid.height || grid.terrain[t.r + 1][t.c] == FOREST ||
                  grid.squares[t.r + 1][t.c] == NULL || grid.squares[t.r + 1][t.c]->species == creature.species))
            {
                grid.squares[t.r+1][t.c ]->species = creature.species;
                grid.squares[t.r+1][t.c ]->programID = 0;
            }
        }
    } else
    {
        point_t q = INFECTjudge(creature, grid);
        if (q.r != creature.location.r || q.c != creature.location.c)
        {
            grid.squares[q.r][q.c]->species = creature.species;
            grid.squares[q.r][q.c]->programID = 0;
        }
    }
}

bool ifempty(creature_t &creature, grid_t &grid)
{
    point_t t = creature.location;
    if (creature.direction == EAST && t.c + 1 < grid.width && (grid.squares[t.r][t.c + 1] == NULL||grid.terrain[t.r][t.c+1]==FOREST)) return true;
    if (creature.direction == WEST && t.c - 1 >= 0 && (grid.squares[t.r][t.c - 1] == NULL||grid.terrain[t.r][t.c - 1]==FOREST))return true;
    if (creature.direction == NORTH && t.r - 1 >= 0 && (grid.squares[t.r - 1][t.c] == NULL||grid.terrain[t.r-1][t.c]==FOREST)) return true;
    if (creature.direction == SOUTH && t.r + 1 < grid.height && (grid.squares[t.r + 1][t.c] == NULL||grid.terrain[t.r+1][t.c]==FOREST)) return true;

    return 0;
}

bool ifenemy(creature_t &creature, grid_t &grid)
{
    point_t t = creature.location;
    if (!ifempty(creature, grid))
    {
        if (creature.direction == EAST && t.c + 1 < grid.width &&
            grid.squares[t.r][t.c + 1]->species != creature.species &&
            grid.terrain[t.r][t.c + 1] != FOREST)
            return 1;
        if (creature.direction == WEST && t.c - 1 >= 0 && grid.squares[t.r][t.c - 1]->species != creature.species &&
            grid.terrain[t.r][t.c - 1] != FOREST)
            return 1;
        if (creature.direction == NORTH && t.r - 1 >= 0 && grid.squares[t.r - 1][t.c]->species != creature.species &&
            grid.terrain[t.r - 1][t.c] != FOREST)
            return 1;
        if (creature.direction == SOUTH && t.r + 1 < grid.height &&
            grid.squares[t.r + 1][t.c]->species != creature.species &&
            grid.terrain[t.r + 1][t.c] != FOREST)
            return 1;
    }
    return 0;
}

bool ifsame(creature_t &creature, grid_t &grid)
{
    point_t t = creature.location;
    if (!ifempty(creature, grid))
    {
        if (creature.direction == EAST && t.c + 1 < grid.width &&
            grid.squares[t.r][t.c + 1]->species == creature.species &&
            grid.terrain[t.r][t.c + 1] != FOREST)
            return 1;
        if (creature.direction == WEST && t.c - 1 >= 0 && grid.squares[t.r][t.c - 1]->species == creature.species &&
            grid.terrain[t.r][t.c - 1] != FOREST)
            return 1;
        if (creature.direction == NORTH && t.r - 1 >= 0 && grid.squares[t.r - 1][t.c]->species == creature.species &&
            grid.terrain[t.r - 1][t.c] != FOREST)
            return 1;
        if (creature.direction == SOUTH && t.r + 1 < grid.height &&
            grid.squares[t.r + 1][t.c]->species == creature.species &&
            grid.terrain[t.r + 1][t.c] != FOREST)
            return 1;
    }
    return false;
}

bool ifwall(creature_t &creature, grid_t &grid)
{
    point_t t = creature.location;
    if (creature.direction == EAST &&( t.c + 1 == grid.width ||
        t.c + 1 < grid.width && grid.terrain[t.r][t.c + 1] == LAKE && !creature.ability[FLY]))
        return true;
    if (creature.direction == WEST && (t.c == 0 ||
        t.c - 1 >= 0 && grid.terrain[t.r][t.c - 1] == LAKE && !creature.ability[FLY]))
        return true;
    if (creature.direction == NORTH && (t.r == 0 ||
        t.r - 1 >= 0 && grid.terrain[t.r - 1][t.c] == LAKE && !creature.ability[FLY]))
        return true;
    if (creature.direction == SOUTH && (t.r + 1 == grid.height ||
        t.r + 1 < grid.height && grid.terrain[t.r + 1][t.c] == LAKE && !creature.ability[FLY]))
        return true;
    return false;
}

bool initWorld(world_t &world, const string &speciesFile, const string &worldFile)
{
    ifstream speciessummery;
    speciessummery.open(speciesFile.c_str());
    if (!speciessummery)
    {
        cout << "Error: Cannot open file " << speciesFile << "!" << endl;
        throw fileopen();
    }
    string spdir;
    getline(speciessummery, spdir);
    while (speciessummery)
    {
        string spname, sppath;
        getline(speciessummery, spname);
        if (spname == "") { continue; }
        sppath = spdir + "/" + spname;
        ifstream spinfo;
        spinfo.open(sppath.c_str());
        if (!spinfo)
        {
            cout << "Error: Cannot open file " << sppath << "!" << endl;
            throw fileopen();
        }
        species_t temp;
        temp.name = spname;
        int i = 0;
        while (spinfo)
        {
            string line, command;
            unsigned int step;
            istringstream iStream;
            getline(spinfo, line);
            if (line == "") break;
            iStream.str(line);

            if (line.substr(0, 2) == "if" || line.substr(0, 2) == "go")
            {
                iStream >> command >> step;
                iStream.clear();
                temp.program[i].address = step;
                temp.program[i].op = findopcode(command);
                i++;
            } else
            {
                iStream >> command;
                iStream.clear();
                temp.program[i].op = findopcode(command);
                i++;
            }
            iStream.clear();
        }
        spinfo.close();
        if (i > MAXPROGRAM)
        {
            cout << "Error: Too many instructions for species " << temp.name << "!" << endl;
            cout << "Maximal number of instructions is " << MAXPROGRAM << "." << endl;
            throw toomanyprograms();
        }
        temp.programSize = i;
        if (world.numSpecies > MAXSPECIES)
        {
            throw toomanyspecies();
        }
        world.numSpecies++;
        int a = world.numSpecies;
        world.species[a - 1] = temp;
    }
    speciessummery.close();

    ifstream worldsummery;
    worldsummery.open(worldFile.c_str());
    if (!worldsummery)
    {
        cout << "Error: Cannot open file " << worldFile << "!" << endl;
        throw fileopen();
    }
    int height, width;
    worldsummery >> height >> width;
    if (height > MAXHEIGHT) { throw illegalheight(); }
    if (width > MAXWIDTH) { throw illegalwidth(); }
    world.grid.height = height;
    world.grid.width = width;
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            char terrain;
            worldsummery >> terrain;
            if (terrain != 'P' && terrain != 'F' && terrain != 'L' && terrain != 'H')
            {
                cout << "Error: Terrain square (" << terrain << ' ' << h << ' ' << w << ") is invalid!" << endl;
                throw illegalterrain();
            }

            world.grid.terrain[h][w] = findterrain(terrain);
            world.grid.squares[h][w] = NULL;
        }
    }
    int n = 0;

    while (worldsummery)
    {
        string creature, name, direction;
        int row, column;
        getline(worldsummery, creature);
        if (creature == "") continue;
        istringstream ss(creature);
        ss >> name >> direction >> row >> column;

        world.creatures[n].direction = finddir(direction);
        world.creatures[n].hillActive = true;


        world.creatures[n].location.c = column;
        world.creatures[n].location.r = row;
        world.creatures[n].species = getspecies(world, name);
        if (row >= world.grid.height || row < 0 || column >= world.grid.width || column < 0)
        {
            cout << "Error: Creature (" << world.creatures[n].species->name << ' ' << direction << ' ' << row << ' '
                 << column << ") is out of bound!" << endl;
            cout << "The grid size is " << world.grid.height << "-by-" << world.grid.width << '.' << endl;
            throw outsideboundry();
        }
        if (world.grid.squares[row][column] != NULL)
        {
            cout << "Error: Creature (" << world.creatures->species->name << ' ' << direction << ' ' << row << ' '
                 << column << ") overlaps with creature (" << world.grid.squares[row][column]->species->name << ' '
                 << directName[world.grid.squares[row][column]->direction] << ' ' << row << ' ' << column << ")!"
                 << endl;
            throw occupied();
        }
        world.grid.squares[row][column] = &world.creatures[n];
        world.creatures[n].programID = 0;

        world.creatures[n].ability[0] = {false};
        world.creatures[n].ability[1] = {false};
        while (!ss.eof())
        {
            string ability;
            ss >> ability;
            if (ability=="") continue;
            ability_t ab = findability(world.creatures[n], ability);
            if (ability != "") world.creatures[n].ability[ab] = 1;
        }
        if (!world.creatures[n].ability[FLY] && world.grid.terrain[row][column] == LAKE)
        {
            cout << "Error: CCreature (" << name << ' ' << direction << ' ' << row << ' ' << column
                 << ") is in a lake square!" << endl;
            cout << "The creature cannot fly!" << endl;
            throw lakeerror();
        }
        ss.clear();
        n++;
    }
    worldsummery.close();
    if (n > MAXCREATURES)
    {
        throw toomanycreatures();
    }
    world.numCreatures = n;
    cout << "Initial state" << endl;
    printGrid(world.grid);
    return true;
}


bool simulateCreature(creature_t &creature, grid_t &grid, bool verbose)
{



    if (!creature.hillActive && grid.terrain[creature.location.r][creature.location.c] == HILL &&
        !creature.ability[FLY])
    {
        creature.hillActive = 1;
        return 0;
    } else
    {   creature.hillActive=false;
        if (!verbose)
            cout << "Creature" << " (" << creature.species->name << ' ' << directName[creature.direction] << ' '
                 << creature.location.r << ' ' << creature.location.c << ")" << " takes action: ";
        else
            cout << "Creature" << " (" << creature.species->name << ' ' << directName[creature.direction] << ' '
                 << creature.location.r << ' ' << creature.location.c << ")" << " takes action:" << endl;

        while (1)
        {
            if (!verbose)
            {
                if (getInstruction(creature).op == INFECT || getInstruction(creature).op == LEFT ||
                    getInstruction(creature).op == RIGHT || getInstruction(creature).op == HOP)
                {
                    cout << opName[getInstruction(creature).op] << endl;
                }
            } else
            {
                if (getInstruction(creature).op == INFECT || getInstruction(creature).op == LEFT ||
                    getInstruction(creature).op == RIGHT || getInstruction(creature).op == HOP)
                {
                    cout << "Instruction " <<
                         creature.programID +
                         1 << ": "
                         << opName[getInstruction(
                                 creature).op]
                         << endl;
                } else
                {
                    int i, p;
                    for (i = 0; i < creature.species->programSize; i++)
                    {
                        if (creature.species->program[i].op == getInstruction(creature).op)
                        {
                            break;
                        }
                    }
                    p = creature.species->program[i].address;
                    cout << "Instruction " << creature.programID + 1 << ": " << opName[getInstruction(creature).op]
                         << ' '
                         << p << endl;
                }

            }
            if (getInstruction(creature).op == LEFT)
            {
                creature.direction = leftFrom(creature.direction);
                creature.programID++;
                break;
            } else if (getInstruction(creature).op == RIGHT)
            {
                creature.direction = rightFrom(creature.direction);
                creature.programID++;
                break;
            } else if (getInstruction(creature).op == HOP)
            {
                hop(grid, creature);
                creature.programID++;
                break;
            } else if (getInstruction(creature).op == INFECT)
            {
                infect(creature, grid);
                creature.programID++;
                break;
            } else if (getInstruction(creature).op == IFEMPTY)
            {
                if (ifempty(creature, grid))
                {
                    int i;
                    for (i = 0; i < creature.species->programSize; i++)
                    {
                        if (creature.species->program[i].op == IFEMPTY)
                        {
                            break;
                        }
                    }
                    creature.programID = creature.species->program[i].address - 1;
                } else creature.programID += 1;
            } else if (getInstruction(creature).op == IFENEMY)
            {
                if (ifenemy(creature, grid))
                {
                    int i;
                    for (i = 0; i < creature.species->programSize; i++)
                    {
                        if (creature.species->program[i].op == IFENEMY)
                        {
                            break;
                        }
                    }
                    creature.programID = creature.species->program[i].address - 1;
                } else creature.programID += 1;
            } else if (getInstruction(creature).op == IFSAME)
            {
                if (ifsame(creature, grid))
                {
                    int i;
                    for (i = 0; i < creature.species->programSize; i++)
                    {
                        if (creature.species->program[i].op == IFSAME)
                        {
                            break;
                        }
                    }
                    creature.programID = creature.species->program[i].address - 1;
                } else creature.programID += 1;
            } else if (getInstruction(creature).op == IFWALL)
            {
                if (ifwall(creature, grid))
                {
                    int i;
                    for (i = 0; i < creature.species->programSize; i++)
                    {
                        if (creature.species->program[i].op == IFWALL)
                        {
                            break;
                        }
                    }
                    creature.programID = creature.species->program[i].address - 1;
                } else creature.programID += 1;
            } else if (getInstruction(creature).op == GO)
            {
                int i;
                for (i = 0; i < creature.species->programSize; i++)
                {
                    if (creature.species->program[i].op == GO)
                    {
                        break;
                    }
                }
                creature.programID = creature.species->program[i].address - 1;
            }
        }
        if (verbose)printGrid(grid);
        return 1;
    }
}

void Simulate(world_t &world, int round, string v)
{
    if (round < 0) throw negativeround();
    bool dout;
    if (v == "v" || v == "verbose") dout = 1;
    else dout = 0;
    int i;
    for (int p = 0; p < round; p++)
    {
        cout << "Round " << p + 1 << endl;
        int k=0;
        for (i = 0; i < world.numCreatures; i++)
        {

           k=simulateCreature(world.creatures[i], world.grid, dout);

        }
        if (!dout&&k) printGrid(world.grid);
    }
}