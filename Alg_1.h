
#include <random>
#include "GameTypes.h"

using namespace std;

// random algorithm class
class Alg_1
{
private:
	mt19937 rng; // random number generator
public:
	Alg_1()
	{
		rng = mt19937(random_device{}()); // seed the random number generator
	}

	// randomly choose an enemy piece that can be captured
	pair<int,int> findCapturable(const vector<vector<CellState>>& boardState)
	{
		vector<pair<int,int>> capturablePieces; // stores all enemy pieces that can be captured

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				if(boardState[i][j].state == -2) // enemy piece that can be captured
					capturablePieces.push_back({i, j});
			}
		}
		
		uniform_int_distribution<int> dist(0, capturablePieces.size() - 1); // create distribution
		int capturableIndex = dist(rng); // get random capturable index

		return capturablePieces[capturableIndex]; // return the piece at the index
	}
	
	// returns a placement move
	PlayerMove placePiece(const vector<vector<CellState>>& boardState)
	{
		int totalMoves = 0; // how many possible moves are there

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				if(boardState[i][j].pieceType == EmptyCell) // a piece can be placed in any empty cell
					++totalMoves;
			}
		}

		uniform_int_distribution<int> dist(1, totalMoves); // create distribution
		int index = dist(rng); // get random number

		int counter = 0; // counter to determine if move index is reached
		bool cellFound = false; // have we reached the move index yet
		int sq, iscn; // stores the coordinates of the move

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				if(boardState[i][j].pieceType == EmptyCell)
					++counter; // increase counter index

				if(counter == index && !cellFound) // randomly chosen move index is reached
				{
					sq = i;      // set square of cell
					iscn = j;    // intersection of cell
					cellFound = true; // set flag since we are done
				}
			}
		}

		PlayerMove move;
		move.moveType = Placement; // set move type to placement
		move.to = {sq, iscn};   // set move destination to cell

		if(boardState[sq][iscn].state == 2) // the chosen move allows a capture
		{
			move.canCapture = true;  // set flag to true indicating a capture
			move.capture = findCapturable(boardState); // find an enemy piece to capture
		}

		return move;
	}
	
	// returns a movement move
	PlayerMove movePiece(const vector<vector<CellState>>& boardState)
	{
		int totalMoves = 0; // stores the total number of moves possible	

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				if(boardState[i][j].pieceType != PlayerPiece) // only player pieces can be moved
					continue; // skip cell

				for(int k = 0; k < 4; ++k)
				{
					// player piece can move to adjacent cell - valid move
					if(boardState[i][j].adj[k] != -1)
						++totalMoves; // increase total moves for each adjacent valid move
				}
			}
		}

		uniform_int_distribution<int> dist(1, totalMoves); // create distribution
		int index = dist(rng); // get random number

		int counter = 0; // counter to determine if move is reached
		bool moveFound = false; // flag to determine if move is reached
		int sq, iscn, moveIndex; // stores the cell of chosen move and which adjacent cell to move to

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					if(boardState[i][j].adj[k] != -1)
						++counter; // increment counter to determine if move is reached

					if(counter == index && !moveFound) // move is reached
					{
						sq = i; // set square of piece to move
						iscn = j; // set intersection of piece to move
						moveIndex = k; // set the adjacent cell that piece moves to (based on CellState struct)
						moveFound = true; // set a flag indicating move is found
					}
				}
			}
		}

		PlayerMove move;

		move.moveType = Movement; // set move type to movement
		move.from = {sq, iscn}; // set cell of piece to move
		move.to = convert(move.from, moveIndex); // set destination of cell, converting to absolute coordinates with convert (GameTypes.h)

		if(boardState[sq][iscn].adj[moveIndex] == 2) // an enemy piece can be captured
		{
			move.canCapture = true; // set capture flag to true
			move.capture = findCapturable(boardState); // set the captured piece
		}

		return move;
	}
};
