#include "iostream"
#include <algorithm>
#include "GamesEngineeringBase.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

// ===== 坐标工具函数 =====
inline float wrap(float v, float max) {
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



// ====== 极简 5x7 位图字体 ======
struct Glyph5x7 {
    // 每一行 5 位有效，从左到右
    unsigned char row[7];
};

// ====== 完整版 getGlyph()，支持 Game UI 所需全部字符 ======
static Glyph5x7 getGlyph(char ch) {
    switch (ch) {
        // --- 数字 ---
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

            // --- 标点和空格 ---
    case ' ': return { {0,0,0,0,0,0,0} };
    case ':': return { {0,0b00100,0,0,0,0b00100,0} };
    case '/': return { {0b00001,0b00010,0b00100,0b01000,0b10000,0b00000,0b00000} };


            // --- 大写字母 ---
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
    
            // --- 小写字母（常用 UI）---
    case 'c': return { {0,0,0b01110,0b10000,0b10000,0b10000,0b01110} };
    case 'e': return { {0,0,0b01110,0b10001,0b11111,0b10000,0b01110} };
    case 'i': return { {0,0b00100,0,0b01100,0b00100,0b00100,0b01110} };
    case 'o': return { {0,0,0b01110,0b10001,0b10001,0b10001,0b01110} };
    case 'r': return { {0,0,0b10110,0b11001,0b10000,0b10000,0b10000} };
    case 's': return { {0,0,0b01111,0b10000,0b01110,0b00001,0b11110} };
    case 't': return { {0b00100,0b00100,0b11111,0b00100,0b00100,0b00100,0b00010} };
    case 'x': return { {0,0,0b10001,0b01010,0b00100,0b01010,0b10001} };

    default:  // 未定义字符 → 用方框 '?'
        return { {0b11111,0b10001,0b10101,0b10001,0b10101,0b10001,0b11111} };
    }
}


// 画单个字符（scale 可放大像素，>=1）
static void drawChar(GamesEngineeringBase::Window& canvas, int x, int y, char ch,
    unsigned char r, unsigned char g, unsigned char b, int scale = 2) {
    Glyph5x7 gph = getGlyph(ch);
    for (int row = 0; row < 7; ++row) {
        unsigned char bits = gph.row[row];
        for (int col = 0; col < 5; ++col) {
            if (bits & (1 << (4 - col))) {
                // 画一个 scale×scale 的方块像素
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

// 画字符串
static void drawString(GamesEngineeringBase::Window& canvas, int x, int y, const char* text,
    unsigned char r, unsigned char g, unsigned char b, int scale = 2, int letterSpacing = 1) {
    int cursor = 0;
    while (text[cursor]) {
        drawChar(canvas, x, y, text[cursor], r, g, b, scale);
        // 每个字符宽度=5像素，间距 1 像素；乘以 scale
        x += (5 * scale) + letterSpacing;
        ++cursor;
    }
}

// 血量条（左上角）
static void drawHPBar(GamesEngineeringBase::Window& canvas, int x, int y,
    int curHP, int maxHP, int w = 200, int h = 16) {
    if (maxHP <= 0) maxHP = 1;
    // 背景（深红）
    for (int i = 0; i < w; ++i)
        for (int j = 0; j < h; ++j)
            canvas.draw(x + i, y + j, 120, 20, 20);
    // 前景（绿）
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
    unsigned int xIndex;
    unsigned int yIndex;

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
                // 读取元数据部分
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
                // 读取地图数据部分
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

    /*bool roll(int X, int Y) {

    }*/

    
    // 根据像素坐标判断位置是否可走
    bool Passable(unsigned int worldX,unsigned int worldY) {
        int tileX = worldX / tileWidth  % tilesWide;
        int tileY = worldY / tileHeight  % tilesHigh;
        //cout << tileX << ", " << tileY << endl;

        //// 限制范围
        //if (tileX < 0 || tileY < 0 || tileX >= (int)tilesWide || tileY >= (int)tilesHigh)
        //    return false; // 出界都视为不可通行

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
        // ---- 1️⃣ 计算相机视野内的tile范围 ----
        int startTileX = camX / tileWidth;
        int startTileY = camY / tileHeight;
        int endTileX = (camX + canvas.getWidth()) / tileWidth + 1;
        int endTileY = (camY + canvas.getHeight()) / tileHeight + 1;

        // ----  遍历当前可见区域的所有tile ----
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
        //cout << "Hero: " << x << ", " << y << endl;
        speed = 300;

        maxHealth = 100;
        health = maxHealth;
        radius = sprite.width / 2;  // 碰撞半径

    }


    void back(GamesEngineeringBase::Window& canvas, int _x, int _y, int mapW, int mapH) {
        x -= _x;
        y -= _y;

        if (x < 0) x += mapW;
        if (y < 0) y += mapH;
        if (x >= mapW) x -= mapW;
        if (y >= mapH) y -= mapH;

    }

    void update(GamesEngineeringBase::Window& canvas, int _x, int _y,int mapW, int mapH) {

        x += _x;
        y += _y;
        if (x < 0) x += mapW;
        if (y < 0) y += mapH;
        if (x >= mapW) x -= mapW;
        if (y >= mapH) y -= mapH;
       
        // 限制在窗口范围内
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
                    continue; // 跳过透明区域

                int screenX = x + xx;
                int screenY = y + yy;

                //防止越界写入
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
                    continue; // 跳过透明区域
                int X = sx + xx, Y = sy + yy;
                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
                    canvas.draw(X, Y, sprite.atUnchecked(xx, yy));
            }

        //cout << "DrawAt" << endl;
    }


    //hero受到伤害（碰撞 子弹）
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


// ================== 公共：Projectile  ==================
struct Projectile {
    float x, y, vx, vy;
    int damage;
    bool fromHero, alive;
    Projectile() : x(0), y(0), vx(0), vy(0), damage(1), fromHero(false), alive(false) {}
    Projectile(float X, float Y, float VX, float VY, int dmg, bool hero)
        : x(X), y(Y), vx(VX), vy(VY), damage(dmg), fromHero(hero), alive(true) {
    }
};

// ================== 父类：BaseNPC（抽象） ==================
class BaseNPC {
protected:
    float x, y;
    float speed;
    unsigned int hp, maxHP;
    float radius;
    bool alive;
    GamesEngineeringBase::Image sprite;

public:
    BaseNPC() : x(0), y(0), speed(0), hp(1), maxHP(1), radius(16), alive(false) {}
    virtual ~BaseNPC() {}

    virtual void init(float startX, float startY) { x = startX; y = startY; alive = true; }
    virtual void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf, Projectile* bullets, int& bulletCount, int MAX_PROJECTILES) = 0;
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
    }

    virtual void takeDamage(int dmg) { 
        hp -= dmg; 
        if (hp <= 0) { 
            hp = 0; 
            alive = false; 
        } 
    }

    bool isAlive() const { return alive; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getRadius() const { return radius; }

protected:
    // 通用“直线追踪”行为
   
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
        if (!sprite.load(file)) { /* 可加日志 */ }
    }
};

// ================== 子类 1：Normal ==================
class NormalNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 60; hp = maxHP = 3; radius = 16; loadSprite("Resources/npc1.png");
    }
    void update(float dt, float heroX, float heroY, int mapW, int mapH,bool inf, Projectile*, int&, int) override {
        chase(dt, heroX, heroY, mapW, mapH, inf);
    }
};

// ================== 子类 2：Runner ==================
class RunnerNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 120; hp = maxHP = 2; radius = 14; loadSprite("Resources/npc2.png");
    }
    void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf, Projectile*, int&, int) override {
        chase(dt, heroX, heroY, mapW, mapH, inf);
    }
};

// ================== 子类 3：Tank ==================
class TankNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 35; hp = maxHP = 6; radius = 20; loadSprite("Resources/npc3.png");
    }
    void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf, Projectile*, int&, int) override {
        chase(dt, heroX, heroY, mapW, mapH, inf);
    }
};

// ================== 子类 4：Turret（静止发射弹幕） ==================
class TurretNPC : public BaseNPC {
    float shootCD;
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 0; hp = maxHP = 4; radius = 18; loadSprite("Resources/npc4.png");
        shootCD = 1.2f;
    }
    
    void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf, Projectile* bullets, int& bulletCount, int MAX_PROJECTILES) override {
        // 不移动，只发射子弹
        //chase(dt, heroX, heroY, 1000, 1000, true);

        shootCD -= dt;
        if (shootCD <= 0.f && bulletCount < MAX_PROJECTILES) {
            shootCD = 1.2f;
            float dx = heroX - x, dy = heroY - y;
            float len = (float)sqrt(dx * dx + dy * dy); if (len < 0.001f) return;
            dx /= len; dy /= len;
            bullets[bulletCount++] = Projectile(x, y, dx * 200.f, dy * 200.f, 1, false);
        }
    }
};

// ================== NPCManager（固定数组，不用 STL） ==================
#define MAX_NPCS 200
#define MAX_PROJECTILES 500

class NPCManager {
    

public:

    BaseNPC* slots[MAX_NPCS];   // 指向多态对象
    int count;

    Projectile enemyBullets[MAX_PROJECTILES];
    int bulletCount;

    NPCManager() : count(0), bulletCount(0) {
        for (int i = 0; i < MAX_NPCS; ++i) slots[i] = 0;
        for (int i = 0; i < MAX_PROJECTILES; ++i) enemyBullets[i].alive = false;
    }

    ~NPCManager() { clear(); }

    void clear() {
        for (int i = 0; i < MAX_NPCS; ++i) { if (slots[i]) { delete slots[i]; slots[i] = 0; } }
        count = 0; bulletCount = 0;
    }

    // 在相机外随机生成：type = 0..3（Normal/Runner/Tank/Turret）
    void spawnOutsideCamera(int type, int camX, int camY, int camW, int camH,int mapW, int mapH) {
        int idx = findFree();
        if (idx < 0) return;

        float spawnX, spawnY;
        int margin = 100;
        int side = rand() % 4;
        switch (side) {
        case 0: 
            spawnX = camX + (rand() % camW);
            spawnY = camY - margin;
            spawnX = wrap(spawnX, mapW);
            spawnY = wrap(spawnY, mapH);
            break;             // 上
        case 1: spawnX = camX + (rand() % camW); 
            spawnY = camY + camH + margin; 
            spawnX = wrap(spawnX, mapW);
            spawnY = wrap(spawnY, mapH);
            break;      // 下
        case 2: spawnX = camX - margin; spawnY = camY + (rand() % camH); spawnX = wrap(spawnX, mapW);
            spawnY = wrap(spawnY, mapH); break;             // 左
        default:spawnX = camX + camW + margin; spawnY = camY + (rand() % camH); spawnX = wrap(spawnX, mapW);
            spawnY = wrap(spawnY, mapH); break;      // 右
        }
        // inifinite add mod
        BaseNPC* p = 0;
        if (type == 0) p = new NormalNPC();
        else if (type == 1) p = new RunnerNPC();
        else if (type == 2) p = new TankNPC();
        else                p = new TurretNPC();

        p->init(spawnX, spawnY);
        slots[idx] = p;
        if (idx + 1 > count) count = idx + 1;

        std::cout << "Spawn NPC type " << type
            << " at (" << spawnX << "," << spawnY << ")\n";

    }

    
    int updateAll(float dt, float heroWorldX, float heroWorldY, int mapW, int mapH, bool inf) {

        // 更新子弹
        for (int i = 0; i < bulletCount; ++i) {
            if (!enemyBullets[i].alive) continue;
            enemyBullets[i].x += enemyBullets[i].vx * dt;
            enemyBullets[i].y += enemyBullets[i].vy * dt;

            enemyBullets[i].x = wrap(enemyBullets[i].x, mapW);
            enemyBullets[i].y = wrap(enemyBullets[i].y, mapH);
            // 超出可简单失效（或做边界循环）
            if (enemyBullets[i].x < -1000 || enemyBullets[i].y < -1000) enemyBullets[i].alive = false;
        }

        int kills = 0;
        for (int i = 0; i < count; ++i) {
            if (!slots[i]) continue;
            if (!slots[i]->isAlive()) {
                delete slots[i]; slots[i] = 0;
                kills++;
                continue;
            }
            slots[i]->update(dt, heroWorldX, heroWorldY,mapW, mapH, inf, enemyBullets, bulletCount, MAX_PROJECTILES);
        }
        //cout << "kills " << kills << endl;
        // 更新子弹...
        return kills; // 返回击杀数量
    }

    void drawAll(GamesEngineeringBase::Window& canvas, int camX, int camY, int mapW, int mapH) {
        // 敌人
        for (int i = 0; i < count; ++i) {
            if (slots[i]) {
                slots[i]->draw(canvas, camX, camY, mapW, mapH);
            } 
        }

        // 敌人子弹（简单画点）
        for (int i = 0; i < bulletCount; ++i) {
            if (!enemyBullets[i].alive) continue;
            int sx = (int)(enemyBullets[i].x - camX);
            int sy = (int)(enemyBullets[i].y - camY);
            if (0 <= sx && sx < (int)canvas.getWidth() && 0 <= sy && sy < (int)canvas.getHeight())
                canvas.draw(sx, sy, 255, 0, 0);
        }
    }

    // Hero 被子弹击中（圆形碰撞）
    bool heroHitByBullet(float heroX, float heroY, float heroR) {
        bool hit = false;
        for (int i = 0; i < bulletCount; ++i) {
            if (!enemyBullets[i].alive) continue;
            float dx = heroX - enemyBullets[i].x, dy = heroY - enemyBullets[i].y;
            if (dx * dx + dy * dy < heroR * heroR) { enemyBullets[i].alive = false; hit = true; }
        }
        return hit;
    }


    // Hero 子弹击中敌人
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
                    std::cout << "💢 Enemy hit! HP reduced.\n";
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
    int score; // 分数统计
    


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
    float heroShootInterval = 0.8f; // 英雄射速

    // === 🧩 子弹参数 ===
    float bulletSpeed = 420.f;
    float bulletDamage = 1.f;

public:

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
       
        
        heroBulletCount = 0;
        heroShootCD = 0;
        npcSpawnTimer = 0;
        npcSpawnInterval = 3.0f; // 初始3秒生成敌人

        gameTime = 0;
        mapW = w.getPixelWidth();
        mapH = w.getPixelHeight();
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
            if (!switchPressed) { // 防止连按
                infiniteMode = !infiniteMode;
                cout << "Map mode switched to : "
                    << (infiniteMode ? "Infinite" : "Fixed Boundary") << endl;
            }
            switchPressed = true;
        }
        else switchPressed = false;
    }

    bool isPassable(int X, int Y, int width, int height) {
        
        // 检查英雄矩形的四个角
        int corners[4][2] = {
            {X, Y},
            {X + width, Y},
            {X, Y + height},
            {X + width, Y + height}
        };


        for (int i = 0; i < 4; ++i) {
            int cx = corners[i][0];
            int cy = corners[i][1];
            if (!w.Passable(cx, cy))
                return false; // 只要有一个角在水 tile，就视为不可通行
        }
        return true;

    }

    int updateX(int _x) {
        // 无限循环模式
        /*mapW = w.getPixelWidth();
        mapH = w.getPixelHeight();*/

        if (_x < 0) _x += mapW;
        //if (_y < 0) _y += mapH;
        if (_x >= mapW) _x -= mapW;
        //if (_y > mapH) _y -= mapH;

        return _x;

    }

    int updateY(int _y) {
        // 无限循环模式
        /*mapW = w.getPixelWidth();
        mapH = w.getPixelHeight();*/

        //if (_x < 0) _x += mapW;
        if (_y < 0) _y += mapH;
        //if (_x > mapW) _x -= mapW;
        if (_y >= mapH) _y -= mapH;

        return _y;

    }
    

    void update(GamesEngineeringBase::Window& canvas, float dt) {
        int dx = 0, dy = 0;
        int move = 2;
        if (canvas.keyPressed('W')) dy -= move;
        if (canvas.keyPressed('S')) dy += move;
        if (canvas.keyPressed('A')) dx -= move;
        if (canvas.keyPressed('D')) dx += move;

        switchMapMode(canvas);

        // === 更新 Hero 移动 ===
        hero.update(canvas, dx, dy, mapW, mapH);
        if (!isPassable(hero.x, hero.y, hero.sprite.width, hero.sprite.height)) {
            hero.back(canvas, dx, dy, mapW, mapH);
        }
        //cout << "Hero: " << hero.x << ", " << hero.y << endl;

        // === 相机跟随 Hero ===
        cam.x = hero.x + hero.sprite.width / 2 - canvas.getWidth() / 2;
        cam.y = hero.y + hero.sprite.height / 2 - canvas.getHeight() / 2;

        if (infiniteMode) {
            cam.x = wrap(cam.x, mapW);
            cam.y = wrap(cam.y, mapH);
        }
        else {
            //todo
            if (cam.x < 0) cam.x = 0;
            if (cam.y < 0) cam.y = 0;
            if (cam.x + cam.width > mapW) cam.x = mapW - cam.width;
            if (cam.y + cam.height > mapH) cam.y = mapH - cam.height;
        }


        // === 敌人生成 ===
        npcSpawnTimer += dt;
        if (npcSpawnTimer >= npcSpawnInterval) {
            int type = rand() % 4;
            npcManager.spawnOutsideCamera(type, cam.x, cam.y, cam.width, cam.height, mapW, mapH);
            npcSpawnTimer = 0;
            if (npcSpawnInterval > 1.0f) npcSpawnInterval -= 0.05f;
        }

        // === 更新敌人 ===
        int kills = npcManager.updateAll(dt, hero.x, hero.y, mapW, mapH, infiniteMode);
        score += kills * 10;

        // === 英雄的世界坐标中心 ===
        float heroWorldX = hero.x + hero.sprite.width * 0.5f;
        float heroWorldY = hero.y + hero.sprite.height * 0.5f;

        /*if (infiniteMode) {
            heroWorldX = updateX(heroWorldX);
            heroWorldY = updateY(heroWorldY);
        }*/

        
        // ====== 自动射击最近敌人（世界坐标修正版） ======
        //heroShootCD -= dt;
        //if (heroShootCD <= 0.f && heroBulletCount < MAX_HERO_PROJECTILES) {
        //    float best = 1e30f;
        //    float targetX = 0, targetY = 0;

        //    // 选最近敌人（考虑无限地图最短距离）
        //    for (int i = 0; i < MAX_NPCS; ++i) {
        //        BaseNPC* e = npcManager.slots[i];
        //        if (!e || !e->isAlive()) continue;
        //        float dx = shortestDelta(e->getX(), heroWorldX, mapW);
        //        float dy = shortestDelta(e->getY(), heroWorldY, mapH);
        //        float d2 = dx * dx + dy * dy;
        //        if (d2 < best) { best = d2; targetX = e->getX(); targetY = e->getY(); }
        //    }

        //    if (best < 1e8f) {
        //        // 从英雄中心点发射（世界坐标）
        //        float dx = shortestDelta(targetX, heroWorldX, mapW);
        //        float dy = shortestDelta(targetY, heroWorldY, mapH);
        //        float len = sqrtf(dx * dx + dy * dy);
        //        if (len > 0.001f) { dx /= len; dy /= len; }

        //        heroBullets[heroBulletCount++] =
        //            Projectile(heroWorldX, heroWorldY, dx * bulletSpeed, dy * bulletSpeed, bulletDamage, true);

        //        heroShootCD = heroShootInterval;
        //        std::cout << "🔥 Hero fired (world) X=" << heroWorldX << " Y=" << heroWorldY << std::endl;
        //    }
        //}


        // ====== 🧩 更新 Hero 子弹（世界坐标 + 包裹） ======
        /*for (int i = 0; i < heroBulletCount; ++i) {
            if (!heroBullets[i].alive) continue;

            heroBullets[i].x += heroBullets[i].vx * dt;
            heroBullets[i].y += heroBullets[i].vy * dt;

            if (infiniteMode) {
                heroBullets[i].x = wrap(heroBullets[i].x, (float)mapW);
                heroBullets[i].y = wrap(heroBullets[i].y, (float)mapH);
            }
            else {
                if (heroBullets[i].x < 0 || heroBullets[i].x >= mapW ||
                    heroBullets[i].y < 0 || heroBullets[i].y >= mapH)
                    heroBullets[i].alive = false;
            }
        }*/


        // ====== 检查 Hero 子弹命中敌人 ======
        npcManager.checkHeroProjectilesHit(heroBullets, heroBulletCount);

        // ====== 检查 Hero 被敌人子弹击中 ======
        if (npcManager.heroHitByBullet(heroWorldX, heroWorldY, hero.radius)) {
            hero.takeDamage(1);
            std::cout << "Hero hit by enemy bullet!\n";
        }

        // ====== Hero vs 敌人碰撞 ======
        for (int i = 0; i < npcManager.count; i++) {
            BaseNPC* e = npcManager.slots[i];
            if (!e || !e->isAlive()) continue;

            float dx = heroWorldX - e->getX();
            float dy = heroWorldY - e->getY();
            float distSq = dx * dx + dy * dy;
            float rSum = hero.radius + e->getRadius();

            if (distSq < rSum * rSum) {
                //hero.takeDamage(1);
            }
        }

        // ====== 检查 Game Over ======
        if (!gameOver && hero.isDead()) {
            gameOver = true;
            gameOverTimer = 3.0f;
        }

        //std::cout << "Hero.x=" << hero.x << " Cam.x=" << cam.x << " BulletStartX=" << heroWorldX << std::endl;

    }


    // ===== 绘制虚拟相机边框 =====
    void drawCameraBox(GamesEngineeringBase::Window& canvas, Camera cam) {
        int cx = 0;                // 相机在屏幕左上角（视图原点）
        int cy = 0;
        int cw = cam.width;
        int ch = cam.height;

        // 边框颜色（蓝色）
        unsigned char R = 50, G = 120, B = 255;

        // 画外框
        for (int x = 0; x < cw; ++x) {
            if (cy >= 0 && cy < (int)canvas.getHeight()) {
                canvas.draw(cx + x, cy, R, G, B);             // 上边
                canvas.draw(cx + x, cy + ch - 1, R, G, B);    // 下边
            }
        }
        for (int y = 0; y < ch; ++y) {
            if (cx >= 0 && cx < (int)canvas.getWidth()) {
                canvas.draw(cx, cy + y, R, G, B);             // 左边
                canvas.draw(cx + cw - 1, cy + y, R, G, B);    // 右边
            }
        }

        // 中心十字线（表示相机中心）
        int centerX = cw / 2;
        int centerY = ch / 2;
        for (int i = -10; i <= 10; ++i) {
            if (centerX + i >= 0 && centerX + i < (int)canvas.getWidth())
                canvas.draw(centerX + i, centerY, 255, 255, 0); // 黄线横
            if (centerY + i >= 0 && centerY + i < (int)canvas.getHeight())
                canvas.draw(centerX, centerY + i, 255, 255, 0); // 黄线竖
        }
    }


    void draw(GamesEngineeringBase::Window& canvas) {
        
        
        /*w.draw(canvas, cam.x, cam.y);
        hero.draw(canvas);*/
        
        
        w.draw(canvas, cam.x, cam.y);
        npcManager.drawAll(canvas, cam.x, cam.y, mapW, mapH);

        
        
         
        // ===== Hero 子弹 =====
        for (int i = 0; i < heroBulletCount; ++i) {
            if (!heroBullets[i].alive) continue;
            int sx = (int)(heroBullets[i].x - cam.x) ;
            int sy = (int)(heroBullets[i].y - cam.y) ;
            
            // 子弹半径（越大越明显）
            const int radius = 4;

            // 用一个简单的圆形绘制算法
            for (int dx = -radius; dx <= radius; ++dx) {
                for (int dy = -radius; dy <= radius; ++dy) {
                    if (dx * dx + dy * dy <= radius * radius) { // 圆形范围
                        int px = sx + dx;
                        int py = sy + dy;

                        if (px >= 0 && px < (int)canvas.getWidth() &&
                            py >= 0 && py < (int)canvas.getHeight()) {
                            canvas.draw(px, py, 255, 0, 0); // 红色
                        }
                    }
                }
            }
        }

        // hero始终画在屏幕中心
        hero.drawAt(canvas, cam.width / 2 - hero.sprite.width / 2,
            cam.height / 2 - hero.sprite.height / 2);

        //drawCameraBox(canvas, cam);

    }

    void drawUI(GamesEngineeringBase::Window& canvas, int score, float fps, int heroHP, int heroMaxHP) {
        // =============================
        // 🔹 1. 血量条区域设置
        // =============================
        int barX = 20;
        int barY = 20;
        int barWidth = 200;
        int barHeight = 16;

        // 背景条（暗红）
        for (int y = 0; y < barHeight; ++y) {
            for (int x = 0; x < barWidth; ++x)
                canvas.draw(barX + x, barY + y, 60, 0, 0);
        }

        // =============================
        // 🔹 2. 当前血量条宽度计算
        // =============================
        float ratio = (float)heroHP / (float)heroMaxHP;
        if (ratio < 0) ratio = 0;
        if (ratio > 1) ratio = 1;
        int filled = (int)(barWidth * ratio);

        // =============================
        // 🔹 3. 动态颜色过渡（绿→黄→红）
        // =============================
        int r, g;
        if (ratio > 0.5f) {           // 绿色到黄色
            r = (int)(255 * (1.0f - ratio) * 2.0f);
            g = 255;
        }
        else {                      // 黄色到红色
            r = 255;
            g = (int)(255 * ratio * 2.0f);
        }

        // =============================
        // 🔹 4. 绘制当前血量条
        // =============================
        for (int y = 0; y < barHeight; ++y) {
            for (int x = 0; x < filled; ++x)
                canvas.draw(barX + x, barY + y, r, g, 0);
        }

        // 边框（浅灰线）
        for (int x = 0; x < barWidth; ++x) {
            canvas.draw(barX + x, barY, 180, 180, 180);
            canvas.draw(barX + x, barY + barHeight - 1, 180, 180, 180);
        }
        for (int y = 0; y < barHeight; ++y) {
            canvas.draw(barX, barY + y, 180, 180, 180);
            canvas.draw(barX + barWidth - 1, barY + y, 180, 180, 180);
        }

        // =============================
        // 🔹 5. 血量文字显示
        // =============================

        char hpText[32];
        sprintf_s(hpText, sizeof(hpText), "HP: %d / %d", heroHP, heroMaxHP);
        drawString(canvas, barX + 5, barY + 22, hpText, 255, 255, 255, 2, 1);

        // =============================
        // 🔹 6. 显示 “Score”
        // =============================
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
        drawString(canvas, canvas.getWidth() - 280, 20, bufScore, 255, 255, 255, 2, 2);

        // =============================
        // 🔹 7. 显示 “FPS”
        // =============================
        int ifps = (int)(fps + 0.5f);
        int fd[6]; int fcnt = 0; if (ifps == 0) fd[fcnt++] = 0;
        while (ifps > 0 && fcnt < 6) { fd[fcnt++] = ifps % 10; ifps /= 10; }
        char bufFPS[24]; idx = 0;
        bufFPS[idx++] = 'F'; bufFPS[idx++] = 'P'; bufFPS[idx++] = 'S';
        bufFPS[idx++] = ':'; bufFPS[idx++] = ' ';
        for (int i = fcnt - 1; i >= 0; --i) bufFPS[idx++] = '0' + fd[i];
        bufFPS[idx] = '\0';
        drawString(canvas, canvas.getWidth() - 280, 50, bufFPS, 255, 220, 40, 2, 2);
    }


   

    void gameOverScreen(GamesEngineeringBase::Window& canvas, float dt) {
        // 半透明黑背景
        for (int y = 0; y < (int)canvas.getHeight(); y++)
            for (int x = 0; x < (int)canvas.getWidth(); x++)
                canvas.draw(x, y, 0, 0, 100);  // RGBA简写，若不支持透明，就直接黑色

        // 居中绘制 GAME OVER 字样（用 drawString 函数）
        int centerX = (int)canvas.getWidth() / 2 - 120;
        int centerY = (int)canvas.getHeight() / 2 - 20;
        drawString(canvas, centerX, centerY, "GAME OVER", 255, 0, 0, 4, 2);

        // 显示提示
        drawString(canvas, centerX - 60, centerY + 60, "Press ESC to Exit", 255, 255, 255, 2, 1);

        // 倒计时退出
        gameOverTimer -= dt;
    }




private:
    // === 工具函数 ===
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
        }

        if (!m.gameOver) {
            m.update(canvas, dt);
            m.draw(canvas);

            m.drawUI(canvas, /*score=*/m.getScore(), fps, /*HP=*/m.getHp(), /*MaxHP=*/m.getMaxHp());
        }
        else {
            m.gameOverScreen(canvas, dt);
        }
        

        canvas.present();
    }
    return 0;

}