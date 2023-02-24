//-------------------------------
// Timber game
//-------------------------------

// Including code from SFML library
#include <SFML/Graphics.hpp>
#include <sstream>
#include <SFML/Audio.hpp>

// making the code easier to type with "using namespace"
using namespace sf;

// Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

// Where is the player/branch? Left or right?
enum class side {LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];


int main(void){
	
	// Create a video mode object
	VideoMode vm(1366, 768);

	// Create and open a window for the game
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	// Create a texture to hold a graphic on the GPU
	Texture textureBackground1;
	Texture textureBackground2;

	// Load a graphic into the texture object
	textureBackground1.loadFromFile("graphics/background1.png");
	textureBackground2.loadFromFile("graphics/background2.png");

	// Create a sprite for background 1 -- I'm making two because the clouds
	Sprite spriteBackground1;
	Sprite spriteBackground2;

	// Prepare grass
	Texture textureGrass;
	textureGrass.loadFromFile("graphics/grass.png");
	Sprite spriteGrass;
	spriteGrass.setTexture(textureGrass);
	spriteGrass.setPosition(450, 640);

	// Attach the texture to the sprite
	spriteBackground1.setTexture(textureBackground1);
	spriteBackground2.setTexture(textureBackground2);

	// Set the sprite background to cover the screen
	spriteBackground1.setPosition(0,0);
	spriteBackground2.setPosition(0,0);

	// Adding the tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);	
	spriteTree.setPosition(586, 0);

	// Adding the bee sprite
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 600);
	
	bool beeActive = false;
	float beeSpeed = 0.0f;

	// Adding 3 cloud sprites
	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");

	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;

	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);

	spriteCloud2.setPosition(200, 80);
	spriteCloud3.setPosition(800, 55);

	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;

	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	// Variable to control the time itself
	Clock clock;

	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 150;
	float timeBarHeight = 40;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1366 / 2) - timeBarStartWidth / 2, 10);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	// Track whether the game is running
	bool paused = true;

	/*
	***************************************************
	HUD
	***************************************************
	*/

	int score = 0;

	Text messageText;
	Text scoreText;

	// We need to choose a font
	Font font;
	font.loadFromFile("fonts/Troubleside-lgjxX.ttf");

	// Set the font to our messages
	messageText.setFont(font);
	scoreText.setFont(font);

	// Assign the actual message
	messageText.setString("Press Enter to Start!");
	scoreText.setString("Score = 0");

	// Changing the size of the message
	messageText.setCharacterSize(60);
	scoreText.setCharacterSize(30);

	// Choose the color
	messageText.setFillColor(Color::Red);
	scoreText.setFillColor(Color::White);

	// Position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top + 
		textRect.height / 2.0f);

	messageText.setPosition(1366 / 2.0f, 768 / 2.0f);
	
	scoreText.setPosition(20,20);

	// Background rectangle for score message
	Texture textureScoreBack;
	textureScoreBack.loadFromFile("graphics/score_background.png");
	Sprite spriteScoreBack;
	spriteScoreBack.setTexture(textureScoreBack);
	spriteScoreBack.setPosition(10, 10);	

	// Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	// Set the texture for each branch sprite
	for(int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-1400, -1400);

		// Set the sprite's origin to dead centre
		// We can then spin it round without changing its position
		branches[i].setOrigin(150, 32.5);
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");

	// Texture unlockable player
	Texture textureGingy;
	textureGingy.loadFromFile("graphics/gingy.png");

	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(350, 450);

	// The player starts on the left
	side playerSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRip;
	textureRip.loadFromFile("graphics/rip.png");
	Sprite spriteRip;
	spriteRip.setTexture(textureRip);
	spriteRip.setPosition(230, 520);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(500, 600);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 500;
	const float AXE_POSITION_RIGHT = 860;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(586, 630);

	// Some other useful log variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// Control the player input
	bool acceptInput = false;

	// Prepare the sounds
	// The player chopping sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);	

	// The player has met his end under a branch
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	// Out of time sound
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	// Variables to controll sprite horizontal flip
	int A, B;

	
	//------------------  Main Game Loop  --------------------
 	while(window.isOpen())
	{	
		/*
		******************************************************
		* Handle the players input
		******************************************************
		*/

		Event event;
		
		while(window.pollEvent(event))
		{
			if(event.type == Event::KeyReleased && !paused)
			{
				// Listen for key presses again
				acceptInput = true;

				// Hide the axe
				spriteAxe.setPosition(2000, 
					spriteAxe.getPosition().y);
			}
		}
		
		
		if(Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Start the game
		if(Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;

			// Reset the time and score variables
			score = 0;
			timeRemaining = 6.0f;

			// Make all the branches disappear -
			// starting in second position

			for(int i = 1; i < NUM_BRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}

			// Make sure the gravestone is hidden
			spriteRip.setPosition(230, 2000);

			// Move the player into position
			spritePlayer.setPosition(350, 450);

			acceptInput = true;
		}

		// Wrap the player controls to make sure we
		// are accepting input

		if(acceptInput)
		{
			// First handle pressing the right cursor key
			if(Keyboard::isKeyPressed(Keyboard::Right))
			{
				B = 1;

				// Make sure the player is on the right
				playerSide = side::RIGHT;
				score++;

				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, 
					spriteAxe.getPosition().y);

				spritePlayer.setPosition(1020, 450);
				
				// Update the branches
				updateBranches(score);

				// Set the log flying to the left
				spriteLog.setPosition(586, 630);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}

			// Handle the left cursor key

			if(Keyboard::isKeyPressed(Keyboard::Left))
			{
				B = 0;

				// Make sure the player is on the left
				playerSide = side::LEFT;

				score++;

				// Add to the amount of time remaining
				timeRemaining += 2/score + .15;

				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);
				
				spritePlayer.setPosition(350, 450);
				
				// Update the branches
				updateBranches(score);

				// Set the log flying
				spriteLog.setPosition(586, 630);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				// Play chop sound
				chop.play();
			}			
		}

		// Reset player texture

		if(score >= 100)
		{
			spritePlayer.setTexture(textureGingy);	
		}

		// Handle horizontal flip for player and axe sprites

		if(A != B)
		{
			spritePlayer.scale(-1, 1);
			spriteAxe.scale(-1, 1);
			A = B;
		}


		/*
		******************************************************
		Update the scene
		******************************************************
		*/

		if(!paused)
		{

			// Measure time
			Time dt = clock.restart();	// dt means delta time

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();

			// Size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));
			
			if(timeRemaining <= 0.0f)
			{
				// Pause the game
				paused = true;

				// Change the message shown to the player
				messageText.setString("Out of time!");

				// Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + 
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);
				
				messageText.setPosition(1366 / 2.0f, 768 / 2.0f);

				// Play the out of time sound
				outOfTime.play();

				// Reset unlockable
				score = 0;
				spritePlayer.setTexture(texturePlayer);

			}

			// Set up the bee
			if(!beeActive)
			{
				// How fast is the bee
				srand((int)time(0));
				beeSpeed = rand() % 200 + 50;
				
				// How high is the bee
				srand((int)time(0) * 10);
				float height = rand() % 200 + 500;
				spriteBee.setPosition(1400, height);
				beeActive = true;
			}
			else // Move the bee, which is now active
			{
				spriteBee.setPosition(
					spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y
				);

				// has the bee reached the left-hand edge of the screen?
				if(spriteBee.getPosition().x < -100)
				{
					// Set it up ready to be a whole new bee next frame
					beeActive = false;
				}
			}

			// Manage the clouds
			// Cloud 1

			if(!cloud1Active)
			{	
				// How fast is the cloud1?
				srand((int)time(0) * 10);
				cloud1Speed = rand() % 200;

				// How high is cloud1? 
				srand((int)time(0) * 10);
				float height = rand() % 200;
				spriteCloud1.setPosition(-200, height);
				cloud1Active = true;
			}
			else
			{
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()),
					spriteCloud1.getPosition().y
				);

				// Has the cloud reached the right-hand edge of the screen?
				if(spriteCloud1.getPosition().x > 1400)
				{
					// Set it up ready to be a whole new cloud next frame
					cloud1Active = false;
				}
			}

			// Cloud 2
			if(!cloud2Active)
			{
				// How fast is cloud 2?
				srand((int)time(0) * 20);
				cloud2Speed = rand() % 200;

				// how high is cloud 2?
				srand((int)time(0) * 20);
				float height = rand() % 300 - 150;
				spriteCloud2.setPosition(-200, height);
				cloud2Active = true;
			}
			else{
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()),
					spriteCloud2.getPosition().y
				);

				// Has cloud 2 reached the right-hand edge of the screen?
				if(spriteCloud2.getPosition().x > 1400)
				{
					cloud2Active = false;
				}
			}

			// Cloud 3
			if(!cloud3Active)
			{
				// How fast is cloud 3
				srand((int)time(0) * 30);
				cloud3Speed = rand() % 200;

				// How high is cloud 3
				srand((int)time(0) * 30);
				float height = rand() % 450 - 150;
				spriteCloud3.setPosition(-200, height);
				cloud3Active = true;
			}
			else
			{
				spriteCloud3.setPosition(
					spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()),
					spriteCloud3.getPosition().y
				);

				// Has cloud 3 reached the right-hand edge of the screen?
				if(spriteCloud3.getPosition().x > 1400)
				{
					cloud3Active = false;
				}
			}

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// Update the branch sprites
			for(int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 400;

				if(branchPositions[i] == side::LEFT)
				{
					// Move the sprite to the left side
					branches[i].setPosition(440, height);

					// Flip the sprite round the otehr way
					branches[i].setRotation(180);
				}
				else if(branchPositions[i] == side::RIGHT)
				{
					// Move the sprite to the right side
					branches[i].setPosition(930, height);

					// Set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else
				{
					// Hide the branch
					branches[i].setPosition(2000, height);
				}
			}

			// Handle a flying log
			if(logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),

					spriteLog.getPosition().y + 
					(logSpeedY * dt.asSeconds())
				);

				// Has the log reached the right hand edge?
				if(spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().y > 1500)
				{
					// Set it up to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(586, 630);
				}
			}

			// Has the player been squished by a branch?
			if(branchPositions[2] == playerSide)
			{
				// death
				paused = true;
				acceptInput = false;

				// Draw the gravestone
				spriteRip.setPosition(350, 450);

				// Hide the player
				spritePlayer.setPosition(2000, 450);				

				// Change the text of the message
				messageText.setString("SQUISHED!!");

				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left + 
					textRect.width /2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1366/2.0f, 768/2.0f);

				// Play death sound
				death.play();

				// Fix the flip bug when player dies on right side

				if(playerSide == side::RIGHT)
				{
					B = 0;
					A = 0;
					spritePlayer.scale(-1, 1);						
					spriteAxe.scale(-1, 1);														
				}

				// Reset unlockable
				score = 0;
				spritePlayer.setTexture(texturePlayer);
				
			}	

		} // End if(!paused)


		/*
		******************************************************
		Draw the scene
		******************************************************
		*/

		// Clear everything from the last frame
		window.clear();

		// Draw our game scene here

		window.draw(spriteBackground1);
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);
		window.draw(spriteBackground2);	

		// Draw the branches
		for(int i = 0; i < NUM_BRANCHES; i++)
		{
			window.draw(branches[i]);
		}

		window.draw(spriteTree);

		// Draw the player
		window.draw(spritePlayer);

		// Draw the axe
		window.draw(spriteAxe);

		// Draw the flying log
		window.draw(spriteLog);

		// Draw the gravestone
		window.draw(spriteRip);

		// Draw grass
		window.draw(spriteGrass);

		// Draw the bee
		window.draw(spriteBee);

		// Draw score background box
		window.draw(spriteScoreBack);

		// Draw the score
		window.draw(scoreText);

		// Draw the time bar
		window.draw(timeBar);
		
		if(paused)
		{
		// Draw the message when paused
			window.draw(messageText);
		}

		// Show everything we just drew
		window.display();
	}

	return 0;
}

// Function definition
void updateBranches(int seed)
{
	// Move all the branches down one place
	for(int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPositions[j] = branchPositions[j - 1];
	}

	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = rand() % 5;

	switch(r)
	{
		case 0:
			branchPositions[0] = side::LEFT;
			break;
		case 1:
			branchPositions[0] = side::RIGHT;
			break;
		default:
			branchPositions[0] = side::NONE;
			break;
	}
}