
#ifndef MOVE_H
#define MOVE_H
#include <utility>

using namespace std;

// converts a cell and an index to an adjacent cell (as per the CellState adj array)
pair<int,int> convert(pair<int,int> cell, int index)
{
	if(index == 0)   // previous square, same intersection
		cell.first -= 1;
	
	else if(index == 1) // next square, same intersection
		cell.first += 1;
	
	else if(index == 2) // same square, previous intersection
		cell.second = (cell.second + 7) % 8;
	
	else // same square, next intersection
		cell.second = (cell.second + 1) % 8;
	
	return cell;
}

enum MoveType
{
    Placement,  // Placement move
    Movement    // Movement move
};

struct PlayerMove // represents a move in placement and movement phases
{
	MoveType moveType;
	
	std::pair<int,int> from, to, capture;
	// coordinates that the piece is moved from (in movement phase) and to
	// and the coordinate of the piece captured (if one is captured)
	
	bool canCapture; // stores if a piece has been captured

	PlayerMove()
	{
		// initialize variables
		canCapture = false;
		from = to = capture = {-1, -1};
	}
};

enum PieceType
{
	EmptyCell,  // Cell is empty
	PlayerPiece, // Cell contains piece of player
	EnemyPiece	// Cell contains piece of enemy
};

struct CellState // represents the state of a cell on the board
{
	PieceType pieceType;

	int state;    // stores the state of a piece with the following values:
	// -2 : belongs to enemy, can be captured
	// -1 : belongs to enemy, cannot be captured
	// 0  : belongs to player
	// 1  : empty spot, does not form a mill
	// 2  : empty spot, does form a mill
	
	int adj[4]; // stores the type of move for player pieces (in movement phase) for up to 4 adjacent cells with the following values:
	// -1: if there is no such adjacent cell (e.g. no previous square for innermost square)
	//     or is occupied by another piece
	//  1: adjacent cell is empty but will not form a mill if given piece moves there
	//  2: adjacent cell is empty and will form a mill if given piece moves there

	// indices refer to:
	// 0: previous square, same intersection
	// 1: next square, same intersection
	// 2: same square, previous intersection
	// 3: same square, next intersection
	
	CellState()
	{
		// intialize adj array
		for(int i = 0; i < 4; ++i)
			adj[i] = -1;
	}	
};

#endif
