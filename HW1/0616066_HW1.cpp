#define BOARD_SIZE 6	//// 6x6 board size

#include <iostream>		// cout, invalid_argument, getline
#include <fstream>		// file open
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <stack>
#include <algorithm>	// find
#include <time.h>		// clock_t, CLOCKS_PER_SEC
#include <stdlib.h>		// exit
using namespace std;

class Car{
public:
	int row, column, length;
	string orient;
	Car(int, int, int, string);
	void printCar();
	friend bool operator== (const Car &n1, const Car &n2);
};

Car::Car(int r, int c, int l, string orientation){
	/*
	Create a car.
	Arguments:
		row: Row # of the bottom left corner of the car (1-6)
		column: Column # of the bottom left corner of the car (0-5)
		length: Length of the car (2-5)
		orient: Orientation of the car (vertical : v or horizontal : h)
	*/
	if ((r >= 0) && (r <= 5))	row = r;
	else	throw invalid_argument("Invalid row. Must be integer in [1, 6].");

	if ((c >= 0) && (c <= 5))	column = c;
	else	throw invalid_argument("Invalid column. Must be integer in [0, 5].");

	if ((l >= 2) && (l <= 5))	length = l;
	else	throw invalid_argument("Invalid length. Must be integer in [2, 5].");

	if (orientation == "v"){
		orient = orientation;
		if (row + length > 6)	throw invalid_argument("Invalid configuration. Car is out of bounds vertically.");
	}
	else if (orientation == "h"){
		orient = orientation;
		if (column + length > 6)	throw invalid_argument("Invalid configuration. Car is out of bounds horizontally.");
	}
	else	throw invalid_argument("Invalid configuration. Must be 'v' or 'h'.");
}

void Car::printCar(){
	cout << "Car(" << row << ", " << column << ", " << length << ", " << orient << ")" << endl;
}

//// Check the equality of cars
bool operator== (const Car &c1, const Car &c2){
	return ((c1.row == c2.row) && (c1.column == c2.column) && (c1.length == c2.length) && (c1.orient == c2.orient));
}

class Board{
public:
	vector <Car> vehicles;
	int carCount;
	float cost;
	Board(vector <Car>);
	vector <vector <string> > getBoard();
	void printBoard();
	bool isSolved();
	int heuristic();
	vector <Board> getMoves();

	Board& operator=(const Board& b) {
	    vehicles = b.vehicles;
	    carCount = b.carCount;
	    cost = b.cost;
	    return *this;
	}

	friend bool operator== (const Board &n1, const Board &n2);
	friend bool operator> (const Board &n1, const Board &n2);
	friend bool operator< (const Board &n1, const Board &n2);
};

Board::Board(vector <Car> cs){
	/*
	Create a board.
	Arguments:
		cs: A vector of cars
	*/
	vehicles = cs;
	carCount = vehicles.size();
	cost = heuristic();
}

int Board::heuristic(){
	float cost;
	int tar_col = vehicles[0].column + vehicles[0].length - 1;
	int tar_row = 2;

	// //h1
	// for(int i=1;i<carCount;i++){
	// 	if(vehicles[i].orient == "v"){
	// 		if(vehicles[i].column > tar_col){
	// 			if(vehicles[i].row <=2 && vehicles[i].row + vehicles[i].length >= 2) cost++;
	// 		}
	// 	}
	// }
	// //endh1

	//h2
	vector <vector <string> > b;
	float l_count=0;
	float r_count=0;
	b = getBoard();
	for (int i=0; i<BOARD_SIZE; i++){
		for (int j=0; j<BOARD_SIZE; j++){
			if(b[i][j] != " _ "){
				if(j > tar_col) r_count++;
				else l_count++;
			}
		}
	}
	float l_total = (BOARD_SIZE - tar_row - 1) * BOARD_SIZE;
	float r_total = (tar_row + 1) * BOARD_SIZE;
	cost = (r_count / r_total) / (l_count / l_total);
	//endh2


	return cost;
}

//// Get board with cars (with their coordinates) in it
vector <vector <string> > Board::getBoard(){

	//// Initialize board
	vector <vector <string> > board(BOARD_SIZE);
	for (int i=0; i<BOARD_SIZE; i++) {
		for (int j=0; j<BOARD_SIZE; j++) {
			board[i].push_back(" _ ");
		}
	}

	//// Place the target car
	for (int i=0; i<vehicles[0].length; i++)	board[vehicles[0].row ][vehicles[0].column + i] = " X ";

	//// Place the obstructive cars
	string index;
	string strName;
	for (int i=1; i<carCount; i++) {
		index = to_string(i);
		strName = " O" + index;
		if (vehicles[i].orient == "h"){
			for (int j=0; j<vehicles[i].length; j++)	board[vehicles[i].row][vehicles[i].column + j] = strName;
		}
		else {
			for (int k=0; k<vehicles[i].length; k++)	board[vehicles[i].row + k][vehicles[i].column] = strName;
		}
	}
	return board;
}

//// Print the board on the console
void Board::printBoard(){
	vector <vector <string> > board = getBoard();
	for (int i=0; i<BOARD_SIZE; i++) {
		for (int j=0; j<BOARD_SIZE; j++) {
			cout << board[i][j];
		}
		cout << endl;
	}
}

//// Check if the target car is out
bool Board::isSolved(){
	return (vehicles[0].column + vehicles[0].length == BOARD_SIZE);
}

//// Get all possible moves available from one board state
//// One move means moving a car just one space (left or right for a car with 'h', up or down for a car with 'v')
vector <Board> Board::getMoves(){

	vector <Board> moves;		// Collection of all possible board states can be moved

	vector <vector <string> > board = getBoard();
	vector <Car> vehiclesCopy;
	for (int i=0; i<carCount; i++) {
		int r = vehicles[i].row;
		int c = vehicles[i].column;
		int l = vehicles[i].length;
		string o = vehicles[i].orient;
		if (o == "h"){
			//// Move to the left
			if ((c >= 1) && (board[r][c - 1] == " _ ")){
				Car newCar(r, c - 1, l, o);
				vehiclesCopy = vehicles;
				vehiclesCopy[i] = newCar;
				moves.push_back(Board(vehiclesCopy));
			}
			//// Move to the right
			if ((c + l <= 5) && (board[r][c + l] == " _ ")){
				Car newCar(r, c + 1, l, o);
				vehiclesCopy = vehicles;
				vehiclesCopy[i] = newCar;
				moves.push_back(Board(vehiclesCopy));
			}
		}
		else{
			//// Move to the up
			if ((r >= 1) && (board[r - 1][c] == " _ ")){
				Car newCar(r - 1, c, l, o);
				vehiclesCopy = vehicles;
				vehiclesCopy[i] = newCar;
				moves.push_back(Board(vehiclesCopy));
			}
			//// Move to the down
			if ((r + l <= 5) && (board[r + l][c] == " _ ")){
				Car newCar(r + 1, c, l, o);
				vehiclesCopy = vehicles;
				vehiclesCopy[i] = newCar;
				moves.push_back(Board(vehiclesCopy));
			}
		}
	}
	return moves;
}

//// Check the equality of boards
//// All positions of the same cars must be equal
bool operator== (const Board &b1, const Board &b2){
	return b1.vehicles == b2.vehicles;
}

bool operator> (const Board &b1, const Board &b2){
	return b1.cost < b2.cost; //let small element first
}

bool operator< (const Board &b1, const Board &b2){
	return b1.cost > b2.cost; //let small element first
}

struct vecBor_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(std::vector<Board> const& a, std::vector<Board> const& b) const
    { 
        return a[a.size()-1].cost > b[b.size()-1].cost;
    }
};

///////////////////////////////////
// Breadth-First Search Function //
///////////////////////////////////

void bfs(Board &board, char *outputFile){
	/*
	Search the solutions by breadth first search.
	Writes solution on output file as a sequence of board states.
	Gives info(nodes, # of solution etc.) on the console.
	Arguments:
		board: Starting board
		outputFile: File to write the solutions as a sequence of board states
	*/

	Board frontBoard(board.vehicles);	//// Copy of board, first element of queue of boards
	vector <Board> frontPath;			//// First element of queue of paths

	vector <Board> discovered;
	vector <vector <Board> > solutions;

	vector <Board> moves;
	map <int, int> depthStates;		//// The boards visited at each depth

	queue <Board> boardQueue;			//// Queue of boards
	boardQueue.push(frontBoard);

	queue <vector <Board> > pathQueue;	//// Queue of paths
	vector <Board> newPath;

	int step=0, find_step;
	bool first_find_sol=false;

	while (boardQueue.size() != 0){
		step++;
		frontBoard = boardQueue.front();
		boardQueue.pop();
		if (pathQueue.size() != 0){
			frontPath = pathQueue.front();
			pathQueue.pop();
		}
		frontPath.push_back(frontBoard);
		newPath = frontPath;

		if (!depthStates[newPath.size()])	depthStates[newPath.size()] = 1;
		else	depthStates[newPath.size()] += 1;

		//// If board is discovered, continue
		if (find(discovered.begin(), discovered.end(), frontBoard) != discovered.end())    continue;
		else	discovered.push_back(frontBoard);

		//// If board is solved
		if (frontBoard.isSolved()){
			solutions.push_back(newPath);
			if(!first_find_sol){
				find_step=step;
				first_find_sol=true;
			} 
			break;
		}	
		else{
			moves = frontBoard.getMoves();
			for (int i=0; i<moves.size(); i++){
				boardQueue.push(moves[i]);
				pathQueue.push(newPath);
			}
		}
	}

	//// Sum of total nodes generated
	int maxNodes = 0;
	for (int i=0; i<depthStates.size(); i++){
		maxNodes += depthStates[i];
	}

	/////////////////////////
	// Checking for cycles //
	/////////////////////////
	int cycleCount = 0;
	for (int i=0; i<solutions.size(); i++){
		for (int j=0; j<solutions.size(); j++){
			if (i != j){
				if (solutions[i].back() == solutions[j].back())		cycleCount++;
			}
		}
	}

	//// Find the shortest solution
	//// Write it on the output file
	vector <Board> shortestSoln;
	if (solutions.size() != 0){
		shortestSoln = solutions[0];
		// for (int i=0; i<solutions.size(); i++){
		// 	if (solutions[i].size() < shortestSoln.size())		shortestSoln = solutions[i];
		// }
		ofstream writefile;
		writefile.open(outputFile);
		if (writefile.is_open()){
			for (int i=0; i<shortestSoln.size(); i++){
				for (int j=0; j<shortestSoln[i].carCount; j++){
					writefile << to_string(shortestSoln[i].vehicles[j].row);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].column);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].length);
					writefile << " ";
					writefile << shortestSoln[i].vehicles[j].orient;
					writefile << "\n";
				}
				writefile << "\n";
			}
			writefile.close();
			cout << "The solution file " << outputFile << " is created." << endl;
		}
		else	cout << "Output file could not be created!" << endl;
	}

	cout << "TOTAL CYCLES:" << cycleCount << endl;
	cout << "MOVE # OF THE SOLUTION:" << shortestSoln.size() - 1 << endl;
	cout << "STEP WHEN FOUND FIRST SOLUTION:" << find_step << endl;
	cout << "TOTAL NODES GENERATED:" << discovered.size() << endl;
	cout << "MAXIMUM # OF NODES KEPT IN MEMORY:" << maxNodes << endl;

}


///////////////////////////////////
// Depth-First Search Function //
///////////////////////////////////

void dfs(Board &board, char *outputFile){

	Board topBoard(board.vehicles);		//// Copy of board, top element of stack of boards
	vector <Board> topPath;				//// Top element of stack of paths

	vector <Board> discovered;
	vector <vector <Board> > solutions;

	vector <Board> moves;
	map <int, int> depthStates;		//// The boards visited at each depth

	stack <Board> boardStack;			//// Stack of boards
	boardStack.push(topBoard);
	stack <vector <Board> > pathStack;	//// Stack of paths
	vector <Board> newPath;

	int step=0, find_step;
	bool first_find_sol=false;
	int max_size;

	while (boardStack.size() != 0){
		step++;

		topBoard = boardStack.top();
		boardStack.pop();

		if (pathStack.size() != 0){
			topPath = pathStack.top();
			pathStack.pop();
		}

		topPath.push_back(topBoard);
		newPath = topPath;
		if(newPath.size()> max_size) max_size=newPath.size();
		if (!depthStates[newPath.size()])	depthStates[newPath.size()] = 1;
		else	depthStates[newPath.size()] += 1;

		//// If board is discovered, continue
		if (find(discovered.begin(), discovered.end(), topBoard) != discovered.end())	continue;
		else	discovered.push_back(topBoard);

		//// If board is solved
		if (topBoard.isSolved()){
			solutions.push_back(newPath);
			if(!first_find_sol){
				find_step=step;
				first_find_sol=true;
			} 
			break;
		}	
		else{
			moves = topBoard.getMoves();
			for (int i=0; i<moves.size(); i++){
				boardStack.push(moves[i]);
				pathStack.push(newPath);
			}
		}
	}

	//// Sum of total nodes generated
	int maxNodes = 0;
	for (int i=0; i<depthStates.size(); i++){
		maxNodes += depthStates[i];
	}

	/////////////////////////
	// Checking for cycles //
	/////////////////////////
	int cycleCount = 0;
	for (int i=0; i<solutions.size(); i++){
		for (int j=0; j<solutions.size(); j++){
			if (i != j){
				if (solutions[i].back() == solutions[j].back())    cycleCount++;
			}
		}
	}

	//// Find the shortest solution
	//// Write it on the output file
	vector <Board> shortestSoln;
	if (solutions.size() != 0){
		shortestSoln = solutions[0];
		// for (int i=0; i<solutions.size(); i++){
		// 	if (solutions[i].size() < shortestSoln.size())		shortestSoln = solutions[i];
		// }
		ofstream writefile;
		writefile.open(outputFile);
		if (writefile.is_open()){
			for (int i=1; i<shortestSoln.size(); i++){
				for (int j=0; j<shortestSoln[i].carCount; j++){
					writefile << to_string(shortestSoln[i].vehicles[j].row);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].column);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].length);
					writefile << " ";
					writefile << shortestSoln[i].vehicles[j].orient;
					writefile << "\n";
				}
				writefile << "\n";
			}
			writefile.close();
			cout << "The solution sequence file " << outputFile << " is created." << endl;
		}
		else	cout << "Output file could not be created!" << endl;
	}

	cout << "TOTAL CYCLES:" << cycleCount << endl;
	cout << "MOVE # OF THE SOLUTION:" << shortestSoln.size() - 1 << endl;
	cout << "STEP WHEN FOUND FIRST SOLUTION:" << find_step << endl;
	cout << "TOTAL NODES GENERATED:" << discovered.size() << endl;
	cout << "MAXIMUM # OF NODES KEPT IN MEMORY:" << maxNodes << endl;

}

///////////////////////////////////////////
//  Iterative-Deepening Search Function  //
///////////////////////////////////////////

void ids(Board &board, char *outputFile){

	int limit_depth=1;

	vector <Board> newPath;
	vector <vector <Board> > solutions;
	vector <Board> moves;
	vector <Board> discovered;
	map <int, int> depthStates;		//// The boards visited at each depth
	int step=0, find_step;
	bool first_find_sol=false;

	while(!solutions.size()){
		
		Board topBoard(board.vehicles);		//// Copy of board, top element of stack of boards
		stack <Board> boardStack;			//// Stack of boards
		boardStack.push(topBoard);
		stack <vector <Board> > pathStack;	//// Stack of paths
		discovered=vector <Board>();
		vector <Board> topPath;

		step=0;
		limit_depth++;
		while (boardStack.size() != 0){
			step++;

			topBoard = boardStack.top();
			boardStack.pop();

			if (pathStack.size() != 0){
				topPath = pathStack.top();
				pathStack.pop();
			}

			topPath.push_back(topBoard);
			newPath = topPath;
			if (!depthStates[newPath.size()])	depthStates[newPath.size()] = 1;
			else	depthStates[newPath.size()] += 1;

			//// If board is discovered, continue
			if (find(discovered.begin(), discovered.end(), topBoard) != discovered.end())	continue;
			else	discovered.push_back(topBoard);

			//// If board is solved
			if (topBoard.isSolved()){
				solutions.push_back(newPath);
				if(!first_find_sol){
					find_step=step;
					first_find_sol=true;
				} 
			}	
			else{
				if(limit_depth > newPath.size()){
					moves = topBoard.getMoves();
					for (int i=0; i<moves.size(); i++){
						boardStack.push(moves[i]);
						pathStack.push(newPath);
					}				
				}

			}
		}

	}

	//// Sum of total nodes generated
	int maxNodes = 0;
	for (int i=0; i<depthStates.size(); i++){
		maxNodes += depthStates[i];
	}

	/////////////////////////
	// Checking for cycles //
	/////////////////////////
	int cycleCount = 0;
	for (int i=0; i<solutions.size(); i++){
		for (int j=0; j<solutions.size(); j++){
			if (i != j){
				if (solutions[i].back() == solutions[j].back())    cycleCount++;
			}
		}
	}

	//// Find the shortest solution
	//// Write it on the output file
	vector <Board> shortestSoln;
	if (solutions.size() != 0){
		shortestSoln = solutions[0];
		for (int i=0; i<solutions.size(); i++){
			// cout<<solutions[i].size()<<endl;
			if (solutions[i].size() < shortestSoln.size())		shortestSoln = solutions[i];
		}
		ofstream writefile;
		writefile.open(outputFile);
		if (writefile.is_open()){
			for (int i=1; i<shortestSoln.size(); i++){
				for (int j=0; j<shortestSoln[i].carCount; j++){
					writefile << to_string(shortestSoln[i].vehicles[j].row);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].column);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].length);
					writefile << " ";
					writefile << shortestSoln[i].vehicles[j].orient;
					writefile << "\n";
				}
				writefile << "\n";
			}
			writefile.close();
			cout << "The solution sequence file " << outputFile << " is created." << endl;
		}
		else	cout << "Output file could not be created!" << endl;
	}

	cout << "TOTAL CYCLES:" << cycleCount << endl;
	cout << "MOVE # OF THE SOLUTION:" << shortestSoln.size() - 1 << endl;
	cout << "STEP WHEN FOUND FIRST SOLUTION:" << find_step << endl;
	cout << "TOTAL NODES GENERATED:" << discovered.size() << endl;
	cout << "MAXIMUM # OF NODES KEPT IN MEMORY:" << maxNodes << endl;
}

//////////////////////////
//  A* Search Function  //
//////////////////////////

void astar(Board &board, char *outputFile){

	vector <Board> newPath;
	vector <Board> topPath;
	vector <vector <Board> > solutions;
	vector <Board> moves;
	vector <Board> discovered;
	map <int, int> depthStates;		//// The boards visited at each depth
	int step=0, find_step;
	bool first_find_sol=false;
		
	Board topBoard(board.vehicles);		//// Copy of board, top element of stack of boards
	priority_queue <Board> boardQueue;	//// queue of boards
	boardQueue.push(topBoard);

	priority_queue<vector<Board>, vector<vector<Board> >, vecBor_comparator> pathQueue;	//// queue of paths
	newPath.push_back(topBoard);
	pathQueue.push(newPath);
	

	step=0;
	first_find_sol=false;

	while (boardQueue.size() != 0){
		step++;

		topBoard = boardQueue.top();
		boardQueue.pop();

		if (pathQueue.size() != 0){
			topPath = pathQueue.top();
			pathQueue.pop();
		}

		if (!depthStates[topPath.size()])	depthStates[topPath.size()] = 1;
		else	depthStates[topPath.size()] += 1;

		//// If board is discovered, continue
		if (find(discovered.begin(), discovered.end(), topBoard) != discovered.end())	continue;
		else	discovered.push_back(topBoard);

		//// If board is solved
		if (topBoard.isSolved()){
			solutions.push_back(topPath);
			if(!first_find_sol){
				find_step=step;
				first_find_sol=true;
			} 
			break;
		}	
		else{
			moves = topBoard.getMoves();
			for (int i=0; i<moves.size(); i++){
				boardQueue.push(moves[i]);
				newPath=topPath;
				newPath.push_back(moves[i]);
				pathQueue.push(newPath);
			}				

		}
	}


	//// Sum of total nodes generated
	int maxNodes = 0;
	for (int i=0; i<depthStates.size(); i++){
		maxNodes += depthStates[i];
	}

	/////////////////////////
	// Checking for cycles //
	/////////////////////////
	int cycleCount = 0;
	for (int i=0; i<solutions.size(); i++){
		for (int j=0; j<solutions.size(); j++){
			if (i != j){
				if (solutions[i].back() == solutions[j].back())    cycleCount++;
			}
		}
	}

	//// Find the shortest solution
	//// Write it on the output file
	vector <Board> shortestSoln;
	if (solutions.size() != 0){
		shortestSoln = solutions[0];
		for (int i=0; i<solutions.size(); i++){
			// cout<<solutions[i].size()<<endl;
			if (solutions[i].size() < shortestSoln.size())		shortestSoln = solutions[i];
		}
		ofstream writefile;
		writefile.open(outputFile);
		if (writefile.is_open()){
			for (int i=1; i<shortestSoln.size(); i++){
				for (int j=0; j<shortestSoln[i].carCount; j++){
					writefile << to_string(shortestSoln[i].vehicles[j].row);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].column);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].length);
					writefile << " ";
					writefile << shortestSoln[i].vehicles[j].orient;
					writefile << "\n";
				}
				writefile << "\n";
			}
			writefile.close();
			cout << "The solution sequence file " << outputFile << " is created." << endl;
		}
		else	cout << "Output file could not be created!" << endl;
	}

	cout << "TOTAL CYCLES:" << cycleCount << endl;
	cout << "MOVE # OF THE SOLUTION:" << shortestSoln.size() << endl;
	cout << "STEP WHEN FOUND FIRST SOLUTION:" << find_step << endl;
	cout << "TOTAL NODES GENERATED:" << discovered.size() << endl;
	cout << "MAXIMUM # OF NODES KEPT IN MEMORY:" << maxNodes << endl;
}

//////////////////////////
//  IDA* Search Function  //
//////////////////////////

void idastar(Board &board, char *outputFile){

	vector <Board> topPath;
	vector <vector <Board> > solutions;
	vector <Board> moves;
	vector <Board> discovered;
	
	map <int, int> depthStates;		//// The boards visited at each depth
	int step=0, find_step;
	bool first_find_sol=false;
	int limit_depth=0;

	while(!solutions.size()){
		limit_depth++;

		Board topBoard(board.vehicles);		//// Copy of board, top element of stack of boards
		priority_queue <Board> boardQueue;	//// queue of boards
		boardQueue.push(topBoard);

		priority_queue<vector<Board>, vector<vector<Board> >, vecBor_comparator> pathQueue;	//// queue of paths
		vector <Board> newPath;
		newPath.push_back(topBoard);
		pathQueue.push(newPath);
		discovered=vector <Board>();
		step=0;
		while (boardQueue.size() != 0){
			step++;

			topBoard = boardQueue.top();
			boardQueue.pop();

			if (pathQueue.size() != 0){
				topPath = pathQueue.top();
				pathQueue.pop();
			}
			if(topPath.size()>=limit_depth) continue;

			if (!depthStates[topPath.size()])	depthStates[topPath.size()] = 1;
			else	depthStates[topPath.size()] += 1;

			//// If board is discovered, continue
			if (find(discovered.begin(), discovered.end(), topBoard) != discovered.end())	continue;
			else	discovered.push_back(topBoard);

			//// If board is solved
			if (topBoard.isSolved()){
				solutions.push_back(topPath);
				if(!first_find_sol){
					find_step=step;
					first_find_sol=true;
				} 
				break;
			}	
			else{
				moves = topBoard.getMoves();
				for (int i=0; i<moves.size(); i++){
					boardQueue.push(moves[i]);
					newPath=topPath;
					newPath.push_back(moves[i]);
					pathQueue.push(newPath);
				}				

			}
		}
	}

	//// Sum of total nodes generated
	int maxNodes = 0;
	for (int i=0; i<depthStates.size(); i++){
		maxNodes += depthStates[i];
	}

	/////////////////////////
	// Checking for cycles //
	/////////////////////////
	int cycleCount = 0;
	for (int i=0; i<solutions.size(); i++){
		for (int j=0; j<solutions.size(); j++){
			if (i != j){
				if (solutions[i].back() == solutions[j].back())    cycleCount++;
			}
		}
	}

	//// Find the shortest solution
	//// Write it on the output file
	vector <Board> shortestSoln;
	if (solutions.size() != 0){
		shortestSoln = solutions[0];
		// for (int i=0; i<solutions.size(); i++){
		// 	if (solutions[i].size() < shortestSoln.size())		shortestSoln = solutions[i];
		// }
		ofstream writefile;
		writefile.open(outputFile);
		if (writefile.is_open()){
			for (int i=1; i<shortestSoln.size(); i++){
				for (int j=0; j<shortestSoln[i].carCount; j++){
					writefile << to_string(shortestSoln[i].vehicles[j].row);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].column);
					writefile << " ";
					writefile << to_string(shortestSoln[i].vehicles[j].length);
					writefile << " ";
					writefile << shortestSoln[i].vehicles[j].orient;
					writefile << "\n";
				}
				writefile << "\n";
			}
			writefile.close();
			cout << "The solution sequence file " << outputFile << " is created." << endl;
		}
		else	cout << "Output file could not be created!" << endl;
	}

	cout << "TOTAL CYCLES:" << cycleCount << endl;
	cout << "MOVE # OF THE SHORTEST SOLUTION:" << shortestSoln.size() << endl;
	cout << "STEP WHEN FOUND FIRST SOLUTION:" << find_step << endl;
	cout << "TOTAL NODES GENERATED:" << discovered.size() << endl;
	cout << "MAXIMUM # OF NODES KEPT IN MEMORY:" << maxNodes << endl;
}


/////////////////////////
// Read file, get cars //
/////////////////////////

vector <Car> readCars(char *inputFile){
	/*
	Reads input file, gets cars.
	Arguments:
		inputFile: File to read the cars
	*/
	ifstream readfile;

	//// Count the # of cars
	int carCount;
	readfile.open(inputFile);
	if (readfile.is_open()){
		string line;
		int i;
		for (i=0; getline(readfile, line); i++)		;
		carCount = i;
		readfile.close();
	}
	else{
		cout << "File could not be opened!" << endl;
		exit(1);
	}

	vector <Car> cars;

	//// Get cars from file
	int idx, r, c, l, o;
	string orientation;
	readfile.open(inputFile);
	if (readfile.is_open()){
		while (!readfile.eof()){
			for (int i=0; i<carCount; i++){
				readfile >> idx;
				readfile >> r;
				readfile >> c;
				readfile >> l;
				readfile >> o;
				if(o==1) orientation="h";
				else orientation="v";
				cars.push_back(Car(r, c, l, orientation));
			}
		}
		readfile.close();
	}
	else{
		cout << "File could not be opened!" << endl;
		exit(1);
	}

	return cars;
}


int main(int argc, char* argv[]){

	vector <Car> cars = readCars(argv[2]);
	Board board(cars);
	cout << "\nSTARTING BOARD STATE:\n" << endl;
	board.printBoard();
	cout << endl;

	clock_t t0 = clock();
	if (string(argv[1]) == "bfs")				bfs(board, argv[3]);
	else if (string(argv[1]) == "dfs")			dfs(board, argv[3]);
	else if (string(argv[1]) == "ids")			ids(board, argv[3]);
	else if (string(argv[1]) == "astar")		astar(board, argv[3]);
	else if (string(argv[1]) == "idastar")		idastar(board, argv[3]);
	else	cout<<"Choose 'bfs' or 'dfs' or 'ids' or 'astar' or 'idastar' as solution method"<<endl;
	cout << "TOTAL " << string(argv[1]) << " TIME:" << double(clock() - t0)/CLOCKS_PER_SEC << " seconds" << endl;

return 0;
}