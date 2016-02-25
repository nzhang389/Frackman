#include "StudentWorld.h"
#include <string>

using namespace std;


int StudentWorld::randInt(int min, int max)
{
    if (max < min)
        std::swap(max, min);
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}
int StudentWorld::min(int x, int y){
    if(x<y)
        return x;
    return y;
}
int StudentWorld::max(int x, int y){
    if(x>y)
        return x;
    return y;
}

double distance(int x, int y){
    int a = x * x + y * y;
    return sqrt( (double) a);
}

StudentWorld::StudentWorld(std::string assetDir)
: GameWorld(assetDir)
{
    
}

void StudentWorld::cleanUp() {
    for(int x = 0; x<64; x++){
        for(int y = 0; y<64;y++){
            if(m_dirt[x][y] != nullptr)
                delete m_dirt[x][y];
        }
    }
    if (m_frackman != nullptr)
        delete m_frackman;
    vector<Actor *>::const_iterator it = m_actors.begin();
    while(it!=m_actors.end()){
        delete (*it);
        it = m_actors.erase(it);
    }
}

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

void StudentWorld::addActor(Actor *a){
    m_actors.push_back(a);
}

int StudentWorld::move(){
    m_frackman->doSomething();
    vector<Actor *>::const_iterator it = m_actors.begin();
    while(it!=m_actors.end()){
        (*it)->doSomething();
        it++;
    }
    if(m_frackman->getHitPoints() <= 0){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    it = m_actors.begin();
    while(it!=m_actors.end()){
        if(!(*it)->isAlive()){
            delete (*it);
            it = m_actors.erase(it);
        }
        else {
            it++;
        }
    }
    
    
    return GWSTATUS_CONTINUE_GAME;
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp


void StudentWorld::clearDirt(int x, int y){
    for(int a = 0; a < 4; a++){
        for(int b = 0; b<4; b++){
            if(hasDirt(x+a, y+b))
               removeDirt(x+a,y+b);
        }
    }
}

void StudentWorld::removeDirt(int x, int y) {
    delete m_dirt[x][y];
    m_dirt[x][y] = nullptr;
}



int StudentWorld::init(){
    m_dirt = new Dirt**[VIEW_WIDTH];
	for(int x = 0; x<VIEW_WIDTH; x++){
        m_dirt[x] = new Dirt*[VIEW_HEIGHT];
        for(int y = 0; y<VIEW_HEIGHT; y++){
            m_dirt[x][y] = nullptr;
            if(x>=30 && x<=33){
                if(y>=0 && y <= 3){
                    m_dirt[x][y] = new Dirt(this,x,y);
                }
            }
            else {
                if(y<60)
                    m_dirt[x][y] = new Dirt(this,x,y);
            }
        }
    }
    m_frackman = new FrackMan(this);
    
    for(int x = 0; x<60; x++){
        for(int y = 0; y<56;y++){
            if(x>=27 && x<=33){
                if(y>=0 && y<=3)
                    actorLocations[x][y] = 0;
                else
                    actorLocations[x][y]=1;
            }
            else{
                actorLocations[x][y] = 0;
            }
        }
    }
    //add Boulders
    int b = min(getLevel()/2 + 2, 6);
    b = 9;
    int x, y;
    for(int n = 0; n<b; n++) {
        addActivatingObject(x,y, true);
        m_actors.push_back(new Boulder(this, x, y));
    }
    
    int g = max((5-getLevel()) / 2, 2);
    for (int n = 0; n<g;n++){
        addActivatingObject(x, y, false);
        m_actors.push_back(new GoldNugget(this, x, y, false));
    }
//    
    int l = min(2 + getLevel(), 20);
    for (int n = 0; n<l; n++){
        addActivatingObject(x, y, false);
        m_actors.push_back(new OilBarrel(this, x, y));
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::giveFrackManGold(){
    m_frackman->addGold();
}


void StudentWorld::addActivatingObject(int &x, int &y, bool isBoulder) {
    //MAYBE ADD SOMETHING LIKE IF THERE ARE NO 0'S RETURN FALSE
    bool validLocation = false;
    int a = 0 ;
    int b = 0;
    while(!validLocation){
        a = randInt(0, 60);
        if(isBoulder)
            b = randInt(20, 56);
        else
            b = randInt(0, 56);
        if(actorLocations[a][b] == 0){
            x = a;
            y = b;
            validLocation = true;
        }
    }
    for(int r = 0; r < 7; r++){
        for(int u = 0; u<= sqrt(36 - r*r); u++){
            changeActorLocationValid(a+r, b+u);
            changeActorLocationValid(a-r, b+u);
            changeActorLocationValid(a+r, b-u);
            changeActorLocationValid(a-r, b-u);
        }
    }
    if(isBoulder){
        clearDirt(a, b);
    }
//    actorLocations[a][b]=2;
}

void StudentWorld::changeActorLocationValid(int x, int y){
    if(x <= 60 && x>=0 && y>=0 && y<=56){
        actorLocations[x][y] = 1;
    }
}

int StudentWorld::annoyAllNearbyActors(Actor* annoyer, int points, int radius){
    vector<Actor *>::const_iterator it = m_actors.begin();
    int x = annoyer->getX();
    int y = annoyer -> getY();
    int numAnnoyed = 0;
    for(;it<m_actors.end();it++){
        if((*it)!=annoyer && distance((*it)->getX()-x, (*it)->getY()-y) <= radius){
            (*it)->annoy(points);
            numAnnoyed++;
        }
    }
    if(distance(m_frackman->getX()-x, m_frackman->getY()-y) <= 3){
        m_frackman->annoy(points);
        numAnnoyed++;
    }
    return numAnnoyed;
}

Actor* StudentWorld::findNearbyFrackMan(Actor* a, int radius) const {
    if(distance(a->getX() - m_frackman->getX(),a->getY() - m_frackman->getY()) <= radius){
        return m_frackman;
    }
    return nullptr;
}

bool StudentWorld::canActorMoveTo(Actor* a, int x, int y) const{
    //MAYBE CHECK FOR
    vector<Actor *>::const_iterator it = m_actors.begin();
    for(;it<m_actors.end();it++){
        if(!(*it)->canActorsPassThroughMe() && a != *it){
            if(distance((*it)->getX()-x, (*it)->getY()-y) <= 3)
                return false;
        }
    }
    return true;
}