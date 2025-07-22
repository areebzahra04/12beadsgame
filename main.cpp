#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace sf;

// window size changed
const int sizeboard = 5;
const int sizecell = 100;    // Size of each cell on the board
const int radiusbead = 20;   // Radius of the bead in each cell
const int sizewindow = 1000; // Increase window size by 100

char board[sizeboard][sizeboard];
bool clicked;
int lastRow, lastCol;
int currentPlayer;

void saveGame(const string& filename)
{
	ofstream outFile(filename);

	if (outFile.is_open())
	{

		for (int i = 0; i < sizeboard; ++i)
		{
			for (int j = 0; j < sizeboard; ++j)
			{
				outFile << board[i][j];
			}
			outFile << endl;
		}

		outFile << currentPlayer << " " << clicked << " " << lastRow << " " << lastCol << endl;
		outFile.close();
		
	}
}

void loadGame(const string& filename)
{
	ifstream inFile("saved_game.txt");
	if (inFile.is_open())
	{
		for (int i = 0; i < sizeboard; ++i)
		{
			for (int j = 0; j < sizeboard; ++j)
			{
				char beadColor;
				inFile >> beadColor; // Read without skipping whitespaces
				/*if (beadColor == ' ') {
					board[i][j] = ' ';
				}
				else {*/
				board[i][j] = beadColor;
				cout << board[i][j];
			}
		}
		int valueofplayer;
		inFile >> valueofplayer >> clicked >> lastRow >> lastCol;
		currentPlayer = valueofplayer;
		inFile.close();
	}
}
void boardinitialize(bool loadFromSave = false)
{
	if (loadFromSave)
	{
		loadGame("saved_game.txt");
	}
	else
	{
		for (int i = 0; i < sizeboard; ++i)
		{
			for (int j = 0; j < sizeboard; ++j)
			{
				board[i][j] = 'E';
			}
		}

		// Set the first 12 circles to red and the last 12 to green
		for (int i = 0; i < 12; ++i)
		{
			board[i / sizeboard][i % sizeboard] = 'R';
			board[(sizeboard - 1) - i / sizeboard][(sizeboard - 1) - i % sizeboard] = 'G';
		}
	}
}

bool gamefinish()
{
	for (int i = 0; i < sizeboard; ++i)
	{
		for (int j = 0; j < sizeboard; ++j)
		{
			if ((currentPlayer == 0 && board[i][j] == 'R') ||
				(currentPlayer == 1 && board[i][j] == 'G'))
			{
				return false;
			}
		}
	}
	return true;
}

void changePlayer()
{
	currentPlayer = (currentPlayer == 0) ? 1 : 0;
}

bool getHasClicked()
{
	return clicked;
}

void setHasClicked(bool value)
{
	clicked = value;
}

int getCurrentPlayer()
{
	return currentPlayer;
}

char getBeadColor(int row, int col)
{
	return board[row][col];
}

bool validnext(int row, int col)
{
	if (row - 1 >= 0 && board[row - 1][col] == board[lastRow][lastCol])
	{
		return true;
	}
	if (row + 1 < sizeboard && board[row + 1][col] == board[lastRow][lastCol])
	{
		return true;
	}
	if (col - 1 >= 0 && board[row][col - 1] == board[lastRow][lastCol])
	{
		return true;
	}
	if (col + 1 < sizeboard && board[row][col + 1] == board[lastRow][lastCol])
	{
		return true;
	}
	return false;
}

bool correctmove(int row, int col)
{
	if (clicked)
	{

		if (board[row][col] == 'E')
		{
			int rowDiff = abs(row - lastRow);
			int colDiff = abs(col - lastCol);
		
			// checking if moving on a valid line
			if (rowDiff == 0 || colDiff == 0 || (rowDiff == colDiff && (row + col) % 2 == 0))
			{
				if (abs(row - lastRow) <= 1 && abs(col - lastCol) <= 1)
				{
					return true;
				}
				int rowDiff = lastRow - row;
				int colDiff = lastCol - col;

				if (abs(rowDiff) > 2 || abs(colDiff) > 2)
				{
					return false;
				}

				int opponentRow = lastRow - (rowDiff == 0 ? 0 : rowDiff / abs(rowDiff));
				int opponentCol = lastCol - (colDiff == 0 ? 0 : colDiff / abs(colDiff));

				if ((currentPlayer == 0 && board[opponentRow][opponentCol] == 'G') ||
					(currentPlayer == 1 && board[opponentRow][opponentCol] == 'R'))
				{
					return true;
				}
			}
		}
	}
	else
	{
		if ((currentPlayer == 0 && board[row][col] == 'R') ||
			(currentPlayer == 1 && board[row][col] == 'G'))
		{
			return true;
		}
	}

	return false;
}

bool eliminate(int row, int col)
{
	if (lastRow == -1 || lastCol == -1)
	{
		return false;
	}

	int rowDiff = row - lastRow;
	int colDiff = col - lastCol;

	int stepRow = (rowDiff == 0) ? 0 : rowDiff / abs(rowDiff);
	int stepCol = (colDiff == 0) ? 0 : colDiff / abs(colDiff);

	int currentRow = lastRow + stepRow;
	int currentCol = lastCol + stepCol;

	while (currentRow != row || currentCol != col)
	{
		if ((currentPlayer == 0 && board[currentRow][currentCol] == 'G') ||
			(currentPlayer == 1 && board[currentRow][currentCol] == 'R'))
		{
			return true;
		}

		currentRow += stepRow;
		currentCol += stepCol;
	}

	return false;
}

void eliminateOpponentBeads(int startRow, int startCol, int endRow, int endCol)
{
	int rowDiff = endRow - startRow;
	int colDiff = endCol - startCol;

	int stepRow = (rowDiff == 0) ? 0 : rowDiff / abs(rowDiff);
	int stepCol = (colDiff == 0) ? 0 : colDiff / abs(colDiff);

	int currentRow = startRow + stepRow;
	int currentCol = startCol + stepCol;

	while (currentRow != endRow || currentCol != endCol)
	{
		try
		{
			if ((currentPlayer == 0 && board[currentRow][currentCol] == 'G') ||
				(currentPlayer == 1 && board[currentRow][currentCol] == 'R'))
			{
				board[currentRow][currentCol] = 'E'; // making changes
			}

			currentRow += stepRow;
			currentCol += stepCol;
		}
		catch (const exception&)
		{
			return;
		}
	}
}

bool moves(int row, int col)
{
	if (!correctmove(row, col))
	{
		clicked = false;
		lastRow = -1;
		lastCol = -1;
		return false;
	}

	if (clicked)
	{
		if (board[row][col] == 'E' || // making changes
			((currentPlayer == 0 && board[row][col] == 'G') ||
				(currentPlayer == 1 && board[row][col] == 'R')) ||
			validnext(row, col))
		{
			board[row][col] = (currentPlayer == 0) ? 'R' : 'G';

			if (eliminate(row, col))
			{
				eliminateOpponentBeads(lastRow, lastCol, row, col);
			}

			if (gamefinish())
			{
				cout << "Player " << (currentPlayer == 0 ? "Two" : "One") << " wins!" << endl;
				saveGame("saved_game.txt");
				return true;
			}

			changePlayer();
			clicked = false;
			board[lastRow][lastCol] = 'E';
			lastRow = -1;
			lastCol = -1;

			saveGame("saved_game.txt");
		}
	}
	else
	{
		clicked = true;
		lastRow = row;
		lastCol = col;
	}

	return true;
}
void drawBoard(RenderWindow& window)
{

	Font font;
	if (!font.loadFromFile("arial.ttf"))
	{
		cout << "Error loading font file 'arial.ttf'" << endl;
		return;
	}

	CircleShape playerOneBead(radiusbead);
	playerOneBead.setFillColor(Color::Red);
	playerOneBead.setOutlineThickness(2);
	playerOneBead.setOutlineColor(Color::Black);

	CircleShape playerTwoBead(radiusbead);
	playerTwoBead.setFillColor(Color::Green);
	playerTwoBead.setOutlineThickness(2);
	playerTwoBead.setOutlineColor(Color::Black);

	CircleShape currentMoveBead(radiusbead);
	currentMoveBead.setOutlineThickness(2);
	currentMoveBead.setOutlineColor(Color::Black);

	// play Player 1 and Player 2 circles at the top
	Text playerOneText("Player 1:", font, 50);

	int playerOne_Text_X = ((sizewindow - sizeboard * sizecell) / 4 - radiusbead) - 50;
	int playerOne_Text_Y = sizecell / 2 - radiusbead;

	playerOneText.setFillColor(Color::Black);

	playerOneText.setPosition(playerOne_Text_X, playerOne_Text_Y);
	window.draw(playerOneText);

	int playerOne_Circle_X = ((sizewindow - sizeboard * sizecell) / 4 - radiusbead) + 200;
	int playerOne_Circle_Y = (sizecell / 2 - radiusbead) + 10;

	playerOneBead.setPosition(playerOne_Circle_X, playerOne_Circle_Y);
	window.draw(playerOneBead);

	Text playerTwoText("Player 2:", font, 50);
	playerTwoText.setFillColor(Color::Black);

	int playerTwo_Text_X = ((sizewindow - sizeboard * sizecell) / 4 - radiusbead) - 50;
	int playerTwo_Text_Y = 4 * (sizecell / 2 - radiusbead);

	playerTwoText.setPosition(playerTwo_Text_X, playerTwo_Text_Y);
	window.draw(playerTwoText);

	int playerTwo_Circle_X = ((sizewindow - sizeboard * sizecell) / 4 - radiusbead) + 200;
	int playerTwo_Circle_Y = (4 * (sizecell / 2 - radiusbead)) + 10;

	playerTwoBead.setPosition(playerTwo_Circle_X, playerTwo_Circle_Y);

	/*playerTwoBead.setPosition((3 * WINDOW_SIZE + BOARD_SIZE * CELL_SIZE) / 4 + 100,
		CELL_SIZE / 2 - BEAD_RADIUS);*/

	window.draw(playerTwoBead);

	Text CurrentPlayer("Current Move:", font, 50);
	CurrentPlayer.setFillColor(Color::Black);

	int Current_Text_X = sizewindow / 2 - radiusbead - 50;
	int Current_Text_Y = sizecell / 2 - radiusbead;

	CurrentPlayer.setPosition(Current_Text_X, Current_Text_Y);
	window.draw(CurrentPlayer);

	// Display the current move circle
	int Current_Circle_X = sizewindow / 2 - radiusbead + 300;
	int Current_Circle_Y = (sizecell / 2 - radiusbead) + 10;

	currentMoveBead.setFillColor((getCurrentPlayer() == 0) ? Color::Red : Color::Green);
	currentMoveBead.setPosition(Current_Circle_X, Current_Circle_Y);
	window.draw(currentMoveBead);

	VertexArray lines(Lines);

	for (int i = 0; i < sizeboard; ++i)
	{
		for (int j = 0; j < sizeboard; ++j)
		{
			char beadColor = getBeadColor(i, j);

			if (beadColor == 'R')
			{
				playerOneBead.setPosition(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead,
					i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead);
				window.draw(playerOneBead);
			}
			else if (beadColor == 'G')
			{
				playerTwoBead.setPosition(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead,
					i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead);
				window.draw(playerTwoBead);
			}

			// Draw the clicked circle in blue
			if (getHasClicked() && getCurrentPlayer() == 0 && i == lastRow && j == lastCol)
			{
				currentMoveBead.setFillColor(Color::Blue);
				currentMoveBead.setPosition(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead,
					i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead);
				window.draw(currentMoveBead);
			}
			else if (getHasClicked() && getCurrentPlayer() == 1 && i == lastRow && j == lastCol)
			{
				currentMoveBead.setFillColor(Color::Blue);
				currentMoveBead.setPosition(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead,
					i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2 - radiusbead);
				window.draw(currentMoveBead);
			}

			// Draw lines between adjacent beads or empty spaces
			if (j < sizeboard - 1)
			{
				lines.append(Vertex(Vector2f((j + 1) * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
					i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
					Color::Black));
				lines.append(Vertex(Vector2f(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
					i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
					Color::Black));
			}

			if (i < sizeboard - 1)
			{
				lines.append(Vertex(Vector2f(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
					(i + 1) * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
					Color::Black));
				lines.append(Vertex(Vector2f(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
					i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
					Color::Black));
			}
			// Draw diagonal lines
			if (i < sizeboard - 1 && j < sizeboard - 1)
			{
				if ((i + j) % 2 == 0)
				{
					// Draw from top-left to bottom-right
					lines.append(Vertex(Vector2f(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
						i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
						Color::Black));
					lines.append(Vertex(Vector2f((j + 1) * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
						(i + 1) * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
						Color::Black));
				}
				else
				{
					// Draw from top-right to bottom-left
					lines.append(Vertex(Vector2f((j + 1) * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
						i * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
						Color::Black));
					lines.append(Vertex(Vector2f(j * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2,
						(i + 1) * sizecell + (sizewindow - sizeboard * sizecell) / 2 + sizecell / 2),
						Color::Black));
				}
			}

		}
	}

	window.draw(lines);

	if (gamefinish())
	{

		Text winnerText("Winner:", font, 100);
		winnerText.setOrigin(winnerText.getLocalBounds().width / 2, winnerText.getLocalBounds().height / 2);
		winnerText.setPosition(sizewindow / 4, sizewindow - radiusbead - 100);
		winnerText.setFillColor(Color::Black);

		CircleShape winnerCircle(radiusbead + 50);
		winnerCircle.setOutlineThickness(2);
		winnerCircle.setOutlineColor(Color::Black);

		if (getCurrentPlayer() == 0)
		{
			winnerCircle.setFillColor(Color::Green);
		}
		else
		{
			winnerCircle.setFillColor(Color::Red);
		}

		winnerCircle.setPosition(sizewindow / 2, sizewindow - 170);

		window.draw(winnerText);
		window.draw(winnerCircle);
		
		return; 
	}
}
int showMenu(RenderWindow& window)
{
    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cout << "Failed to load font" << endl;
        return 2;
    }

    const int numButtons = 3;
    string buttonTexts[numButtons] = { "Start New Game", "Load Saved Game", "Exit" };
    RectangleShape buttons[numButtons];
    Text buttonLabels[numButtons];

    Vector2f buttonSize(350, 70);
    float startY = 300;

    for (int i = 0; i < numButtons; ++i)
    {
        buttons[i].setSize(buttonSize);
        buttons[i].setPosition((sizewindow - buttonSize.x) / 2, startY + i * 100);
        buttons[i].setFillColor(Color(40, 40, 40));
        buttons[i].setOutlineColor(Color::White);
        buttons[i].setOutlineThickness(2);

        buttonLabels[i].setFont(font);
        buttonLabels[i].setString(buttonTexts[i]);
        buttonLabels[i].setCharacterSize(28);
        buttonLabels[i].setFillColor(Color::White);
        FloatRect textRect = buttonLabels[i].getLocalBounds();
        buttonLabels[i].setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
        buttonLabels[i].setPosition(buttons[i].getPosition().x + buttonSize.x / 2,
                                    buttons[i].getPosition().y + buttonSize.y / 2);
    }

    Text title("12 BEADS GAME", font, 72);
    title.setFillColor(Color::Red);
    title.setOutlineColor(Color::Green);
    title.setOutlineThickness(3);
    FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2, titleBounds.top + titleBounds.height / 2);
    title.setPosition(sizewindow / 2, 150);

    while (window.isOpen())
    {
        Vector2i mousePos = Mouse::getPosition(window);
        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                for (int i = 0; i < numButtons; ++i)
                {
                    if (buttons[i].getGlobalBounds().contains(mousePos.x, mousePos.y))
                        return i;
                }
            }
        }

       
        for (int i = 0; i < numButtons; ++i)
        {
            if (buttons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                buttons[i].setFillColor(Color(100, 100, 255));
            } else {
                buttons[i].setFillColor(Color(40, 40, 40));
            }
        }

        window.clear(Color(220, 220, 220));

        window.draw(title);
        for (int i = 0; i < numButtons; ++i) {
            window.draw(buttons[i]);
            window.draw(buttonLabels[i]);
        }
        window.display();
    }

    return 2;
}


int main()
{
    RenderWindow window(VideoMode(sizewindow, sizewindow), "12 Beads Game");

    int menuChoice = showMenu(window);

    if (menuChoice == 2) return 0;

    currentPlayer = rand() % 2;
    clicked = false;

    bool loadFromSave = (menuChoice == 1);
    boardinitialize(loadFromSave);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                int col = (event.mouseButton.x - (sizewindow - sizeboard * sizecell) / 2) / sizecell;
                int row = (event.mouseButton.y - (sizewindow - sizeboard * sizecell) / 2) / sizecell;
                moves(row, col);
            }
        }

        window.clear(Color::White);
        drawBoard(window);
        window.display();
    }

    return 0;
}
