// 2741179 - Muhammad Shah Khan
#include <array>
#include <algorithm>
#include "GameTypes.h"

using namespace std;

class Alg_2
{
public:
	// returns the absolute value of an integer
	int abs(int x)
	{
		if(x >= 0)
			return x;
		else
			return -x;
	}

	// finds the best set of 3 cells within the same square by a scoring function:
	// for each set of 3 cells in which a mill can be formed, each cell is assigned a weight:
	// 0 for an empty cell, 1 for a player cell, -2 for an enemy cell
	// the set of 3 cells with the greatest score is returned
	array<int,3> bestSameSquare(const vector<vector<CellState>>& boardState)
	{
		// contains scores for each set of 3 cells and the
		// starting cell (cell with smallest intersection number)
		vector<array<int,3>> scores;

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; j += 2)
			{
				// a mill can be formed in square i, cells j, j+1, j+2
				bool allFilled = true; // are all cells filled

				for(int k = 0; k < 3; ++k)
				{
					if(boardState[i][(j+k) % 8].pieceType == EmptyCell)
						allFilled = false; // there is at least one empty cell among the three
				}

				if(allFilled) // no empty cells, go the next set of three cells
					continue;

				int score = 0; // score of three cells

				for(int k = 0; k < 3; ++k)
				{
					PieceType pieceType = boardState[i][(j+k) % 8].pieceType; // type of piece in cell

					if(pieceType == PlayerPiece)
						++score; // increase score by 1

					else if(pieceType == EnemyPiece)
						score -= 2; // decrease score by 2
				}

				scores.push_back({score, i, j}); // add score and starting cell to array
			}
		}

		// sort the array with the highest scoring cell first
		sort(scores.begin(), scores.end(), [](array<int,3> a, array<int,3> b){
			return a[0] > b[0];
		});

		// return the score and cell of the cell with the highest score
		return scores[0];
	}

	// finds the best set of 3 cells that lie across all 3 squares by a scoring function:
	// for each set of 3 cells in which a mill can be formed, each cell is assigned a weight:
	// 0 for an empty cell, 1 for a player cell, -2 for an enemy cell
	// the set of 3 cells with the greatest score is returned
	array<int,2> bestDifferentSquares(const vector<vector<CellState>>& boardState)
	{
		vector<array<int,2>> scores; // stores the score and intersection of each set of 3 cells
		// all sets of 3 cells have the same intersection but distinct square numbers

		for(int i = 0; i < 8; ++i)
		{
			bool allFilled = true; // are all cells filled

			for(int j = 0; j < 3; ++j)
			{
				if(boardState[j][i].pieceType == EmptyCell)
					allFilled = false; // at least one empty cell
			}

			if(allFilled)
				continue; // skip set of three cells
				
			int score = 0; // score of three cells

			for(int j = 0; j < 3; ++j)
			{
				PieceType pieceType = boardState[j][i].pieceType; // type of piece in cell

				if(pieceType == PlayerPiece)
					++score; // increase score by 1

				else if(pieceType == EnemyPiece)
					score -= 2; // decrease score by 2
			}

			scores.push_back({score, i});  // add score and intersection number to list
		}

		// sort scores with highest score first
		sort(scores.begin(), scores.end(), [](array<int,2> a, array<int,2> b){
			return a[0] > b[0];
		});

		// return highest score and intersection number
		return scores[0];
	}
	
	// compares the best cell in same square and different squares and returns the better one
	pair<int,int> bestCell(const vector<vector<CellState>>& boardState)
	{
		array<int,3> same = bestSameSquare(boardState); // best cells in same square
		array<int,2> different = bestDifferentSquares(boardState); // best cells across squares

		if(same[0] > different[0]) // is score for same squares higher than different
		{
			int sq = same[1]; // square of starting cell
			int iscn = same[2]; // intersection of starting cell

			for(int k = 0; k < 3; ++k)
			{
				// find an empty cell in set of three cells
				if(boardState[sq][(iscn + k) % 8].pieceType == EmptyCell)
					return {sq, (iscn + k) % 8};
			}
		}

		else // the score for different squares is higher
		{
			int iscn = different[1]; // intersection number of three cells

			for(int k = 0; k < 3; ++k)
			{
				// find an empty cell in set of three cells
				if(boardState[k][iscn].pieceType == EmptyCell)
					return {k, iscn};
			}
		}

		return {-1, -1}; // an error has occurred. ensure something is returned
	}

	// returns an enemy piece that can be captured (the first one found)
	pair<int,int> findCapturable(const vector<vector<CellState>>& boardState)
	{
		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				if(boardState[i][j].state == -2) // enemy piece that can be captured
					return {i, j}; // return cell of piece that can be captured
			}
		}

		return {-1, -1}; // an error has occurred (at least one enemy piece is capturable)
	}

	PlayerMove placePiece(const vector<vector<CellState>>& boardState)
	{
		PlayerMove move;

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				if(boardState[i][j].state == 2) // a mill can be formed, form one
				{
					move.moveType = Placement;		
					move.to = pair<int,int> {i, j}; 

					move.canCapture = true;
					move.capture = findCapturable(boardState); // find a piece to capture
					// there is always at least one capturable piece

					return move;
				}
			}
		}

		// no mill could be formed
		// place a piece in the best cell (by score)
		pair<int,int> best = bestCell(boardState);

		move.moveType = Placement;
		move.to = best;

		return move;
	}
	
	PlayerMove movePiece(const vector<vector<CellState>>& boardState)
	{
		PlayerMove move;

		vector<pair<int,int>> movablePieces; // create list of possible moves

		for(int i = 0; i < 3; ++i)
		{
			for(int j = 0; j < 8; ++j)
			{
				if(boardState[i][j].pieceType != PlayerPiece) // only player pieces can be moved
					continue;

				for(int k = 0; k < 4; ++k)
				{
					if(boardState[i][j].adj[k] != -1) // valid move
					{
						movablePieces.push_back({i, j}); // add piece to list
						break;
					}
				}
			}
		}

		for(int i = 0; i < movablePieces.size(); ++i)
		{
			int sq = movablePieces[i].first; // square of movable piece
			int iscn = movablePieces[i].second; // intersection of movable piece 

			CellState piece = boardState[sq][iscn]; // state of piece

			for(int k = 0; k < 4; ++k)
			{
				if(piece.adj[k] == 2) // move can form a mill, form one
				{
					move.moveType = Movement;

					move.from = movablePieces[i];
					move.to = convert(movablePieces[i], k);
					// convert from relative to absolute coordinates (GameTypes.h)

					move.canCapture = true;
					move.capture = findCapturable(boardState);

					return move;
				}
			}
		}

		// no mill could be formed
		// move the closest piece to the best cell (by score)

		pair<int,int> best = bestCell(boardState);
		int bestIndex = 0; // index of best move
		int counter = 0; // counter to determine if best move is reached
		int minDistance = 20; // set minimum higher than any possible value

		for(int i = 0; i < movablePieces.size(); ++i)
		{
			for(int k = 0; k < 4; ++k)
			{
				int sq = movablePieces[i].first;
				int iscn = movablePieces[i].second;

				if(boardState[sq][iscn].adj[k] == -1) // invalid move, skip
					continue;

				pair<int,int> adjacent = convert(movablePieces[i], k);
				// convert from absolute to relative coordinates

				int currentDistance = abs(adjacent.first - best.first) + abs(adjacent.second - best.second);
				// distance from resulting cell to best cell

				if(currentDistance < minDistance) // have we found a better move
				{
					minDistance = currentDistance; // set distance to new minimum
					bestIndex = counter; // set index to current index
				}

				++counter; // increase index
			}
		}

		for(int i = 0; i < movablePieces.size(); ++i)
		{
			for(int k = 0; k < 4; ++k)
			{
				int sq = movablePieces[i].first;
				int iscn = movablePieces[i].second;

				if(boardState[sq][iscn].adj[k] == -1)
					continue;

				pair<int,int> adjacent = convert(movablePieces[i], k);

				if(bestIndex == 0) // index has counted down to 0, best cell is reached
				{
					move.moveType = Movement;
					move.from = movablePieces[i];
					move.to = adjacent;
				}

				--bestIndex; // index counts down until it reaches zero, then best move is reached
			}
		}

		return move;
	}
};
