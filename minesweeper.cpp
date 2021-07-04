#include <iostream>
//#include <utility>
#include <ctime>
#include <fstream>
#include <cstring>
#include <vector>
#include <sstream>

#define datafile "savadata.txt"
#define indexfile "primaryfile.txt"
#define sindexfile "secondaryfile.txt"

using namespace std;

fstream dfile, ifile, sifile;
int i, indsize, sindsize;
char buffer[1000], skey[20];

const int BEGINNER = 0;
const int INTERMEDIATE = 1;
//const int ADVANCED = 2;
const int MAXSIDE = 30;

int NROWS;
int NCOLUMNS;
int MINES;
time_t seed;

class savData {
    public:
        char savid[15], gameid[15], nmoves[5], etiles[5], nrow[5], ncol[5], gameboard[MAXSIDE][MAXSIDE];
        int flags = 0;
        void add(const char *, const char *, int, int, const char[][MAXSIDE]);
        void pack(); //(const char *, const char *, int, int, const char[][MAXSIDE]);
        friend int search(char *);
        void datadisp(int i);
        void unpack();
}s, sav[50];

class index {
    public:
        char savid[15], addr[5];
        void initial();
        void write();
}id[50], in;

class sindex {
public:
	char savid[15], gameid[15];
	void sinitial();
	void swrite();
}sind, sid[50];

void opener(fstream &file, string fn, ios_base::openmode mode) {
	file.open(fn, mode);
	if (!file) {
		cout << "Unable to open";
		exit(1);
	}
}

void index::initial() {
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
void sindex::sinitial() {
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

void index::write() {
	opener(ifile, indexfile, ios::out);
	for (i = 0; i < indsize; i++)
		ifile << id[i].savid << "|" << id[i].addr << "\n";
}

//func to write the sid[] to secondary index file
void sindex::swrite() {
	opener(sifile, sindexfile, ios::out);
	for (i = 0; i < sindsize; i++)
		sifile << sid[i].gameid << "|" << sid[i].savid << "\n";
}

/*int search(long long savid)
{
	int low = 0;
	int high = indsize - 1;
	int mid;
	while (low <= high)
	{
		mid = (low + high) / 2;
		cout << savid << " " << atoi(id[mid].savid) << endl;
		if (savid == atoi(id[mid].savid))
			return mid;
		else if (savid > atoi(id[mid].savid))
			low = mid + 1;
		else
			high = mid - 1;
	}
	return -1;
}*/

int pri_search(long long savid) {
    int i;
    for (i = 0; i < indsize; ++i) {
        cout << savid << " " << atoll(id[i].savid) << endl;
        if (savid == atoll(id[i].savid))
            return i;
    }
    return -1;
}

void savData::pack() { //(const char* savid, const char* gameid, int nMoves, int eTiles, const char gameBoard[][MAXSIDE]) {
    stringstream ss;
    strcpy_s(buffer, savid);	strcat_s(buffer, "|");
    strcat_s(buffer, gameid);	strcat_s(buffer, "|");
    strcat_s(buffer, nrow);	strcat_s(buffer, "|");
    strcat_s(buffer, ncol);	strcat_s(buffer, "|");
    strcat_s(buffer, nmoves);	strcat_s(buffer, "|");
    strcat_s(buffer, etiles);	strcat_s(buffer, "|");
    for (int i = 0; i < NROWS; ++i) {
        for (int j = 0; j < NCOLUMNS; ++j) {
            strcat_s(buffer, string(1, gameboard[i][j]).c_str()); strcat_s(buffer, "|");
        }
    }
}

void savData::unpack() {
    int k = 0;
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

void savData::add(const char* savid, const char* gameid, int nMoves, int eTiles, const char gameBoard[][MAXSIDE]) {
    int k;
    if (pri_search(atoi(savid)) >= 0) {
        cout <<"id is already present we can't add to index file\n";
        return;
    }

    strcpy_s(this->savid, savid);
    strcpy_s(this->gameid, gameid);;
    strcpy_s(this->nrow, to_string(NROWS).c_str());
    strcpy_s(this->ncol, to_string(NCOLUMNS).c_str());
    strcpy_s(this->nmoves, to_string(nMoves).c_str());
    strcpy_s(this->etiles, to_string(eTiles).c_str());
    for (int i = 0; i < NROWS; ++i)
        for (int j = 0; j < NCOLUMNS; ++j)
            gameboard[i][j] = gameBoard[i][j];
    opener(dfile, datafile, ios::app);
    pack();
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

void savData::datadisp(int i) {
    unpack();
    if (dfile.eof()) return;
    cout << i << "\t" << gameid <<"\t"<< nrow <<"\t"<< ncol <<"\t" << nmoves <<"\t"<< etiles << endl;
}

void deleteOldest() {
    int i = 0, k = 0;
    opener(dfile, datafile, ios::in);
    while (dfile)
        sav[i++].unpack(); //sav[i].datadisp(i++);
    --i;
    dfile.close();

    opener(dfile, datafile, ios::out);
    for (int i = 1; i < sindsize; ++i) {
        sav[i].pack();
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

long long sec_search() {
    char gameid[15], savid[15];
    savid[0] = 0;
    long long ret_id;
    strcpy_s(gameid, to_string(seed).c_str());
    cout << gameid << " " << seed << endl;
    int pos, j, flag = 0;
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

void chooseDifficultyLevel() {
    int level;

    cout << "Enter the Difficulty Level" << endl;
    cout << "Press 0 for BEGINNER     (9  * 9  cells and 10 mines)" << endl;
    cout << "Press 1 for INTERMEDIATE (16 * 16 cells and 40 mines)" << endl;
    //cout << "Press 2 for ADVANCED     (16 * 30 cells and 99 mines)" << endl;

    cin >> level;

    if (level == BEGINNER) {
        NROWS = 9;
        NCOLUMNS = 9;
        MINES = 10;
    }

    if (level == INTERMEDIATE) {
        NROWS = 16;
        NCOLUMNS = 16;
        MINES = 40;
    }

    /*if (level == ADVANCED) {
        NROWS = 16;
        NCOLUMNS = 30;
        MINES = 99;
    }*/

    return;
}

void saveGame(char gameBoard[][MAXSIDE], int movesPlayed, int eTiles) {
    int idkey = ((NROWS * NCOLUMNS) + 10) - movesPlayed;
    const char* gseed = to_string(seed).c_str();
    const char* saveid = to_string((int)(seed / idkey)).c_str();
    if (sindsize == 9)
        deleteOldest();
    s.add(saveid, gseed, movesPlayed, eTiles, gameBoard);
    in.write();
    sind.swrite();
    cout << "\nGame saved\n\n";
    dfile.close();
    ifile.close();
    sifile.close();
}

void loadSaveGame(char gameBoard[][MAXSIDE], int &nmoves, int &etiles, int &flags) {
    int i = 0, rrn;
    cout << "list of save files\n";
    opener(dfile, datafile, ios::in);
    while (dfile)
        sav[i].datadisp(i++);
    --i;
    cout << "Enter RRN of save file to load: ";
    cin >> rrn;
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

void clearBoards(char mineBoard[][MAXSIDE], char gameBoard[][MAXSIDE]) {
    for (int i = 0; i < NROWS; i++)
        for (int j = 0; j < NCOLUMNS; j++)
            gameBoard[i][j] = mineBoard[i][j] = '.';
    return;
}

void clearMineBoard(char mineBoard[][MAXSIDE]) {
    for (int i = 0; i < NROWS; i++)
        for (int j = 0; j < NCOLUMNS; j++)
            mineBoard[i][j] = '.';
    return;
}

void placeMines(char mineBoard[][MAXSIDE], int mines) {
    int placed = 0;
    while (placed < mines) {
        int random = rand() % (NROWS * NCOLUMNS);
        int row = random / NCOLUMNS;
        int col = random % NROWS;
        if (mineBoard[row][col] == '#') continue; // already a mine
        mineBoard[row][col] = '#';
        placed++;
    }
    return;
}

// replace the mine in (row, col) and put it to a vacant space
void replaceMine(int row, int col, char mineBoard[][MAXSIDE]) {
    placeMines(mineBoard, 1);  // add a new mine
    mineBoard[row][col] = '.'; // remove the old one
    return;
}

char indexToChar(int index) {
    if (index < 10)
        return index + '0';
    else
        return 'a' + (index - 10);
}

int charToIndex(char ch) {
    if (ch <= '9')
        return ch - '0';
    else
        return (ch - 'a') + 10;
}

void displayBoard(char gameBoard[][MAXSIDE]) {
    // top line
    cout << "    ";
    for (int i = 0; i < NCOLUMNS; i++)
        cout << indexToChar(i) << ' ';
    cout << endl << endl;

    // rows
    for (int i = 0; i < NROWS; i++) {
        cout << indexToChar(i) << "   ";
        for (int j = 0; j < NCOLUMNS; j++)
            cout << gameBoard[i][j] << " ";
        cout << "  " << indexToChar(i);
        cout << endl;
    }

    // bottom line
    cout << endl << "    ";
    for (int i = 0; i < NCOLUMNS; i++)
        cout << indexToChar(i) << ' ';
    cout << endl;

    return;
}

bool isValid(int row, int col) {
    return (row >= 0) && (row < NROWS) && (col >= 0) && (col < NCOLUMNS);
}

bool isMine(int row, int col, char board[][MAXSIDE]) {
    return (board[row][col] == '#');
}

// return vector of all neighbours of row, col
vector < pair <int, int> > getNeighbours(int row, int col) {
    vector < pair <int, int> > neighbours;

    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            if (dx != 0 || dy != 0)
                if (isValid(row+dx, col+dy))
                    neighbours.push_back(make_pair(row+dx, col+dy));

    return neighbours;
}

// count the number of mines in the adjacent cells
int countAdjacentMines(int row, int col, char mineBoard[][MAXSIDE]) {
    vector < pair <int, int> > neighbours = getNeighbours(row, col);

    int count = 0;
    for (int i = 0; i < neighbours.size(); i++)
        if (isMine(neighbours[i].first, neighbours[i].second, mineBoard))
            count++;

    return count;
}

void uncoverBoard(char gameBoard[][MAXSIDE], char mineBoard[][MAXSIDE], int row, int col, int *nMoves) {
    (*nMoves)++;
    int count = countAdjacentMines(row, col, mineBoard);
    gameBoard[row][col] = count + '0';

    // if cell number == 0, then uncover all the neighboring cells. (only uncover if neighbour == '.')
    if (count == 0) {
        vector < pair <int, int> > neighbours = getNeighbours(row, col);

        for (int i = 0; i < neighbours.size(); i++)
            if (gameBoard[neighbours[i].first][neighbours[i].second] == '.')
                uncoverBoard(gameBoard, mineBoard, neighbours[i].first, neighbours[i].second, nMoves);
    }

    return;
}

void markMines(char gameBoard[][MAXSIDE], char mineBoard[][MAXSIDE], bool won) {
    for (int i = 0; i < NROWS; i++) {
        for (int j = 0; j < NCOLUMNS; j++) {
            if (gameBoard[i][j] == '.' && mineBoard[i][j] == '#') {
                if (won) {
                    gameBoard[i][j] = 'F';
                }
                else {
                    gameBoard[i][j] = '#';
                }
            }
        }
    }
}

void playMinesweeper() {
    // initialization
    char mineBoard[MAXSIDE][MAXSIDE], gameBoard[MAXSIDE][MAXSIDE];
    int choice, flags = 0;
    int nMoves = 0, movesPlayed = 0;

    cout << "1. Play new game\n2. Load previously saved game\nEnter your choice: ";
    cin >> choice;
    switch (choice) {
        case 1: seed = time(NULL);
            //srand(time(NULL));
            srand(seed);
            chooseDifficultyLevel();
            clearBoards(mineBoard, gameBoard);
            placeMines(mineBoard, MINES);
            flags = MINES;
            break;
        case 2: if (sindsize == 0) {
                cout << "No Save files found\n";
                return;
            }
            loadSaveGame(gameBoard, movesPlayed, nMoves, flags);
            srand(seed);
            if (NROWS == 9 && NCOLUMNS == 9)
                MINES = 10;
            else if (NROWS == 16 && NCOLUMNS == 16)
                MINES = 40;
            clearMineBoard(mineBoard);
            placeMines(mineBoard, MINES);
            flags = MINES - flags;
            cout << "\nGame Loaded\n";
            //exit(0);
            break;
        default: cout << "Invalid Choice\n";
            return;
    }

    /*for (int i = 0; i < NROWS; ++i) {
        for (int j = 0; j < NCOLUMNS; ++j) {
            cout << mineBoard[i][j] << " ";
        }
        cout << endl;
    }*/

    int nMovesTotal = NROWS * NCOLUMNS - MINES;

    // lets play!
    bool gameOver = false;

    while (!gameOver) {
        displayBoard(gameBoard);
        cout << flags << " flags left" << endl << endl;

        // take input
        char x, y, z;
        cout << "Enter your move, (row, column, safe(s)/flag(f)/save(c)/reload(r)) -> ";
        cin >> x >> y >> z;
        cout << endl;

        int row = charToIndex(x);
        int col = charToIndex(y);
        //cout << "tile: " << gameBoard[row][col] << " " << mineBoard[row][col] << "|" << endl;
        if (nMoves == 0)
            if (isMine(row, col, mineBoard))
                replaceMine(row, col, mineBoard);
        if (z == 'c') {
            saveGame(gameBoard, movesPlayed, nMoves);
        }

        if (z == 'r') {
            long long savid = sec_search();
            cout << savid  << endl;
            if (savid ==  -1) {
                    cout << "cannot reload\n";
                    continue;
            }
            int pos = pri_search(savid);
            cout << pos << endl;
            /*for (int j = 0; j < indsize; ++j) {
                streampos p = atoi(id[j].addr);
                char t[15];
                dfile.seekg(p, ios::beg);
                dfile.getline(t, 15, '|');
                cout << id[j].addr << " " << p << " " << t << " " << dfile.tellg() << endl;
            }*/
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

        if (z == 's') {
            ++movesPlayed;
            if (gameBoard[row][col] == '.' && mineBoard[row][col] == '.') {
                uncoverBoard(gameBoard, mineBoard, row, col, &nMoves);
                if (nMoves == nMovesTotal) {
                    markMines(gameBoard, mineBoard, true);
                    displayBoard(gameBoard);
                    cout << endl << "You won!!! :)" << endl;
                    gameOver = true;
                }
            }
            else if (gameBoard[row][col] == '.' && mineBoard[row][col] == '#') {
                // game over
                gameBoard[row][col] = '#';
                markMines(gameBoard, mineBoard, false);
                displayBoard(gameBoard);
                cout << endl << "You lost! :(" << endl;
                gameOver = true;
            }
            else {
                // illegal move
                cout << "Illegal move. ";
                if (gameBoard[row][col] == 'F')
                    cout << "Cell is a flag. Use f to toggle flag off. ";
                else
                    cout << "Cell is already a number. ";
                cout << endl;
            }
        }

        if (z == 'f') {
            ++movesPlayed;
            if (gameBoard[row][col] == '.') {
                if (flags != 0) {
                    gameBoard[row][col] = 'F';
                    flags--;
                }
                else {
                    cout << "Illegal move. Too many flags!" << endl;
                }
            }
            else if (gameBoard[row][col] == 'F') {
                // undo a flag
                gameBoard[row][col] = '.';
                flags++;
            }
            else {
                // illegal move
                cout << "Illegal move. Cell is a number. " << endl;
            }
        }
    }

    return;
}

int main() {
    in.initial();
    sind.sinitial();
    //cout << indsize << " " << sindsize << endl;
    //chooseDifficultyLevel();
    playMinesweeper();
    return 0;
}

/*
         N.W   N   N.E
           \   |   /
            \  |  /
        W----Cell----E
             / | \
           /   |  \
        S.W    S   S.E

        Cell --> Current Cell ( row,   col   )
        N    --> North        ( row-1, col   )
        S    --> South        ( row+1, col   )
        E    --> East         ( row,   col+1 )
        W    --> West         ( row,   col-1 )
        N.E  --> North-East   ( row-1, col+1 )
        N.W  --> North-West   ( row-1, col-1 )
        S.E  --> South-East   ( row+1, col+1 )
        S.W  --> South-West   ( row+1, col-1 )
*/
