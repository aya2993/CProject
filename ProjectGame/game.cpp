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
	GM_Game,
};

GameMode currentGameMode;
int hotButton;
bool enemyIsAI;

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

internal void simulateGame(Input* input, float dt) {
	clearScreen(COLOR1);
	drawRect(0, 0, arenaHalfSizeX, arenaHalfSizeY, COLOR2);

	if (currentGameMode == GM_Game) {
		//player 1 movement 
		float player1DdpY = 0.f;
		float player1DdpX = 0.f;


		if (isDown(Button_Up)) player1DdpY += 1000;
		if (isDown(Button_Down)) player1DdpY -= 1000;
		if (isDown(Button_Left)) player1DdpX -= 1000;
		if (isDown(Button_Right)) player1DdpX += 1000;

		//
			//wiederstand
		player1DdpY -= player1DpY * 15.f;
		player1DdpX -= player1DpX * 15.f;


		player1PY = player1PY + player1DpY * dt + player1DdpY * dt * dt * .5f;
		player1DpY = player1DpY + player1DdpY * dt;

		player1PX = player1PX + player1DpX * dt + player1DdpX * dt * dt * .5f;
		player1DpX = player1DpX + player1DdpX * dt;

		// collision for player 1
		if (player1PY + playerHalfSizeY > arenaHalfSizeY) {
			player1PY = arenaHalfSizeY - playerHalfSizeY;
			player1DpY *= -1;
		}
		else if (player1PY - playerHalfSizeY < -arenaHalfSizeY) {
			player1PY = -arenaHalfSizeY + playerHalfSizeY;
			player1DpY *= -1;
		}

		if (player1PX + playerHalfSizeX > arenaHalfSizeX) {
			player1PX = arenaHalfSizeX - playerHalfSizeX;
			player1DpX *= -1;
		}
		else if (player1PX - playerHalfSizeX < -arenaHalfSizeX) {
			player1PX = -arenaHalfSizeX + playerHalfSizeX;
			player1DpX *= -1;
		}
		//


			//player 2 movement

		float player2DdpY = 0.f;
		float player2DdpX = 0.f;

		if (enemyIsAI == 0) {
			if (isDown(Button_W)) player2DdpY += 1000;
			if (isDown(Button_S)) player2DdpY -= 1000;
			if (isDown(Button_A)) player2DdpX -= 1000;
			if (isDown(Button_D)) player2DdpX += 1000;
		}
		else {
			//if (ballPY > player2PY) player2DdpY += 1000;
			//else if (ballPY < player2PY) player2DdpY -= 1000;
			player2DdpY += (ballPY - player2PY) * 200.f;
			if (player1DdpY > 1300) player1DdpY = 1300;
			else if (player1DdpY < -1300) player1DdpY = -1300;
		}
		player2DdpY -= player2DpY * 15.f;
		player2DdpX -= player2DpX * 15.f;

		player2PY = player2PY + player2DpY * dt + player2DdpY * dt * dt * .5f;
		player2DpY = player2DpY + player2DdpY * dt;

		player2PX = player2PX + player2DpX * dt + player2DdpX * dt * dt * .5f;
		player2DpX = player2DpX + player2DdpX * dt;

		//collision for player 2 (fixed)
		if (player2PY + playerHalfSizeY > arenaHalfSizeY) {
			player2PY = arenaHalfSizeY - playerHalfSizeY;
			player2DpY *= -1;
		}
		else if (player2PY - playerHalfSizeY < -arenaHalfSizeY) {
			player2PY = -arenaHalfSizeY + playerHalfSizeY;
			player2DpY *= -1;
		}

		if (player2PX + playerHalfSizeX > arenaHalfSizeX) {
			player2PX = arenaHalfSizeX - playerHalfSizeX;
			player2DpX *= -1;
		}
		else if (player2PX - playerHalfSizeX < -arenaHalfSizeX) {
			player2PX = -arenaHalfSizeX + playerHalfSizeX;
			player2DpX *= -1;
		}



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
			ballDpY *= -.5;
		}
		else if (ballPY - ballHalfSize < -arenaHalfSizeY) {
			ballPY = -arenaHalfSizeY + ballHalfSize;
			ballDpY *= -.5;
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
 else {
	 //Menu
	 
		if (pressed(Button_Left) || pressed(Button_Right)) {
			
			hotButton = !hotButton;
			
		}

		

		if (pressed (Button_Enter)){
			playMusic(L"Theme.wav");
			currentGameMode = GM_Game;
			enemyIsAI = hotButton ? 0 : 1;
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
}