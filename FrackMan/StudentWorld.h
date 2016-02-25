#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <vector>
#include <random>
#include "Actor.h"



class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld(){
    }
    int randInt(int min, int max);
    int min(int x, int y);
    int max(int x, int y);
   
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	
    // Add an actor to the world.
    void addActor(Actor* a);
    
    // Clear a 4x4 region of dirt.
    void clearDirt(int x, int y);
    
    // Can actor move to x,y?
    bool canActorMoveTo(Actor* a, int x, int y) const;
    
    // Annoy all other actors within radius of annoyer, returning the
    // number of actors annoyed.
    int annoyAllNearbyActors(Actor* annoyer, int points, int radius);
    
    // Reveal all objects within radius of x,y.
    void revealAllNearbyObjects(int x, int y, int radius);
    
    // If the FrackMan is within radius of a, return a pointer to the
    // FrackMan, otherwise null.
    Actor* findNearbyFrackMan(Actor* a, int radius) const;
    
    // If at least one actor that can pick things up is within radius of a,
    // return a pointer to one of them, otherwise null.
    Actor* findNearbyPickerUpper(Actor* a, int radius) const;
    
    // Annoy the FrackMan.
    void annoyFrackMan();
    
    // Give FrackMan some sonar charges.
    void giveFrackManSonar();
    
    // Give FrackMan some water.
    void giveFrackManWater();
    
    void giveFrackManGold();
    
    // Is the Actor a facing toward the FrackMan?
    bool facingTowardFrackMan(Actor* a) const;
    
    // If the Actor a has a clear line of sight to the FrackMan, return
    // the direction to the FrackMan, otherwise GraphObject::none.
    GraphObject::Direction lineOfSightToFrackMan(Actor* a) const;
    
    // Return whether the Actor a is within radius of FrackMan.
    bool isNearFrackMan(Actor* a, int radius) const;
    
    // Determine the direction of the first move a quitting protester
    // makes to leave the oil field.
    GraphObject::Direction determineFirstMoveToExit(int x, int y);
    
    // Determine the direction of the first move a hardcore protester
    // makes to approach the FrackMan.
    
    GraphObject::Direction determineFirstMoveToFrackMan(int x, int y);
    
    void removeDirt(int x, int y);

    
    bool hasDirt(int x, int y){
        if (x >= 0 && x < VIEW_WIDTH && y >= 0 && y < VIEW_HEIGHT && m_dirt[x][y] != nullptr)
            return true;
        return false;
    }
private:
    void addActivatingObject(int &x, int &y, bool isBoulder);
    void changeActorLocationValid(int x, int y);
    Dirt *** m_dirt;
    int  actorLocations [60][56];
    FrackMan * m_frackman;
    std::vector<Actor *> m_actors;
};


#endif // STUDENTWORLD_H_
