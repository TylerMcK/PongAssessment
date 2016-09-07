#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define PLAYER_WIDTH (1)
#define ZOMBIE_WIDTH (5)
#define ZOMBIE_HEIGHT (5)

//FINISH
int PaddleHeight() {
	int height = 7;
	if (screen_height() < 21) {
		height = (screen_height() - 5 - 1) / 2;
	}
	return height;
}	

#define PLAYER_HEIGHT (PaddleHeight())
// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */

// Global Variables
int lives  = 10;
int score = 0;
int level = 1;
int startingTime;
int currentTime;
int realTime = 0;
int pauseTime = 0;


bool initial = true;
bool dir_changed;

int sw;
int sh; 
int pw;
int ph;

//Levl 3
int x_star;
int y_star;
int temp3 = 1;

// Location Variables
int bx;
int by;
int py;
int oy;
	
static char * player_char =
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"
/**/	"|"

//static char * msg_image =
/**/	"Goodbye and thank-you for playing ZombieDash Jr."
/**/	"            Press any key to exit...            ";

// Declare sprite_ids
sprite_id player;
sprite_id opponent;
sprite_id ball;

// Environment Setup
void EnvironmentSetup(void) {
	clear_screen();
	sw = screen_width();
	sh = screen_height();
	// --- Game Border --- //
	// Upper Top Border
	draw_line(0, 0, sw - 1, 0, '*');
	// Inner Top Border
	draw_line(0, 6, sw - 1, 6, '*');
	//Bottom Border
	draw_line(0, sh-1, sw - 1, sh-1, '*');
	//Left Border
	draw_line(0, 0, 0, sh-1, '*');
	//Right Border
	draw_line(sw-1, 0, sw - 1, sh-1, '*');

	// --- Info Panel --- //
	draw_formatted(sw/4, 3, "Lives = %2d *Score = %2d *Level = %2d *Time = %2d",
		       	lives, score, level, realTime);


}


void SetupBall(void) {
	sprite_move_to(ball, sw / 2, (sh + 7)/2);
	sprite_draw(ball);
	sprite_turn_to(ball, 0.6, 0);
	sprite_turn(ball, 140); 
}

// Game setup
void setup(void) {
	// Variable Declaration
	startingTime = get_current_time();
	int sh = screen_height(), sw = screen_width();
	int pw = PLAYER_WIDTH, ph = PLAYER_HEIGHT;
	// --- Create and Draw Sprites --- //
	// Player Entity
	player = sprite_create(3, (sh + ph)/2, pw, ph, player_char);	
	sprite_draw(player);

	// Ball Entity
	ball = sprite_create(sw/2, (sh + 7)/2, 1, 1, "O");
	sprite_draw(ball);	
	SetupBall();

	// Opponent Entity
	opponent = sprite_create(sw - 3, (sh + ph)/2, pw, ph, player_char);	

	// Refresh the screen
	show_screen();
}

// Help screen
void HelpScreen(void) {
	clear_screen();
	draw_string(30, 10, "Tyler McKerihan N9447482");
	draw_string(20, 12, "Instructions:");
	draw_string(20, 13, "Move up: w");
	draw_string(20, 14, "Press Any Key to Continue");
	show_screen();
	getchar();
	clear_screen();
	return;
}

//Quit screen
void QuitScreen(void) {
	clear_screen();
	draw_string(30, 10, "Quit Screen");
	show_screen();
	getchar();
}

// Ball physics
void MoveBall(void) {
	sprite_step(ball);

	oy = sprite_y(opponent);

	ph = PLAYER_HEIGHT;

	int bx = round(sprite_x(ball));
	int by = round(sprite_y(ball));

	double bdx = sprite_dx(ball);
	double bdy = sprite_dy(ball);
	dir_changed = false;

	// Ball hit LEFT side
	if (bx == 0) {
		SetupBall();
		lives--;
	// Ball hits RIGHT side
	} else if (bx == sw - 2) {
		if (level == 1) {
			bdx = -bdx;
			dir_changed = true;
		} else if (level > 1) {
			SetupBall();
			score++;
		}
	}

	if (by == 6 || by == sh - 1) {
		bdy = -bdy;
		dir_changed = true;
	}

	// --- Sprite Collision Check --- //
	// Ball with player
	if (bx == 4 && by >= py && by <= py + ph + 1) {
		bdx = -bdx;

		dir_changed = true;
		score++;
	}

	// Ball with opponent
	if (bx == sw - 4 && by >= oy && by <= oy + ph + 1) {
		bdx = -bdx;
		dir_changed = true;
	}

	if (dir_changed) {
		sprite_back(ball);
		sprite_turn_to(ball, bdx, bdy);
	}
	
}

// Opponent physics
void MoveOpponent(void) {
	by = sprite_y(ball);
	int omy = oy + ph/2;

	if (omy > by && oy != 7) {
		sprite_move(opponent, 0, -1);
	} else if (omy < by && oy + ph != sh - 1) {
		sprite_move(opponent, 0, +1);
	}
}

//Accelerate the ball to centre
void accelerate_ball_towards_centre(void) {
	double x_diff = x_star - sprite_x(ball);
	double y_diff = y_star - sprite_y(ball);

    	double dist_squared = pow(x_diff, 2) + pow(y_diff, 2);

    	if (dist_squared < 1e-10) {
		dist_squared = 1e-10;
	}

    	double dist = sqrt(dist_squared);

    	double dx = sprite_dx(ball);
	double dy = sprite_dy(ball);
    	
    	double a = 1/dist_squared;

    	dx = dx + (a * x_diff / dist);
	dy = dy + (a * y_diff / dist);

	int speed = hypot(dx, dy);
    
    	if (speed > 1) {
		dx = dx / speed;
		dy = dy / speed;
	}

    	sprite_turn_to(ball, dx, dy);
}

//Level 3 star physics
void StarPhysics(void) {

	if (temp3 == 1) {
		x_star = screen_width() / 2;
		y_star = screen_height() / 2;

		double sdx = x_star - sprite_x(ball);
		double sdy = y_star - sprite_y(ball);

		double dist = hypot(sdx, sdy);

		sdx  = sdx / dist;
		sdy = sdy / dist;

		sdx = sdx * 0.21;
		sdy = sdy * 0.21;

		sprite_turn_to(ball, sdx, sdy);

		accelerate_ball_towards_centre();
	}
}

// Play one turn of game.
void process(void) {
	currentTime = get_current_time() - startingTime;
	realTime = currentTime - pauseTime;
	py = sprite_y(player);
	bx = sprite_x(ball);
	by = sprite_y(ball);
	oy = sprite_y(opponent);

	x_star = screen_width() / 2;
	y_star = screen_height() / 2;

	// Check player input
	int key = get_char();

	EnvironmentSetup();


	// --- Key Inputs --- //
	// -- Player Controls -- //
	// Move player up
	if ( key == 'w' && py > 7 ) {
		sprite_move( player, 0, -1 );
	}
	// Move player down
	else if ( key == 's' && py < sh - PLAYER_HEIGHT - 1) {
		sprite_move( player, 0, +1 );
	}

	// -- System Controls -- //
	// Level change
	else if (key == 'l') {
		if (level == 4) {
			level = 1;
		} else {
		       level++;
		}
	}
	// Help Screen
	if (key == 'h' || initial) {
		int tempTime = get_current_time();
		HelpScreen();
		initial = false;
		EnvironmentSetup();
		pauseTime += get_current_time() - tempTime;
	}

	//Quit Screen
	if (key == 'q') {
		QuitScreen();
		game_over = true;
	}

	MoveBall();
	
	if (level > 1) {
		MoveOpponent();
	}

	if (level == 3) {
		draw_char(x_star, y_star, '*');

		if (bx > sw/2 - 10 && bx < sw/2 + 10 &&
		    by < sh/2 + 10 && by > sh/2 - 10) { 	
			StarPhysics();
			temp3++;
		} else {
			temp3 = 1;
		}
	}

	
	
	// Draw Sprites
	sprite_draw(player);
	sprite_draw(ball);
	if (level > 1) {
		sprite_draw(opponent);
	}

	// (x)	Draw the zombie.
	//sprite_draw( zombie );
}

// Clean up game
void cleanup(void) {
	// STATEMENTS
}


// Program entry point.
int main(void) {
	setup_screen();

	auto_save_screen(true);

	setup();
	show_screen();

	while ( !game_over ) { 
		process();

		if ( update_screen ) {
			show_screen();
		}

		timer_pause(DELAY);
	}

	cleanup();

	return 0;
}
