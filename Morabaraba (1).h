
#ifndef MORABARABA_H
#define MORABARABA_H
#include <vector>
#include <array>
#include <utility>
#include "GameTypes.h"

using namespace std;

class Morabaraba
{
private:
	int pieceCount;	                    // the amount of pieces that each player starts with

	vector<vector<int>> board;          // board[i][j] represents the cell in square i, intersection j:
	                                    // 0 for an empty cell, 1 for player 1, 2 for player 2

	vector<int> cows;                   // stores how many cows each player has

	int lastCapture;                        // how many moves since the last piece was captured

	int moveCount;                      // how many moves have occurred. used to determine if the game is a draw (250 move limit)

	vector<pair<int,int>> prevFrom;     // stores the position from which each players piece was moved in the last turn
	vector<pair<int,int>> prevTo;       // stores the position to which each players piece was moved in the last turn

	vector<int> cowsCaptured;           // stores how many cows of each player was captured to determine remaining cows

public:
	Morabaraba(int pieces)
	{
		pieceCount = pieces;

		board.resize(3);                        // create board of size 3 by 8
		for(int i = 0; i < 3; ++i)
			board[i].resize(8);

		// initalize all variables
		cows.resize(2);
		cows[0] = cows[1] = 0;

		lastCapture = 0;
		moveCount = 0;   		

		prevFrom.resize(2);
		prevFrom[0] = prevFrom[1] = {-1, -1};   // indicates that there is no previous move

		prevTo.resize(2);
		prevTo[0] = prevTo[1] = {-1, -1};       // indicates that there is no previous move

		cowsCaptured.resize(2);
		cowsCaptured[0] = cowsCaptured[1] = 0;
	}

	vector<vector<int>> getBoardDisplay()           // returns the board as is (with each cell 0, 1, or 2)
	{
		return board;
	}

	int getOtherPlayer(int player)   // returns the opposing player
	{	
		if(player == 1)
			return 2;
		else
			return 1;
	}

	int getIndex(int player) 	 // returns the index in the cows array for a given player
	{
		if(player == 1)
			return 0;
		else
			return 1;
	}

	int getCowsLeft(int player)     // returns the number of remaining cows for given player
	{
		int index = getIndex(player);
		return pieceCount - cowsCaptured[index];
	}

	array<pair<int,int>,3> getMill(pair<int,int> cell) // returns a mill containing a given cell (it is assumed that such a mill exists)
	{
		array<pair<int,int>,3> millCells;
		
		int player = board[cell.first][cell.second]; // which player forms a mill

		for(int iscn = 0; iscn < 8; ++iscn)          // determines if a mill is formed across three cells
		{
			bool containsCell = false;           // does the set of three cells contain the given cell
			bool mill = true;                    // does the set of three cells make a mill for the given player

			for(int sq = 0; sq < 3; ++sq)
			{
				if(board[sq][iscn] != player) // cells do not form a mill
					mill = false;

				if(sq == cell.first && iscn == cell.second) // cells  contain given cell
					containsCell = true;
			}

			if(containsCell && mill)             // a mill containing the cell is found
			{
				for(int sq = 0; sq < 3; ++sq)
					millCells[sq] = {sq,iscn};
				return millCells;
			}
		}

		for(int sq = 0; sq < 3; ++sq)                  // determines if a mill is formed within one square
		{
			for(int iscn = 0; iscn < 8; iscn += 2) // iterate through each corner of the cell (where a mill can start)
			{
				bool containsCell = false;     // does the set of three cells contain the given cell
				bool mill = true;

				for(int k = 0; k < 3; ++k)     // does the set of three cells make a mill for the given player
				{
					if(board[sq][(iscn+k) % 8] != player) // cells do not form a mill
						mill = false;
					if(sq == cell.first && (iscn+k) % 8 == cell.second) // cells do not contain given cell
						containsCell = true;
				}

				if(mill && containsCell)       // a mill containing the given cell is found
				{
					for(int k = 0; k < 3; ++k)
						millCells[k] = {sq,(iscn+k) % 8};
					return millCells;
				}
			}
		}
		return millCells;
	}

	bool makesMill(int sq, int iscn, int player)      // checks if a cow will make a mill for a given player if placed at a given cell
	{
		bool makesMill = false;	     // return value, true if cow will make a mill

		if(board[(sq+1)%3][iscn] == player && board[(sq+2)%3][iscn] == player) // mill formed across three squares
			makesMill=true;

		if(iscn % 2 == 0) 	    // even intersection
		{
		    if(board[sq][(iscn+1)%8] == player && board[sq][(iscn+2)%8] == player)
				makesMill=true; // cow makes mill with next two intersections
		    
		    else if(board[sq][(iscn+7)%8] == player && board[sq][(iscn+6)%8] == player)
				makesMill=true; // cow makes mill with previous two intersections
		}

		else			     // odd intersection
		{
			if(board[sq][(iscn+1)%8]==player && board[sq][(iscn+7)%8]==player)
				makesMill=true; // cow contained in mill with next and previous intersection
		}

		return makesMill;
	}

	vector<vector<CellState>> getBoardState(int player) // gets the state of the board for a given player
	{
		vector<vector<CellState>> returnState;      // board state to be returned
		int otherPlayer = getOtherPlayer(player);
		bool outMill = false; // are there any enemy cows that are not in a mill

		returnState.resize(3);
		for(int i = 0; i < 3; ++i)
			returnState[i].resize(8);
		
		// determine if enemy has a piece not contained in a mill
		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				// there is an enemy cow outside a mill, mill cows are protected
				if(board[i][j] == otherPlayer && !makesMill(i, j, otherPlayer))
					outMill = true;
			}
		}
		
		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				// reference to current cell
				CellState& curCell = returnState[i][j];
				
				if(board[i][j] == player)
				{
					curCell.pieceType = PlayerPiece;
					curCell.state = 0; // set state of piece to 0 (player piece) (GameTypes.h)

					board[i][j] = 0; // remove piece to check if mill will be formed without this piece

					// for each adjacent cell of given piece, determine if a mill
					// will be formed if the piece moves to that cell
					// adj array stores 2 if a mill will be formed, 1 if not

					if(i-1 >= 0 && board[i-1][j] == 0) // previous square, same intersection
						curCell.adj[0] = (makesMill(i-1, j, player) ? 2 : 1);	

					if(i+1 <= 2 && board[i+1][j] == 0) // next square, same intersection	
						curCell.adj[1] = (makesMill(i+1, j, player) ? 2 : 1);	

					if(board[i][(j+7)%8] == 0) // same square, previous intersection
						curCell.adj[2] = (makesMill(i, (j+7)%8, player) ? 2 : 1);	

					if(board[i][(j+1)%8] == 0) // same square, next intersection
						curCell.adj[3] = (makesMill(i, (j+1)%8, player) ? 2 : 1);	

					board[i][j] = player; // replace piece that was removed
				}
				
				else if(board[i][j] == 0)
				{
					curCell.pieceType = EmptyCell;
					curCell.state = (makesMill(i, j, player) ? 2 : 1); // will a mill be formed or not
				}
				
				else
				{
					curCell.pieceType = EnemyPiece;
					if(outMill && makesMill(i, j, otherPlayer)) // is there an enemy piece outside a mill while this piece is in a mill
						curCell.state = -1; // enemy piece cannot be captured
					else
						curCell.state = -2; // enemy piece can be captured
				}
			}
		}

		int index = getIndex(player);
		pair<int,int> pFrom, pTo;
		pFrom = prevFrom[index];
		pTo = prevTo[index];
		
		// player did not break a mill in previous move
		if(pTo == pair{-1, -1})
			return returnState;
		
		// player did break a mill in previous move
		// ensure player does not move back to same cell by setting adjacent value to -1 (invalid move)
		// it is assumed that either the square or intersection of from and to are the same (as for any valid move)
		if(pTo.first - 1 == pFrom.first) // previous square, same intersection
			returnState[pTo.first][pTo.second].adj[0] = -1;	

		else if(pTo.first + 1 == pFrom.first) // next square, same intersection
			returnState[pTo.first][pTo.second].adj[1] = -1;	

		else if((pTo.second + 7) % 8 == pFrom.second) // same square, previous intersection
			returnState[pTo.first][pTo.second].adj[2] = -1;	

		else if((pTo.second + 1) % 8 == pFrom.second) // same square, next intersection
			returnState[pTo.first][pTo.second].adj[3] = -1;	

		return returnState;
	}

	// returns the status of the game:
	// -1 if the game is not over
	// 0 if the game is declared a draw
	// 1 if player 1 has won
	// 2 if player 2 has won
	int getGameStatus(int player)
	{
		if(lastCapture == 10 || moveCount == 250)
			return 0; // a draw has occurred

		int index = getIndex(player);		
		int otherPlayer = getOtherPlayer(player);

		if(cows[index] == 2) // player only has 2 cows left, loses
			return otherPlayer;

		vector<vector<CellState>> playerState = getBoardState(player);

		bool availableMoves = false; // does the player have any available moves

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					if(playerState[i][j].adj[k] != -1) // the player does have an available move
						availableMoves = true;
				}
			}
		}

		if(!availableMoves) // the player does not have any available moves, loses
			return otherPlayer;

		return -1;          // indicates the game is not over yet
	}

	void placePiece(const PlayerMove& move, int player)
	{
		++moveCount; // increase move count

		pair<int,int> to = move.to; // extract to pair from move
		board[to.first][to.second] = player; // change cell where piece is placed

		++cows[getIndex(player)]; // increase cow count of player

		if(move.canCapture) // has a capture occurred
		{
			pair<int,int> capture = move.capture; // extract capture pair from move
			board[capture.first][capture.second] = 0; // remove captured enemy piece

			int otherPlayer = getOtherPlayer(player);
			int otherCowIndex = getIndex(otherPlayer);

			--cows[otherCowIndex]; // decrease cow count of enemy player
			++cowsCaptured[otherCowIndex]; // increase the number of cows of enemy captured
		}
	}

	void movePiece(const PlayerMove& move, int player)
	{
		++moveCount; // increase move count

		pair<int,int> from, to;
		from = move.from;
		to = move.to;

		int index = getIndex(player);
		int otherPlayer = getOtherPlayer(player);

		if(makesMill(from.first, from.second, player))
		{
			// player breaks a mill, store information so that this cell is forbidden on the next move
			prevFrom[index] = from;
			prevTo[index] = to;
		}

		else   // otherwise, the player can move back to this cell on the next move
			prevFrom[index] = prevTo[index] = {-1, -1};

		board[from.first][from.second] = 0; // remove player piece from 'from' cell
		board[to.first][to.second] = player; // place player piece in 'to' cell

		if(move.canCapture) // has a capture taken place
		{
			pair<int,int> capture = move.capture; // cell of captured piece
			board[capture.first][capture.second] = 0;

			int otherCowIndex = getIndex(otherPlayer);

			--cows[otherCowIndex];
			++cowsCaptured[otherCowIndex];

			lastCapture = 0; // reset the last capture counter
		}

		else if(cows[0] == 3 && cows[1] == 3) // players have 3 cows each and no capture occurred
			++lastCapture;
	}
};
#endif
