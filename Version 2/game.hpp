#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <ctime>
#include <vector>
#include <fstream>
#include <cstring>

#include "SFML/Graphics.hpp"

#define datafile "savadata.txt"
#define indexfile "primaryfile.txt"
#define sindexfile "secondaryfile.txt"

constexpr int BEGINNER = 0;
constexpr int INTERMEDIATE = 1;
//constexpr int ADVANCED = 2;
constexpr int MAXSIDE = 30;

class Game {
    public:
        int NROWS;
        int NCOLUMNS;
        int MINES;
        time_t seed;

        class savData {
            public:
                char savid[15], gameid[15], nmoves[5], etiles[5], nrow[5], ncol[5], gameboard[MAXSIDE][MAXSIDE];
                int flags = 0;

                void add(Game *, const char *, const char *, int, int, const char[][MAXSIDE]);
                void pack(Game *); //(const char *, const char *, int, int, const char[][MAXSIDE]);
                //int pri_search(long long);
                //void datadisp(int i);
                void unpack();
        };

        class index {
            public:
                char savid[15], addr[5];
                void initial();
                void write();
        };

        class sindex {
            public:
                char savid[15], gameid[15];
                void sinitial();
                void swrite();
        };

        Game();

        long long sec_search();
        void deleteOldest();
        void saveGame(char gameBoard[][MAXSIDE], int movesPlayed, int eTiles);
        void loadSaveGame(sf::RenderWindow &app, char gameBoard[][MAXSIDE], int &nmoves, int &etiles, int &flags);
        int datadisp(sf::RenderWindow &app);

        void chooseDifficultyLevel(sf::RenderWindow &app);
        void clearBoards(char mineBoard[][MAXSIDE], char gameBoard[][MAXSIDE]);
        void clearMineBoard(char mineBoard[][MAXSIDE]);
        void placeMines(char mineBoard[][MAXSIDE], int mines);
        void replaceMine(int row, int col, char mineBoard[][MAXSIDE]);
        char indexToChar(int index);
        int charToIndex(char ch);
        void displayBoard(const char gameBoard[][MAXSIDE], sf::RenderWindow &app, const int flag);
        bool isValid(int row, int col);
        bool isMine(int row, int col, char board[][MAXSIDE]);
        std::vector <std::pair<int, int> > getNeighbours(int row, int col);
        int countAdjacentMines(int row, int col, char mineBoard[][MAXSIDE]);
        void uncoverBoard(char gameBoard[][MAXSIDE], char mineBoard[][MAXSIDE], int row, int col, int *nMoves);
        void markMines(char gameBoard[][MAXSIDE], char mineBoard[][MAXSIDE], bool won);
        void playMinesweeper(sf::RenderWindow &app, char mineBoard[][MAXSIDE], char gameBoard[][MAXSIDE],
                             int flags, int nMoves, int movesPlayed);
        void play(sf::RenderWindow &app);
        void load(sf::RenderWindow &app);
};

#endif // GAME_H
