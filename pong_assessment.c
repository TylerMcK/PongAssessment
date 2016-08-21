#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

// ADD INCLUDES HERE

// Configuration
#define DELAY (10) /* Millisecond delay between game updates */

#define PLAYER_WIDTH (1)
#define PLAYER_HEIGHT (5)
#define ZOMBIE_WIDTH (5)
#define ZOMBIE_HEIGHT (5)

// Game state.
bool game_over = false; /* Set this to true when game is over */
bool update_screen = true; /* Set to false to prevent screen update. */

// Global Variables
int lives  = 10;
int score = 0;
int level = 1;
int clock = 0;
bool clock_pause = false;

static char * msg_image =
/**/	"Goodbye and thank-you for playing ZombieDash Jr."
/**/	"            Press any key to exit...            ";

// Declare sprite_ids
sprite_id player;


// Game setup
void setup(void) {
	// Variable Declaration
	int sw = screen_width(), pw = PLAYER_WIDTH;
	int sh = screen_height(), ph = PLAYER_HEIGHT;

	// --- Create and Draw Sprites --- //
	// Player Entity
	player = sprite_create(3, (sh-ph)/2, pw, ph, "|");
	sprite_draw(player);
	
	// (n)	Set up the zombie at a random location on the screen.
	//int xrange = w - wz - 2;
	//int yrange = h - hz - 2;
	//srand( get_current_time() );
	//int zx = rand() % xrange + 1;
	//int zy = rand() % yrange + 1;
	//zombie = sprite_create( zx, zy, wz, hz, zombie_image );

	// (o) Draw the zombie.
	//sprite_draw( zombie );

	// (p) Set the zombie in motion.
	//sprite_turn_to ( zombie, 0.2, 0 );
	//sprite_turn( zombie, rand() % 360 );

	// Refresh the screen
	show_screen();
}

// Play one turn of game.
void process(void) {
	int sw = screen_width(), pw = PLAYER_WIDTH;
	int sh = screen_height(), ph = PLAYER_HEIGHT;
	int clock = 0;

	// Check player input
	int key = get_char();
	
	// (y) Test for end of game.

	// (g)	Get the current screen coordinates of the hero in integer variables 
	//		by rounding the actual coordinates.
	int px = sprite_x(player);
	int py = sprite_y(player);

	// --- Key Inputs --- //
	// -- Player Controls -- //
	// Move player Up
	if ( key == 'w' && px > 1 ) {
		sprite_move( player, 0, -1 );
	}
	// Move player down
	else if ( key == 's' && px < sw - pw - 1) {
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

	if (!clock_pause) {
		clock++;
	}	

	// (q.a) Test to move the zombie if key is 'z' or ERROR.
	//else if ( key == 'z' || key < 0 ) {
		// (r) Zombie takes one step.
	//	sprite_step( zombie );

		// (s) Get screen location of zombie.
	//	int zx = round( sprite_x( zombie ) );
	//	int zy = round( sprite_y( zombie ) );

		// (t) Get the displacement vector of the zombie.
	//	double zdx = sprite_dx( zombie );
	//	double zdy = sprite_dy( zombie );
	//	bool dir_changed = false;

		// (u) Test to see if the zombie hit the left or right border.
	//	if ( zx == 0 || zx == w - 5 ) {
	//		zdx = -zdx;
	//		dir_changed = true;
	//	}

		// (v) Test to see if the zombie hit the top or bottom border.
	//	if ( zy == 6  || zy == h - 5 ) {
	//		zdy = -zdy;
	//		dir_changed = true;
	//	}

		// (w) Test to see if the zombie needs to step back and change direction.
	//	if ( dir_changed ) {
	//		sprite_back( zombie );
	//		sprite_turn_to( zombie, zdx, zdy );
	//	}

	// (q.b) End else-if test to move the zombie if key is 'z' or negative.
	//}
	
	// Leave next line intact
	clear_screen();

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
		       	lives, score, level, clock);

	// (l)	Draw the hero.
	sprite_draw(player);

	// (x)	Draw the zombie.
	//sprite_draw( zombie );
}

// Clean up game
void cleanup(void) {
	// STATEMENTS
}

// Help screen


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
