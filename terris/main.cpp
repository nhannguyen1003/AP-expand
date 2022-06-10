
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string.h>

#include <vector>
#include <iostream>

// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"

#include <time.h>
using namespace sf;

const int M = 20;  // row
const int N = 10;   // colummns
const int RESIZE = 2;

int field[M][N] = {0}; //

bool first = true;

struct Point{
    int x,y;
    Point() :x(0),y(0) {}
    } a[4], b[4], c[4];

int figures[7][4] =
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};  // piece

bool check()
{
   for (int i=0;i<4;i++)
      if (a[i].x<0 || a[i].x>=N || a[i].y>=M) return 0;
      else if (field[a[i].y][a[i].x]) return 0;

   return 1;
};


int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(320*RESIZE+200, 480*RESIZE - 100), "The Game!");

    Texture t1,t2,t3;
    t1.loadFromFile(resourcePath() + "images/tiles.png");
    t2.loadFromFile(resourcePath() + "images/replay.png");
    t3.loadFromFile(resourcePath() + "images/frame.png");

    Sprite s(t1), predS(t1), replay(t2), frame(t3);
    Vector2f r(RESIZE,RESIZE);
    s.setScale(r);
    predS.scale(r);
    //replay.setScale(r);
    frame.scale(r);
    
    
    
    Text scoreText,lostText;
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf"))
    {
        std::cout<<"Hello";
    }
    scoreText.setFont(font);
    scoreText.setCharacterSize(60);
    scoreText.setStyle( sf::Text::Bold );
    scoreText.setFillColor(Color::Green);
    scoreText.setPosition(1.5*370,  400);
    
    lostText.setFont(font);
    lostText.setCharacterSize(90);
    lostText.setStyle( sf::Text::Bold );
    lostText.setFillColor(Color::Green);


    
    
    int score = 0;
    int dx=0;  //-1 left -- 1 right
    bool rotate=0;
    int colorNum=1, predict = 1;
    float timer=0,delay=0.3;

    Clock clock;

    do
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer+=time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
              if (e.key.code==Keyboard::Up) rotate=true;
              else if (e.key.code==Keyboard::Left) dx=-1;
              else if (e.key.code==Keyboard::Right) dx=1;
              else if (e.key.code==Keyboard::Down) delay = 0.05;
        }


    //Move left or right
    for (int i=0;i<4;i++)
    {
        b[i]=a[i];
        a[i].x+=dx;
    }
    if (!check()) for (int i=0;i<4;i++) a[i]=b[i];

    //////Rotate//////
    if (rotate)
      {
        Point p = a[1]; //center of rotation
        for (int i=0;i<4;i++)
          {
            int x = a[i].y-p.y;
            int y = a[i].x-p.x;
            a[i].x = p.x - x;
            a[i].y = p.y + y;
           }
        if (!check()) for (int i=0;i<4;i++) a[i]=b[i];
      }

    ///Fall
    if(first) delay = 0;
    if (timer>delay)
      {
        if(first)
        {
            colorNum=1 + rand()%7;
            int n=rand() %7;
            for (int i=0;i<4;i++)
              {
               a[i].x = figures[n][i] % 2;
               a[i].y = figures[n][i] / 2;
              }
            predict =1 + rand()%7;
            n = rand() %7;
            for (int i=0;i<4;i++)
              {
               c[i].x = figures[n][i] % 2;
               c[i].y = figures[n][i] / 2;
              }
            first = false;
        }
        else
        {
            for (int i=0;i<4;i++)
            {
                b[i]=a[i];
                a[i].y+=1;
            }
            if (!check())
            {
                for (int i=0;i<4;i++) field[b[i].y][b[i].x] = colorNum;
                for (int i=0;i<4;i++)
                  {
                   a[i].x = c[i].x;
                   a[i].y = c[i].y;
                  }
                colorNum = predict;
                predict = 1 + rand()%7;
                int n = rand() %7;
                for (int i=0;i<4;i++)
                  {
                   c[i].x = figures[n][i] % 2;
                   c[i].y = figures[n][i] / 2;
                  }
                
            }
        }

         timer=0;
      }
    
    
    ///////check lines//////////
    int tall = 0;
    int k=M-1;
    for (int i=M-1;i>0;i--)
    {
        int count=0;
        for (int j=0;j<N;j++)
        {
            if (field[i][j]) count++;
            field[k][j]=field[i][j];
        }
        if (count<N) k--;
        else score++;
        if (count) tall++;
    }
    scoreText.setString("SCORE\n\n      " + std::to_string(score));
    if (tall == 19)
    {
        for (int i=0;i<M;i++)
            for (int j=0;j<N;j++) field[i][j] = 0;
        for (int i=0;i<4;i++) a[i].x = a[i].y = 0;
        first = true;
        window.clear(Color::Black);
        scoreText.setString("You score is " + std::to_string(score));
        lostText.setString("YOU LOSE!");
        lostText.setPosition(200, 100);
        scoreText.setPosition(250,300);
        window.draw(lostText);
        window.draw(scoreText);
        replay.setPosition(320, 450);
        window.draw(replay);
        window.display();
        while(true)
        {
            bool v = true;
            while (window.pollEvent(e))
            {
                if (e.type == Event::Closed)
                    window.close();
                if (e.type == Event::KeyPressed)
                    if (e.key.code==Keyboard::Space) v = false;
                
                if(e.type == Event::MouseMoved)
                    {
                      sf::Vector2i mousePos = sf::Mouse::getPosition( window );
                      sf::Vector2f mousePosF( static_cast<float>( mousePos.x ), static_cast<float>( mousePos.y ) );
                    }
                  else if (e.type == Event::MouseButtonPressed)
                    {
                      sf::Vector2i mousePos = sf::Mouse::getPosition( window );
                      sf::Vector2f mousePosF( static_cast<float>( mousePos.x ), static_cast<float>( mousePos.y ) );
                      if ( replay.getGlobalBounds().contains( mousePosF ) )
                      {
                        std::cout << "Clicked, yay!" << std::endl;
                          v = false;
                      }
                    }
                
            }
            if(!v)
            {
                window.clear(Color::Black);
                scoreText.setString("SCORE\n\n    " + std::to_string(score));
                scoreText.setPosition(1.5*370,  450);
                break;
            }
        }
    }
    dx=0; rotate=0; delay=0.3;

    /////////draw//////////
    window.clear(Color::Black);
    //window.draw(background);
          
    for (int i=0;i<M;i++)
     for (int j=0;j<N;j++)
       {
         if (field[i][j]==0) continue;
         s.setTextureRect(IntRect(field[i][j]*18,0,18,18));
         s.setPosition(j*18*RESIZE,i*18*RESIZE);
         s.move(28*RESIZE,31*RESIZE); //offset
         window.draw(s);
       }

    for (int i=0;i<4;i++)
      {
        s.setTextureRect(IntRect(colorNum*18,0,18,18));
        s.setPosition(a[i].x*18*RESIZE,a[i].y*18*RESIZE);
        s.move(28*RESIZE,31*RESIZE); //offset
        window.draw(s);
      }
    
        //predict box
        sf::RectangleShape rect(sf::Vector2f(0,0));
            rect.move(550, 150);
        rect.setSize(sf::Vector2f(200, 200));
            rect.setFillColor(Color::Black);
        rect.setOutlineThickness(10);
            rect.setOutlineColor(Color::White);
        window.draw(rect);
        
    for (int i=0;i<4;i++)
        {
            s.setTextureRect(IntRect(predict*18,0,18,18));
            s.setPosition((c[i].x+15)*18*RESIZE,(c[i].y+3)*18*RESIZE);
            s.move(28*RESIZE,31*RESIZE); //offset
            window.draw(s);
          }
    
    window.draw(scoreText);
    window.draw(frame);
    window.display();
    } while (window.isOpen());

    return 0;
}
