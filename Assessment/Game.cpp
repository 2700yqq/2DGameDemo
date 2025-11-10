#include "iostream"
#include <algorithm>
#include "GamesEngineeringBase.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;


inline float wrap(float v, float max) {
    // Wrap a value so it stays within [0, max)
    while (v < 0) v += max;
    while (v >= max) v -= max;
    return v;
}

inline float shortestDelta(float a, float b, float range) {
    float d = a - b;
    if (d > range / 2) d -= range;
    if (d < -range / 2) d += range;
    return d;
}


struct Glyph5x7 {
    unsigned char row[7];
};

static Glyph5x7 getGlyph(char ch) {
    switch (ch) {
    case '0': return { {0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110} };
    case '1': return { {0b00100,0b01100,0b00100,0b00100,0b00100,0b00100,0b01110} };
    case '2': return { {0b01110,0b10001,0b00001,0b00010,0b00100,0b01000,0b11111} };
    case '3': return { {0b11110,0b00001,0b00001,0b01110,0b00001,0b00001,0b11110} };
    case '4': return { {0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010} };
    case '5': return { {0b11111,0b10000,0b11110,0b00001,0b00001,0b10001,0b01110} };
    case '6': return { {0b00110,0b01000,0b10000,0b11110,0b10001,0b10001,0b01110} };
    case '7': return { {0b11111,0b00001,0b00010,0b00100,0b01000,0b10000,0b10000} };
    case '8': return { {0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110} };
    case '9': return { {0b01110,0b10001,0b10001,0b01111,0b00001,0b00010,0b01100} };

    case ' ': return { {0,0,0,0,0,0,0} };
    case ':': return { {0,0b00100,0,0,0,0b00100,0} };
    case '/': return { {0b00001,0b00010,0b00100,0b01000,0b10000,0b00000,0b00000} };

    case 'A': return { {0b01110,0b10001,0b10001,0b11111,0b10001,0b10001,0b10001} };
    case 'B': return { {0b11110,0b10001,0b10001,0b11110,0b10001,0b10001,0b11110} };
    case 'C': return { {0b01110,0b10001,0b10000,0b10000,0b10000,0b10001,0b01110} };
    case 'D': return { {0b11100,0b10010,0b10001,0b10001,0b10001,0b10010,0b11100} };
    case 'E': return { {0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b11111} };
    case 'F': return { {0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b10000} };
    case 'G': return { {0b01110,0b10001,0b10000,0b10111,0b10001,0b10001,0b01110} };
    case 'H': return { {0b10001,0b10001,0b10001,0b11111,0b10001,0b10001,0b10001} };
    case 'I': return { {0b01110,0b00100,0b00100,0b00100,0b00100,0b00100,0b01110} };
    case 'J': return { {0b00011,0b00001,0b00001,0b00001,0b10001,0b10001,0b01110} };
    case 'K': return { {0b10001,0b10010,0b10100,0b11000,0b10100,0b10010,0b10001} };
    case 'L': return { {0b10000,0b10000,0b10000,0b10000,0b10000,0b10000,0b11111} };
    case 'M': return { {0b10001,0b11011,0b10101,0b10101,0b10001,0b10001,0b10001} };
    case 'N': return { {0b10001,0b11001,0b10101,0b10011,0b10001,0b10001,0b10001} };
    case 'O': return { {0b01110,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110} };
    case 'P': return { {0b11110,0b10001,0b10001,0b11110,0b10000,0b10000,0b10000} };
    case 'Q': return { {0b01110,0b10001,0b10001,0b10001,0b10101,0b10010,0b01101} };
    case 'R': return { {0b11110,0b10001,0b10001,0b11110,0b10100,0b10010,0b10001} };
    case 'S': return { {0b01111,0b10000,0b10000,0b01110,0b00001,0b00001,0b11110} };
    case 'T': return { {0b11111,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100} };
    case 'U': return { {0b10001,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110} };
    case 'V': return { {0b10001,0b10001,0b10001,0b10001,0b10001,0b01010,0b00100} };
    case 'W': return { {0b10001,0b10001,0b10001,0b10101,0b10101,0b10101,0b01010} };
    case 'X': return { {0b10001,0b10001,0b01010,0b00100,0b01010,0b10001,0b10001} };
    case 'Y': return { {0b10001,0b10001,0b01010,0b00100,0b00100,0b00100,0b00100} };
    case 'Z': return { {0b11111,0b00001,0b00010,0b00100,0b01000,0b10000,0b11111} };
    
    case 'c': return { {0,0,0b01110,0b10000,0b10000,0b10000,0b01110} };
    case 'e': return { {0,0,0b01110,0b10001,0b11111,0b10000,0b01110} };
    case 'i': return { {0,0b00100,0,0b01100,0b00100,0b00100,0b01110} };
    case 'o': return { {0,0,0b01110,0b10001,0b10001,0b10001,0b01110} };
    case 'r': return { {0,0,0b10110,0b11001,0b10000,0b10000,0b10000} };
    case 's': return { {0,0,0b01111,0b10000,0b01110,0b00001,0b11110} };
    case 't': return { {0b00100,0b00100,0b11111,0b00100,0b00100,0b00100,0b00010} };
    case 'x': return { {0,0,0b10001,0b01010,0b00100,0b01010,0b10001} };

    default:  
        return { {0b11111,0b10001,0b10101,0b10001,0b10101,0b10001,0b11111} };
    }
}


static void drawChar(GamesEngineeringBase::Window& canvas, int x, int y, char ch,
    unsigned char r, unsigned char g, unsigned char b, int scale = 2) {
    Glyph5x7 gph = getGlyph(ch);
    for (int row = 0; row < 7; ++row) {
        unsigned char bits = gph.row[row];
        for (int col = 0; col < 5; ++col) {
            if (bits & (1 << (4 - col))) {
                for (int sx = 0; sx < scale; ++sx)
                    for (int sy = 0; sy < scale; ++sy) {
                        int px = x + col * scale + sx;
                        int py = y + row * scale + sy;
                        if (px >= 0 && px < (int)canvas.getWidth() &&
                            py >= 0 && py < (int)canvas.getHeight())
                            canvas.draw(px, py, r, g, b);
                    }
            }
        }
    }
}

static void drawString(GamesEngineeringBase::Window& canvas, int x, int y, const char* text,
    unsigned char r, unsigned char g, unsigned char b, int scale = 2, int letterSpacing = 1) {
    int cursor = 0;
    while (text[cursor]) {
        drawChar(canvas, x, y, text[cursor], r, g, b, scale);
        x += (5 * scale) + letterSpacing;
        ++cursor;
    }
}

static void drawHPBar(GamesEngineeringBase::Window& canvas, int x, int y,
    int curHP, int maxHP, int w = 200, int h = 16) {
    if (maxHP <= 0) maxHP = 1;
    for (int i = 0; i < w; ++i)
        for (int j = 0; j < h; ++j)
            canvas.draw(x + i, y + j, 120, 20, 20);
    int fill = (int)(w * (float)curHP / (float)maxHP);
    if (fill < 0) fill = 0; if (fill > w) fill = w;
    for (int i = 0; i < fill; ++i)
        for (int j = 0; j < h; ++j)
            canvas.draw(x + i, y + j, 20, 220, 20);
}



class tile {
    GamesEngineeringBase::Image sprite;
public:
    tile() {}
    void load(std::string filename) {
        sprite.load(filename);
    }
    void draw(GamesEngineeringBase::Window& canvas,int x, int y) {
        for (unsigned int i = 0; i < sprite.height; i++)
            // bounds checking goes here
            if (i + y > 0 && i + y < canvas.getHeight()) {
                for (unsigned int j = 0; j < sprite.width; j++) {
                    if (j + x > 0 && j + x < canvas.getWidth()) {
                        canvas.draw(x + j, y + i, sprite.atUnchecked(j, i));
                    }
                    
                }
            }
            
           
    }
    unsigned int getHeight() { return sprite.height; }
    unsigned int getWidth() { return sprite.width; }
    GamesEngineeringBase::Image& getSprite() { return sprite; }

};


const unsigned int tileNum = 24;
class tileSet {
    tile t[tileNum];
public:
    // create and load tiles here
    tileSet(std::string pre = "") {

        for (unsigned int i = 0; i < tileNum; i++) {
            std::string filename;
            filename = "Resources/" + pre + std::to_string(i) + ".png";
            t[i].load(filename);
        }
    }
    // access individual tile here
    tile& operator[](unsigned int index) { return t[index]; }
};


const int worldSize = 1000;
class world {
    tileSet ts;
    unsigned int tilesWide = 0;
    unsigned int tilesHigh = 0;
    unsigned int tileWidth = 0;
    unsigned int tileHeight = 0;
    unsigned int** data;
    bool** isPassable;

public:
    

    world() : ts() {

        tilesWide = worldSize;
        tilesHigh = worldSize;
        tileWidth = worldSize;
        tileHeight = worldSize;
        
        data = new unsigned int* [worldSize];
        isPassable = new  bool* [worldSize];

        for (int i = 0; i < worldSize; ++i) {
            data[i] = new unsigned int[worldSize];
            isPassable[i] = new bool[worldSize];

        }

        for (int i = 0; i < worldSize; i++) {
            for (int j = 0; j < worldSize; j++) {
                data[i][j] = rand() % tileNum;
                isPassable[i][j] = true;
            }
        }


        


    }

    //- File handling start

    world(const std::string& filename) : ts() {
        tilesWide = worldSize;
        tilesHigh = worldSize;
        tileWidth = worldSize;
        tileHeight = worldSize;
        
        data = new unsigned int* [worldSize];
        isPassable = new bool* [worldSize];

        for (int i = 0; i < worldSize; ++i) {
            data[i] = new unsigned int[worldSize];
            isPassable[i] = new bool[worldSize];

        }

        for (int i = 0; i < worldSize; i++) {
            for (int j = 0; j < worldSize; j++) {
                data[i][j] = rand() % tileNum;
                isPassable[i][j] = true;
            }
        }


        std::ifstream fin(filename);

        if (!fin.is_open()) {
            std::cerr << "Failed to open tile file: " << filename << std::endl;
            return;
        }

        std::string line;
        bool readingData = false;
        int index1 = 0;
        int index2 = 0;
        while (std::getline(fin, line)) {
            if (line.empty()) continue;

            if (line.rfind("layer", 0) == 0) {
                readingData = true;
                continue;
            }

            if (!readingData) {
                //Read metadata section
                std::stringstream ss(line);
                std::string key;
                ss >> key;

                if (key == "tileswide") ss >> tilesWide;
                else if (key == "tileshigh") ss >> tilesHigh;
                else if (key == "tilewidth") ss >> tileWidth;
                else if (key == "tileheight") ss >> tileHeight;

                //cout << "tilesWide: " << tilesWide << ";tilesHigh: " << tilesHigh << ";tileWidth: " << tileWidth << ";tileHeight: " << tileHeight << endl;

            }
            else {
                // Read map date section
                std::stringstream ss(line);
                string value;

                while (std::getline(ss, value, ',') && value != "") {
                    //data[0][0] = std::stoi(value);

                    data[index1][index2] = std::stoi(value);

                    if (stoi(value) >= 14 && stoi(value) <= 22) {
                        //water impassable
                        isPassable[index1][index2] = false;
                    }
                    else {
                        isPassable[index1][index2] = true;
                    }

                    //cout << value << endl;
                    index2++;
                }

                index1++;
                index2 = 0;
            }


        }

        fin.close();

    }
    int getPixelWidth()  const { return tilesWide * tileWidth; }
    int getPixelHeight() const { return tilesHigh * tileHeight; }

    

    
    bool Passable(unsigned int worldX,unsigned int worldY, bool inf) {
       
        int tileX = worldX / tileWidth  % tilesWide;
        int tileY = worldY / tileHeight  % tilesHigh;
        
        return isPassable[tileY][tileX];
    }

    ~world() {
        for (int i = 0; i < worldSize; ++i)
            delete[] data[i];
        delete[] data;

        for (int i = 0; i < worldSize; ++i)
            delete[] isPassable[i];
        delete[] isPassable;
    }

    //- File Handling end

    void draw(GamesEngineeringBase::Window& canvas, unsigned int camX, unsigned int camY)
    {
        int startTileX = camX / tileWidth;
        int startTileY = camY / tileHeight;
        int endTileX = (camX + canvas.getWidth()) / tileWidth + 1;
        int endTileY = (camY + canvas.getHeight()) / tileHeight + 1;

        for (int ty = startTileY; ty < endTileY; ++ty) {
            for (int tx = startTileX; tx < endTileX; ++tx) {

                
                int tileX = ((tx % (int)tilesWide) + (int)tilesWide) % (int)tilesWide;
                int tileY = ((ty % (int)tilesHigh) + (int)tilesHigh) % (int)tilesHigh;

                unsigned int id = data[tileY][tileX];

                
                int drawX = tx * tileWidth - camX;
                int drawY = ty * tileHeight - camY;

                
                ts[id].draw(canvas, drawX, drawY);
            }
        }
    }

};

class Hero {
public:
    int x, y;
    int speed;
    int health;
    int maxHealth;
    int radius;
    GamesEngineeringBase::Image sprite;
    
    Hero(GamesEngineeringBase::Window& canvas, std::string filename) {
        sprite.load(filename);

        if (!sprite.load(filename)) {
            std::cerr << "Failed to load hero" << std::endl;
        }
        else {
            std::cerr << "Load hero! -- " << filename << endl;
        }

        x = (canvas.getWidth() - sprite.width) / 2;
        y = (canvas.getHeight() - sprite.height) / 2;
        speed = 300;

        maxHealth = 100;
        health = maxHealth;
        radius = sprite.width / 2;  

    }


    void back(GamesEngineeringBase::Window& canvas, int _x, int _y, int mapW, int mapH) {
        x -= _x;
        y -= _y;

        if (x < 0) x += mapW;
        if (y < 0) y += mapH;
        if (x >= mapW) x -= mapW;
        if (y >= mapH) y -= mapH;

    }

    void update(GamesEngineeringBase::Window& canvas, int _x, int _y,int mapW, int mapH, bool inf) {

        x += _x;
        y += _y;
        if (inf) {
            x = wrap(x, mapW);
            y = wrap(y, mapH);
        }
        else {
            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x + sprite.width > mapW) x = mapW - sprite.width;
            if (y + sprite.height > mapH) y = mapH - sprite.height;
        }
        
       
       /* if (x < 0) x = 0;
        if (x > (int)canvas.getWidth() - (int)sprite.width)
            x = (int)canvas.getWidth() - (int)sprite.width;

        if (y < 0) y = 0;
        if (y > (int)canvas.getHeight() - (int)sprite.height)
            y = (int)canvas.getHeight() - (int)sprite.height;*/

    }

    void draw(GamesEngineeringBase::Window& canvas) {
        for (unsigned int yy = 0; yy < sprite.height; yy++) {
            for (unsigned int xx = 0; xx < sprite.width; xx++) {

                if (sprite.alphaAtUnchecked(xx, yy) == 0)
                    continue; 

                int screenX = x + xx;
                int screenY = y + yy;

                if (screenX >= 0 && screenX < (int)canvas.getWidth() &&
                    screenY >= 0 && screenY < (int)canvas.getHeight()) {
                        canvas.draw(screenX, screenY, sprite.atUnchecked(xx, yy));
                    
                    
                }
            }
        }

    }

    void drawAt(GamesEngineeringBase::Window& canvas, int sx, int sy) {
        for (unsigned yy = 0; yy < sprite.height; ++yy)
            for (unsigned xx = 0; xx < sprite.width; ++xx) {
                if (sprite.alphaAtUnchecked(xx, yy) == 0)
                    continue; 
                int X = sx + xx, Y = sy + yy;
                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
                    canvas.draw(X, Y, sprite.atUnchecked(xx, yy));
            }

        //cout << "DrawAt" << endl;
    }


    void takeDamage(int dmg) {
        health -= dmg;

        if (health < 0) health = 0;
        std::cout << "Hero hit! HP = " << health << std::endl;
    }

    bool isDead() const { return health <= 0; }

                   
};



struct Camera {
    int x; // top-left corner of what we see in world space
    int y;
    unsigned int width;
    unsigned int height;

    
};

struct Projectile {
    float x, y, vx, vy;
    int damage;
    bool fromHero, alive;
    Projectile() : x(0), y(0), vx(0), vy(0), damage(1), fromHero(false), alive(false) {}
    Projectile(float X, float Y, float VX, float VY, int dmg, bool hero)
        : x(X), y(Y), vx(VX), vy(VY), damage(dmg), fromHero(hero), alive(true) {
    }
};


#define MAX_ENEMY_PROJECTILES 500
class BaseNPC {
protected:
    float x, y;
    float speed;
    unsigned int hp, maxHP;
    float radius;
    bool alive;
    GamesEngineeringBase::Image sprite;

   

    static const int MAX_BULLETS = 10;
    Projectile bullets[MAX_BULLETS];
    int bulletCount;



public:
    BaseNPC() : x(0), y(0), speed(0), hp(1), maxHP(1), radius(16), alive(false), bulletCount(0){}
    virtual ~BaseNPC() {}

    virtual void init(float startX, float startY) { x = startX; y = startY; alive = true; }
    virtual void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf) = 0;

    void updateBullets(float dt, int mapW, int mapH, bool inf) {
        for (int i = 0; i < bulletCount; ++i) {
            if (!bullets[i].alive) continue;
            bullets[i].x += bullets[i].vx * dt;
            bullets[i].y += bullets[i].vy * dt;
            if (inf) {
                bullets[i].x = wrap(bullets[i].x, mapW);
                bullets[i].y = wrap(bullets[i].y, mapH);
            }
            else {
                if (bullets[i].x < 0 || bullets[i].x >= mapW ||
                    bullets[i].y < 0 || bullets[i].y >= mapH)
                    bullets[i].alive = false;
            }
        }
    }

    virtual void draw(GamesEngineeringBase::Window& canvas, int camX, int camY, int mapW, int mapH) {
        if (!alive) return;
        int sx = (int)(x - camX), sy = (int)(y - camY);
        sx = wrap(sx, mapW);
        sy = wrap(sy, mapH);

        for (unsigned yy = 0; yy < sprite.height; ++yy)
            for (unsigned xx = 0; xx < sprite.width; ++xx) {
                if (sprite.alphaAtUnchecked(xx, yy) == 0) continue;

                int X = sx + xx, Y = sy + yy;
                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
                canvas.draw(X, Y, sprite.atUnchecked(xx, yy));
            }

        for (int i = 0; i < bulletCount; ++i) {
            if (!bullets[i].alive) continue;
            int bx = (int)(bullets[i].x - camX);
            int by = (int)(bullets[i].y - camY);
            if (bx < 0 || bx >= (int)canvas.getWidth() || by < 0 || by >= (int)canvas.getHeight())
                continue;

            const int radius = 3;
            for (int dx = -radius; dx <= radius; ++dx)
                for (int dy = -radius; dy <= radius; ++dy)
                    if (dx * dx + dy * dy <= radius * radius)
                        if (bx + dx >= 0 && bx + dx < canvas.getWidth() && by + dy >= 0 && by + dy < canvas.getHeight()) {

                            canvas.draw(bx + dx, by + dy, 255, 200, 0);

                        }
        }
    }

    bool hitHero(float heroX, float heroY, float heroR) {
        for (int i = 0; i < bulletCount; ++i) {
            if (!bullets[i].alive) continue;
            float dx = heroX - bullets[i].x;
            float dy = heroY - bullets[i].y;
            if (dx * dx + dy * dy < heroR * heroR) {
                bullets[i].alive = false;
                return true;
            }
        }
        return false;
    }


    virtual void takeDamage(int dmg) { 
        hp -= dmg; 
        if (hp <= 0) { 
            hp = 0; 
            alive = false; 
            for (int i = 0; i < bulletCount; ++i)
                bullets[i].alive = false;
        } 
    }


    bool isAlive() const { return alive; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getRadius() const { return radius; }

    virtual unsigned int getHP() const { return hp; }
    virtual unsigned int getMaxHP() const { return maxHP; }
    enum class NPCKind { Normal, Runner, Tank, Turret };
    virtual NPCKind getType() const = 0;
    virtual unsigned int getBulletCount() const { return bulletCount; }
    virtual Projectile getBullet(int i) const {
        if (i < 0 || i >= bulletCount)
            return Projectile();
        return bullets[i]; 
    }

    void addBullet(const Projectile& p) {
        if (bulletCount < MAX_BULLETS)
            bullets[bulletCount++] = p;
    }

    void setPos(float nx, float ny) { x = nx; y = ny; }

    void knockback(float fromX, float fromY, float pixels) {
        float dx = x - fromX;
        float dy = y - fromY;
        float len = sqrtf(dx * dx + dy * dy);
        if (len > 0.001f) {
            x += dx / len * pixels;
            y += dy / len * pixels;
        }
    }


protected:
    
    void chase(float dt, float heroX, float heroY, int mapW, int mapH, bool infinite = false) {
        float dx = heroX - x, dy = heroY - y;

        if (infinite && mapW > 0 && mapH > 0) {
            dx = shortestDelta(heroX, x, mapW);
            dy = shortestDelta(heroY, y, mapH);
        }
        float len = sqrtf(dx * dx + dy * dy);
        if (len > 0.001f) { dx /= len; dy /= len; x += dx * speed * dt; y += dy * speed * dt; }
        if (infinite) { x = wrap(x, mapW); y = wrap(y, mapH); }
    }

    void loadSprite(const char* file) {
        if (!sprite.load(file)) {}
    }

};

class NormalNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 60; hp = maxHP = 3; radius = 16; loadSprite("Resources/npc1.png");
    }
    void update(float dt, float heroX, float heroY, int mapW, int mapH,bool inf) override {
        chase(dt, heroX, heroY, mapW, mapH, inf);
    }
    NPCKind getType() const override { return NPCKind::Normal; }

};


class RunnerNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 120; hp = maxHP = 2; radius = 14; loadSprite("Resources/npc2.png");
    }
    void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf) override {
        chase(dt, heroX, heroY, mapW, mapH, inf);
    }
    NPCKind getType() const override { return NPCKind::Runner; }

};

class TankNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 35; hp = maxHP = 6; radius = 20; loadSprite("Resources/npc3.png");
    }
    void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf) override {
        chase(dt, heroX, heroY, mapW, mapH, inf);
    }
    NPCKind getType() const override { return NPCKind::Tank; }

};


class TurretNPC : public BaseNPC {
    float shootCD;
    

public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 0; hp = maxHP = 4; radius = 18; loadSprite("Resources/npc4.png");
        shootCD = 0.6f;
    }
    
    void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf) override {

        shootCD -= dt;
        if (shootCD <= 0.f && bulletCount < MAX_BULLETS) {
            shootCD = 1.2f;
            float dx = heroX - x, dy = heroY - y;
            float len = (float)sqrt(dx * dx + dy * dy); if (len < 0.001f) return;
            dx /= len; dy /= len;
            bullets[bulletCount++] = Projectile(x, y, dx * 200.f, dy * 200.f, 1, false);
        }

        updateBullets(dt, mapW, mapH, inf);

       
    }

    NPCKind getType() const override { return NPCKind::Turret; }

};

#define MAX_NPCS 200


class NPCManager {
    

public:

    BaseNPC* slots[MAX_NPCS];   
    int count;

    

    NPCManager() : count(0) {
        for (int i = 0; i < MAX_NPCS; ++i) slots[i] = 0;
    }

    ~NPCManager() { clear(); }

    void clear() {
        for (int i = 0; i < MAX_NPCS; ++i) { if (slots[i]) { delete slots[i]; slots[i] = 0; } }
        count = 0;
    }

    void spawnOutsideCamera(int type, int camX, int camY, int camW, int camH,int mapW, int mapH, bool inf) {
        int idx = findFree();
        if (idx < 0) return;

        float spawnX, spawnY;
        int margin = 100;
        int side = rand() % 4;
        switch (side) {
        case 0: 
            spawnX = camX + (rand() % camW);
            spawnY = camY - margin;
            
            break;             // 上
        case 1: 
            spawnX = camX + (rand() % camW); 
            spawnY = camY + camH + margin; 
            break;      // 下
        case 2: 
            spawnX = camX - margin;
            spawnY = camY + (rand() % camH); 
            break;             // 左
        default:
            spawnX = camX + camW + margin; 
            spawnY = camY + (rand() % camH);
            break;      // 右
        }

        if (inf) {
            spawnX = wrap(spawnX, mapW);
            spawnY = wrap(spawnY, mapH);
        }
        else {
            if (spawnX < 0) spawnX = 10;
            if (spawnY < 0) spawnY = 10;
            if (spawnX > mapW - 32) spawnX = mapW - 32;
            if (spawnY > mapH - 32) spawnY = mapH - 32;
        }


        BaseNPC* p = 0;
        if (type == 0) p = new NormalNPC();
        else if (type == 1) p = new RunnerNPC();
        else if (type == 2) p = new TankNPC();
        else                p = new TurretNPC();

        p->init(spawnX, spawnY);
        slots[idx] = p;
        if (idx + 1 > count) count = idx + 1;

      /*  std::cout << "Spawn NPC type " << type
            << " at (" << spawnX << "," << spawnY << ")\n";*/

    }

    
    int updateAll(float dt, float heroWorldX, float heroWorldY, int mapW, int mapH, bool inf) {

        int kills = 0;
        
        for (int i = 0; i < count;++i) {

            if (!slots[i]) continue;
            if (!slots[i]->isAlive()) {
                delete slots[i]; 
                slots[i] = 0;
                kills++;
                continue;
            }

            slots[i]->update(dt, heroWorldX, heroWorldY,mapW, mapH, inf);
            
        }
        //cout << "kills " << kills << endl;
        return kills; 
    }

    void drawAll(GamesEngineeringBase::Window& canvas, int camX, int camY, int mapW, int mapH) {

        for (int i = 0; i < count; ++i) {
            if (slots[i]) {
                slots[i]->draw(canvas, camX, camY, mapW, mapH);
            } 
        }


    }

    //bool heroHitByBullet(float heroX, float heroY, float heroR) {
    //    bool hit = false;
    //    for (int i = 0; i < bulletCount; ++i) {
    //        if (!enemyBullets[i].alive) continue;
    //        float dx = heroX - enemyBullets[i].x, dy = heroY - enemyBullets[i].y;
    //        if (dx * dx + dy * dy < heroR * heroR) { enemyBullets[i].alive = false; hit = true; }
    //    }
    //    return hit;
    //}


    void checkHeroProjectilesHit(Projectile* heroBullets, int& heroBulletCount) {
        for (int i = 0; i < heroBulletCount; ++i) {
            if (!heroBullets[i].alive) continue;

            for (int j = 0; j < count; ++j) {
                if (!slots[j] || !slots[j]->isAlive()) continue;

                float dx = heroBullets[i].x - slots[j]->getX();
                float dy = heroBullets[i].y - slots[j]->getY();
                float dist2 = dx * dx + dy * dy;
                float hitRange = slots[j]->getRadius() * 2.0f;

                if (dist2 < hitRange * hitRange) {
                    slots[j]->takeDamage(heroBullets[i].damage);
                    heroBullets[i].alive = false;
                    break;
                }
            }
        }
    }


private:
    int findFree() {
        for (int i = 0; i < MAX_NPCS; ++i) if (slots[i] == 0) return i;
        return -1;
    }
};

#define MAX_HERO_PROJECTILES 300


class Manager {
    Hero hero;
    world w;
    unsigned int x = 0;
    unsigned int y = 0;
    
    Camera cam;
    bool infiniteMode = true;
    int score; 
    int mapW;
    int mapH;

    //NPC-related
    NPCManager npcManager;

    //Projectile heroProjectiles[MAX_PROJECTILES];
    float npcSpawnTimer;
    float npcSpawnInterval;
    unsigned int gameTime;

    //herobullets
    Projectile heroBullets[MAX_HERO_PROJECTILES];
    int heroBulletCount = 0;
    float heroShootCD = 0.f;
    float heroShootInterval = 0.5f; // 英雄射速



    float bulletSpeed = 420.f;
    float bulletDamage = 1.f;


    int   aoeN = 3;                 
    float aoeCD = 0.f;              
    float aoeCooldown = 5.f;        
    float aoeFlashTimer = 0.f;
    struct Powerup {
        float x = 0, y = 0;
        bool  active = false;
        int   type = 0;
    };

    Powerup powerup;
    float powerupSpawnTimer = 0.f;   
    float powerupSpawnInterval = 15.f;
    bool powerupMsgActive = false;
    float powerupMsgTimer = 0.f;
    string powerupMsg;

    int   currentLevel = 0;
    float levelTimer = 0.f;        
    const float LEVEL_DURATION = 120.f; 
    string levelFiles[2] = {
        "Resources/tiles_infinite.txt", 
        "Resources/tiles_fixed.txt"
    };
    bool levelInfinite[2] = { true, false };

    string saveFile = "savegame.txt";

    

public:
    float messageTimer = 0.f;       
    string messageText = "";   

    bool gameOver;
    float gameOverTimer;
    

    Manager(GamesEngineeringBase::Window& canvas) : hero(canvas, "Resources/plane.png"), w() {}
    Manager(GamesEngineeringBase::Window& canvas, string filename) : hero(canvas, "Resources/hero1.png"), w(filename),score(0), gameOver(false), gameOverTimer(0.0f) {
        cam.width = canvas.getWidth();
        cam.height = canvas.getHeight();
        /*cam.x = hero.x - w.getPixelWidth() / 2;
        cam.y = hero.y - w.getPixelHeight() / 2;*/
        cam.x = 0;
        cam.y = 0;
        mapW = w.getPixelWidth();
        mapH = w.getPixelHeight();
       
       /* hero.x = mapW / 2 - hero.sprite.width / 2;
        hero.y = mapH / 2 - hero.sprite.height / 2;*/

        heroBulletCount = 0;
        heroShootCD = 0;
        npcSpawnTimer = 0;
        npcSpawnInterval = 3.0f; 
        gameTime = 0;

        loadLevel(0);
        
    }
    Manager(GamesEngineeringBase::Window& canvas, string filename, float _dt) : hero(canvas, "Resources/hero1.png"), w(filename){

        
    }

    int getScore() {
        return score;
    }

    int getHp() {
        //cout << hero.health;
        return hero.health;
    }

    int getMaxHp() {
        return hero.maxHealth;
    }


    void switchMapMode(GamesEngineeringBase::Window& canvas) {
        static bool switchPressed = false;
        if (canvas.keyPressed('M')) {
            if (!switchPressed) { 
                infiniteMode = !infiniteMode;
                cout << "Map mode switched to : "
                    << (infiniteMode ? "Infinite" : "Fixed Boundary") << endl;
            }
            switchPressed = true;
        }
        else switchPressed = false;
    }

    bool isPassable(int X, int Y, int width, int height) {
        
        int corners[4][2] = {
            {X, Y},
            {X + width, Y},
            {X, Y + height},
            {X + width, Y + height}
        };


        for (int i = 0; i < 4; ++i) {
            int cx = corners[i][0];
            int cy = corners[i][1];
            if (!w.Passable(cx, cy, infiniteMode))
                return false;
        }
        return true;

    }

   
    void loadLevel(int idx) {
        if (idx < 0) idx = 0; if (idx > 1) idx = 1;
        currentLevel = idx;

        
        std::ifstream fin(levelFiles[idx]);
        std::string fileToLoad = fin.is_open() ? levelFiles[idx] : "Resources/tiles.txt";
        if (fin.is_open()) fin.close();

        
        w.~world();
        new(&w) world(fileToLoad);

        mapW = w.getPixelWidth();
        mapH = w.getPixelHeight();

        infiniteMode = levelInfinite[idx];


        hero.x = cam.width / 2 - hero.sprite.width / 2;
        hero.y = cam.height / 2 - hero.sprite.height / 2;
        hero.health = hero.maxHealth;       
        hero.speed = 200;
        heroShootCD = 0.f;
        aoeCD = 0.f;                        


        cam.x = hero.x + hero.sprite.width / 2 - cam.width / 2;
        cam.y = hero.y + hero.sprite.height / 2 - cam.height / 2;

        npcManager.clear();
        heroBulletCount = 0;
        heroShootCD = 0;
        levelTimer = 0.f;

        
        npcSpawnTimer = 0.f;
        npcSpawnInterval = 3.0f;  
        powerup.active = false;
        powerupSpawnTimer = 0.f;


        std::cout << "Loaded level " << idx
            << " file=" << fileToLoad
            << " infinite=" << (infiniteMode ? "true" : "false") << "\n";

        messageText = "LEVEL " + std::to_string(idx + 1) + " START";
        messageTimer = 2.0f;
    }

    

    void update(GamesEngineeringBase::Window& canvas, float dt) {
        if (canvas.keyPressed('P')) {
            saveGame();
            messageText = "GAME SAVE!";
            messageTimer = 1.f;
        }
        if (canvas.keyPressed('L')) {
            loadGame();
            messageText = "GAME LOAD!";
            messageTimer = 1.f;
        }


        if (aoeCD > 0.f) aoeCD -= dt;
        if (canvas.keyPressed('Q') && aoeCD <= 0.f) {
            castAOE();
            aoeCD = aoeCooldown;
            aoeFlashTimer = 0.1f; 
        }


        
        int dx = 0, dy = 0;
        int move = hero.speed * dt;
        if (canvas.keyPressed('W')) dy -= move;
        if (canvas.keyPressed('S')) dy += move;
        if (canvas.keyPressed('A')) dx -= move;
        if (canvas.keyPressed('D')) dx += move;

        switchMapMode(canvas);
        hero.update(canvas, dx, dy, mapW, mapH, infiniteMode);
        if (!isPassable(hero.x, hero.y, hero.sprite.width, hero.sprite.height)) {
            hero.back(canvas, dx, dy, mapW, mapH);
        }
        //cout << "Hero: " << hero.x << ", " << hero.y << endl;
        if (!infiniteMode) {
            if (hero.x < 0) hero.x = 0;
            if (hero.y < 0) hero.y = 0;
            if (hero.x + hero.sprite.width > mapW) hero.x = mapW - hero.sprite.width;
            if (hero.y + hero.sprite.height > mapH) hero.y = mapH - hero.sprite.height;
        }

        cam.x = hero.x + hero.sprite.width / 2 - canvas.getWidth() / 2;
        cam.y = hero.y + hero.sprite.height / 2 - canvas.getHeight() / 2;
        //cout << "cam:" << cam.x << ", " << cam.y << endl;

        if (infiniteMode) {
            cam.x = wrap(cam.x, mapW);
            cam.y = wrap(cam.y, mapH); 
        }
        else {
            if (cam.x < 0) cam.x = 0;
            if (cam.y < 0) cam.y = 0;
            if (cam.x + cam.width > mapW) cam.x = mapW - cam.width;
            if (cam.y + cam.height > mapH) cam.y = mapH - cam.height;
        }

       

        npcSpawnTimer += dt;
        if (npcSpawnTimer >= npcSpawnInterval) {
            int type = rand() % 4;
            npcManager.spawnOutsideCamera(type, cam.x, cam.y, cam.width, cam.height, mapW, mapH, infiniteMode);
            npcSpawnTimer = 0;
            if (npcSpawnInterval > 1.0f) npcSpawnInterval -= 0.02f;
        }

        int kills = npcManager.updateAll(dt, hero.x, hero.y, mapW, mapH, infiniteMode);
        score += kills * 10;

      
        float heroWorldX = hero.x + hero.sprite.width * 0.5f;
        float heroWorldY = hero.y + hero.sprite.height * 0.5f;

        if (infiniteMode) {
            heroWorldX = wrap(heroWorldX, mapW);
            heroWorldY = wrap(heroWorldY, mapH);
        }
        
        heroShootCD -= dt;
        if (heroShootCD <= 0.f && heroBulletCount < MAX_HERO_PROJECTILES) {
            float best = 1e30f;
            float targetX = 0, targetY = 0;

            for (int i = 0; i < MAX_NPCS; ++i) {
                BaseNPC* e = npcManager.slots[i];
                if (!e || !e->isAlive()) continue;
                float dx = shortestDelta(e->getX(), heroWorldX, mapW);
                float dy = shortestDelta(e->getY(), heroWorldY, mapH);
                float d2 = dx * dx + dy * dy;
                if (d2 < best) { best = d2; targetX = e->getX(); targetY = e->getY(); }
            }

            if (best < 1e8f) {
                float dx = shortestDelta(targetX, heroWorldX, mapW);
                float dy = shortestDelta(targetY, heroWorldY, mapH);
                float len = sqrtf(dx * dx + dy * dy);
                if (len > 0.001f) { dx /= len; dy /= len; }

                heroBullets[heroBulletCount++] =
                    Projectile(heroWorldX, heroWorldY, dx * bulletSpeed, dy * bulletSpeed, bulletDamage, true);

                heroShootCD = heroShootInterval;
            }
        }


        for (int i = 0; i < heroBulletCount; ++i) {
            if (!heroBullets[i].alive)
            {
                continue;
            }
            
            heroBullets[i].x += heroBullets[i].vx * dt;
            heroBullets[i].y += heroBullets[i].vy * dt;

            if (infiniteMode) {
                heroBullets[i].x = wrap(heroBullets[i].x, mapW);
                heroBullets[i].y = wrap(heroBullets[i].y, mapH);
            }
            else {
                if (heroBullets[i].x < 0 || heroBullets[i].x >= mapW ||
                    heroBullets[i].y < 0 || heroBullets[i].y >= mapH)
                    heroBullets[i].alive = false;

            }

            int ox = wrap(heroBullets[i].x - cam.x, mapW);
            int oy = wrap(heroBullets[i].y - cam.y, mapH);
            if (ox < 0 || ox >= cam.width ||
                oy < 0 || oy >= cam.height) {
                //cout << "herobullet dead:" << heroBullets[i].x << ", " << heroBullets[i].y << endl;

                heroBullets[i].alive = false;
            }
             
        }

      
        npcManager.checkHeroProjectilesHit(heroBullets, heroBulletCount);

      
        for (int i = 0; i < npcManager.count; ++i) {
            BaseNPC* e = npcManager.slots[i];
            if (!e || !e->isAlive()) continue;
            if (e->hitHero(heroWorldX, heroWorldY, hero.radius)) {
                hero.takeDamage(1);
                //std::cout << "💥 Hero hit by turret bullet!\n";
            }
        }

        powerupSpawnTimer += dt;
        if (!powerup.active && powerupSpawnTimer >= powerupSpawnInterval) {
            powerupSpawnTimer = 0.f;
            powerup.active = true;
            powerup.type = rand() % 2; // 0:attack speed, 1:AOE+1

            powerup.x = cam.x + 100 + rand() % (max(10, (int)cam.width - 200));
            powerup.y = cam.y + 100 + rand() % (max(10, (int)cam.height - 200));
        }


        if (powerup.active) {
            float hx = hero.x + hero.sprite.width * 0.5f;
            float hy = hero.y + hero.sprite.height * 0.5f;
            float dxp = hx - powerup.x;
            float dyp = hy - powerup.y;
            if (dxp * dxp + dyp * dyp < (hero.radius + 20) * (hero.radius + 20)) {
                if (powerup.type == 0) {
                    heroShootInterval = max(0.2f, heroShootInterval * 0.8f);
                    powerupMsg = "SPEED UP!";
                    
                }
                else {
                    aoeN = min(10, aoeN + 1);
                    powerupMsg =  "AOE + 1!";   
                }

                powerupMsgActive = true;
                powerupMsgTimer = 2.0f;  
                powerup.active = false;

                std::cout << "Power-Up Picked! → " << powerupMsg << std::endl;
            }
        }


        for (int i = 0; i < npcManager.count; i++) {
            BaseNPC* e = npcManager.slots[i];
            if (!e || !e->isAlive()) continue;

            float dx = heroWorldX - e->getX();
            float dy = heroWorldY - e->getY();
            float distSq = dx * dx + dy * dy;
            float rSum = hero.radius + e->getRadius();

            if (distSq < rSum * rSum) {
                hero.takeDamage(1);
            }
        }


        if (!gameOver && hero.health <= 0) {
            gameOver = true;
            gameOverTimer = 0.f;
            //std::cout << " Hero has died. GAME OVER.\n";
        }

        levelTimer += dt;
        if (levelTimer >= LEVEL_DURATION) {
            if (currentLevel == 0) {
                loadLevel(1);
            }
            else {
                gameOver = true;
                gameOverTimer = 5.0f;
            }
        }

        //std::cout << "Hero.x=" << hero.x << " Cam.x=" << cam.x << " BulletStartX=" << heroWorldX << std::endl;

    }


    void drawCameraBox(GamesEngineeringBase::Window& canvas, Camera cam) {
        int cx = 0;                
        int cy = 0;
        int cw = cam.width;
        int ch = cam.height;

        unsigned char R = 50, G = 120, B = 255;

        for (int x = 0; x < cw; ++x) {
            if (cy >= 0 && cy < (int)canvas.getHeight()) {
                canvas.draw(cx + x, cy, R, G, B);             
                canvas.draw(cx + x, cy + ch - 1, R, G, B);    
            }
        }
        for (int y = 0; y < ch; ++y) {
            if (cx >= 0 && cx < (int)canvas.getWidth()) {
                canvas.draw(cx, cy + y, R, G, B);             
                canvas.draw(cx + cw - 1, cy + y, R, G, B);    
            }
        }
        int centerX = cw / 2;
        int centerY = ch / 2;
        for (int i = -10; i <= 10; ++i) {
            if (centerX + i >= 0 && centerX + i < (int)canvas.getWidth())
                canvas.draw(centerX + i, centerY, 255, 255, 0); 
            if (centerY + i >= 0 && centerY + i < (int)canvas.getHeight())
                canvas.draw(centerX, centerY + i, 255, 255, 0); 
        }
    }

    void drawBullets(GamesEngineeringBase::Window& canvas, int camX, int camY, Projectile bullets[], int bulletCount, int color) {
        for (int i = 0; i < bulletCount; ++i) {
            if (!bullets[i].alive) {
                //cout << "bullet dead" << endl;
                continue;
            }
            int sx = (int)(bullets[i].x - camX);
            int sy = (int)(bullets[i].y - camY);

           /* sx = wrap(bullets[i].x, mapW) - cam.x;
            sy = wrap(bullets[i].y, mapH) - cam.y;*/

            
            if (sx < 0 || sy < 0 ||
                sx >= (int)canvas.getWidth() ||
                sy >= (int)canvas.getHeight()) continue;
            /*if (sx < 0 || sx >= canvas.getWidth() || sy < 0 || sy >= canvas.getHeight()) {
                continue;
            }*/

            
            const int radius = 4;

            for (int dx = -radius; dx <= radius; ++dx) {
                for (int dy = -radius; dy <= radius; ++dy) {
                    if (dx * dx + dy * dy <= radius * radius) { 
                        int px = wrap(sx + dx, mapW);
                        int py = wrap(sy + dy, mapH);

                        if (px >= 0 && px < (int)canvas.getWidth() &&
                            py >= 0 && py < (int)canvas.getHeight()) {
                            //cout << "draw point"   << i << endl;
                            canvas.draw(px, py, color, 0, 0); 
                        }
                    }
                }
            }
        }

    }

    void drawCircle(GamesEngineeringBase::Window& canvas,
        int cx, int cy, int r,
        int R, int G, int B,
        bool filled = false)
    {
        for (int dx = -r; dx <= r; ++dx) {
            for (int dy = -r; dy <= r; ++dy) {
                int dist2 = dx * dx + dy * dy;
                if (filled) {
                    if (dist2 <= r * r) {
                        int px = cx + dx, py = cy + dy;
                        if (px >= 0 && px < (int)canvas.getWidth() &&
                            py >= 0 && py < (int)canvas.getHeight())
                            canvas.draw(px, py, R, G, B);
                    }
                }
                else {
                    if (dist2 >= (r - 2) * (r - 2) && dist2 <= r * r) {
                        int px = cx + dx, py = cy + dy;
                        if (px >= 0 && px < (int)canvas.getWidth() &&
                            py >= 0 && py < (int)canvas.getHeight())
                            canvas.draw(px, py, R, G, B);
                    }
                }
            }
        }
    }


    void draw(GamesEngineeringBase::Window& canvas, float dt) {
        
        
        /*w.draw(canvas, cam.x, cam.y);
        hero.draw(canvas);*/
        
        
        w.draw(canvas, cam.x, cam.y);
        npcManager.drawAll(canvas, cam.x, cam.y, mapW, mapH);
      
        if (infiniteMode) {
            drawBullets(canvas, cam.x, cam.y, heroBullets, heroBulletCount, 300);
            hero.drawAt(canvas, cam.width / 2 - hero.sprite.width / 2,
                cam.height / 2 - hero.sprite.height / 2);
        }
        else {
            // 固定地图模式
            drawBullets(canvas, cam.x, cam.y, heroBullets, heroBulletCount, 300);
            hero.drawAt(canvas, hero.x - cam.x, hero.y - cam.y);
        }
       
        
        
        // draw power-up
 
        if (powerup.active) {
            int sx = (int)(powerup.x - cam.x);
            int sy = (int)(powerup.y - cam.y);
            int pr = 10; 
            for (int dx = -pr; dx <= pr; ++dx)
                for (int dy = -pr; dy <= pr; ++dy)
                    if (dx * dx + dy * dy <= pr * pr) {
                        int px = sx + dx, py = sy + dy;
                        if (0 <= px && px < (int)canvas.getWidth() &&
                            0 <= py && py < (int)canvas.getHeight()) {
                            if (powerup.type == 0) {
                                canvas.draw(px, py, 0, 255, 255);   
                            }
                            else {
                                canvas.draw(px, py, 255, 0, 255);   
                            }             
                        }
                    }
        }

        if (aoeFlashTimer > 0.f) {
            aoeFlashTimer -= dt;

            int hx = cam.width / 2;
            int hy = cam.height / 2;
            int r = 150; 

            
            int alpha = (int)(255 * (aoeFlashTimer / 0.25f));
            if (alpha < 50) alpha = 50;

            for (int dx = -r; dx <= r; ++dx) {
                for (int dy = -r; dy <= r; ++dy) {
                    int dist2 = dx * dx + dy * dy;
                    if (dist2 >= (r - 3) * (r - 3) && dist2 <= r * r) {
                        int px = hx + dx;
                        int py = hy + dy;
                        if (px >= 0 && px < (int)canvas.getWidth() &&
                            py >= 0 && py < (int)canvas.getHeight()) {
                            canvas.draw(px, py, 255, 255 - (rand() % 60), 0);
                        }
                    }
                }
            }

            for (int dx = -6; dx <= 6; ++dx)
                for (int dy = -6; dy <= 6; ++dy) {
                    int px = hx + dx, py = hy + dy;
                    if (px >= 0 && px < (int)canvas.getWidth() &&
                        py >= 0 && py < (int)canvas.getHeight())
                        canvas.draw(px, py, 255, 255, 255);
                }
        }
        
      
      



    }

    void drawUI(GamesEngineeringBase::Window& canvas, int score, float fps, int heroHP, int heroMaxHP, float dt) {
       // hp bar
        int barX = 20;
        int barY = 20;
        int barWidth = 200;
        int barHeight = 16;

        for (int y = 0; y < barHeight; ++y) {
            for (int x = 0; x < barWidth; ++x)
                canvas.draw(barX + x, barY + y, 60, 0, 0);
        }

        
        float ratio = (float)heroHP / (float)heroMaxHP;
        if (ratio < 0) ratio = 0;
        if (ratio > 1) ratio = 1;
        int filled = (int)(barWidth * ratio);

        
        int r, g;
        if (ratio > 0.5f) {           
            r = (int)(255 * (1.0f - ratio) * 2.0f);
            g = 255;
        }
        else {                      
            r = 255;
            g = (int)(255 * ratio * 2.0f);
        }

        
        for (int y = 0; y < barHeight; ++y) {
            for (int x = 0; x < filled; ++x)
                canvas.draw(barX + x, barY + y, r, g, 0);
        }
        for (int x = 0; x < barWidth; ++x) {
            canvas.draw(barX + x, barY, 180, 180, 180);
            canvas.draw(barX + x, barY + barHeight - 1, 180, 180, 180);
        }
        for (int y = 0; y < barHeight; ++y) {
            canvas.draw(barX, barY + y, 180, 180, 180);
            canvas.draw(barX + barWidth - 1, barY + y, 180, 180, 180);
        }


        char hpText[32];
        sprintf_s(hpText, sizeof(hpText), "HP: %d / %d", heroHP, heroMaxHP);
        drawString(canvas, barX + 5, barY + 22, hpText, 255, 255, 255, 2, 1);

        //score and fps
        char bufScore[32];
        int s = score, digits[12], dcnt = 0;
        if (s == 0) digits[dcnt++] = 0;
        while (s > 0 && dcnt < 12) { digits[dcnt++] = s % 10; s /= 10; }
        int idx = 0;
        bufScore[idx++] = 'S'; bufScore[idx++] = 'c'; bufScore[idx++] = 'o';
        bufScore[idx++] = 'r'; bufScore[idx++] = 'e'; bufScore[idx++] = ':';
        bufScore[idx++] = ' ';
        for (int i = dcnt - 1; i >= 0; --i) bufScore[idx++] = '0' + digits[i];
        bufScore[idx] = '\0';
        drawString(canvas, canvas.getWidth() - 120, 20, bufScore, 255, 255, 255, 2, 2);

        
        int ifps = (int)(fps + 0.5f);
        int fd[6]; int fcnt = 0; if (ifps == 0) fd[fcnt++] = 0;
        while (ifps > 0 && fcnt < 6) { fd[fcnt++] = ifps % 10; ifps /= 10; }
        char bufFPS[24]; idx = 0;
        bufFPS[idx++] = 'F'; bufFPS[idx++] = 'P'; bufFPS[idx++] = 'S';
        bufFPS[idx++] = ':'; bufFPS[idx++] = ' ';
        for (int i = fcnt - 1; i >= 0; --i) bufFPS[idx++] = '0' + fd[i];
        bufFPS[idx] = '\0';
        drawString(canvas, canvas.getWidth() - 120, 50, bufFPS, 255, 220, 40, 2, 2);

        

        char bufAOE[32];
        if (aoeCD <= 0.0f) {
            sprintf_s(bufAOE, sizeof(bufAOE), "AOE: READY");
            drawString(canvas, 20, 60, bufAOE, 0, 255, 0, 2, 1);
        }
        else {
            sprintf_s(bufAOE, sizeof(bufAOE), "AOE: %.1fs", aoeCD);
            drawString(canvas, 20, 60, bufAOE, 255, 255, 0, 2, 1);
        }

        char bufLV[64];
        sprintf_s(bufLV, "LEVEL %d: TIME %.0fs", currentLevel + 1, levelTimer);
        drawString(canvas, 20, 105, bufLV, 200, 200, 255, 2, 1);

        if (infiniteMode) {
            drawString(canvas, 20, canvas.getHeight() - 40, "MAP: INFINITE", 100, 255, 255, 2, 2);
        }
        else {
            drawString(canvas, 20, canvas.getHeight() - 40, "MAP: FIXED", 255, 100, 100, 2, 2);
        }
        
        //save and load game
        if (messageTimer > 0.f) {
            messageTimer -= dt;

            const char* msg = messageText.c_str();
            int msgLen = (int)strlen(msg);

            int cx = (int)canvas.getWidth() / 2;
            int cy = 60;
            int scale = 3;        
            int spacing = 2;     
            int textWidth = msgLen * (5 * scale + spacing);
            int startX = cx - textWidth / 2;

            float fade = max(0.f, messageTimer / 2.f);
            unsigned char r = (unsigned char)(255 * fade);
            unsigned char g = (unsigned char)(255 * fade);
            unsigned char b = (unsigned char)(80 * fade);

            if (messageText.find("SAVED") != std::string::npos) {
                r = 0; g = (unsigned char)(255 * fade); b = 0; 
            }
            else if (messageText.find("LOADED") != std::string::npos) {
                r = 0; g = (unsigned char)(180 * fade); b = (unsigned char)(255 * fade); 
            }

            drawString(canvas, startX, cy, msg, r, g, b, scale, spacing);
        }


    }


    void gameOverScreen(GamesEngineeringBase::Window& canvas, float dt) {
        for (int y = 0; y < (int)canvas.getHeight(); y++)
            for (int x = 0; x < (int)canvas.getWidth(); x++)
                canvas.draw(x, y, 0, 0, 100);  
        int centerX = (int)canvas.getWidth() / 2 - 120;
        int centerY = (int)canvas.getHeight() / 2 - 20;
        drawString(canvas, centerX, centerY, "GAME OVER", 255, 0, 0, 4, 2);

        // 显示提示
        drawString(canvas, centerX - 60, centerY + 60, "PRESS ESC TO EXIT", 255, 255, 255, 2, 1);

        // 倒计时退出
        gameOverTimer -= dt;
    }


    //游戏进度保存和读取
    void saveGame() {
        std::ofstream fou(saveFile, std::ios::binary);
        if (!fou.is_open()) { std::cerr << "Save failed\n"; return; }

        // 基本
        fou.write((char*)&currentLevel, sizeof(currentLevel));
        fou.write((char*)&infiniteMode, sizeof(infiniteMode));
        fou.write((char*)&score, sizeof(score));
        fou.write((char*)&levelTimer, sizeof(levelTimer));

        // hero
        fou.write((char*)&hero.x, sizeof(hero.x));
        fou.write((char*)&hero.y, sizeof(hero.y));
        fou.write((char*)&hero.health, sizeof(hero.health));

        // camera
        fou.write((char*)&cam.x, sizeof(cam.x));
        fou.write((char*)&cam.y, sizeof(cam.y));

        // 射击
        fou.write((char*)&heroShootCD, sizeof(heroShootCD));
        fou.write((char*)&heroShootInterval, sizeof(heroShootInterval));

        // AOE
        fou.write((char*)&aoeN, sizeof(aoeN));
        fou.write((char*)&aoeCD, sizeof(aoeCD));
        fou.write((char*)&aoeCooldown, sizeof(aoeCooldown));

        // power-up
        fou.write((char*)&powerup, sizeof(powerup));
        fou.write((char*)&powerupSpawnTimer, sizeof(powerupSpawnTimer));

        // NPC
        fou.write((char*)&npcManager.count, sizeof(npcManager.count));
        for (int i = 0; i < npcManager.count; ++i) {
            bool has = npcManager.slots[i] && npcManager.slots[i]->isAlive();
            fou.write((char*)&has, sizeof(has));

            if (!has) continue;

            BaseNPC* e = npcManager.slots[i];
            auto type = e->getType();
            fou.write((char*)&type, sizeof(type));

            float ex = e->getX(), ey = e->getY();
            fou.write((char*)&ex, sizeof(ex));
            fou.write((char*)&ey, sizeof(ey));

            unsigned int hp = e->getHP();
            fou.write((char*)&hp, sizeof(hp));

            int bulletCount = e->getBulletCount();  
            fou.write((char*)&bulletCount, sizeof(bulletCount));

            for (int b = 0; b < bulletCount; ++b) {
                Projectile p = e->getBullet(b);
                fou.write((char*)&p, sizeof(Projectile));
            }
        }


        fou.write((char*)&heroBulletCount, sizeof(heroBulletCount));
        for (int i = 0; i < heroBulletCount; ++i) {
            fou.write((char*)&heroBullets[i], sizeof(Projectile));
        }

        // ====== time-related ======
        fou.write((char*)&levelTimer, sizeof(levelTimer));
        fou.write((char*)&npcSpawnTimer, sizeof(npcSpawnTimer));
        fou.write((char*)&npcSpawnInterval, sizeof(npcSpawnInterval));
        fou.write((char*)&powerupSpawnTimer, sizeof(powerupSpawnTimer));
        fou.write((char*)&heroShootCD, sizeof(heroShootCD));
        fou.write((char*)&aoeCD, sizeof(aoeCD));
        fou.write((char*)&aoeFlashTimer, sizeof(aoeFlashTimer));


        fou.close();
        std::cout << "Game saved.\n";
    }

    void loadGame() {
        std::ifstream fin(saveFile, std::ios::binary);
        if (!fin.is_open()) { std::cerr << "Load failed\n"; return; }

        fin.read((char*)&currentLevel, sizeof(currentLevel));
        fin.read((char*)&infiniteMode, sizeof(infiniteMode));
        fin.read((char*)&score, sizeof(score));
        fin.read((char*)&levelTimer, sizeof(levelTimer));

        loadLevel(currentLevel);

        fin.read((char*)&hero.x, sizeof(hero.x));
        fin.read((char*)&hero.y, sizeof(hero.y));
        fin.read((char*)&hero.health, sizeof(hero.health));
        fin.read((char*)&cam.x, sizeof(cam.x));
        fin.read((char*)&cam.y, sizeof(cam.y));
        fin.read((char*)&heroShootCD, sizeof(heroShootCD));
        fin.read((char*)&heroShootInterval, sizeof(heroShootInterval));

        // AOE
        fin.read((char*)&aoeN, sizeof(aoeN));
        fin.read((char*)&aoeCD, sizeof(aoeCD));
        fin.read((char*)&aoeCooldown, sizeof(aoeCooldown));

        // power-up
        fin.read((char*)&powerup, sizeof(powerup));
        fin.read((char*)&powerupSpawnTimer, sizeof(powerupSpawnTimer));

        // NPC
        fin.read((char*)&npcManager.count, sizeof(npcManager.count));

        for (int i = 0; i < npcManager.count; ++i) {
            bool has;
            fin.read((char*)&has, sizeof(has));
            if (!has) { npcManager.slots[i] = nullptr; continue; }

            BaseNPC::NPCKind type;
            fin.read((char*)&type, sizeof(type));

            float ex, ey;
            fin.read((char*)&ex, sizeof(ex));
            fin.read((char*)&ey, sizeof(ey));

            unsigned int hp;
            fin.read((char*)&hp, sizeof(hp));

            
            BaseNPC* npc = nullptr;
            switch (type) {
            case BaseNPC::NPCKind::Normal: npc = new NormalNPC(); break;
            case BaseNPC::NPCKind::Runner: npc = new RunnerNPC(); break;
            case BaseNPC::NPCKind::Tank:   npc = new TankNPC(); break;
            case BaseNPC::NPCKind::Turret: npc = new TurretNPC(); break;
            }

            if (npc) {
                npc->init(ex, ey);
                while (npc->getHP() > hp) npc->takeDamage(1);
                npcManager.slots[i] = npc;

                int bulletCount = 0;
                fin.read((char*)&bulletCount, sizeof(bulletCount));

                for (int b = 0; b < bulletCount; ++b) {
                    Projectile p;
                    fin.read((char*)&p, sizeof(Projectile));
                    npc->addBullet(p);  
                }
            }
        }

       


        fin.read((char*)&heroBulletCount, sizeof(heroBulletCount));
        for (int i = 0; i < heroBulletCount; ++i) {
            fin.read((char*)&heroBullets[i], sizeof(Projectile));
        }

        fin.read((char*)&levelTimer, sizeof(levelTimer));
        fin.read((char*)&npcSpawnTimer, sizeof(npcSpawnTimer));
        fin.read((char*)&npcSpawnInterval, sizeof(npcSpawnInterval));
        fin.read((char*)&powerupSpawnTimer, sizeof(powerupSpawnTimer));
        fin.read((char*)&heroShootCD, sizeof(heroShootCD));
        fin.read((char*)&aoeCD, sizeof(aoeCD));
        fin.read((char*)&aoeFlashTimer, sizeof(aoeFlashTimer));


        fin.close();
        std::cout << "Game loaded.\n";
    }



private:
    
    void castAOE() {
        if (aoeCD > 0.f) return; 

        struct Entry { int idx; unsigned int maxhp; };
        Entry list[MAX_NPCS];
        int cnt = 0;
        for (int i = 0; i < npcManager.count; ++i) {
            BaseNPC* e = npcManager.slots[i];
            if (!e || !e->isAlive()) continue;
            list[cnt].idx = i;
            list[cnt].maxhp = e->getMaxHP();
            cnt++;
        }

        if (cnt == 0) {
            std::cout << "AOE no targets.\n";
            return;
        }

        for (int i = 0; i < cnt - 1; ++i) {
            int best = i;
            for (int j = i + 1; j < cnt; ++j)
                if (list[j].maxhp > list[best].maxhp)
                    best = j;
            if (best != i) { Entry tmp = list[i]; list[i] = list[best]; list[best] = tmp; }
        }

        const int AOE_DAMAGE = 15;     
        const float AOE_RANGE = 200.f; 
        int applied = 0;

        float hx = hero.x + hero.sprite.width * 0.5f;
        float hy = hero.y + hero.sprite.height * 0.5f;

        for (int k = 0; k < cnt && applied < aoeN; ++k) {
            int idx = list[k].idx;
            BaseNPC* e = npcManager.slots[idx];
            if (!e || !e->isAlive()) continue;

            float dx = e->getX() - hx;
            float dy = e->getY() - hy;
            float dist2 = dx * dx + dy * dy;

            if (dist2 < AOE_RANGE * AOE_RANGE) {
                e->takeDamage(AOE_DAMAGE);
                applied++;

                
                float len = sqrtf(dist2);
                if (len > 0.01f) {
                    e->knockback(hx, hy, 20.f);
                }
            }
        }

        aoeCD = aoeCooldown;
        std::cout << "💥 AOE cast! Damage=" << AOE_DAMAGE
            << " hit=" << applied << "/" << aoeN << " in range="
            << AOE_RANGE << "\n";

        aoeFlashTimer = 0.25f;  
    }


    int findClosestEnemy() {
        float hx = cam.x + hero.x;
        float hy = cam.y + hero.y;
        float bestDist = 1e9;
        int bestIdx = -1;
        for (int i = 0; i < MAX_NPCS; ++i) {
            if (!npcManagerSlotAlive(i)) continue;
            float dx = npcManagerEnemyX(i) - hx;
            float dy = npcManagerEnemyY(i) - hy;
            float dist = dx * dx + dy * dy;
            if (dist < bestDist) { bestDist = dist; bestIdx = i; }
        }
        return bestIdx;
    }

    bool npcManagerSlotAlive(int i) {
        return npcManager.slots[i] && npcManager.slots[i]->isAlive();
    }
    float npcManagerEnemyX(int i) { return npcManager.slots[i]->getX(); }
    float npcManagerEnemyY(int i) { return npcManager.slots[i]->getY(); }
    void npcManagerDamage(int i, int dmg) { npcManager.slots[i]->takeDamage(dmg); }

};

int main() {

    srand(static_cast<unsigned int>(time(nullptr)));
    // Create a canvas window with dimensions 1024x768 and title "Example"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 1024, "Assessment");

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;
    float dt = timer.dt();

    float fps = 0.0f;
    float msPerFrame = dt * 1000.0f;
    static float printTimer = 0.0f;

    Manager m(canvas, "Resources/tiles.txt");
    while (true)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();
        // Clear the window for the next frame rendering
        canvas.clear();

        if (canvas.keyPressed(VK_ESCAPE)) break;

        dt = timer.dt();
        if (dt > 0.0f) {
            fps = 1.0f / dt;
            msPerFrame = dt * 1000.0f;
            
            printTimer += dt;
            if (printTimer > 2.0f) {
                std::cout << "FPS: " << fps << " (" << msPerFrame << " ms/frame)\n"; 
                printTimer = 0.0f; 
            }

            /*fps = 0.9 * fps + 0.1f * (1.0f / max(dt, 1e-6f));*/
        }

        if (!m.gameOver) {
            m.update(canvas, dt);
            m.draw(canvas, dt);

            m.drawUI(canvas, /*score=*/m.getScore(), fps, /*HP=*/m.getHp(), /*MaxHP=*/m.getMaxHp(), dt);
        }
        else {
            m.gameOverScreen(canvas, dt);
        }
        

        canvas.present();
    }
    return 0;

}