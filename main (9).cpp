// 2718951 - Bongani Mthembu
// 2741179 - Muhammad Shah Khan
#include <fstream>
#include <string>
#include <utility>
#include "Morabaraba.h"
#include "GameTypes.h"
#include "Alg_1.h"
#include "Alg_2.h"

// create a string (with or without comma) from a cell to be output to file
string cellString(pair<int,int> cell, bool comma)
{
	string cellString; // string to be returned
	
	if(cell == pair{-1, -1}) // placement move
		cellString = "00";
	
	else
	{
		cellString += "S";
		cellString += ('0' + cell.first); // square number
		
		if(comma) // comma should be included
			cellString += ",";
		
		cellString += "i";
		cellString += ('0' + cell.second); // intersection number
	}
	
	return cellString;
}

// create a full move from a string (including any capture)
string moveString(PlayerMove move, int player, Morabaraba& game)
{
	string moveString; // string to be returned
	
	moveString += "Alg_" + to_string(player) + " "; // player that played that move
	moveString += cellString(move.from, true) + "-" + cellString(move.to, true); // from and to cells
	
	if(move.canCapture) // capture takes place
	{
		moveString += " (mill = "; // output the mill that was formed
		array<pair<int,int>,3> millCells = game.getMill(move.to); // get the mill using the getMill function 
		
		for(int k = 0; k < 3; ++k) // output each of the cells in the mill
		{
			moveString += cellString(millCells[k], false);
			if(k < 2)
				moveString += ", ";
		
		}
		
		moveString += ") ";
		int enemy = player % 2 + 1;
		
		moveString += "Alg_" + to_string(enemy) + " losses cow ("; // enemy player loses a cow
		moveString += cellString(move.capture, true);
		moveString += ") | " + to_string(game.getCowsLeft(enemy)) + " left";
	}
	
	return moveString;

}

void simulateGame(int pieceCount) // simulates one instance of the game with starting piece count given
{
	Morabaraba game(pieceCount); // initialize game class
	Alg_1 player1; // random algorithm
	Alg_2 player2; // greedy algorithm

	ofstream output;
	output.open("morabarabaResults.txt", ios::app); // append output to file

	output << pieceCount << endl; // output piece count
	
	int player = 1;

	// do all placement moves
	for(int i = 0; i < 2 * pieceCount; ++i)
	{
		vector<vector<CellState>> boardState = game.getBoardState(player); // get board state

		PlayerMove move;

		if(player == 1) // get move from player 1
			move = player1.placePiece(boardState);

		else // get move from player 2
			move = player2.placePiece(boardState);

		game.placePiece(move, player); // process placement move with Morabaraba class

		output << moveString(move, player, game) << endl;  // output move to file

		player = player % 2 + 1; // switch players
	}

	player = 1; // set player to 1 for movement phase

	int gameStatus = game.getGameStatus(player); // get status of game to ensure player 1 can move

	while(gameStatus == -1) // the game has not ended yet
	{
		vector<vector<CellState>> boardState = game.getBoardState(player); // get state of board

		PlayerMove move;

		if(player == 1) // get move from player 1
			move = player1.movePiece(boardState);

		else // get move from player 2
			move = player2.movePiece(boardState);

		game.movePiece(move, player); // process movement move with Morabaraba class

		output << moveString(move, player, game) << endl; // output move to file

		player = player % 2 + 1; // switch player

		gameStatus = game.getGameStatus(player); // get status of game
	}

	output << endl;

	if(gameStatus == 0) // draw
		output << "its a draw" << endl;

	else if(gameStatus == 1) // player 1 wins
		output << "Alg_1 wins" << endl;

	else // player 2 wins
		output << "Alg_2 wins" << endl;

	output.close();
}

int main()
{
	vector<int> pieceCounts; // list of all piece counts in input file

	ifstream input;
	input.open("input.txt");

	int pieceCount;

	while(input >> pieceCount) // get all input piece counts
		pieceCounts.push_back(pieceCount);

	input.close();

	ofstream output;
	output.open("morabarabaResults.txt");
	output.close(); // open and close output file to ensure it is empty

	for(int i = 0; i < pieceCounts.size(); ++i)
	{
		simulateGame(pieceCounts[i]); // simulate the game for given piece count

		if(i < pieceCounts.size() - 1) // output a newline after every game except for last game
		{
			ofstream output;
			output.open("morabarabaResults.txt", ios::app);
			output << endl;
			output.close();
		}
	}
	return 0;
}
