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
    bool annoy(unsigned int amt){
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
    
    virtual bool hurtsFrackMan() const{
        return true;
    }
    
    // Can this actor need to be picked up to finish the level?
    virtual bool needsToBePickedUpToFinishLevel() const {
        return false;
    }
    
    bool moveForward(Actor * a, Direction dir);
    
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
    // Does this actor hunt the FrackMan?
    
    
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
        m_sonar = 1;
        m_water = 5;
    }
    virtual ~FrackMan(){}
    virtual void doSomething();
    virtual bool canDigThroughDirt() const {
        return true;
    }
    virtual void addGold() {
        m_gold++;
    }
    void addSonar(){
        m_sonar++;
    }
    int getSonar(){
        return m_sonar;
    }
    void takeSonar(){
        m_sonar--;
    }
    void useSonar();
    void addWater(){
        m_water+=5;
    }
    int getWater(){
        return m_water;
    }
    void setWater(int x){
        m_water = x;
    }
    int getGold(){
        return m_gold;
    }
    void setGold(int g){
        m_gold = g;
    }
private:
    int m_gold;
    int m_sonar;
    int m_water;
};

class Protester : public Man
{
    //ORIGINALLY HAD UNSIGNED INT SCORE
public:
    Protester(StudentWorld* world, int imageID, int startX, int startY,
                         unsigned int hitPoints) : Man(world, imageID, startX, startY, left, hitPoints) {
        m_mustLeave = false;
    }
    virtual void doSomething() = 0;
//    virtual bool annoy(unsigned int amount);
    virtual void addGold() = 0;
    virtual bool huntsFrackMan() const {
        return true;
    }
    
    // Set state to having gien up protest
    void setMustLeaveOilField(){
        m_mustLeave = true;
    }
    
    void leave();
    
    void setSquaresToMove(int a){
        numSquaresToMoveInCurrentDirection = a;
    }
    
    int numSquaresToMove(){
        return numSquaresToMoveInCurrentDirection;
    }
    
    bool timeToMove();
    bool mustLeave(){
        return m_mustLeave;
    }
    
    // Set number of ticks until next move
    void setTicksToNextMove(int a);
private:
    int numSquaresToMoveInCurrentDirection;
    bool m_mustLeave;
    int ticksToWait = 0;
};


class RegularProtester : public Protester
{
public:
    RegularProtester(StudentWorld* world, int startX, int startY): Protester(world, IID_PROTESTER, startX, startY, 5){
        
    }
    virtual void doSomething();
    virtual void addGold();
};

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


class Squirt : public Actor
{
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction startDir): Actor(world, IID_WATER_SPURT, startX, startY, startDir, 1, 1){
        m_travelDistance = 4;
    }
    virtual void doSomething();
    virtual bool hurtsFrackMan() const{
        return false;
    }
    int travelDistance(){
        return m_travelDistance;
    }
    void setTravelDistance(int x){
        m_travelDistance = x;
    }
private:
    int m_travelDistance;
};

class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* world, int imageID, int startX, int startY, 
                     int soundToPlay, bool activateOnPlayer,
                     bool activateOnProtester, bool initiallyActive, int score, int ticksToLive=0): Actor(world, imageID, startX, startY, right, 1, 2){
        m_sound = soundToPlay;
        m_activeOnPlayer = activateOnPlayer;
        m_activeOnProtestor = activateOnProtester;
        m_active = initiallyActive;
        m_ticksToLive = ticksToLive;
        m_score = score;
//        GraphObject::setVisible(false);
    }
    //for getting picked up
    int getScore(){
        return m_score;
    }
    int getSound(){
        return m_sound;
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
    void setTicksToLive(int x);
    int ticksToLive(){
        return m_ticksToLive;
    }
private:
    int m_score;
    int m_sound;
    int m_ticksToLive;
    bool m_activeOnPlayer;
    bool m_activeOnProtestor;
    bool m_active;
};

class OilBarrel : public ActivatingObject
{
public:
    OilBarrel(StudentWorld* world, int startX, int startY): ActivatingObject(world, IID_BARREL,startX, startY,  SOUND_FOUND_OIL, true, false, true, 1000) {
        
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
    GoldNugget(StudentWorld* world, int startX, int startY, bool temporary): ActivatingObject(world, IID_GOLD, startX, startY,  SOUND_GOT_GOODIE, !temporary, temporary, !temporary, 10){
    //if it is temporary, it should not start out visible
        
    }
    virtual ~GoldNugget(){}
    virtual void doSomething();
};


class SonarKit : public ActivatingObject
{
public:
    SonarKit(StudentWorld* world, int startX, int startY, int ticksToLive): ActivatingObject(world, IID_SONAR, startX, startY,  SOUND_GOT_GOODIE, true, false, false, 75, ticksToLive) {
        GraphObject::setVisible(true);
    }
    virtual ~SonarKit(){}
    virtual void doSomething();
};

class WaterPool : public ActivatingObject
{
public:
    WaterPool(StudentWorld* world, int startX, int startY, int ticksToLive): ActivatingObject(world, IID_WATER_POOL, startX, startY,  SOUND_GOT_GOODIE, true, false, false, 100, ticksToLive) {
        GraphObject::setVisible(true);
    }
    virtual ~WaterPool(){}
    virtual void doSomething();
};



#endif // ACTOR_H_
