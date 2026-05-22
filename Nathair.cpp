/*  Nathair - The Last Snake of Ireland
    -------------------------------------------------------------------------------------------
    Credits / Attribution: (in no particular order)
    -------------------------------------------------------------------------------------------
        Lane Hauck (1976)     -- Original creator of the "Blockade" arcade game
        Peter Trefonas (1978) -- Creator of the "Worm" evolution of Blockade
        Taneli Armanto (1997) -- Creator of the "Snake" evolution of Worm

        Jacob "Cloaked Figure" Cronly -- Nathair's Programming and Design

        Dr. Andrew Cain -- Creator of the SplashKit programming library --> https://splashkit.io 
            -- (Also thanks and credit to the SIT faculty at Deakin)

        Shamrock sprite by StreamlineHQ (Founder: Vincent Le Moign)
            I heavily modified the original image's colouring and style to fit the game.
            Link to image: https://www.streamlinehq.com/elements/download/shamrock--31896 
            Licence (cc By attribution 4.0): https://creativecommons.org/licenses/by/4.0/ 

        Berenika font by Wojciech Kalinowski
            --> https://www.dafont.com/berenika.font 

        Music Track Generated using Suno (https://suno.com) (Non-commercial use)
            Prompt: "celtic, folklore, fairytale, story"

    -------------------------------------------------------------------------------------------
    Special Thanks: (in no particular order)

        Ben Philip -- For being a great tutor and giving me the idea to make a snake game
        Alex Wu    -- For being a great tutor and providing guidance with my studies

        Vadym "YellowAfterlife" Diachenko (https://yal.cc)
        -- For being an incredible programming mentor over the years! <3
        -- Thankyou for teaching me almost everything I know about code!
        
        Hayden "TabularElf" (https://tabelf.link) 
        -- For being a great friend and teaching this old dog new tricks! <3    
*/


//------------------------------------------------------------------------------ dependencies    
#include "splashkit.h"          // use splashkit


//------------------------------------------------------------------------------ enumerators
// Nathair's "bearing" direction, integer degrees (anti-clockwise) alias
enum eDIR 
{
    eDIR_RIGHT = 0,             // bearing right
    eDIR_UP    = 90,            // bearing up
    eDIR_LEFT  = 180,           // bearing left
    eDIR_DOWN  = 270            // bearing down
};


//------------------------------------------------------------------------------ constants
const int cSCREEN_W = 800;      // constant window width
const int cSCREEN_H = 600;      // constant window height
const int cSCENE_W = 800;       // constant play area width
const int cSCENE_H = 544;       // constant play area height (since 600/32 = 18.75)
// ... i can use the remaining height for user-interface / heads-up-display
const int cMAX_ARRAY = 100;     // max array size on stack
const int cCELL_SIZE = 32;      // grid cell size


//------------------------------------------------------------------------------ structs
// storage struct for all game resources
struct resources
{
    // load colours
    color rgba_background;          // dark lush foresty aqua green
    color rgba_ui_hud;              // dark blueish but not quite black
    color rgba_ui_text;             // gold-ish?
    color rgba_ui_text_gameover;    // red-ish-pink-ish?
    color rgba_ui_halt_overlay;     // dark blueish but not quite black, faded alpha
    // fonts
    font fnt_berenika;              // main game font
    // Nathair's images
    bitmap bmp_nathair_right;       // Nathair's right facing head sprite
    bitmap bmp_nathair_up;          // Nathair's up facing head sprite
    bitmap bmp_nathair_left;        // Nathair's left facing head sprite
    bitmap bmp_nathair_down;        // Nathair's down facing head sprite
    bitmap bmp_nathair_chunk;       // Nathair's body chunk sprite
    // other images
    bitmap bmp_sward;               // grass "sward" tile
    bitmap bmp_shamrock;            // shamrock sprite
    bitmap bmp_dev_cross;           // debug node sprite
    // audio
    music mus_nathair_lark;         // main game music
};


// Nathair's body part struct
struct chunk
{
    vector_2d pos;              // this chunk's position
};


// Nathair's struct
struct snake
{
    vector_2d pos;              // Nathair's position
    eDIR dir;                   // Nathair's movement direction
    eDIR dir_next;              // Nathair's next direction to move in
    bool moving;                // Nathair should move?
    bitmap *sprite;             // POINTER* of Nathair's image to use
    int num_eaten;              // Nathair's eaten shamrocks
    int num_chunks;             // Nathair's body length in chunks
    chunk chunks[cMAX_ARRAY];   // Nathair's body chunks array (up to 100)
};


// edible shamrock struct
struct shamrock
{
    vector_2d pos;  // shamrock position
    bool show;      // show this shamrock?
};


// gamestate struct
struct game
{
    resources assets;                   // game assets
    bool halt;                          // main game is halted?
    bool pause;                         // main game is paused? (by user)
    bool gameover;                      // main game has ended? (Nathair lost?)
    double clock;                       // main game ticks
    double time_to_update;              // main game update timer
    shamrock shamrocks[cMAX_ARRAY];     // all shamrocks on screen
    int num_shamrocks;                  // number of present shamrocks on-screen
    snake nathair;                      // main player snake Nathair
};


//------------------------------------------------------------------------------ functions
// func load game resources
resources assets_load()
{
    // make new resources struct
    resources assets = {};

    // load colours
    assets.rgba_background = rgba_color(0,48,32,255);               // dark lush foresty aqua green
    assets.rgba_ui_hud     = rgba_color(0,16,16,255);               // dark blueish but not quite black
    assets.rgba_ui_text    = rgba_color(255,240,128,255);           // gold-ish?
    assets.rgba_ui_text_gameover = rgba_color(255,160,160,255);     // red-pink-ish?
    assets.rgba_ui_halt_overlay  = rgba_color(0,16,16,170);         // dark blueish but not quite black, faded alpha
    // load fonts
    assets.fnt_berenika = load_font("berenika","font_berenika/Berenika-Bold.ttf");  // load main berenika font
    // load Nathair's images
    assets.bmp_nathair_right = load_bitmap("nathair_right","nathair_right.png");    // load Nathair's right facing head sprite
    assets.bmp_nathair_up    = load_bitmap("nathair_up","nathair_up.png");          // load Nathair's left facing head sprite
    assets.bmp_nathair_left  = load_bitmap("nathair_left","nathair_left.png");      // load Nathair's left facing head sprite
    assets.bmp_nathair_down  = load_bitmap("nathair_down","nathair_down.png");      // load Nathair's down facing head sprite
    assets.bmp_nathair_chunk = load_bitmap("nathair_chunk","nathair_chunk.png");    // load Nathair's body chunk sprite
    // load other images
    assets.bmp_sward     = load_bitmap("sward","sward.png");                        // load sward grass tile
    assets.bmp_shamrock  = load_bitmap("shamrock","shamrock.png");                  // load shamrock sprite
    assets.bmp_dev_cross = load_bitmap("dev_cross","dev_cross.png");                // load debug node sprite
    // load audio
    assets.mus_nathair_lark = load_music("mus_nathair_lark","nathair_lark.ogg");    // load main game music

    // return struct of resources
    return assets;
}


// func add chunk to Nathair
void chunk_add(snake &danger_noodle)
{
    // get shorthand for Nathair
    snake &s = danger_noodle;

    // add a chunk to Nathair
    s.chunks[s.num_chunks] = {};

    // start new chunk in the tail
    s.chunks[s.num_chunks].pos = {s.chunks[s.num_chunks-1].pos};

    // increase the amount of Nathair's chunks by 1
    s.num_chunks += 1;
}


// func get ranom grid snapped coordinate in scene
vector_2d cell_random()
{
    // get a new random cell coordinate
    double x,y;
    x = rnd(cSCENE_W/32) *32;
    y = rnd(cSCENE_H/32) *32;

    // make a new vec2 for the cell coordinate
    vector_2d cell = {x,y};

    // return the cell coordinate
    return cell;
}


// func make new snake player instance
snake snake_create(game &scene)
{
    snake s = {};               // make Nathair's struct

    // Nathair's starting xy position as random cell
    s.pos = cell_random();

    s.moving  = false;               // Nathair starts idle
    s.dir     = eDIR_RIGHT;          // Nathair's default direction
    s.dir_next = eDIR_RIGHT;         // Nathair next change direction
    // there's likely a much better way to solve directions, but it did not occur to me immedately

    // set Nathair's sprite
    s.sprite = &scene.assets.bmp_nathair_right;

    s.num_eaten = 0;            // Nathair's starting shamrocks

    // make Nathair's first "neck" chunk
    s.chunks[0] = {};
    s.chunks[0].pos.x = s.pos.x -cCELL_SIZE;
    s.chunks[0].pos.y = s.pos.y;

    // number of Nathair's starting chunks (beyond the head)
    s.num_chunks = 1;

    // return a struct of Nathair
    return s;
}


// func make new shamrock instance
void shamrock_create(game &scene)
{
    // the code technically allows for multiple shamrocks...
    // ...but i'm not sure i'll use this?

    // get new random cell pos for shamrock
    vector_2d pos = cell_random();

    // write shamrock into game scene's shamrocks
    scene.shamrocks[scene.num_shamrocks] = {pos,true};
    // add 1 to the scene's shamrocks
    scene.num_shamrocks += 1;
}


// func get reverse direction (used once), returns the reverse of an eDIR direction
eDIR dir_get_reverse(eDIR &direction)
{
    // create new direction for revese direction
    eDIR reverse;

    // determine Nathair's current forbidden reverse direction
    switch(direction)
    {
        case eDIR_RIGHT:  reverse = eDIR_LEFT;   break;
        case eDIR_UP:     reverse = eDIR_DOWN;   break;
        case eDIR_LEFT:   reverse = eDIR_RIGHT;  break;
        case eDIR_DOWN:   reverse = eDIR_UP;     break;
    }

    // return reverse direction
    return reverse;
}


// func set Nathair's direction from keyboard input
void snake_input_direct(snake &danger_noodle)
{
    // get shorthand ref for Nathair
    snake &s = danger_noodle;

    // get keyboard input
    bool right, up, left, down;
    right = key_down(RIGHT_KEY);
    up    = key_down(UP_KEY);
    left  = key_down(LEFT_KEY);
    down  = key_down(DOWN_KEY);

    // if any input, then Nathair starts moving... (only happens once ever)
    if(right || up || left || down)     // if any input...
    {  s.moving = true;  }              // ...then Nathair starts moving

    // get move direction from keyboard input
    if(right)  {s.dir_next = eDIR_RIGHT;}
    if(up)     {s.dir_next = eDIR_UP;}
    if(left)   {s.dir_next = eDIR_LEFT;}
    if(down)   {s.dir_next = eDIR_DOWN;}
}


// func move Nathair's chunks
void snake_chunks_move(snake &danger_noodle)
{
    // get shorthand for Nathair
    snake &s = danger_noodle;

    // if Nathair has more than just a head and a neck...
    if(s.num_chunks > 1)
    {
        // update Nathair's body chunk positions...
        // for each chunk from the tail to the neck...
        for(int i=s.num_chunks-1; i>0; i--)
        {   
            // set this chunk's position to that of the chunk ahead
            s.chunks[i].pos = s.chunks[i-1].pos;
        }
    }

    // update Nathair's "neck" chunk's position either way
    s.chunks[0].pos = s.pos;
}


// func move Nathair in their direction
void snake_move(snake &danger_noodle)                                
{
    // get shorthand ref for Nathair
    snake &s = danger_noodle;

    // if Nathair is moving...
    if(s.moving)
    {
        // update the chunks of Nathair
        snake_chunks_move(s);

        // get Nathair's forbidden reverse direction
        eDIR dir_reverse = dir_get_reverse(s.dir);

        // make sure Nathair isn't trying to move in the forbidden reverse direction
        if(s.dir_next != dir_reverse)
        {   s.dir = s.dir_next;  }      // set Nathair's current direction to their assigned next direction

        // move Nathair's head along their set movement direction
        switch(s.dir)
        {
            case eDIR_RIGHT:  s.pos.x += cCELL_SIZE;  break;    // move right
            case eDIR_UP:     s.pos.y -= cCELL_SIZE;  break;    // move up
            case eDIR_LEFT:   s.pos.x -= cCELL_SIZE;  break;    // move left
            case eDIR_DOWN:   s.pos.y += cCELL_SIZE;  break;    // move down
        }
    }

    // keep nathair in the screen, wrap around
    double farx,fary;
    farx = cSCENE_W -cCELL_SIZE;
    fary = cSCENE_H -cCELL_SIZE;

    if(s.pos.x > farx)  {s.pos.x = 0;}      // too far right, goto screen left
    if(s.pos.x < 0)     {s.pos.x = farx;}   // too far left,  goto screen right
    if(s.pos.y > fary)  {s.pos.y = 0;}      // too far down,  goto screen top
    if(s.pos.y < 0)     {s.pos.y = fary;}   // too far up,    goto screen bottom
}


// func make new game struct on startup
game game_new(resources &assets)
{
    // start playing the game music at full volume at the beginning
    play_music("mus_nathair_lark");
    set_music_volume(1);

    // setup the game scene
    game scene = {};                        // make a new game scene
    scene.assets = assets;                  // set game's assets to be ref'd assets... or else...

    scene.halt = false;                     // start game unhalted
    scene.pause = false;                    // start game unpaused
    scene.gameover = false;                 // game over is false

    scene.clock = 0;                        // set main gamer ticks to 0
    scene.time_to_update = 16;              // set default ticks to next update
    scene.nathair = snake_create(scene);    // make Nathair
    scene.num_shamrocks = 0;                // start with 0 shamrocks
    shamrock_create(scene);                 // add a single shamrock to the scene

    // return the game scene
    return scene;                           
}


// func Nathair vs all shamrocks collision
void snake_vs_shamrocks(game &scene)
{
    // get shorthand for lassie Nathair
    snake &s = scene.nathair;

    // for all shamrocks in the scene...
    for(int i=0; i<scene.num_shamrocks; i++)
    {
        // get shorthand for the shamrock
        shamrock &c = scene.shamrocks[i];

        // check if Nathair collides with the shamrock...
        // check if Nathair's x position is near enough is good enough...
        if(s.pos.x > c.pos.x-4  &&  s.pos.x < c.pos.x+4)
        {
            // AYE! we're fair to middling NOW lassie!
            // check if Nathair's y position is near enough is good enough...
            if(s.pos.y > c.pos.y-4  &&  s.pos.y < c.pos.y+4)
            {
                // we're AN-MHAITH lassie! -- Nathair eats the shamrock
                if(s.num_chunks < cMAX_ARRAY)
                {
                    chunk_add(s);               // grow Nathair
                    s.num_eaten += 1;           // add 1 to shamrock score
                    shamrock_create(scene);     // make a new random shamrock
                }

                // set the shamrock to no longer draw
                c.show = false;

                // do "swap n pop" on shamrocks array
                c = scene.shamrocks[scene.num_shamrocks-1];
                scene.num_shamrocks -= 1;
            }
        }
    }
}


// func Nathair vs herself collision
void snake_vs_self(game &scene)
{
    // get shorthand for Nathair
    snake &s = scene.nathair;

    // test Nathair for collision with herself...
    // for each chunk of Nathair...
    for(int i=0; i<s.num_chunks; i++)
    {
        // get shorthand for this chunk
        chunk &ch = s.chunks[i];

        // check to see if Nathair's head's x coordinate is at this chunk's x coordinate...
        // ...near enough is good enough, use a 4 pixel buffer either side...
        if(s.pos.x > ch.pos.x-4  &&  s.pos.x < ch.pos.x+4)
        {
            // now check to see if Nathair's head's y coordinate is at this chunk's y coordinate...
            // ...near enough is good enough, use a 4 pixel buffer either side...
            if(s.pos.y > ch.pos.y-4  &&  s.pos.y < ch.pos.y+4)
            {
                // Nathair has collided with a chunk of herself... game over!

                // halt the game and show the gameover screen
                // user later triggers starting a new game
                scene.halt = true;
                scene.gameover = true;
            }
        }
    }
}


// func update game state step
void update(game &scene)
{
    process_events();           // process windows events
    snake &s = scene.nathair;   // get shorthand for Nathair

    // get user input for pause/unpause game or play again...
    bool input_pause_or_play = (key_typed(P_KEY) || key_typed(ESCAPE_KEY) || key_typed(SPACE_KEY));

    // check if game over has occurred...
    if(scene.gameover)
    {
        // if user presses key to play again...
        if(input_pause_or_play)
        {
            scene = game_new(scene.assets);
            return;
        }
    }

    // check if player presses pause game key... (and player has not lost)
    if(input_pause_or_play && !scene.gameover)
    {
        scene.halt = !scene.halt;       // flip halted state
        scene.pause = scene.halt;       // set pause state AS halted state
    }

    // if game is halted...
    if(scene.halt)
    {
        // reduce voume of the music when game is halted
        set_music_volume(.25);
    }

    // if game is unhalted...
    if(!scene.halt)
    {
        // music plays at full volume when game is unhalted
        set_music_volume(1);

        // check to see if music has stopped playing...
        if(!music_playing())
        {
            // repeat the music track playback...
            play_music("mus_nathair_lark");
        }

        // add 1 to ticks per frame...
        scene.clock += 1;

        // how fast the game runs, quarter of a second, reduced by shamrocks eaten
        double time_to_update = 16 -(s.num_eaten *.5);

        // make sure time to update isn't absurdly fast
        if(time_to_update < 6)
        {  time_to_update = 6;  }

        // get Nathair's direction from keyboard input
        snake_input_direct(s);

        // if main update timer ticks should cause a timed update...
        if(scene.clock >= time_to_update)
        {
            snake_move(s);              // move Nathair from keyboard input
            snake_vs_self(scene);       // check Nathair vs herself
            snake_vs_shamrocks(scene);  // check Nathair vs shamrocks
            scene.clock = 0;            // reset timer ticks
        }
    }
}


// func render ground tiles
void render_ground_tiles(game &scene)
{
    // for each tile width in scene...
    for(int x=0; x<cSCENE_W; x+=128)
    {
        // for each tile height in scene...
        for(int y=0; y<cSCENE_H; y+=128)
        {
            // render a tile of sward
            draw_bitmap(scene.assets.bmp_sward, x, y);
        }
    }
}


// func render Nathair's chunks
void render_chunks(game &scene)
{
    // draw all of Nathair's body chunks...
    for(int i=0; i<scene.nathair.num_chunks; i++)
    {
        // get shorthand for this Nathair chunk
        chunk &c = scene.nathair.chunks[i];

        // draw this Nathair chunk
        draw_bitmap(scene.assets.bmp_nathair_chunk, c.pos.x, c.pos.y);
    }
}


// func render Nathair
void render_nathair(game &scene)
{
    // getshorthand for Nathair and assets
    resources &res = scene.assets;      // shorthand for resource assets
    snake &s = scene.nathair;           // shorthand for nathair

    // render Nathair's chunks
    render_chunks(scene);

    // determine Nathair's head image... set pointer to ref of the image
    switch(s.dir)
    {   
        case eDIR_RIGHT:  s.sprite = &res.bmp_nathair_right;  break;   // Nathair's head facing right
        case eDIR_UP:     s.sprite = &res.bmp_nathair_up;     break;   // Nathair's head facing up
        case eDIR_LEFT:   s.sprite = &res.bmp_nathair_left;   break;   // Nathair's head facing left
        case eDIR_DOWN:   s.sprite = &res.bmp_nathair_down;   break;   // Nathair's head facing down
    }

    // draw Nathair's head (deref the sprite pointer to use Nathair's image)
    draw_bitmap(*s.sprite, s.pos.x, s.pos.y);
}


// func render all shamrocks
void render_shamrocks(game &scene)
{
    // for all shamrocks... draw all shamrocks
    for(int i=0; i<scene.num_shamrocks; i++)
    {
        // get shorthand for this shamrock
        shamrock &clover = scene.shamrocks[i];

        // if this shamrock is visible...
        if(clover.show)
        {
            // draw this shamrock
            draw_bitmap(scene.assets.bmp_shamrock, clover.pos.x, clover.pos.y);
        }
    }    
}


// func render heads up display
void render_hud(game &scene)
{
    // draw a black rectangle for a user-interface strip / heads up display
    fill_rectangle(scene.assets.rgba_ui_hud, 0, cSCENE_H, cSCREEN_W, 64);

    // draw icon for shamrocks
    draw_bitmap(scene.assets.bmp_shamrock, 8, cSCENE_H +12);

    // draw text for shamrock score:
    draw_text("Shamrocks: "+to_string(scene.nathair.num_eaten), scene.assets.rgba_ui_text, scene.assets.fnt_berenika, 24, 48, cSCENE_H +12);
}


// func render pause menu
void render_menu_pause(game &scene)
{
    // draw pause menu title to the screen (182x seems to be the middle?)
    draw_text("*  Game Paused  *", scene.assets.rgba_ui_text, scene.assets.fnt_berenika, 48, 182, 100);

    // draw pause menu control hints text to the screen (155x is middle?)
    draw_text("Press [Space / Escape / P] to continue", scene.assets.rgba_ui_text, scene.assets.fnt_berenika, 24, 155, 175);
}


// func render game over menu
void render_menu_gameover(game &scene)
{
    // get shorthand for assets struct
    resources &res = scene.assets;

    // draw game over title to the screen (200x is middle)
    draw_text("*  Game Over!  *", res.rgba_ui_text_gameover, res.fnt_berenika, 48, 200, 100);

    // draw game over menu flavour text to the screen (78x is middle)
    draw_text("Nathair got tangled and banished by Saint Patrick!", res.rgba_ui_text_gameover, res.fnt_berenika, 24, 78, 175);

    // draw game over menu control hints text to the screen
    draw_text("Press [Space / Escape / P] to play again", res.rgba_ui_text_gameover, res.fnt_berenika, 24, 155, 225);
}


// func render faded halt foreground overlay
void render_halt_overlay(game &scene)
{
    // draw a black rectangle for a user-interface strip / heads up display
    fill_rectangle(scene.assets.rgba_ui_halt_overlay, 0, 0, cSCREEN_W, cSCREEN_H);
}


// func render the game
void render(game &scene)
{
    clear_screen(scene.assets.rgba_background);     // clear screen with background colour

    render_ground_tiles(scene);                     // render sward tiles
    render_shamrocks(scene);                        // render all shamrocks to screen
    render_nathair(scene);                          // render Nathair to the scene
    
    // check if game is halted...
    if(scene.halt)
    {
        // draw the halted game overlay...
        render_halt_overlay(scene);

        // check if game is paused by user...
        if(scene.pause)
        {
            // draw pause menu text...
            render_menu_pause(scene);
        }

        // check if game over...
        if(scene.gameover)
        {
            // draw game over text...
            render_menu_gameover(scene);
        }
    }

    render_hud(scene);                              // render the heads up display

    refresh_screen(60);                             // refresh the screen at 60 frames per second
}


//------------------------------------------------------------------------------ main function
int main()
{    
    // open main window
    open_window("Nathair - The Last Snake of Ireland", cSCREEN_W, cSCREEN_H);

    // setup game...
    resources assets = assets_load();   // load game resources...
    game scene = game_new(assets);      // create a new game scene with resources

    // main loop...   
    while(!quit_requested())
    {
        update(scene);      // update the game
        render(scene);      // render the game
    }

    // end of main function and program
    return 0;
}