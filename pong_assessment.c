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
		height = (screen_height() - 7 - 1) / 2;
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
int seconds = 0;
int minutes = 0;


bool initial = true;
bool dir_changed;

int sw;
int sh; 
int pw;
int ph;

//Level 3
int x_star;
int y_star;
int temp3 = 1;
int anomalyDelay = 0;

//Level 4
bool initial4 = true;

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
/**/	"|";

static char * anomaly_char =
/**/"[  |  ]"
/**/" [ | ] "
/**/"-- * --"
/**/" [ | ] "
/**/"[  |  ]";

static char * three_char =
/**/"-----"
/**/"|   |"
/**/"| 3 |"
/**/"|   |"
/**/"-----";

static char * two_char =
/**/"-----"
/**/"|   |"
/**/"| 2 |"
/**/"|   |"
/**/"-----";

static char * one_char =
/**/"-----"
/**/"|   |"
/**/"| 1 |"
/**/"|   |"
/**/"-----";

static char * helpScreen_char =
/**/"   Tyler McKerihan   "
/**/"       N9447482      "
/**/"   w: Move Up        "
/**/"   s: Move Down      "
/**/"   l: Change Level   "
/**/"   h: Help Screen    "
/**/"   q: Quit           ";


// Declare sprite_ids
sprite_id player;
sprite_id opponent;
sprite_id ball;

// Environmental Sprites
sprite_id anomaly;
sprite_id three;
sprite_id two;
sprite_id one;
sprite_id helpScreen;

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
	draw_formatted(sw/4, 3, "Lives = %2d *Score = %2d *Level = %2d *Time = %1d:%1d",
		       	lives, score, level, minutes, seconds);


}


void SetupBall(void) {
	sprite_move_to(ball, sw / 2, sh/2);
	sprite_draw(ball);
	sprite_turn_to(ball, 0.5, 0);
	int launchDirection = rand() % 90;
	sprite_turn(ball, 230); 
}

// Create Initial Barriers Array 
void CreateBarriersArray(void) {
	int msw = screen_width() / 2;
	int shs1 = (screen_height() + 5) * 1/3;
	int shs2 = (screen_height() + 5) * 2/3;
	int qsw1 = screen_width() * 1/4;
	
	
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
	ball = sprite_create(sw/2, sh /2, 1, 1, "O");
	sprite_draw(ball);	
	SetupBall();

	// Anomaly
	anomaly = sprite_create(sw/2, sh/2, 7, 5, anomaly_char);

	// Countdown Sprites
	three = sprite_create(sw/2, sh/2, 5, 5, three_char);
        two = sprite_create(sw/2, sh/2, 5, 5, two_char);
	one = sprite_create(sw/2, sh/2, 5, 5, one_char);	

	helpScreen = sprite_create(sw/2, sh/2, 21, 7, helpScreen_char);

	// Opponent Entity
	opponent = sprite_create(sw - 3, (sh + ph)/2, pw, ph, player_char);	
	
	// Barriers Array
	CreateBarriersArray();

	// Refresh the screen
	show_screen();
}

// Countdown
void countdown(void) {
	clear_screen();
	sprite_draw(three);
	show_screen();
	timer_pause(300);
	clear_screen();
	sprite_draw(two);
	show_screen();
	timer_pause(300);
	clear_screen();
	sprite_draw(one);
	show_screen();
	timer_pause(300);
	clear_screen();
}

// Help screen
void HelpScreen(void) {
	clear_screen();
	sprite_draw(helpScreen);
	show_screen();
	getchar();
	clear_screen();
	countdown();
	return;
}



// Quit screen
void QuitScreen(void) {
	clear_screen();
	draw_string(30, 10, "Quit Screen");
	show_screen();
	getchar();
}

// Ball physics
void MoveBall() {
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

	// --- LEVEL 4 --- //
	if (level == 4) {
		int swq1 = screen_width() * 1/4;
		int swq2 = screen_width() * 3/4;
		int shs1 = screen_height() * 1/3;
		int shs2 = screen_height() * 2/3;
		int barrierSize = swq2 - swq1 + 1;

		int barrierArray1[barrierSize];
		int barrierArray2[barrierSize];

		int temp = 0;
		for (int i = swq1; i <= swq2; i++) {
			if (barrierArray1[temp] != -10) {
				barrierArray1[temp] = i;
				draw_char(barrierArray1[temp], shs1, '=');
			}
		       
			if (barrierArray2[temp] != -10) {
				barrierArray2[temp] = i;
				draw_char(barrierArray2[temp], shs2, '=');
			} 
			
			if (barrierArray1[temp] == -10) {
				draw_char(barrierArray1[temp], shs1, ' ');
			}

			if (barrierArray2[temp] == -10) {
				draw_char(barrierArray2[temp], shs2, ' ');
			}	
			temp++;
		}

		for (int i = 0; i < barrierSize + 1; i++) {
			if (bx == barrierArray1[i] && by == shs1) {
				barrierArray1[i] = -10;
				bdy = -bdy;
				dir_changed = true;
			}

			if (bx == barrierArray2[i] && by == shs2) {
				barrierArray2[i] = -10;
				bdy = -bdy;
				dir_changed = true;
			}

			if (bx == barrierArray1[i] && by == shs1 ||
					bx == barrierArray2[i] && by == shs2) {
				bdx = -bdx;
				dir_changed = true;
			}

		}

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

//Level 4 barrier setup
/*void SetupBarriers(void) {
	int swq1 = screen_width() * 1/4;
	int swq3 = screen_width() * 3/4;
	int shs1 = (screen_height() - 7) * 1/3;
	int shs2 = (screen_height() - 7) * 2/3;

	draw_line(swq1, shs1, swq3, shs1, '=');
	draw_line(swq1, shs2, swq3, shs2, '=');
}*/


// Play one turn of game.
void process(void) {
	currentTime = get_current_time() - startingTime;
	realTime = currentTime - pauseTime;
	
	// Time formatting
	seconds = realTime % 60;
	minutes = (realTime / 60) % 60;

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
		countdown();
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
		if (anomalyDelay >= 500) {
			sprite_draw(anomaly);

			if (bx > sw/2 - 10 && bx < sw/2 + 10 &&
					by < sh/2 + 10 && by > sh/2 - 10) { 	
				StarPhysics();
				temp3++;
			} else {
				temp3 = 1;
			}
		} else {
			anomalyDelay++;
		}
	} else {
		anomalyDelay = 0;
	}

	if (level == 4) {
		if (initial4) {
			initial4 = false;
		}
	} else { 
		initial4 = true;
	}

	// Draw Sprites
	sprite_draw(player);
	sprite_draw(ball);
	if (level > 1) {
		sprite_draw(opponent);
	}
}

// Program entry point.
int main(void) {
	setup_screen();

	auto_save_screen(false);

	setup();
	show_screen();

	while ( !game_over ) { 
		process();

		if ( update_screen ) {
			show_screen();
		}

		timer_pause(DELAY);
	}

	return 0;
}
