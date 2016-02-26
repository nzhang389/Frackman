#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <random>
using namespace std;


bool Actor::moveForward(Actor * a, Direction dir){
    switch (dir) {
        case GraphObject::up:
            return moveToIfPossible(a->getX(), a->getY()+1);
        case GraphObject::left:
            return moveToIfPossible(a->getX() - 1, a->getY());
        case GraphObject::right:
            return moveToIfPossible(a->getX() + 1, a->getY());
        case GraphObject::down:
            return moveToIfPossible(a->getX(), a->getY()-1);
        default:
            break;
    }
    return false;
}

bool Actor::moveToIfPossible(int x, int y){
    if(x>=0 && x<=VIEW_WIDTH - SPRITE_WIDTH && y>=0 && y<=VIEW_HEIGHT - SPRITE_HEIGHT){
        bool a = getWorld()->canActorMoveTo(this, x, y);
        if(a)
            moveTo(x, y);
        return a;
    }
    else{
        return false;
    }
}

bool Boulder::hasDirtUnder(){
    if(getWorld()->hasDirt(getX(), getY()-1) || getWorld()->hasDirt(getX() + 1, getY()-1) || getWorld()->hasDirt(getX() + 2, getY()-1) || getWorld()->hasDirt(getX()+3, getY()-1))
        return true;
    return false;
}

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

void FrackMan::useSonar(){
    if(getSonar() == 0)
        return;
    takeSonar();
    getWorld()->revealAllNearbyObjects(getX(), getY(), 12);
}

void FrackMan::doSomething(){
    int input;
    if(getWorld()->getKey(input) == true){
        switch (input) {
            case KEY_PRESS_ESCAPE:
                annoy(100);
                return;
                break;
            case KEY_PRESS_SPACE:
                if(getWater() != 0){
                    setWater(getWater()-1);
                    getWorld()->shootSquirt(this);
                }
                break;
            case KEY_PRESS_TAB:
                if(getGold() > 0){
                    setGold(getGold()-1);
                    getWorld()->frackmanDropsGold(this);
                }
                break;
            case 'z':
            case 'Z':
                useSonar();
                break;
            case KEY_PRESS_LEFT:
                if(getDirection() == left && getX() > 0)
                    moveToIfPossible(getX() - 1, getY());
                else
                    setDirection(left);
                moveTo(getX(), getY());
                break;
            case KEY_PRESS_RIGHT:
                 if(getDirection() == right && getX() < VIEW_WIDTH - SPRITE_WIDTH)
                    moveToIfPossible(getX() + 1, getY());
                else
                    setDirection(right);
                moveTo(getX(), getY());
                break;
            case KEY_PRESS_DOWN:
                if(getDirection() == down && getY() > 0)
                    moveToIfPossible(getX(), getY() - 1);
                else
                    setDirection(down);
                moveTo(getX(), getY());
                break;
            case KEY_PRESS_UP:
                if(getDirection() == up && getY() < VIEW_HEIGHT - SPRITE_HEIGHT)
                    moveToIfPossible(getX(), getY() + 1);
                else
                    setDirection(up);
                moveTo(getX(), getY());
                break;
            default:
                break;
        }
    }
    int didRemoveDirt = 0;
    for(int x = 0; x<SPRITE_WIDTH; x++){
        for(int y = 0; y<SPRITE_HEIGHT; y++){
            if(getWorld()->hasDirt(getX() + x, getY() + y)){
                didRemoveDirt++;
                getWorld()->removeDirt(getX() + x, getY() + y);
            }
        }
    }
    if(didRemoveDirt)
        getWorld()->playSound(SOUND_DIG);
    
//    getX()
}

bool ActivatingObject::getSeen(){
    if(!GraphObject::isVisible() && getWorld()->findNearbyFrackMan(this, 4) != nullptr){
        GraphObject::setVisible(true);
        return true;
    }
    return false;
}

bool ActivatingObject::getPickedUp(ActivatingObject * a){
    if(getWorld()->findNearbyFrackMan(this, 3) != nullptr && a->activateOnPlayer()){
        return true;
    }
    return false;
}

void ActivatingObject::setTicksToLive(int x){
    m_ticksToLive = x;
}

void Boulder::doSomething(){
    if(!isAlive()){
        return;
    }
    switch (m_state) {
        case 0:
            if(!hasDirtUnder())
                m_state=1;
            break;
        case 1:
            m_wait++;
            if(m_wait>=30){
                getWorld()->playSound(SOUND_FALLING_ROCK);
                m_state = 2;
            }
            break;
        case 2:
            if(!hasDirtUnder() && moveToIfPossible(getX(), getY() - 1)){
                if(getWorld()->annoyAllNearbyActors(this, 100, 3)!=0)
                    cerr << "hi";
            }
            else {
                setDead();
            }
        default:
            break;
    }
}

void OilBarrel::doSomething(){
    if(!isAlive()){
        return;
    }
    if(getSeen()){
        GraphObject::setVisible(true);
        return;
    }
    if(getPickedUp(this)){
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(getScore());
        getWorld()->foundOil();
        setDead();
    }
}

void GoldNugget::doSomething(){
    if(!isAlive()){
        return;
    }
    if(getSeen()){
        GraphObject::setVisible(true);
        return;
    }
    if(getPickedUp(this)){
        setDead();
        getWorld()->giveFrackManGold();
        getWorld()->playSound(getSound());
        getWorld()->increaseScore(getScore());
    }
    
}

void SonarKit::doSomething(){
    if(!isAlive()){
        return;
    }
    if(getPickedUp(this)){
        setDead();
        getWorld()->giveFrackManSonar();
        getWorld()->playSound(getSound());
        getWorld()->increaseScore(getScore());
    }
    if(ticksToLive() == 0){
        setDead();
        return;
    }
    setTicksToLive(ticksToLive()-1);
}


void WaterPool::doSomething(){
    if(!isAlive()){
        return;
    }
    if(getPickedUp(this)){
        setDead();
        getWorld()->giveFrackManWater();
        getWorld()->playSound(getSound());
        getWorld()->increaseScore(getScore());
    }
    if(ticksToLive() == 0){
        setDead();
        return;
    }
    setTicksToLive(ticksToLive()-1);
}

void Squirt::doSomething(){
    getWorld()->annoyAllNearbyActors(this, 2, 3);
    if(travelDistance() == 0){
        setDead();
        return;
    }
    bool didMove = moveForward(this, getDirection());
    if(!didMove){
        setDead();
        return;
    }
    else{
        setTravelDistance(travelDistance()-1);
    }
}


void RegularProtester::addGold(){
    //PLAY SOUND i give up
    //INCREASE SCORE by 25
    setMustLeaveOilField();
}

//void HardcoreProtester::addGold(){
//    //play sound im rich
//    //incrase score by 50
//    setTicksToNextMove(getWorld()->max(50, 100 - getWorld()->getLevel()));
//}

void Protester::setTicksToNextMove(int a){
    ticksToWait = a;
}

bool Protester::timeToMove(){
    if(ticksToWait == 0){
        ticksToWait = getWorld()->max(0, (3 - getWorld()->getLevel() / 4));
        return true;
    }
    ticksToWait--;
    return false;
}

void Protester::leave(){
    getWorld()->protesterLeave(this);
}

void RegularProtester::doSomething(){
    if(!isAlive())
        return;
    if(mustLeave()){
        leave();
        return;
    }
    if(!timeToMove()){
        return;
    }
    if(getWorld()->findNearbyFrackMan(this, 4) != nullptr && getWorld()->facingTowardFrackMan(this)){
        //SOUND
        getWorld()->annoyFrackMan();
        setTicksToNextMove(15);
        return;
    }
    Direction dir =getWorld()->lineOfSightToFrackMan(this);
    if(dir != GraphObject::none){
        if(getWorld()->findNearbyFrackMan(this, 4) == nullptr){
            //CHECK IF HES ABLE TO WALK TOWARDS FRACKMAN
            setDirection(dir);
            moveForward(this, dir);
            setSquaresToMove(0);
            
        }
    }
    setSquaresToMove(numSquaresToMove()-1);
//    if(numSquaresToMove() <= 0)
}


//
//
//
//





