#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <string>

using namespace sf;
using namespace std;
const int h = 20;
const int H = 30;
const int W = 20;
const int N = 3;
int Ph = 5;
int Fh = 2;
int y = (H-5)*h;
Texture Pt, Ft;
int r;

class Player {
    public:
        FloatRect rect;
        float v, x;
        Sprite sprite;
        int score;
        int life;
        Player() {
            life = 3;
            score = 0;
            v = 0;
            x = W*h/2;
            rect = FloatRect(x,(H-Ph)*h,Ph*h,Ph*h);
            sprite.setTexture(Pt);
            sprite.setTextureRect(IntRect(0,0,Ph*h,Ph*h));
            sprite.setPosition(rect.left, rect.top);
        }
        ~Player(){}
        void update(float t) {
            if (x >= (W-Ph)*h) x = (W-Ph)*h;
            if (x <= 0) x = 0;
            x = x + v * t;
            rect.left = x;
            sprite.setPosition(rect.left, rect.top);
            v = 0;
        }
        bool isAlive() {
            if (life <= 0) return false;
            else return true;
            }
};

class Fruit {
    public:
        FloatRect rect;
        float v, V0, maxV, x, y;
        Sprite sprite;
        bool isDropped;
        bool onTheGround;
        int value;
        Fruit() {
            v = 0;
            maxV = V0;
            isDropped = false;
            onTheGround = false;
            x = rand() % ((W-Fh)*h);
            y = - Fh*h;
            rect = FloatRect(x,y,Fh*h,Fh*h);
            sprite.setTexture(Ft);
            sprite.setTextureRect(IntRect(0,0,Fh*h,Fh*h));
            sprite.setPosition(rect.left, rect.top);
            }
        ~Fruit(){}
        void update(float t) {
            if (y >= (H-Fh)*h) {y = (H-Fh)*h; v = 0; onTheGround = true;}
            y = y + v * t;
            rect.top = y;
            sprite.setPosition(rect.left, rect.top);
            }
        int dropFruit(float dc) {
            r = (rand() * 5) + 600;
            if ((isDropped == false)&&(dc > r)) {
                    isDropped = true;
                    v = maxV;
                    return 1;
                }
                return 0;
            }
};

class Apple: public Fruit{
public:
    Apple() : Fruit() {
        Fruit::value = 1;
        Fruit::V0 = 0.08;
        }
};
class Pear: public Fruit {
public:
    Pear() : Fruit() {
        Fruit::value = 2;
        Fruit::sprite.setTextureRect(IntRect(Fh*h,0,Fh*h,Fh*h));
        Fruit::V0 = 0.07;
        }
    };
//Фабрики объектов
class Factory
{
  public:
    virtual Fruit* createFruit() = 0;
    virtual ~Factory() {}
};

class AppleFactory: public Factory
{
  public:
    Fruit* createFruit() {
      return new Apple;
    }
};

class PearFactory: public Factory
{
  public:
    Fruit* createFruit() {
      return new Pear;
    }
};

int main()
{
    Pt.loadFromFile("players.png");
    Ft.loadFromFile("fruits.png");
    AppleFactory* apple_factory = new AppleFactory;
    PearFactory* pear_factory = new PearFactory;
    vector<Fruit*> fruits;

    for (int i = 0; i < N; i++) {
        r = rand() % 2;
        if (r == 0) fruits.push_back(apple_factory->createFruit());
        if (r == 1) fruits.push_back(pear_factory->createFruit());
    }

    srand(time(0));
    Player* plr = new Player;

    RenderWindow window(VideoMode(W*h, H*h), "Go!");

    Font font;
	font.loadFromFile("font.ttf");
    Text gameText("",font,20);
	gameText.setColor(Color(40, 70, 30));
	gameText.setPosition(10,10);
    gameText.setString("Life: 3\nScore: 0");
    string gameResult;

	Clock clock;
	Clock dropClock;
    float t, dc; //время
    while (window.isOpen())
    {
		t = clock.getElapsedTime().asMicroseconds();
		clock.restart();

		t = t/300;  //регулируем скорость игры
		if (t > 20) t = 20;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Left))    plr->v=-0.1;
	    if (Keyboard::isKeyPressed(Keyboard::Right))    plr->v=0.1;

	    dc = dropClock.getElapsedTime().asMilliseconds();
        for (int i = 0; i < fruits.size(); i++) {
            fruits[i]->update(t);
            if (fruits[i]->dropFruit(dc) == 1) {dropClock.restart(); dc = 0;}
            fruits[i]->maxV = fruits[i]->V0 + float(plr->score)/1000;
        }

        for (int i = 0; i < fruits.size(); i++) {
                if  (plr->rect.intersects(fruits[i]->rect)&&fruits[i]->y <= (H-Ph)*h) {
                    plr->score += fruits[i]->value;
                    delete fruits[i];
                    r = rand() % 2;
                    if (r == 0) fruits[i] = apple_factory->createFruit();
                    if (r == 1) fruits[i] = pear_factory->createFruit();
                    break;
                    }
                if (fruits[i]->onTheGround == true) {
                    plr->life--;
                    delete fruits[i];
                    r = rand() % 2;
                    if (r == 0) fruits[i] = apple_factory->createFruit();
                    if (r == 1) fruits[i] = pear_factory->createFruit();
                    break;
                }
        }
        plr->update(t);

        gameResult = "Life: " + std::to_string(plr->life) + "\nScore: " + std::to_string(plr->score);
        gameText.setString(gameResult);

        if (plr->isAlive() == false) {
                gameResult = "Game Over\nScore: " + std::to_string(plr->score);
                gameText.setString(gameResult);
                fruits.clear();
                }
        if (plr->isAlive() == false) gameText.setPosition((W-Ph)*h/2, (H-Ph)*h/2);
        else gameText.setPosition(10,10);

        window.clear(Color(107,140,255));

		window.draw(plr->sprite);
        for (int i = 0; i < fruits.size(); i++) {
                if (fruits[i]->y >= -Fh*h)  window.draw(fruits[i]->sprite);
        }

        window.draw(gameText);
		window.display();


        if (plr->isAlive() == false) {
            cout << "Score: " << plr->score << " Play again? (Y/N): ";
            char ans;
            cin >> ans;
            if (ans == 'N') break;
            if (ans == 'Y') {
                for (int i = 0; i < N; i++) {
                    r = rand() % 2;
                    if (r == 0) fruits.push_back(apple_factory->createFruit());
                    if (r == 1) fruits.push_back(pear_factory->createFruit());
                }
                delete plr;
                plr = new Player;
            }
            }
    }
    return 0;
}
