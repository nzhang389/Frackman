#include "StudentWorld.h"
#include <string>
#include <queue>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

const int GOOD = -2;
const int BAD = -1;
const int BOULDER = -3;

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


class Coord
{
public:
    Coord(int rr, int cc, int hh) : m_r(rr), m_c(cc), m_h(hh){}
    int r() const { return m_r; }
    int c() const { return m_c; }
    int h() const { return m_h; }
private:
    int m_r;
    int m_c;
    int m_h;
};


StudentWorld::StudentWorld(std::string assetDir)
: GameWorld(assetDir)
{
}
int StudentWorld::init(){
    m_dirt = new Dirt**[VIEW_WIDTH];
    for(int x = 0; x<VIEW_WIDTH; x++){
        m_dirt[x] = new Dirt*[VIEW_HEIGHT];
        for(int y = 0; y<VIEW_HEIGHT; y++){
            m_dirt[x][y] = nullptr;
            heatMap[x][y] = GOOD;
            if(x>=30 && x<=33){
                if(y>=0 && y <= 3){
                    m_dirt[x][y] = new Dirt(this,x,y);
                    heatMap[x][y] = BAD;
                }
            }
            else {
                if(y<60){
                    m_dirt[x][y] = new Dirt(this,x,y);
                    heatMap[x][y] = BAD;
                }
            }
        }
    }
    solveHeatMap();
    m_frackman = new FrackMan(this);
    
    
    //add Boulders
    int b = min(getLevel()/2 + 2, 6);
//    b = 6;
    for(int n = 0; n<b; n++) {
        addActivatingObject('b');
    }
    
    int g = max((5-getLevel()) / 2, 2);
//    g = 2;
    for (int n = 0; n<g;n++){
        addActivatingObject('g');
    }
    
    int l = min(2 + getLevel(), 20);
//    l = 20;
    oilLeft = l;
    for (int n = 0; n<l; n++){
        addActivatingObject('l');
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move(){
    setDisplayText();
    solveHeatMap();
    m_frackman->doSomething();
    vector<Actor *>::const_iterator it = m_actors.begin();
    while(it!=m_actors.end()){
        (*it)->doSomething();
        it++;
    }
    
    int g = getLevel() * 25 + 300;
//    g = 10;
    if(1 == randInt(1, g)){
        int ticksToLast = max(100, 300 - getLevel());
        if(1 == randInt(1, 5)){
            m_actors.push_back(new SonarKit(this, 0,60, ticksToLast));
        }
        else{
            //find location for waterpool
           
            vector<Coord> myVector;
            
            int counter = 0;
            for(int x = 0; x <= 60; x++){
                for(int y = 0; y<=60;y++){
                    if(awayFromDirt(x, y)){
                        myVector.push_back(Coord(x,y, 0));
                        counter++;
                    }
                }
            }
            int indexOfNewWater = randInt(0, counter);
            
            m_actors.push_back(new WaterPool(this, myVector[indexOfNewWater].r(), myVector[indexOfNewWater].c(), ticksToLast));
        }
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
    
    if(m_frackman->getHitPoints() <= 0){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    if(oilLeft == 0){
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
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

string formatter(int score, int level, int lives, int health, int squirts, int gold, int sonar, int oilLeft){
    ostringstream oss;
    
    oss.fill('0');
    oss << "Scr: " << setw(6) << score << "  ";
    oss.fill(' ');
    oss << "Lvl: " << setw(2) << level << "  ";
    oss << "Lives: " << lives << "  ";
    oss << "Hlth: " << setw(3) << health * 10;
    oss << "%" << "  ";
    oss << "Wtr: " << setw(2) << squirts << "  ";
    oss << "Gld: " << setw(2) << gold << "  ";
    oss << "Sonar: " << setw(2) << sonar << "  ";
    oss << "Oil Left: " << setw(2) << oilLeft << "  ";
    return oss.str();
}

void StudentWorld::setDisplayText(){
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_frackman->getHitPoints();
    int squirts = m_frackman->getWater();
    int gold = m_frackman->getGold();
    int sonar = m_frackman->getSonar();
    // Next, create a string from your statistics, of the form:
    // Scr: 321000 Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Sonar: 1 Oil Left: 2
    string s = formatter(score, level, lives, health, squirts, gold, sonar, oilLeft);
    // Finally, update the display text at the top of the screen with your // newly created stats
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
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
    heatMap[x][y] = GOOD;
}





void StudentWorld::annoyFrackMan(){
    m_frackman->annoy(2);
}

void StudentWorld::giveFrackManGold(){
    m_frackman->addGold();
}

void StudentWorld::giveFrackManSonar(){
    m_frackman->addSonar();
}

void StudentWorld::giveFrackManWater(){
    m_frackman->addWater();
}

void StudentWorld::foundOil(){
    oilLeft--;
}

void StudentWorld::addActivatingObject(char type) {
    bool validLocation = false;
    int x = 0;
    int y = 0;
    vector<Actor *>::const_iterator it;
    while(!validLocation){
        x=randInt(0, 60);
        if(x>=27 && x<= 33)
            continue;
        if(type == 'b')
            y = randInt(20, 56);
        else
            y = randInt(0, 56);
        it=m_actors.begin();
        validLocation = true;
        while(it!=m_actors.end()){
            if(distance((*it)->getX() - x,(*it)->getY() -y ) <= 6){
                validLocation = false;
                break;
            }
            it++;
        }
    }
    if(type == 'b'){
        m_actors.push_back(new Boulder(this, x, y));
        clearDirt(x, y);
    }
    else if(type == 'g')
        m_actors.push_back(new GoldNugget(this, x, y, false));
    else
        m_actors.push_back(new OilBarrel(this, x, y));
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
    if(annoyer->hurtsFrackMan()){
        if(distance(m_frackman->getX()-x, m_frackman->getY()-y) <= 3){
            m_frackman->annoy(points);
            numAnnoyed++;
        }
    }
    return numAnnoyed;
}
void StudentWorld::revealAllNearbyObjects(int x, int y, int radius){
    playSound(SOUND_SONAR);
    vector<Actor *>::const_iterator it = m_actors.begin();
    for(;it<m_actors.end();it++){
        if(distance((*it)->getX()-x, (*it)->getY()-y) <= radius){
            (*it)->setVisible(true);
        }
    }
}

Actor* StudentWorld::findNearbyFrackMan(Actor* a, int radius) const {
    if(distance(a->getX() - m_frackman->getX(),a->getY() - m_frackman->getY()) <= radius){
        return m_frackman;
    }
    return nullptr;
}

bool StudentWorld::facingTowardFrackMan(Actor *a) const{
    if(lineOfSightToFrackMan(a)==a->getDirection())
        return true;
    return false;
}

void StudentWorld::shootSquirt(Actor *a){
    playSound(SOUND_PLAYER_SQUIRT);
    m_actors.push_back(new Squirt(this, a->getX(),a->getY(), a->getDirection()));
}

GraphObject::Direction StudentWorld::lineOfSightToFrackMan(Actor* a) const{
    int fx = m_frackman->getX();
    int fy = m_frackman->getY();
    int mx = a->getX();
    int my = a->getY();
    if(fx - mx >= -3 && fx - mx <=3){
        if(fy > my)
            return GraphObject::up;
        else
            return GraphObject::down;
    }
    if(fy - my >= -3 && fy - my <=3){
        if(fx > mx)
            return GraphObject::right;
        else
            return GraphObject::left;
    }
    return GraphObject::none;
}

bool StudentWorld::canActorMoveTo(Actor* a, int x, int y) const{
    if(x>=0 && x<=VIEW_WIDTH - SPRITE_WIDTH && y>=0 && y<=VIEW_HEIGHT - SPRITE_HEIGHT){
        if(!a->canDigThroughDirt()){
            if(!awayFromDirt(x, y))
                return false;
        }
        
        vector<Actor *>::const_iterator it = m_actors.begin();
        for(;it<m_actors.end();it++){
            if(!(*it)->canActorsPassThroughMe() && a != *it){
                if(distance((*it)->getX()-x, (*it)->getY()-y) <= 3)
                    return false;
            }
        }
    }
    return true;
}
//BAD means there is dirt there
//good means there is no dirt there

bool StudentWorld::awayFromDirt(int x, int y) const{
    for(int a = 0; a<SPRITE_WIDTH; a++){
        for (int b = 0; b<SPRITE_HEIGHT; b++){
            if(heatMap[x+a][y + b] == BAD)
                return false;
        }
    }
    return true;
}

bool StudentWorld::canProtesterLeaveTo(Protester* a, int x, int y, int h){
    if(heatMap[x][y] == h-1 && canActorMoveTo(a, x, y)) {
        return true;
    }
    return false;
}

void StudentWorld::protesterLeave(Protester * a){
    int x = a->getX();
    int y = a->getY();
    int myHeat = heatMap[x][y];
    //NEED TO SET DIRECTION ALSO
    if(canProtesterLeaveTo(a, x+1, y, myHeat)){
        a->moveTo(x+1, y);
        return;
    }
    if(canProtesterLeaveTo(a, x-1, y, myHeat)){
        a->moveTo(x-1, y);
        return;
    }
    if(canProtesterLeaveTo(a, x, y+1, myHeat)){
        a->moveTo(x, y + 1);
        return;
    }
    if(canProtesterLeaveTo(a, x, y-1, myHeat)){
        a->moveTo(x, y-1);
        return;
    }
}

void StudentWorld::frackmanDropsGold(FrackMan* a){
    m_actors.push_back(new GoldNugget(this, m_frackman->getX(), m_frackman->getY(), true));
}

//WORKING ON THIS
//IT DOESNT SEE BOULDRE PROPERLY

void StudentWorld::solveHeatMap(){
    for (int x = 0; x<64; x++){
        for ( int y = 0; y< 64; y++){
            if(m_dirt[x][y] == nullptr)
                heatMap[x][y] = GOOD;
            else
                heatMap[x][y] = BAD;
        }
    }
    vector<Actor *>::const_iterator it = m_actors.begin();
    for(;it<m_actors.end();it++){
        if(!(*it)->canActorsPassThroughMe()){
            for(int x = 0; x<4; x++){
                for (int y =0; y<4; y++){
                    if(distance(x,y) <=3)
                        heatMap[(*it)->getX() + x][(*it)->getY() + y] = BOULDER;
                }
            }
        }
    }
    queue<Coord> mapToFill;
    mapToFill.push(Coord(63,63,1));
    while(!mapToFill.empty()){
        Coord a = mapToFill.front();
        int r = a.r();
        int c = a.c();
        if(heatMap[r][c]!=GOOD){
            mapToFill.pop();
            continue;
        }
        int h = a.h();
        heatMap[r][c] = h;
        mapToFill.pop();
        if(r!=0 && heatMap[r-1][c] == GOOD ){
            mapToFill.push(Coord(r-1,c,h+1));
        }
        if(c != 63 && heatMap[r][c+1] == GOOD){
            mapToFill.push(Coord(r,c+1,h+1));
        }
        if(r != 63 && heatMap[r+1][c] == GOOD){
            mapToFill.push(Coord(r+1,c,h+1));
        }
        if(c!=0 && heatMap[r][c-1] == GOOD){
            mapToFill.push(Coord(r,c-1, h+1));
        }
    }
//    cout.precision(3);
//    for(int x =0; x<64;x++) {
//        for(int y =0; y<64;y++){
//            std::printf("%-2i", heatMap[x][y]);
//        }
//        cout << endl;
//        
//    }
}