#include "game.hpp"

using namespace std;
using namespace sf;

Game::savData s, sav[50];
Game::index id[50], in;
Game::sindex sind, sid[50];

std::fstream dfile, ifile, sifile;
int i, indsize, sindsize;
char buffer[1000], skey[20];

int offset = 1;
int w = 32;
Vector2f saveBtn(18 * w, 6 * w);
Vector2f reloadBtn(18 * w, 11 * w);

IntRect rboard, rsave, rreload;


Game::Game() {
    in.initial();
    sind.sinitial();
}

void opener(fstream &file, string fn, ios_base::openmode mode) {
	file.open(fn, mode);
	if (!file) {
		cout << "Unable to open\n";
		exit(1);
	}
}

void Game::index::initial() {
	ifile.open(indexfile, ios::in);
	if (!ifile) {
		indsize = 0;
		return;
	}
	for (indsize = 0;; indsize++) {
		ifile.getline(id[indsize].savid, 15, '|');
		ifile.getline(id[indsize].addr, 5, '\n');
		if (ifile.eof())
			break;
	}
	ifile.close();
}

//func to bring in the contents of secondary index file to main memory - sid[]
void Game::sindex::sinitial() {
	sifile.open(sindexfile, ios::in);
	if (!sifile) {
		sindsize = 0;
		return;
	}
	for (sindsize = 0;; sindsize++) {
		sifile.getline(sid[sindsize].gameid, 20, '|');
		sifile.getline(sid[sindsize].savid, 15, '\n');
		if (sifile.eof())
			break;
	}
	sifile.close();
}

void Game::index::write() {
	opener(ifile, indexfile, ios::out);
	for (i = 0; i < indsize; i++)
		ifile << id[i].savid << "|" << id[i].addr << "\n";
}

//func to write the sid[] to secondary index file
void Game::sindex::swrite() {
	opener(sifile, sindexfile, ios::out);
	for (i = 0; i < sindsize; i++)
		sifile << sid[i].gameid << "|" << sid[i].savid << "\n";
}

int pri_search(long long savid) {
    int i;
    for (i = 0; i < indsize; ++i) {
        if (savid == atoll(id[i].savid))
            return i;
    }
    return -1;
}

void Game::savData::pack(Game *game) {
    strcpy_s(buffer, savid);	strcat_s(buffer, "|");
    strcat_s(buffer, gameid);	strcat_s(buffer, "|");
    strcat_s(buffer, nrow);	strcat_s(buffer, "|");
    strcat_s(buffer, ncol);	strcat_s(buffer, "|");
    strcat_s(buffer, nmoves);	strcat_s(buffer, "|");
    strcat_s(buffer, etiles);	strcat_s(buffer, "|");
    for (int i = 0; i < game->NROWS; ++i) {
        for (int j = 0; j < game->NCOLUMNS; ++j) {
            strcat_s(buffer, string(1, gameboard[i][j]).c_str()); strcat_s(buffer, "|");
        }
    }
}

void Game::savData::unpack() {
    int k = 0;
    flags = 0;
	dfile.getline(savid, 15, '|');
	dfile.getline(gameid, 15, '|');
	dfile.getline(nrow, 5, '|');
	dfile.getline(ncol, 5, '|');
	dfile.getline(nmoves, 5, '|');
	dfile.getline(etiles, 5, '|');
	dfile.getline(buffer, 1000, '\n');
	for (int i = 0; i < atoi(nrow); ++i) {
        for (int j = 0; j < atoi(ncol); ++j) {
            //dfile.getline(gameboard[i][j], 1, '|');
            gameboard[i][j] = buffer[k];
            k += 2;
            if (gameboard[i][j] == 'F')
                flags++;
        }
	}
}

void Game::savData::add(Game *game, const char* savid, const char* gameid, int nMoves, int eTiles, const char gameBoard[][MAXSIDE]) {
    int k;
    if (pri_search(atoi(savid)) >= 0) {
        cout <<"id is already present we can't add to index file\n";
        return;
    }

    strcpy_s(this->savid, savid);
    strcpy_s(this->gameid, gameid);;
    strcpy_s(this->nrow, to_string(game->NROWS).c_str());
    strcpy_s(this->ncol, to_string(game->NCOLUMNS).c_str());
    strcpy_s(this->nmoves, to_string(nMoves).c_str());
    strcpy_s(this->etiles, to_string(eTiles).c_str());
    for (int i = 0; i < game->NROWS; ++i)
        for (int j = 0; j < game->NCOLUMNS; ++j)
            gameboard[i][j] = gameBoard[i][j];
    opener(dfile, datafile, ios::app);
    pack(game);
    dfile.seekg(0, ios::end);
    k = dfile.tellg();
    dfile << buffer <<"\n";
    strcpy_s(id[indsize].savid, savid);
    _itoa_s(k, id[indsize].addr, 10);
    indsize++;
    strcpy_s(sid[sindsize].gameid, gameid);
    strcpy_s(sid[sindsize].savid, savid);
    sindsize++;
}

void Game::deleteOldest() {
    int i = 0, k = 0;
    opener(dfile, datafile, ios::in);
    while (dfile)
        sav[i++].unpack(); //sav[i].datadisp(i++);
    --i;
    dfile.close();

    opener(dfile, datafile, ios::out);
    for (int i = 1; i < sindsize; ++i) {
        sav[i].pack(this);
        dfile.seekg(0, ios::end);
        k = dfile.tellg();
        _itoa_s(k, id[i].addr, 10);
        dfile << buffer << endl;
        sid[i - 1] = sid[i];
        id[i - 1] = id[i];
    }
    --sindsize; --indsize;
    dfile.close();
}

long long Game::sec_search() {
    char gameid[15], savid[15];
    savid[0] = 0;
    long long ret_id;
    strcpy_s(gameid, to_string(seed).c_str());
    cout << gameid << " " << seed << endl;
    int j, flag = 0;
    for (j = 0; j<sindsize; j++) {
        if (strcmp(gameid, sid[j].gameid) == 0 && flag == 0) {
            flag = 1;
            strcpy_s(savid, sid[j].savid);
        } else if (strcmp(gameid, sid[j].gameid) == 0 && flag == 1) {
            strcpy_s(savid, sid[j].savid);
            continue;
        }
    }
    if (strlen(savid) == 0)
        ret_id = -1;
    else
        ret_id = atoi(savid);
    return ret_id;
}

void Game::saveGame(char gameBoard[][MAXSIDE], int movesPlayed, int eTiles) {
    int idkey = ((NROWS * NCOLUMNS) + 10) - movesPlayed;
    const char* gseed = to_string(seed).c_str();
    const char* saveid = to_string((int)(seed / idkey)).c_str();
    while (sindsize >= 9)
        deleteOldest();
    s.add(this, saveid, gseed, movesPlayed, eTiles, gameBoard);
    in.write();
    sind.swrite();
    cout << "\nGame saved\n\n";
    dfile.close();
    ifile.close();
    sifile.close();
}

RectangleShape rect_maker(float pos_y) {
    RectangleShape slot(Vector2f(13 * w, 1.5 * w));
    slot.setPosition(Vector2f(4 * w, pos_y * w));
    slot.setFillColor(Color(225, 225, 225));
    slot.setOutlineThickness(3);
	slot.setOutlineColor(Color(187, 187, 187));
	return slot;
}

Text text_maker(int i, Font &font, RectangleShape slotbox) {
    //strcpy_s(buffer, sav[i].savid);	strcat_s(buffer, " - ");
    strcpy_s(buffer, sav[i].gameid);	strcat_s(buffer, " - ");
    strcat_s(buffer, sav[i].nrow);	strcat_s(buffer, " - ");
    strcat_s(buffer, sav[i].ncol);	strcat_s(buffer, " - ");
    strcat_s(buffer, sav[i].nmoves);	strcat_s(buffer, " - ");
    strcat_s(buffer, sav[i].etiles);
    string s(buffer);
    cout << s<< endl;
    String text = String::fromUtf8(s.begin(), s.end());
    //cout << text << endl;
    Text slot(text, font);
    slot.setFillColor(Color::Black);
    slot.setCharacterSize(15);

    FloatRect rectBounds = slotbox.getGlobalBounds();
    FloatRect textBounds = slot.getGlobalBounds();
    slot.setPosition(rectBounds.left + (rectBounds.width / 2) - (textBounds.width / 2),
                     rectBounds.top + (rectBounds.height / 2) - textBounds.height);

    return slot;
}

int Game::datadisp(RenderWindow &app) {
    RectangleShape slot1 = rect_maker(0.6);
    RectangleShape slot2 = rect_maker(2.5);
    RectangleShape slot3 = rect_maker(4.4);
    RectangleShape slot4 = rect_maker(6.3);
    RectangleShape slot5 = rect_maker(8.2);
    RectangleShape slot6 = rect_maker(10.1);
    RectangleShape slot7 = rect_maker(12);
    RectangleShape slot8 = rect_maker(13.9);
    RectangleShape slot9 = rect_maker(15.8);

    Font font;
    if (!font.loadFromFile("res/arial.ttf")) {
        cout << "Error loading font\n";
        app.close();
        exit(1);
    }

    Text text1, text2, text3, text4, text5, text6, text7, text8, text9;
    switch (sindsize) {
        case 9: text9 = text_maker(8, font, slot9);
        case 8: text8 = text_maker(7, font, slot8);
        case 7: text7 = text_maker(6, font, slot7);
        case 6: text6 = text_maker(5, font, slot6);
        case 5: text5 = text_maker(4, font, slot5);
        case 4: text4 = text_maker(3, font, slot4);
        case 3: text3 = text_maker(2, font, slot3);
        case 2: text2 = text_maker(1, font, slot2);
        case 1: text1 = text_maker(0, font, slot1);
            break;
        default: break;
    }


    while (app.isOpen()) {
        app.clear(Color(211, 211, 211));

        app.draw(slot1);
        app.draw(slot2);
        app.draw(slot3);
        app.draw(slot4);
        app.draw(slot5);
        app.draw(slot6);
        app.draw(slot7);
        app.draw(slot8);
        app.draw(slot9);

        switch (sindsize) {
            case 9: app.draw(text9);
            case 8: app.draw(text8);
            case 7: app.draw(text7);
            case 6: app.draw(text6);
            case 5: app.draw(text5);
            case 4: app.draw(text4);
            case 3: app.draw(text3);
            case 2: app.draw(text2);
            case 1: app.draw(text1);
                break;
            default: break;
        }

        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
            else if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                if (slot1.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot1 clicked\n";
                    return 0;
                } else if (slot2.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot2 clicked\n";
                    return 1;
                } else if (slot3.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot3 clicked\n";
                    return 2;
                } else if (slot4.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot4 clicked\n";
                    return 3;
                } else if (slot5.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot5 clicked\n";
                    return 4;
                } else if (slot6.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot6 clicked\n";
                    return 5;
                } else if (slot7.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot7 clicked\n";
                    return 6;
                } else if (slot8.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot8 clicked\n";
                    return 7;
                } else if (slot9.getGlobalBounds().contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "slot9 clicked\n";
                    return 8;
                }
            }
        }
        app.display();
    }
}

void Game::loadSaveGame(RenderWindow &app, char gameBoard[][MAXSIDE], int &nmoves, int &etiles, int &flags) {
    int i = 0, rrn;
    cout << "list of save files\n";
    opener(dfile, datafile, ios::in);
    while (dfile) {
        sav[i].unpack();
        ++i;
    }
    --i;
    rrn = datadisp(app);
    cout << "RRN: " << rrn << endl;
    //cin >> rrn;
    if (rrn < 0 || rrn >= i) {
        cout << "Invalid RRN\n";
        exit(0);
    }
    seed = atoi(sav[rrn].gameid);
    NROWS = atoi(sav[rrn].nrow);
    NCOLUMNS = atoi(sav[rrn].ncol);
    nmoves = atoi(sav[rrn].nmoves);
    etiles = atoi(sav[rrn].etiles);
    flags = sav[rrn].flags;
    for (int j = 0; j < NROWS; ++j)
        for (int k = 0; k < NCOLUMNS; ++k)
            gameBoard[j][k] = sav[rrn].gameboard[j][k];
    dfile.close();
}

void Game::chooseDifficultyLevel(RenderWindow &app) {
    int level = 0, flag = 0;

    //cout << "Enter the Difficulty Level" << endl;
    //cout << "Press 0 for BEGINNER     (9  * 9  cells and 10 mines)" << endl;
    //cout << "Press 1 for INTERMEDIATE (16 * 16 cells and 40 mines)" << endl;
    //cout << "Press 2 for ADVANCED     (16 * 30 cells and 99 mines)" << endl;

    //cin >> level;

    RectangleShape easy_r(Vector2f(3 * w, 2 * w));
    RectangleShape hard_r(Vector2f(3 * w, 2 * w));
    RectangleShape view_r(Vector2f(13 * w, 3 * w));

    easy_r.setPosition(Vector2f(9 * w, 10 * w));
    easy_r.setFillColor(Color(225, 225, 225));
    easy_r.setOutlineThickness(5);
	easy_r.setOutlineColor(Color(187, 187, 187));
	hard_r.setPosition(Vector2f(9 * w, 14 * w));
    hard_r.setFillColor(Color(225, 225, 225));
    hard_r.setOutlineThickness(5);
	hard_r.setOutlineColor(Color(187, 187, 187));
	view_r.setPosition(Vector2f(4 * w, 4 * w));
    view_r.setFillColor(Color(211, 211, 211));
    view_r.setOutlineThickness(8);
	view_r.setOutlineColor(Color(187, 187, 187));

	Font font;
	if (!font.loadFromFile("res/arial.ttf")) {
        cout << "Error loading font\n";
        app.close();
        exit(1);
    }

    Text view("CHOOSE DIFFICULTY", font);
    view.setFillColor(Color::Black);
    view.setCharacterSize(32);
    Text easy("Easy", font);
    easy.setFillColor(Color::Black);
    easy.setCharacterSize(20);
    Text hard("Hard", font);
    hard.setFillColor(Color::Black);
    hard.setCharacterSize(20);

    FloatRect rect1Bounds = easy_r.getGlobalBounds();
    FloatRect text1Bounds = easy.getGlobalBounds();
    FloatRect rect2Bounds = hard_r.getGlobalBounds();
    FloatRect text2Bounds = hard.getGlobalBounds();
    FloatRect rect3Bounds = view_r.getGlobalBounds();
    FloatRect text3Bounds = view.getGlobalBounds();

    view.setPosition(rect3Bounds.left + (rect3Bounds.width / 2) - (text3Bounds.width / 2),
                     rect3Bounds.top + (rect3Bounds.height / 2) - text3Bounds.height);
    easy.setPosition(rect1Bounds.left + (rect1Bounds.width / 2) - (text1Bounds.width / 2),
                     rect1Bounds.top + (rect1Bounds.height / 2) - (text1Bounds.height - 4));  //
    hard.setPosition(rect2Bounds.left + (rect2Bounds.width / 2) - (text2Bounds.width / 2),
                     rect2Bounds.top + (rect2Bounds.height / 2) - text2Bounds.height);

    while (app.isOpen() && !flag) {
        app.clear(Color(211, 211, 211));
        app.draw(view_r);
        app.draw(view);
        app.draw(easy_r);
        app.draw(easy);
        app.draw(hard_r);
        app.draw(hard);

        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();
            else if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                if (rect1Bounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Easy clicked\n";
                    level = 0;
                    flag = 1;
                } else if (rect2Bounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Hard clicked\n";
                    level = 1;
                    flag = 1;
                }
            }
        }
        app.display();
    }

    if (level == BEGINNER) {
        NROWS = 9;
        NCOLUMNS = 9;
        MINES = 10;
        offset = 4;
    }

    if (level == INTERMEDIATE) {
        NROWS = 16;
        NCOLUMNS = 16;
        MINES = 40;
        offset = 1;
    }

    /*if (level == ADVANCED) {
        NROWS = 16;
        NCOLUMNS = 30;
        MINES = 99;
    }*/

}

void Game::clearBoards(char mineBoard[][MAXSIDE], char gameBoard[][MAXSIDE]) {
    for (int i = 0; i < NROWS; i++)
        for (int j = 0; j < NCOLUMNS; j++)
            gameBoard[i][j] = mineBoard[i][j] = '.';
}

void Game::clearMineBoard(char mineBoard[][MAXSIDE]) {
    for (int i = 0; i < NROWS; i++)
        for (int j = 0; j < NCOLUMNS; j++)
            mineBoard[i][j] = '.';
}

void Game::placeMines(char mineBoard[][MAXSIDE], int mines) {
    int placed = 0;
    while (placed < mines) {
        int random = rand() % (NROWS * NCOLUMNS);
        int row = random / NCOLUMNS;
        int col = random % NROWS;
        if (mineBoard[row][col] == '#') continue; // already a mine
        mineBoard[row][col] = '#';
        placed++;
    }
}

// replace the mine in (row, col) and put it to a vacant space
void Game::replaceMine(int row, int col, char mineBoard[][MAXSIDE]) {
    placeMines(mineBoard, 1);  // add a new mine
    mineBoard[row][col] = '.'; // remove the old one
}

char Game::indexToChar(int index) {
    if (index < 10)
        return index + '0';
    else
        return 'a' + (index - 10);
}

int Game::charToIndex(char ch) {
    if (ch <= '9')
        return ch - '0';
    else
        return (ch - 'a') + 10;
}

bool Game::isValid(int row, int col) {
    return (row >= 0) && (row < NROWS) && (col >= 0) && (col < NCOLUMNS);
}

bool Game::isMine(int row, int col, char board[][MAXSIDE]) {
    return (board[row][col] == '#');
}

// return vector of all neighbours of row, col
vector <pair<int, int> > Game::getNeighbours(int row, int col) {
    vector <pair<int, int> > neighbours;

    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            if (dx != 0 || dy != 0)
                if (isValid(row+dx, col+dy))
                    neighbours.push_back(make_pair(row+dx, col+dy));

    return neighbours;
}

// count the number of mines in the adjacent cells
int Game::countAdjacentMines(int row, int col, char mineBoard[][MAXSIDE]) {
    vector <pair<int, int> > neighbours = getNeighbours(row, col);

    int count = 0;
    for (unsigned int i = 0; i < neighbours.size(); i++)
        if (isMine(neighbours[i].first, neighbours[i].second, mineBoard))
            count++;

    return count;
}

void Game::uncoverBoard(char gameBoard[][MAXSIDE], char mineBoard[][MAXSIDE], int row, int col, int *nMoves) {
    (*nMoves)++;
    int count = countAdjacentMines(row, col, mineBoard);
    gameBoard[row][col] = count + '0';

    // if cell number == 0, then uncover all the neighboring cells. (only uncover if neighbour == '.')
    if (count == 0) {
        vector <pair<int, int> > neighbours = getNeighbours(row, col);

        for (unsigned int i = 0; i < neighbours.size(); i++)
            if (gameBoard[neighbours[i].first][neighbours[i].second] == '.')
                uncoverBoard(gameBoard, mineBoard, neighbours[i].first, neighbours[i].second, nMoves);
    }

    return;
}

void Game::markMines(char gameBoard[][MAXSIDE], char mineBoard[][MAXSIDE], bool won) {
    for (int i = 0; i < NROWS; i++) {
        for (int j = 0; j < NCOLUMNS; j++) {
            if (gameBoard[i][j] == '.' && mineBoard[i][j] == '#') {
                if (won)
                    gameBoard[i][j] = 'F';
                else
                    gameBoard[i][j] = '#';
            }
            if (gameBoard[i][j] == 'F' && mineBoard[i][j] != '#') {
                if (!won)
                    gameBoard[i][j] = '.';
            }
        }
    }
}

void Game::displayBoard(const char gameBoard[][MAXSIDE], RenderWindow &app, const int flags) {
    app.clear(Color(211, 211, 211));

    Texture t;
	if (!t.loadFromFile("res/tiles.jpg")) {
        cout << "Error loading texture\n";
        app.close();
        exit(1);
	}
	Sprite s(t);

	Font flagfont;
	if (!flagfont.loadFromFile("res/DS-DIGI.ttf")) {
        cout << "Error loading font\n";
        app.close();
        exit(1);
	}
	sf::Text flagleft(to_string(flags), flagfont);
	flagleft.setFillColor(Color::Black);
	flagleft.setPosition(19.5 * w, 0.9 * w);
	Sprite flag = s;
	flag.setTextureRect(IntRect(11 * w, 0, w, w));
	flag.setPosition(18 * w, 1 * w);
	app.draw(flag);
	app.draw(flagleft);

	Texture saveButton;
	if (!saveButton.loadFromFile("res/save.png")) {
        cout << "Error loading texture\n";
        app.close();
        exit(1);
	}
	Sprite s_save(saveButton);
	sf::RectangleShape save_r(sf::Vector2f(w, w));
	s_save.setPosition(saveBtn);
	save_r.setPosition(saveBtn);
	save_r.setOutlineThickness(5);
	save_r.setOutlineColor(Color(187, 187, 187));
    app.draw(save_r);
	app.draw(s_save);

	Texture reloadButton;
	if (!reloadButton.loadFromFile("res/reload.png")) {
        cout << "Error loading texture\n";
        app.close();
        exit(1);
	}
	Sprite s_reload(reloadButton);
	RectangleShape reload_r(sf::Vector2f(w, w));
	s_reload.setPosition(reloadBtn);
	reload_r.setPosition(reloadBtn);
	reload_r.setOutlineThickness(5);
	reload_r.setOutlineColor(Color(187, 187, 187));
    app.draw(reload_r);
	app.draw(s_reload);

	sf::RectangleShape board(sf::Vector2f(NROWS * w, NCOLUMNS * w));
	board.setPosition(offset * w, offset * w);
	board.setOutlineThickness(7);
	board.setOutlineColor(Color(176, 176, 176));
    app.draw(board);

    int tile_index;
    for (int i = 0; i < NROWS; i++) {
        for (int j = 0; j < NCOLUMNS; j++) {
                if (gameBoard[i][j] == '.')
                    tile_index = 10;
                else if (gameBoard[i][j] == 'F')
                    tile_index = 11;
                else if (gameBoard[i][j] == '#')
                    tile_index = 9;
                else
                    tile_index = gameBoard[i][j] - '0';
                s.setTextureRect(IntRect(tile_index * w, 0, w, w));
                s.setPosition((i + offset) * w, (j + offset) * w);
                app.draw(s);
        }
    }

    IntRect board_rect(board.getPosition().x, board.getPosition().y,
                      board.getGlobalBounds().width, board.getGlobalBounds().height);
    IntRect save_rect(save_r.getPosition().x, save_r.getPosition().y,
                      save_r.getGlobalBounds().width, save_r.getGlobalBounds().height);
    IntRect reload_rect(reload_r.getPosition().x, reload_r.getPosition().y,
                      reload_r.getGlobalBounds().width, reload_r.getGlobalBounds().height);
    rboard = board_rect;
    rsave = save_rect;
    rreload = reload_rect;
    app.display();
}

int displayWin(RenderWindow &app) {
    RectangleShape menu_r(Vector2f(3 * w, 2 * w));
    menu_r.setPosition(Vector2f(9 * w, 14 * w));
    menu_r.setFillColor(Color(225, 225, 225));
    menu_r.setOutlineThickness(5);
	menu_r.setOutlineColor(Color(187, 187, 187));

    Font font;
	if (!font.loadFromFile("res/arial.ttf")) {
        cout << "Error loading font\n";
        app.close();
        exit(1);
	}
    Text menu("Menu", font);
    menu.setFillColor(Color::Black);
    menu.setCharacterSize(20);

    FloatRect rectBounds = menu_r.getGlobalBounds();
    FloatRect textBounds = menu.getGlobalBounds();

    menu.setPosition(rectBounds.left + (rectBounds.width / 2) - (textBounds.width / 2),
                     rectBounds.top + (rectBounds.height / 2) - textBounds.height);

    Texture t;
    if (!t.loadFromFile("res/win.png")) {
        cout << "Error loading texture\n";
        app.close();
        exit(1);
	}
	Sprite s(t);
	s.setPosition(3.5 * w, 2 * w);

    while(app.isOpen()) {
        app.clear(Color(211, 211, 211));
        //app.draw(title);
        app.draw(s);
        app.draw(menu_r);
        app.draw(menu);
        Event e;
        while (app.pollEvent(e)) {
            if(e.type == Event::Closed)
                app.close();
            else if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
                if (rectBounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Menu clicked\n";
                    return 1;
                }
        }
        app.display();
    }
    return 0;
}

int displayLose(RenderWindow &app) {
    RectangleShape menu_r(Vector2f(3 * w, 2 * w));
    RectangleShape reload_r(Vector2f(3 * w, 2 * w));
    menu_r.setPosition(Vector2f(13 * w, 14 * w));
    menu_r.setFillColor(Color(225, 225, 225));
    menu_r.setOutlineThickness(5);
	menu_r.setOutlineColor(Color(187, 187, 187));
	reload_r.setPosition(Vector2f(5 * w, 14 * w));
    reload_r.setFillColor(Color(225, 225, 225));
    reload_r.setOutlineThickness(5);
	reload_r.setOutlineColor(Color(187, 187, 187));

    Font font;
	if (!font.loadFromFile("res/arial.ttf")) {
        cout << "Error loading font\n";
        app.close();
        exit(1);
	}
    Text menu("Menu", font);
    menu.setFillColor(Color::Black);
    menu.setCharacterSize(20);
    Text reload("Reload", font);
    reload.setFillColor(Color::Black);
    reload.setCharacterSize(20);

    FloatRect rect1Bounds = menu_r.getGlobalBounds();
    FloatRect text1Bounds = menu.getGlobalBounds();
    FloatRect rect2Bounds = reload_r.getGlobalBounds();
    FloatRect text2Bounds = reload.getGlobalBounds();

    menu.setPosition(rect1Bounds.left + (rect1Bounds.width / 2) - (text1Bounds.width / 2),
                     rect1Bounds.top + (rect1Bounds.height / 2) - text1Bounds.height);
    reload.setPosition(rect2Bounds.left + (rect2Bounds.width / 2) - (text2Bounds.width / 2),
                     rect2Bounds.top + (rect2Bounds.height / 2) - text2Bounds.height);

    Texture t;
    if (!t.loadFromFile("res/lose.png")) {
        cout << "Error loading texture\n";
        app.close();
        exit(1);
	}
	Sprite s(t);
	s.setPosition(3.5 * w, 2 * w);

    while(app.isOpen()) {
        app.clear(Color(211, 211, 211));
        //app.draw(title);
        app.draw(s);
        app.draw(reload_r);
        app.draw(reload);
        app.draw(menu_r);
        app.draw(menu);
        Event e;
        while (app.pollEvent(e)) {
            if(e.type == Event::Closed)
                app.close();
            else if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
                if (rect1Bounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Menu clicked\n";
                    return 1;
                }
                else if (rect2Bounds.contains(Mouse::getPosition(app).x, Mouse::getPosition(app).y)) {
                    cout << "Reload clicked\n";
                    return 2;
                }
            }
        }
        app.display();
    }
    return 0;
}

void Game::playMinesweeper(RenderWindow &app, char mineBoard[][MAXSIDE], char gameBoard[][MAXSIDE],
                           int flags, int nMoves, int movesPlayed) {
    /*char mineBoard[MAXSIDE][MAXSIDE], gameBoard[MAXSIDE][MAXSIDE];
    int flags = 0;
    int nMoves = 0, movesPlayed = 0;*/

    //RenderWindow app(VideoMode(672, 576), "Minesweeper");

    /*seed = time(NULL);
    srand(seed);
    chooseDifficultyLevel();
    clearBoards(mineBoard, gameBoard);
    placeMines(mineBoard, MINES);
    flags = MINES;*/

    int nMovesTotal = NROWS * NCOLUMNS - MINES;

    // lets play!
    bool gameOver = false;
    int menu = 0;

    while (app.isOpen() && !gameOver) {
        Event e;
        displayBoard(gameBoard, app, flags);

        while (app.pollEvent(e)) {
            Vector2i pos = Mouse::getPosition(app);
            int x = (pos.x / w) - offset;
            int y = (pos.y / w) - offset;

            switch (e.type) {
                case Event::Closed: app.close();
                    break;
                case Event::MouseButtonPressed:
                    cout << nMoves << endl;
                    if (rsave.contains(Vector2i(pos.x, pos.y))) {
                        cout << "save clicked\n";
                        if (nMoves == 0)
                            continue;
                        saveGame(gameBoard, movesPlayed, nMoves);
                    } else if (rreload.contains(Vector2i(pos.x, pos.y))) {
                        cout << "reload clicked\n";
                        if (nMoves == 0)
                            continue;
                        cout << "reload in\n";
                        long long savid = sec_search();
                        if (savid == -1) {
                            cout << "cannot reload\n";
                            continue;
                        }
                        int pos = pri_search(savid);
                        opener(dfile, datafile, ios::in);
                        dfile.seekg(atoi(id[pos].addr), ios::beg);
                        s.unpack();
                        movesPlayed = atoi(s.nmoves);
                        nMoves = atoi(s.etiles);
                        cout << MINES << " " << s.flags << endl;
                        flags = MINES - s.flags;
                        for (int j = 0; j < NROWS; ++j)
                        for (int k = 0; k < NCOLUMNS; ++k)
                            gameBoard[j][k] = s.gameboard[j][k];
                        dfile.close();
                    } else if (rboard.contains(Vector2i(pos.x, pos.y))) {
                        if (e.mouseButton.button == Mouse::Left) {
                            cout << "true left\n";
                            if (nMoves == 0) {
                            if (isMine(x, y, mineBoard))
                                replaceMine(x, y, mineBoard);
                            }
                            ++movesPlayed;
                            if (gameBoard[x][y] == '.' && mineBoard[x][y] == '.') {
                                uncoverBoard(gameBoard, mineBoard, x, y, &nMoves);
                                if (nMoves == nMovesTotal) {
                                    markMines(gameBoard, mineBoard, true);
                                    displayBoard(gameBoard, app, flags);
                                    cout << endl << "You won!!! :)" << endl;
                                    gameOver = true;
                                    menu = displayWin(app);
                                }
                            } else if (gameBoard[x][y] == '.' && mineBoard[x][y] == '#') {
                                // game over
                                gameBoard[x][y] = '#';
                                markMines(gameBoard, mineBoard, false);
                                displayBoard(gameBoard, app, flags);
                                cout << endl << "You lost! :(" << endl;
                                int options = displayLose(app);
                                if (options == 2) {
                                    long long savid = sec_search();
                                    if (savid == -1) {
                                        cout << "cannot reload\n";
                                        menu = 1;
                                        gameOver = true;
                                    } else {
                                        int pos = pri_search(savid);
                                        opener(dfile, datafile, ios::in);
                                        dfile.seekg(atoi(id[pos].addr), ios::beg);
                                        s.unpack();
                                        movesPlayed = atoi(s.nmoves);
                                        nMoves = atoi(s.etiles);
                                        flags = MINES - s.flags;
                                        for (int j = 0; j < NROWS; ++j)
                                            for (int k = 0; k < NCOLUMNS; ++k)
                                                gameBoard[j][k] = s.gameboard[j][k];
                                        dfile.close();
                                    }
                                } else if (options == 1) {
                                    menu = 1;
                                    gameOver = true;
                                } else
                                    gameOver = true;
                            }
                        } else if (e.mouseButton.button == Mouse::Right) {
                            cout << "true right\n";
                            if (nMoves == 0)
                                continue;
                            ++movesPlayed;
                            if (gameBoard[x][y] == '.') {
                                if (flags != 0) {
                                    gameBoard[x][y] = 'F';
                                    flags--;
                                }
                            } else if (gameBoard[x][y] == 'F') {
                                // undo a flag
                                gameBoard[x][y] = '.';
                                flags++;
                            }
                        }
                    }
                    break;
                default: break;
            }
        }
    }
}

void Game::play(RenderWindow &app) {
    char mineBoard[MAXSIDE][MAXSIDE], gameBoard[MAXSIDE][MAXSIDE];
    int flags = 0;
    int nMoves = 0, movesPlayed = 0;

    seed = time(NULL);
    srand(seed);
    chooseDifficultyLevel(app);
    clearBoards(mineBoard, gameBoard);
    placeMines(mineBoard, MINES);
    flags = MINES;
    playMinesweeper(app, mineBoard, gameBoard, flags, nMoves, movesPlayed);
}

void Game::load(sf::RenderWindow &app) {
    char mineBoard[MAXSIDE][MAXSIDE], gameBoard[MAXSIDE][MAXSIDE];
    int flags = 0;
    int nMoves = 0, movesPlayed = 0;

    if (sindsize == 0) {
            cout << "No Save files found\n";
            return;
        }
        loadSaveGame(app, gameBoard, movesPlayed, nMoves, flags);
        srand(seed);
        if (NROWS == 9 && NCOLUMNS == 9) {
            MINES = 10;
            offset = 4;
        }
        else if (NROWS == 16 && NCOLUMNS == 16) {
            MINES = 40;
            offset = 1;
        }
        clearMineBoard(mineBoard);
        placeMines(mineBoard, MINES);
        flags = MINES - flags;
        cout << "\nGame Loaded\n";
        playMinesweeper(app, mineBoard, gameBoard, flags, nMoves, movesPlayed);
}

