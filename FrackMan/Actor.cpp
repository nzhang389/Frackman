#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <random>
using namespace std;



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

void FrackMan::doSomething(){
    int input;
    //MAKE SURE IT DOESN'T LEAVE
    if(getWorld()->getKey(input) == true){
        switch (input) {
            case KEY_PRESS_LEFT:
                if(getDirection() == left && getX() > 0)
                    moveToIfPossible(getX() - 1, getY());
                else
                    setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                 if(getDirection() == right && getX() < VIEW_WIDTH - SPRITE_WIDTH)
                    moveToIfPossible(getX() + 1, getY());
                else
                    setDirection(right);
                break;
            case KEY_PRESS_DOWN:
                if(getDirection() == down && getY() > 0)
                    moveToIfPossible(getX(), getY() - 1);
                else
                    setDirection(down);
                break;
            case KEY_PRESS_UP:
                if(getDirection() == up && getY() < VIEW_HEIGHT - SPRITE_HEIGHT)
                    moveToIfPossible(getX(), getY() + 1);
                else
                    setDirection(up);
                break;
            default:
                break;
        }
    }
    for(int x = 0; x<SPRITE_WIDTH; x++){
        for(int y = 0; y<SPRITE_HEIGHT; y++){
            if(getWorld()->hasDirt(getX() + x, getY() + y)){
                getWorld()->removeDirt(getX() + x, getY() + y);
            }
        }
    }
    
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
                //PLAY SOUND
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
        setDead();
        //PLAY SOUND AFFECT
        //INCREASE SCORE
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
        //PLAY SOUND AFFECT
        //INCREASE SCORE
    }
    
}
//
//
//Protester::Protester(StudentWorld* world, int imageID, int startX, int startY,
//          unsigned int hitPoints) : Man(world, imageID, startX, startY, left, hitPoints) {
//    m_mustLeave = false;
//    numSquaresToMoveInCurrentDirection = getWorld()->randInt(8, 60);
//}
//
//void Protester::doSomething(){
//    if(!isAlive())
//        return;
//    if(ticksToWait != 0){
//        ticksToWait--;
//        return;
//    }
////    if(must
//    
//    ticksToWait = getWorld()->max(0, (3 - getWorld()->getLevel() / 4));
//}
//
//
//
//
//
//





