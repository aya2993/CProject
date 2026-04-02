#define isDown(b) input->buttons[b].isDown
#define pressed(b) (input->buttons[b].isDown && input->buttons[b].changed)
#define released(b) (!input->buttons[b].isDown && input->buttons[b].changed)

#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")


float playerHalfSizeX = 2.5, playerHalfSizeY = 12;
float arenaHalfSizeX = 85, arenaHalfSizeY = 45;

//player 1 variables
float player1PY, player1DpY;
float player1PX = 80, player1DpX;

//player 2 variables
float player2PY, player2DpY;
float player2PX = -80 , player2DpX;

//Ball movement variables
float ballPX, ballPY, ballDpX = 100, ballDpY, ballHalfSize = 1.f;

//score variables
int player1Score, player2Score;

//Menu variables
enum GameMode {
	GM_Menu,
	GM_Menu2,
	GM_Game,
};

GameMode currentGameMode;
int hotButton;
bool enemyIsAI;
bool hardMode;


//sound functions
static void playMusic(const wchar_t* filepath) {
	// looped async playback
	PlaySoundW(filepath, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

/*static void stopMusic() {
	// stop any PlaySound playback
	PlaySoundW(NULL, NULL, 0);
}*/

static void playSfx(const wchar_t* filepath) {
	// simple async sound effect (note: PlaySound will stop previous non-looped sound if called)
	PlaySoundW(filepath, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

//players system

void movePlayer(float& pX, float& pY, float& dpX, float& dpY, float ddpX, float ddpY, float dt) {
	pY = pY + dpY * dt + ddpY * dt * dt * 0.5f;
	dpY = dpY + ddpY * dt;

	pX = pX + dpX * dt + ddpX * dt * dt * 0.5f;
	dpX = dpX + ddpX * dt;
}

void handlePlayerCollision(float& pX, float& pY, float& dpX, float& dpY) {

	if (pY + playerHalfSizeY > arenaHalfSizeY) {
		pY = arenaHalfSizeY - playerHalfSizeY;
		dpY *= -1;
	}
	else if (pY - playerHalfSizeY < -arenaHalfSizeY) {
		pY = -arenaHalfSizeY + playerHalfSizeY;
		dpY *= -1;
	}

	if (pX + playerHalfSizeX > arenaHalfSizeX) {
		pX = arenaHalfSizeX - playerHalfSizeX;
		dpX *= -1;
	}
	else if (pX - playerHalfSizeX < -arenaHalfSizeX) {
		pX = -arenaHalfSizeX + playerHalfSizeX;
		dpX *= -1;
	}
}

internal void simulateGame(Input* input, float dt) {
	clearScreen(COLOR1);
	drawRect(0, 0, arenaHalfSizeX, arenaHalfSizeY, COLOR2);
	if (currentGameMode == GM_Game) {
		float bounceMultiplier = hardMode ? 1.0f : 0.5f;
		float aiSpeed = hardMode ? 200.f : 100.f;

		//player 1 movement 

		float p1ddpX = 0, p1ddpY = 0;

		if (isDown(Button_Up))    p1ddpY += 1000;
		if (isDown(Button_Down))  p1ddpY -= 1000;
		if (isDown(Button_Left))  p1ddpX -= 1000;
		if (isDown(Button_Right)) p1ddpX += 1000;

		p1ddpY -= player1DpY * 15.f;
		p1ddpX -= player1DpX * 15.f;

		movePlayer(player1PX, player1PY, player1DpX, player1DpY, p1ddpX, p1ddpY, dt);
		handlePlayerCollision(player1PX, player1PY, player1DpX, player1DpY);

		//player 2 movement
		float p2ddpX = 0, p2ddpY = 0;

		if (!enemyIsAI) {
			if (isDown(Button_W)) p2ddpY += 1000;
			if (isDown(Button_S)) p2ddpY -= 1000;
			if (isDown(Button_A)) p2ddpX -= 1000;
			if (isDown(Button_D)) p2ddpX += 1000;
		}
		else {
			p2ddpY += (ballPY - player2PY) * aiSpeed;
		}

		p2ddpY -= player2DpY * 15.f;
		p2ddpX -= player2DpX * 15.f;

		movePlayer(player2PX, player2PY, player2DpX, player2DpY, p2ddpX, p2ddpY, dt);
		handlePlayerCollision(player2PX, player2PY, player2DpX, player2DpY);


		//ball movement
		drawRect(ballPX, ballPY, 2, 2, COLOR5);
		ballPX += ballDpX * dt;
		ballPY += ballDpY * dt;

		if (ballPX + ballHalfSize > player1PX - playerHalfSizeX &&
			ballPX - ballHalfSize < player1PX + playerHalfSizeX &&
			ballPY + ballHalfSize > player1PY - playerHalfSizeY &&
			ballPY + ballHalfSize < player1PY + playerHalfSizeY) {

			ballPX = player1PX - playerHalfSizeX - ballHalfSize;
			ballDpX *= -1;
			ballDpY += (ballDpY - player1DpY) * 1.f;
		}
		else if (ballPX + ballHalfSize > player2PX - playerHalfSizeX &&
			ballPX - ballHalfSize < player2PX + playerHalfSizeX &&
			ballPY + ballHalfSize > player2PY - playerHalfSizeY &&
			ballPY + ballHalfSize < player2PY + playerHalfSizeY) {

			ballPX = player2PX + playerHalfSizeX + ballHalfSize;
			ballDpX *= -1;
			ballDpY += (ballDpY - player1DpY) * 1.f;
		}

		//collision top and bottom
		if (ballPY + ballHalfSize > arenaHalfSizeY) {
			ballPY = arenaHalfSizeY - ballHalfSize;
			ballDpY *= -bounceMultiplier;
		}
		else if (ballPY - ballHalfSize < -arenaHalfSizeY) {
			ballPY = -arenaHalfSizeY + ballHalfSize;
			ballDpY *= -bounceMultiplier;
		}

		//collision left and right
		if (ballPX + ballHalfSize > arenaHalfSizeX) {
			ballDpX *= -1;
			ballDpY = 0;
			ballPX = 0;
			ballPY = 0;
			player1Score++;			
		}
		else if (ballPX - ballHalfSize < -arenaHalfSizeX) {
			ballDpX *= -1;
			ballDpY = 0;
			ballPX = 0;
			ballPY = 0;
			player2Score++;
		}

		//draw score
		float atX = -80;
		for (int i = 0; i < player1Score; i++) {
			drawRect(atX, 47.f, 1, 1, 0x450C1B);
			atX += 2.5;
			
		}

		atX = 80;
		for (int i = 0; i < player2Score; i++) {
			drawRect(atX, 47.f, 1, 1, 0x450C1B);
			atX -= 2.5;
		}

		//draw players
		drawRect(player1PX, player1PY, playerHalfSizeX, playerHalfSizeY, COLOR3);
		drawRect(player2PX, player2PY, playerHalfSizeX, playerHalfSizeY,  COLOR3);
	}

	else if (currentGameMode == GM_Menu) {
		//Menu

		if (pressed(Button_Left) || pressed(Button_Right)) {
			hotButton = !hotButton;
			playSfx(L"button.wav");

		}



		if (pressed(Button_Enter)) {
			enemyIsAI = (hotButton == 0); // 0 = Singleplayer → AI
			currentGameMode = GM_Menu2;

			playSfx(L"select.wav");
		}
		if (hotButton == 0) {
			drawText("SINGLE PLAYER", -80, -10, 1, COLOR1);
			drawText("MULTIPLAYER", 20, -10, 1, COLOR5);
		}
		else {
			drawText("SINGLE PLAYER", -80, -10, 1, COLOR5);
			drawText("MULTIPLAYER", 20, -10, 1, COLOR1);
		}

		drawText("BEST GAME EVER", -60, 40, 1.5, COLOR5);

	}
	else if (currentGameMode == GM_Menu2) {

		if (pressed(Button_Left) || pressed(Button_Right)) {
			hotButton = !hotButton;
			playSfx(L"button.wav");
		}

		if (pressed(Button_Enter)) {
			hardMode = hotButton;
			currentGameMode = GM_Game;
			playSfx(L"select.wav");
			playMusic(L"test.wav");
		}

		drawText("EASY", -50, -10, 1, hotButton == 0 ? COLOR1 : COLOR5);
		drawText("HARD", 20, -10, 1, hotButton == 1 ? COLOR1 : COLOR5);
		drawText("SELECT DIFFICULTY", -80, 40, 1.5, COLOR5);
	}
}