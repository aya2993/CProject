#define isDown(b) input->buttons[b].isDown
#define pressed(b) (input->buttons[b].isDown && input->buttons[b].changed)
#define released(b) (!input->buttons[b].isDown && input->buttons[b].changed)


float playerHalfSizeX = 2.5, playerHalfSizeY = 12;
float arenaHalfSizeX = 85, arenaHalfSizeY = 45;

//player 1 variables
float player1PY, player1DpY;
float player1PX = 80, player1DpX;

//player 2 variables
float player2PY, player2DpY;
float player2PX, player2DpX;

//Ball movement variables
float ballPX, ballPY, ballDpX = 100, ballDpY, ballHalfSize = 1.f;


/*internal void simulatePlayer(float* pX, float* pY, float* dp, float ddpX, float ddpY, float dt) {
	ddpY -= player1DpY * 15.f;
	ddpX -= player1DpX * 15.f;


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
}*/

internal void simulateGame(Input* input, float dt) {
	clearScreen(0xE491A6);
	drawRect(0, 0, arenaHalfSizeX, arenaHalfSizeY, 0xA2CB8B);


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
	//float player2DdpX = 0.f;

	if (isDown(Button_W)) player2DdpY += 1000;
	if (isDown(Button_S)) player2DdpY -= 1000;

	player2DdpY -= player2DpY * 15.f;
	//player2DdpX -= player2DpX * 15.f;

	player2PY = player2PY + player2DpY * dt + player2DdpY * dt * dt * .5f;
	player2DpY = player2DpY + player2DdpY * dt;

	//collision for player 2 (fixed)
	if (player2PY + playerHalfSizeY > arenaHalfSizeY) {
		player2PY = arenaHalfSizeY - playerHalfSizeY;
		player2DpY *= -1;
	}
	else if (player2PY - playerHalfSizeY < -arenaHalfSizeY) {
		player2PY = -arenaHalfSizeY + playerHalfSizeY;
		player2DpY *= -1;
	}

	//ball movement
	drawRect(ballPX, ballPY, 1, 1, 0x00ff00);
	ballPX += ballDpX * dt;
	ballPY += ballDpY * dt;

	if (ballPX + ballHalfSize > player1PX - playerHalfSizeX &&
		ballPX - ballHalfSize < player1PX + playerHalfSizeX &&
		ballPY + ballHalfSize > player1PY - playerHalfSizeY &&
		ballPY + ballHalfSize < player1PY + playerHalfSizeY) {

		ballPX = player1PX - playerHalfSizeX - ballHalfSize;
		ballDpX *= -1;
		ballDpY += (ballDpY - player1DpY) * 1.f ;
	}
	else if (ballPX + ballHalfSize > -80 - playerHalfSizeX &&
			ballPX - ballHalfSize < -80 + playerHalfSizeX &&
			ballPY + ballHalfSize > player2PY - playerHalfSizeY &&
			ballPY + ballHalfSize < player2PY + playerHalfSizeY) {

		ballPX = -80 + playerHalfSizeX + ballHalfSize;
		ballDpX *= -1;
		ballDpY += (ballDpY - player1DpY) * 1.f;
	}

	//collision top and bottom
	if (ballPY + ballHalfSize > arenaHalfSizeY) {
		ballPY = arenaHalfSizeY - ballHalfSize;
		ballDpY *= -1;
	}
	else if (ballPY - ballHalfSize < -arenaHalfSizeY) {
		ballPY = -arenaHalfSizeY + ballHalfSize;
		ballDpY *= -1;
	}

	//collision left and right
	if (ballPX + ballHalfSize > arenaHalfSizeX) {
		ballDpX *= -1;
		ballDpY = 0;
		ballPX = 0;
		ballPY = 0;
	}
	else if (ballPX - ballHalfSize < -arenaHalfSizeX) {
		ballDpX *= -1;
		ballDpY = 0;
		ballPX = 0;
		ballPY = 0;
	}

	//draw players
	drawRect(player1PX, player1PY, playerHalfSizeX, playerHalfSizeY, 0x450C1B);
	drawRect(-80, player2PY, playerHalfSizeX, playerHalfSizeY, 0x450C1B);
	
}