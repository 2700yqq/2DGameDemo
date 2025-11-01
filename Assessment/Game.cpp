//#include "iostream"
//#include <algorithm>
//#include "GamesEngineeringBase.h"
//#include <vector>
//#include <cstdlib>
//#include <ctime>
//
//using namespace std;
//
//
//class Player {
//public:
//    int x, y;
//    int speed;
//    GamesEngineeringBase::Image sprite;
//
//    Player() {
//        sprite.load("Resources/hero.png");
//
//        if (!sprite.load("Resources/hero.png")) {
//            std::cerr << "Failed to load hero.png" << std::endl;
//        }
//
//        x = 512; y = 384;
//        speed = 300;
//    }
//
//    void update(GamesEngineeringBase::Window& w, float dt) {
//        int move = static_cast<int>(speed * dt);
//        if (w.keyPressed('W')) y -= move;
//        if (w.keyPressed('S')) y += move;
//        if (w.keyPressed('A')) x -= move;
//        if (w.keyPressed('D')) x += move;
//
//        // 限制在窗口范围内
//        if (x < 0) x = 0;
//        if (x > (int)w.getWidth() - (int)sprite.width)
//            x = (int)w.getWidth() - (int)sprite.width;
//
//        if (y < 0) y = 0;
//        if (y > (int)w.getHeight() - (int)sprite.height)
//            y = (int)w.getHeight() - (int)sprite.height;
//
//    }
//
//    void draw(GamesEngineeringBase::Window& w) {
//        for (unsigned int yy = 0; yy < sprite.height; yy++)
//            for (unsigned int xx = 0; xx < sprite.width; xx++)
//                w.draw(x + xx, y + yy, sprite.atUnchecked(xx, yy));
//    }
//                   
//};
//
//
//struct Enemy {
//    int x, y;
//    float speed;
//    GamesEngineeringBase::Image sprite;
//
//    Enemy(GamesEngineeringBase::Window& w) {
//        sprite.load("Resources/enemy.png");
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
//        for (unsigned int yy = 0; yy < sprite.height; yy++)
//            for (unsigned int xx = 0; xx < sprite.width; xx++)
//                if (sprite.alphaAtUnchecked(xx, yy) > 0)
//                    w.draw(x + xx, y + yy, sprite.atUnchecked(xx, yy));
//    }
//};
//
//class Game {
//public:
//    GamesEngineeringBase::Window canvas;
//    GamesEngineeringBase::Timer timer;
//    bool running = true;
//    Player player;
//    std::vector<Enemy> enemies;
//    GamesEngineeringBase::Image background;
//
//
//    Game() {
//        srand(static_cast<unsigned int>(time(0)));
//        canvas.create(1024, 1024, "WM908 Game Framework");
//
//        background.load("Resources/landscape.png");
//        
//        enemies.push_back(Enemy(canvas));
//
//        /*for (int i = 0; i < 5; ++i) {
//            enemies.push_back(Enemy(canvas));
//        }*/
//
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
//        for (int y = 0; y < canvas.getHeight(); y++) {
//            for (int x = 0; x < canvas.getWidth(); x++) {
//                // 计算背景图对应像素坐标（浮点比例）
//                int srcX = x * background.width / canvas.getWidth();
//                int srcY = y * background.height / canvas.getHeight();
//                canvas.draw(x, y, background.atUnchecked(srcX, srcY));
//            }
//        }
//                
//
//        player.draw(canvas);
//        //for (auto& e : enemies) e.draw(canvas);
//        canvas.present();
//    };
//};
//
//int main() {
//    Game game;
//    game.run();
//    return 0;
//}


#include "iostream"
#include "GamesEngineeringBase.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// === TILE SYSTEM =============================================================

enum class TileType {
    Grass = 0,
    Sand = 1,
    Stone = 2,
    Water = 3
};

struct Tile {
    GamesEngineeringBase::Image sprite;
    TileType type;

    Tile() = default;
    Tile(const Tile&) = delete;            // 禁止拷贝
    Tile& operator=(const Tile&) = delete; // 禁止赋值
    Tile(Tile&&) noexcept = default;       // 允许移动
    Tile& operator=(Tile&&) noexcept = default;
};


struct TileMap {
    int tilesWide = 0;
    int tilesHigh = 0;
    int tileWidth = 0;
    int tileHeight = 0;
    std::vector<std::vector<int>> data;
};


TileMap loadTileFile(const std::string& filename) {
    TileMap map;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open tile file: " << filename << std::endl;
        return map;
    }

    std::string line;
    bool readingData = false;

    while (std::getline(file, line)) {
        // 去掉首尾空格
        if (line.empty()) continue;

        // 判断是否进入地图数据部分
        if (line.rfind("layer", 0) == 0) {
            readingData = true;
            continue;
        }

        if (!readingData) {
            // 读取元数据部分
            std::stringstream ss(line);
            std::string key;
            ss >> key;

            if (key == "tileswide") ss >> map.tilesWide;
            else if (key == "tileshigh") ss >> map.tilesHigh;
            else if (key == "tilewidth") ss >> map.tileWidth;
            else if (key == "tileheight") ss >> map.tileHeight;


        }
        else {
            // 读取地图数据部分
            std::stringstream ss(line);
            std::string num;
            std::vector<int> row;

            while (std::getline(ss, num, ',')) {
                if (!num.empty())
                    row.push_back(std::stoi(num));
            }

            if (!row.empty())
                map.data.push_back(row);
        }
    }

    file.close();

    // 如果 tile 宽高没指定，使用默认值
    if (map.tileWidth == 0) map.tileWidth = 32;
    if (map.tileHeight == 0) map.tileHeight = 32;

    map.tilesHigh = (int)map.data.size();
    map.tilesWide = map.tilesHigh > 0 ? (int)map.data[0].size() : 0;

    std::cout << "Loaded tile map: " << map.tilesWide << "x" << map.tilesHigh
        << " tiles (" << map.tileWidth << "x" << map.tileHeight << " px)" << std::endl;

    return map;
}


std::vector<Tile> loadTilesFromFile(const std::string& filename) {
    std::vector<Tile> tiles;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "❌ Failed to open tile definition file: " << filename << std::endl;
        return tiles;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string idStr, path;
        std::getline(ss, idStr, ',');
        std::getline(ss, path);

        // 去掉多余空格
        idStr.erase(remove_if(idStr.begin(), idStr.end(), ::isspace), idStr.end());
        path.erase(remove_if(path.begin(), path.end(), ::isspace), path.end());

        if (idStr.empty() || path.empty()) {
            std::cerr << "⚠️ Skipping malformed line " << lineNumber << " in " << filename << std::endl;
            continue;
        }

        int id = std::stoi(idStr);
        Tile tile;
        tile.type = static_cast<TileType>(id);

        // 尝试直接加载
        bool ok = tile.sprite.load(path.c_str());


        if (!ok) {
            std::cerr << "❌ Failed to load tile image: " << path
                << "  (line " << lineNumber << ")" << std::endl;
            continue; // 跳过此 tile，避免崩溃
        }

        // 使用移动语义插入，避免复制错误
        tiles.emplace_back(std::move(tile));
    }

    file.close();

    std::cout << "✅ Successfully loaded " << tiles.size()
        << " tiles from " << filename << std::endl;

    return tiles;
}


bool isPassable(TileType t, bool isHero) {
    if (t == TileType::Water && isHero)
        return false;
    return true;
}

// === PLAYER / ENEMY ==========================================================

class Hero {
public:
    int x, y;
    int speed;
    GamesEngineeringBase::Image sprite;

    Hero() {
        sprite.load("Resources/hero.png");

        if (!sprite.load("Resources/hero.png")) {
            std::cerr << "Failed to load hero.png" << std::endl;
        }

        x = 512;
        y = 384;
        speed = 200;
    }

    void update(GamesEngineeringBase::Window& w, float dt, TileMap& map, std::vector<Tile>& tiles) {
        int move = static_cast<int>(speed * dt);
        int nextX = x, nextY = y;

        if (w.keyPressed('W')) nextY -= move;
        if (w.keyPressed('S')) nextY += move;
        if (w.keyPressed('A')) nextX -= move;
        if (w.keyPressed('D')) nextX += move;

        // 碰撞检测：Hero 不能进入 Water
        int tileX = (nextX + sprite.width / 2) / 32;
        int tileY = (nextY + sprite.height / 2) / 32;
        if (tileY >= 0 && tileY < map.tileHeight && tileX >= 0 && tileX < map.tileWidth) {
            TileType t = tiles[map.data[tileY][tileX]].type;
            if (!isPassable(t, true)) {
                return; // 阻止移动
            }
        }

        x = nextX;
        y = nextY;
    }

    void draw(GamesEngineeringBase::Window& w, int camX, int camY) {
        for (unsigned int yy = 0; yy < sprite.height; yy++)
            for (unsigned int xx = 0; xx < sprite.width; xx++)
                if (sprite.alphaAtUnchecked(xx, yy) > 0)
                    w.draw(x + xx - camX, y + yy - camY, sprite.atUnchecked(xx, yy));
    }
};

// === TILEMAP DRAWING =========================================================

void drawTileMap(GamesEngineeringBase::Window& w, TileMap& map, std::vector<Tile>& tiles,
    int cameraX, int cameraY, bool infinite) {

    int startX = cameraX / map.tileWidth;
    int startY = cameraY / map.tileHeight;
    int endX = (cameraX + w.getWidth()) / map.tileWidth + 1;
    int endY = (cameraY + w.getHeight()) / map.tileHeight + 1;

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            int mapX = x;
            int mapY = y;

            if (infinite) {
                mapX = (x % map.tilesWide + map.tilesWide) % map.tilesWide;
                mapY = (y % map.tilesHigh + map.tilesHigh) % map.tilesHigh;
            }
            else {
                if (x < 0 || x >= map.tilesWide || y < 0 || y >= map.tilesHigh)
                    continue;
            }

            int tileID = map.data[mapY][mapX];
            if (tileID < 0 || tileID >= (int)tiles.size()) continue;

            Tile& t = tiles[tileID];
            for (unsigned int yy = 0; yy < t.sprite.height; yy++)
                for (unsigned int xx = 0; xx < t.sprite.width; xx++)
                    if (t.sprite.alphaAtUnchecked(xx, yy) > 0)
                        w.draw(x * map.tileWidth + xx - cameraX,
                            y * map.tileHeight + yy - cameraY,
                            t.sprite.atUnchecked(xx, yy));
        }
    }
}

// === GAME ====================================================================

class Game {
public:
    GamesEngineeringBase::Window canvas;
    GamesEngineeringBase::Timer timer;
    Hero player;
    TileMap map;
    std::vector<Tile> tiles;
    bool infiniteWorld;

    Game(bool infinite = false) : infiniteWorld(infinite) {
        srand(static_cast<unsigned int>(time(0)));
        canvas.create(1024, 768, infinite ? "Infinite World" : "Fixed World");


        // 1. 从文件加载 tile 贴图定义
        tiles = std::move(loadTilesFromFile("Resources/tileset.txt"));


        // 2. 加载 .tile 地图文件
        map = loadTileFile("Resources/tiles.txt");
    }


    void run() {
        while (!canvas.keyPressed(VK_ESCAPE)) {
            float dt = timer.dt();
            update(dt);
            render();
        }
    }

    void update(float dt) {
        player.update(canvas, dt, map, tiles);
    }


    template <typename T>
    T clamp(T value, T minVal, T maxVal) {
        return max(minVal, min(value, maxVal));
    }


    void render() {
        canvas.clear();

        int cameraX = player.x - canvas.getWidth() / 2;
        int cameraY = player.y - canvas.getHeight() / 2;

        if (!infiniteWorld) {
            cameraX = clamp(cameraX, 0, map.tileWidth * 32 - (int)canvas.getWidth());
            cameraY = clamp(cameraY, 0, map.tileHeight * 32 - (int)canvas.getHeight());
        }

        drawTileMap(canvas, map, tiles, cameraX, cameraY, infiniteWorld);
        player.draw(canvas, cameraX, cameraY);

        canvas.present();
    }
};

// === MAIN ====================================================================

//int main() {
//    // 测试固定边界版本
//    Game fixed(false);
//    fixed.run();
//
//     //测试无限地图版本
//     /*Game infinite(true);
//     infinite.run();*/
//
//    return 0;
//}
