#include "iostream"
#include <algorithm>
#include "GamesEngineeringBase.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;


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
    


public:
    world() : ts() {
        data = new unsigned int* [worldSize];
        for (int i = 0; i < worldSize; ++i)
            data[i] = new unsigned int[worldSize];


        tilesWide = worldSize;
        tilesHigh = worldSize;
        tileWidth = worldSize;
        tileHeight = worldSize;
        for (int i = 0; i < worldSize; i++) {
            for (int j = 0; j < worldSize; j++) {
                data[i][j] = rand() % tileNum;
            }
        }

    }

    //- File handling start

    world(const std::string& filename) : ts() {
        data = new unsigned int* [worldSize];
        for (int i = 0; i < worldSize; ++i)
            data[i] = new unsigned int[worldSize];


        tilesWide = worldSize;
        tilesHigh = worldSize;
        tileWidth = worldSize;
        tileHeight = worldSize;
        for (int i = 0; i < worldSize; i++) {
            for (int j = 0; j < worldSize; j++) {
                data[i][j] = rand() % tileNum;
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
                    //cout << value << endl;
                    index2++;
                }

                index1++;
                index2 = 0;
            }


        }

        fin.close();

       /* cout << index1 << " " << index2 << endl;
        for (int i = 0; i < index1; i++) {
            for (int j = 0; j < tilesHigh; j++) {
                cout << data[i][j] << " ";
            }
            cout << endl;
        }*/

       

    }

    ~world() {
        for (int i = 0; i < worldSize; ++i)
            delete[] data[i];
        delete[] data;
    }

    //- File Handling end


    void draw(GamesEngineeringBase::Window& canvas,unsigned int x, unsigned int y) {

        unsigned int offset = tileWidth / 2;
        unsigned int X = x / 384;
        unsigned int Y = y / 384;

        for (int i = 0; i < tilesWide; i++) {
            for (int j = 0; j < tilesHigh; j++) {
                unsigned int tileID = data[i][j];

                // 计算绘制位置
                int xPos = i * tileWidth;
                int yPos = j * tileHeight;

                // 绘制该 tile
                //ts[tileID].setPosition(xPos, yPos);
                ts[tileID].draw(canvas, xPos, yPos);

                // Debug输出
                 //std::cout << "Draw tile ID=" << tileID << " at (" << xPos << "," << yPos << ")\n";
            }
        }
       /* ts[data[Y % tileHeight]].draw(canvas, (canvas.getHeight() / 2) + offset);
        ts[tarray[(Y + 1) % size]].draw(canvas, offset);
        ts[tarray[(Y + 2) % size]].draw(canvas, offset - (canvas.getHeight() / 2));*/
    }

   /* void drawAlphas(GamesEngineeringBase::Window& canvas, unsigned int y) {

        unsigned int offset = y % 384;
        unsigned int Y = y / 384;

        alphas[tarray[Y % size]].draw(canvas, (canvas.getHeight() / 2) + offset);
        alphas[tarray[(Y + 1) % size]].draw(canvas, offset);
        alphas[tarray[(Y + 2) % size]].draw(canvas, offset - (canvas.getHeight() / 2));
    }*/


    // this a simple first collision that just draws the line of collision
   /* void collision(GamesEngineeringBase::Window& canvas, Plane& h, unsigned int y) {

        int Y = y / 384;
        tile& T = alphas[tarray[Y % size]];

        unsigned int yCoord = T.getHeight() - ((h.getYCollide() + y) % T.getHeight());

        for (unsigned int x = 0; x < canvas.getWidth(); x++)
            if (T.getSprite().at(x, yCoord, 0) < 1)
                canvas.draw(x, canvas.getHeight() - h.getYCollide(), 255, 0, 0);
    }

    void collision2(GamesEngineeringBase::Window& canvas, Plane& h, unsigned int y) {

        int Y = y / 384;
        tile& T = alphas[tarray[Y % size]];

        unsigned int yCoord = T.getHeight() - ((h.getYCollide() + y) % T.getHeight());

        for (unsigned int x = 0; x < h.getWidth(); x++)
            if (T.getSprite().at(x + h.getX(), yCoord, 0) < 1)
                canvas.draw(x + h.getX(), canvas.getHeight() - h.getYCollide(), 255, 0, 0);
            else
                canvas.draw(x + h.getX(), canvas.getHeight() - h.getYCollide(), 0, 255, 0);
    }*/
};


class Hero {
public:
    int x, y;
    int speed;
    GamesEngineeringBase::Image sprite;

    Hero(GamesEngineeringBase::Window& canvas, std::string filename) {
        sprite.load(filename);

        if (!sprite.load(filename)) {
            std::cerr << "Failed to load hero" << std::endl;
        }
        else {
            std::cerr << "Load hero!" << endl;
        }

        x = 672; y = 672;
        speed = 300;
    }

    void update(GamesEngineeringBase::Window& canvas, float dt) {
        int move = static_cast<int>(speed * dt);
        if (canvas.keyPressed('W')) y -= move;
        if (canvas.keyPressed('S')) y += move;
        if (canvas.keyPressed('A')) x -= move;
        if (canvas.keyPressed('D')) x += move;

        // 限制在窗口范围内
        if (x < 0) x = 0;
        if (x > (int)canvas.getWidth() - (int)sprite.width)
            x = (int)canvas.getWidth() - (int)sprite.width;

        if (y < 0) y = 0;
        if (y > (int)canvas.getHeight() - (int)sprite.height)
            y = (int)canvas.getHeight() - (int)sprite.height;

    }

    void draw(GamesEngineeringBase::Window& canvas) {
        for (unsigned int yy = 0; yy < sprite.height; yy++) {
            for (unsigned int xx = 0; xx < sprite.width; xx++) {
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
                   
};


//struct Enemy {
//    int x, y;
//    float speed;
//    GamesEngineeringBase::Image sprite;
//
//    Enemy(GamesEngineeringBase::Window& w) {
//        sprite.load("Resources/enemy.png");
//
//        if (!sprite.load("Resources/enemy.png")) {
//            std::cerr << "Failed to load enemy.png" << std::endl;
//        }
//        else {
//            std::cerr << "Load enemy!" << endl;
//        }
//
//        x =(w.getWidth() - sprite.width) > 0 ?  rand() % (w.getWidth() - sprite.width) : rand();
//
//        y = -static_cast<int>(sprite.height);
//        speed = static_cast<float>(100 + rand() % 200);
//    }
//
//    void update(float dt, GamesEngineeringBase::Window& w) {
//        y += static_cast<int>(speed * dt);
//        if (y > w.getHeight()) { // 回到顶部
//            y = -static_cast<int>(sprite.height);
//            x = rand() % (w.getWidth() - sprite.width);
//            speed = static_cast<float>(100 + rand() % 200);
//        }
//    }
//
//    void draw(GamesEngineeringBase::Window& w) {
//        for (unsigned int yy = 0; yy < sprite.height; yy++) {
//            for (unsigned int xx = 0; xx < sprite.width; xx++) {
//                int screenX = x + xx;
//                int screenY = y + yy;
//
//                //防止越界写入
//                if (screenX >= 0 && screenX < (int)w.getWidth() &&
//                    screenY >= 0 && screenY < (int)w.getHeight()) {
//                        w.draw(screenX, screenY, sprite.atUnchecked(xx, yy));
//                }
//            }
//        }
//    }
//};

//class Game {
//public:
//    GamesEngineeringBase::Window canvas;
//    GamesEngineeringBase::Timer timer;
//    bool running = true;
//    Hero hero;
//    std::vector<Enemy> enemies;
//    GamesEngineeringBase::Image background;
//
//
//    Game() {
//        srand(static_cast<unsigned int>(time(0)));
//        canvas.create(1024, 768, "WM908 Game Framework");
//
//        background.load("Resources/landscape.png");
//        player = Hero();
//
//       /* enemies.push_back(Enemy(canvas));
//
//        enemies.push_back(Enemy(canvas));*/
//
//        /*for (int i = 0; i < 2; i++) {
//            enemies.push_back(Enemy(canvas));
//        }*/
//
//    }
//    void draw(GamesEngineeringBase::Window & canvas) {
//        for (int y = 0; y < canvas.getHeight(); y++) {
//            for (int x = 0; x < canvas.getWidth(); x++) {
//                // 计算背景图对应像素坐标（浮点比例）
//                int srcX = x * background.width / canvas.getWidth();
//                int srcY = y * background.height / canvas.getHeight();
//                canvas.draw(x, y, background.atUnchecked(srcX, srcY));
//            }
//        }
//    }
//
//    void run() {
//        while (running) {
//            float dt = timer.dt();
//            handleInput();
//            update(dt);
//            render();
//        }
//    }
//
//
//private:
//    void handleInput() {
//        // 可以在这里检测键盘输入，例如：
//        if (canvas.keyPressed(VK_ESCAPE))
//            running = false;
//    };
//
//    void update(float dt) {
//        player.update(canvas, dt);
//        for (auto& e : enemies) e.update(dt, canvas);
//    };
//
//    void render() {
//        canvas.clear();
//
//        draw(canvas);
//       
//        
//        /*for (auto& e : enemies){
//            e.draw(canvas);
//        }*/
//
//        player.draw(canvas);
//        canvas.present();
//    };
//};

struct Camera {
    int x; // top-left corner of what we see in world space
    int y;
    int width;
    int height;
};




class Manager {
    Hero hero;
    world w;
    unsigned int x = 0;
    unsigned int y = 0;
    float dt;
    Camera cam;

    //bool drawAlpha = false;
public:
    Manager(GamesEngineeringBase::Window& canvas) : hero(canvas, "Resources/L.png"), w() {}
    Manager(GamesEngineeringBase::Window& canvas, string filename) : hero(canvas, "Resources/L.png"), w(filename) {}
    Manager(GamesEngineeringBase::Window& canvas, string filename, float dt) : hero(canvas, "Resources/L.png"), w(filename), dt(){
        cam.width = canvas.getWidth();
        cam.height = canvas.getHeight();
        cam.x = hero.x - cam.width / 2;
        cam.y = hero.y - cam.height / 2;
    }


    void update(GamesEngineeringBase::Window& canvas) {
        int x = 0;
        y = 0;
        //scroll
        //y += 2;
        //drawAlpha = false;
        if (canvas.keyPressed(VK_UP)) y += 5;
        if (canvas.keyPressed(VK_DOWN)) y -= 1;
        if (canvas.keyPressed(VK_LEFT)) x -= 1;
        if (canvas.keyPressed(VK_RIGHT)) x += 1;
        //if (canvas.keyPressed('A')) drawAlpha = true;
        hero.update(canvas, dt);

        // Camera follows hero
        cam.x = hero.x - cam.width / 2;
        cam.y = hero.y - cam.height / 2;

        // Clamp camera inside world bounds
        /*if (cam.x < 0) cam.x = 0;
        if (cam.y < 0) cam.y = 0;
        if (cam.x + cam.width > w.getPixelWidth())
            cam.x = w.getPixelWidth() - cam.width;
        if (cam.y + cam.height > w.getPixelHeight())
            cam.y = w.getPixelHeight() - cam.height;*/


    }
    void draw(GamesEngineeringBase::Window& canvas) {
        //if (drawAlpha) w.drawAlphas(canvas, y);
        //else w.draw(canvas, y);
        w.draw(canvas, x, y);
        hero.draw(canvas);
        //w.collision2(canvas, hero, y);

    }

};

int main() {

    srand(static_cast<unsigned int>(time(nullptr)));
    // Create a canvas window with dimensions 1024x768 and title "Example"
    GamesEngineeringBase::Window canvas;
    canvas.create(1344, 1344, "Assessment");

    // Timer object to manage time-based events, such as movement speed
    GamesEngineeringBase::Timer timer;
    float dt = timer.dt();

    //Manager m(canvas);
    Manager m(canvas, "Resources/tiles.txt", dt);
    while (true)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();
        // Clear the window for the next frame rendering
        canvas.clear();

        if (canvas.keyPressed(VK_ESCAPE)) break;

        m.update(canvas);
        m.draw(canvas);

        canvas.present();
    }
    return 0;

}