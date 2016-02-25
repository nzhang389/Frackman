#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <random>

class StudentWorld;



class Actor: public GraphObject {
public:
    Actor(StudentWorld* sw, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0): GraphObject(imageID, startX, startY, dir, size, depth){
        m_world = sw;
        GraphObject::setVisible(true);
        m_isAlive = true;
    }
    virtual ~Actor(){
    }
    // Action to perform each tick.
    virtual void doSomething() = 0;
    // Is this actor alive?
    bool isAlive() const{
        return m_isAlive;
    }
    
    // Mark this actor as dead.
    void setDead(){
        m_isAlive = false;
    }
    
    // Annoy this actor.
    // return true if it was successfully annoyed
    virtual bool annoy(unsigned int amt){
        return false;
    }
    
    // Can other actors pass through this actor?
    virtual bool canActorsPassThroughMe() const {
        return true;
    }
    
    // Can this actor dig through dirt?
    virtual bool canDigThroughDirt() const {
        return false;
    }
    
    // Can this actor pick items up?
    virtual bool canPickThingsUp() const {
        return false;
    }
    
    // Does this actor hunt the FrackMan?
    virtual bool huntsFrackMan() const {
        return false;
    }
    
    // Can this actor need to be picked up to finish the level?
    virtual bool needsToBePickedUpToFinishLevel() const {
        return false;
    }
    
    // Move this actor to x,y if possible, and return true; otherwise,
    // return false without moving.
    bool moveToIfPossible(int x, int y);
    StudentWorld * getWorld(){
        return m_world;
    }
private:
    StudentWorld * m_world;
    bool m_isAlive;
};

class Dirt: public Actor {
public:
    Dirt(StudentWorld* sw, int startX, int startY): Actor(sw, IID_DIRT, startX, startY, right, 0.25, 3) {
    }
    virtual ~Dirt(){
        
    }
   virtual void doSomething(){};
private:
    
};

class Boulder : public Actor
{
    //0 = stable;
    //1 = waiting;
    //2 = falling;
    
public:
    Boulder(StudentWorld* world, int startX, int startY): Actor(world, IID_BOULDER, startX, startY, down, 1, 1) {
        m_state = 0;
        m_wait = 0;
    }
    virtual ~Boulder(){}
    virtual bool canActorsPassThroughMe() const{
        return false;
    }
    virtual void doSomething();
private:
    bool hasDirtUnder();
    int m_state;
    int m_wait;
};


class Man: public Actor{
public:
    Man(StudentWorld* sw, int imageID, int startX, int startY, Direction dir, int myHealth): Actor(sw, imageID, startX, startY, dir) {
        m_health = myHealth;
    }
    virtual ~Man(){}
    
    // Pick up a gold nugget.
    virtual void addGold() = 0;
    
    // How many hit points does this actor have left?
    //THIS WAS UNSIGNED PREVIOUSLY
    int getHitPoints() const {
        return m_health;
    }
    virtual bool canActorsPassThroughMe() const{
        return true;
    }
    
    //this is probably taking damage
    virtual bool annoy(unsigned int amount) {
        m_health -= amount;
        return true;
    }
    virtual bool canPickThingsUp() const {
        return true;
    }
    
    virtual void doSomething() = 0;
private:
    int m_health;
};

class FrackMan: public Man{
public:
    FrackMan(StudentWorld* sw): Man(sw, IID_PLAYER, 30, 60, right, 10){
        m_gold = 0;
    }
    virtual ~FrackMan(){}
    virtual void doSomething();
    virtual bool canDigThroughDirt() const {
        return true;
    }
    virtual void addGold() {
        m_gold++;
    }
private:
    int m_gold;
};

//class Protester : public Man
//{
//    //ORIGINALLY HAD UNSIGNED INT SCORE
//public:
//    Protester(StudentWorld* world, int imageID, int startX, int startY,
//              unsigned int hitPoints); 
//    virtual void doSomething() = 0;
////    virtual bool annoy(unsigned int amount);
//    virtual void addGold();
//    virtual bool huntsFrackMan() const {
//        return true;
//    }
//    
//    // Set state to having gien up protest
//    void setMustLeaveOilField(){
//        m_mustLeave = true;
//    }
//    
//    bool mustLeave(){
//        return m_mustLeave;
//    }
//    
//    // Set number of ticks until next move
//    void setTicksToNextMove();
//private:
//    int numSquaresToMoveInCurrentDirection;
//    bool m_mustLeave;
//    int ticksToWait = 0;
//};
//
//
//class RegularProtester : public Protester
//{
//public:
//    RegularProtester(StudentWorld* world, int startX, int startY): Protester(world, IID_PROTESTER, startX, startY, 5){
//        
//    }
//    virtual void doSomething();
//    virtual void addGold();
//};
//
//class HardcoreProtester : public Protester
//{
//public:
//    HardcoreProtester(StudentWorld* world, int startX, int startY): Protester(world, IID_HARD_CORE_PROTESTER, startX, startY, 20){
//        
//    }
//    virtual void doSomething();
//    virtual void addGold();
//};


//
//class Squirt : public Actor
//{
//public:
//    Squirt(StudentWorld* world, int startX, int startY, Direction startDir);
//    virtual void doSomething();
//};
//
class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* world, int imageID, int startX, int startY, 
                     int soundToPlay, bool activateOnPlayer,
                     bool activateOnProtester, bool initiallyActive): Actor(world, imageID, startX, startY, right, 1, 2){
        m_sound = soundToPlay;
        m_activeOnPlayer = activateOnPlayer;
        m_activeOnProtestor = activateOnProtester;
        m_active = initiallyActive;
//        GraphObject::setVisible(false);
    }
    virtual ~ActivatingObject(){}
    virtual void doSomething() = 0;
    bool getSeen();
    bool getPickedUp(ActivatingObject *);
    bool activateOnPlayer(){
        return m_activeOnPlayer;
    }
    bool activateOnProtestor(){
        return m_activeOnProtestor;
    }
    // Set number of ticks until this object dies
    void setTicksToLive();
private:
    int m_sound;
    bool m_activeOnPlayer;
    bool m_activeOnProtestor;
    bool m_active;
};

class OilBarrel : public ActivatingObject
{
public:
    OilBarrel(StudentWorld* world, int startX, int startY): ActivatingObject(world, IID_BARREL,startX, startY,  SOUND_FOUND_OIL, true, false, true) {
        
    }
    virtual ~OilBarrel(){}
    virtual void doSomething();
    virtual bool needsToBePickedUpToFinishLevel() const {
        return true;
    }
};

class GoldNugget : public ActivatingObject
{
public:
    GoldNugget(StudentWorld* world, int startX, int startY, bool temporary): ActivatingObject(world, IID_GOLD, startX, startY,  SOUND_GOT_GOODIE, !temporary, temporary, !temporary){
    //if it is temporary, it should not start out visible
        
    }
    virtual ~GoldNugget(){}
    virtual void doSomething();
};


//class SonarKit : public ActivatingObject
//{
//public:
//    SonarKit(StudentWorld* world, int startX, int startY): ActivatingObject(world, startX, startY, IID_SONAR, SOUND_GOT_GOODIE, true, false, false) {
//        
//    }
//    virtual void doSomething();
//};
//
//class WaterPool : public ActivatingObject
//{
//public:
//    WaterPool(StudentWorld* world, int startX, int startY): ActivatingObject(world, startX, startY, IID_WATER_POOL, SOUND_GOT_GOODIE, true, false, false) {
//        
//    }
//    virtual void doSomething();
//};
//
//

#endif // ACTOR_H_
