/*  Nathair - The Last Snake of Ireland

    Github Link --> https://github.com/CloakedFigure/Nathair 
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
#include "splashkit.h"      // use splashkit


//------------------------------------------------------------------------------ constants
const int cSCREEN_W = 800;                  // constant window width
const int cSCREEN_H = 600;                  // constant window height
const int cSCREEN_W_HALF = cSCREEN_W/2;     // constant half window width
const int cSCREEN_H_HALF = cSCREEN_H/2;     // constant half window height
const int cSCENE_W = 800;                   // constant play area width
const int cSCENE_H = 544;                   // constant play area height (since 600/32 = 18.75)
// ... i can use the remaining height for user-interface / heads-up-display
const int cMAX_ARRAY = 100;                 // max array size on stack
const int cMAX_LEVELS = 5;                  // max number of levels
const int cMAX_QUARTZ = 2;                  // max number of quartz rocks
const int cCELL_SIZE = 32;                  // grid cell size
const int cFRAME_RATE = 60;                 // constant frames per second
const int cFRAME_FLASH_HIDE = 8;            // constant flash frame to hide Nathair
const string cSTRING_PLAY = "Press [Space / Escape / P] to play";    // string for the control for unhalting the game


//------------------------------------------------------------------------------ enumerators
// enumerator for the current level of the game
enum eLEVEL
{
    eLEVEL_MURRISK  = 0,    // the first level  -- "fields of murrisk"
    eLEVEL_REEKBASE = 1,    // the second level -- "base of the reek"
    eLEVEL_SHOULDER = 2,    // the third level  -- "shoulder of the reek"
    eLEVEL_SUMMIT   = 3,    // the fourth level -- "summit of crough patrick"
    eLEVEL_FINAL    = 4     // the fifth level  -- "all across ireland"
};


// Nathair's "bearing" direction, integer degrees (anti-clockwise) alias
enum eDIR 
{
    eDIR_RIGHT = 0,         // bearing right
    eDIR_UP    = 90,        // bearing up
    eDIR_LEFT  = 180,       // bearing left
    eDIR_DOWN  = 270        // bearing down
};


//------------------------------------------------------------------------------ structs
// storage struct for all game resources
struct resources
{
    // load colours
    color rgba_forest;              // dark lush foresty aqua green
    color rgba_blue_dark;           // dark blueish but not quite black
    color rgba_gold;                // gold-ish?
    color rgba_gold_bright;         // alternate text colour
    color rgba_collision_zone;      // collision zone colour
    color rgba_warning_zone;        // Saint Pat's movement warning zone
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
    bitmap bmp_saint_patrick_right;     // Saint Patrick's sprite facing right
    bitmap bmp_saint_patrick_up;        // Saint Patrick's sprite facing up
    bitmap bmp_saint_patrick_left;      // Saint Patrick's sprite facing left
    bitmap bmp_saint_patrick_down;      // Saint Patrick's sprite facing down
    bitmap bmp_quartz;              // quartz boulder image
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
    int flash_frames;           // number of frames Nathair will flash for
    int lives;                  // Nathair's number of lives remaining
    int num_eaten;              // Nathair's eaten shamrocks
    int num_chunks;             // Nathair's body length in chunks
    chunk chunks[cMAX_ARRAY];   // Nathair's body chunks array (up to 100)
};


// Saint Patrick's struct
struct saint_patrick
{
    vector_2d pos;          // Saint Patrick's position
    eDIR dir;               // Saint Patrick's movement direction
    bitmap *sprite;         // Saint Patrick's current sprite
    bool present;           // is Saint Patrick present on this level?
    int frames_until_move;  // how many frame ticks it takes Saint Patrick to move
    int frames_this_cell;   // how many frame ticks Saint Pat has been on his current space
};


// quartz boulder struct
struct quartz
{
    vector_2d pos;      // rock position
};


// edible shamrock struct
struct shamrock
{
    vector_2d pos;  // shamrock position
    bool show;      // show this shamrock?
};


// level struct
struct level
{
    string title;                       // level's title text
    string desc[cMAX_LEVELS];           // level's descriptions text array
    int shamrock_goal;                  // level's shamrocks goal
};


// gamestate struct
struct game
{
    resources assets;                   // game assets
    bool halt;                          // main game is halted?
    bool intro;                         // main game is showing intro?
    bool pause;                         // main game is paused? (by user)
    bool gameover;                      // main game has ended? (Nathair lost?)

    eLEVEL stage;                       // the stage/level of the game
    level stages[cMAX_LEVELS];          // array for all the game's levels

    double clock;                       // main game ticks
    double time_to_update;              // main game update timer

    quartz rocks[cMAX_QUARTZ];          // make array for quartz boulders
    bool has_quartz;                    // does the current level have quartz?

    shamrock shamrocks[cMAX_ARRAY];     // all shamrocks on screen
    int num_shamrocks;                  // number of present shamrocks on-screen

    snake nathair;                      // main player snake Nathair

    saint_patrick saint_pat;            // hostile entity of Saint Patrick
};


//------------------------------------------------------------------------------ functions
// func input get pause/play key press
bool input_play_toggle()
{
    return (key_typed(P_KEY) || key_typed(ESCAPE_KEY) || key_typed(SPACE_KEY));
}


// func load game resources
resources assets_load()
{
    // make new resources struct
    resources assets = {};

    // load colours
    assets.rgba_forest      = rgba_color(0,48,32,255);              // dark lush foresty aqua green
    assets.rgba_blue_dark   = rgba_color(0,16,16,255);              // dark blueish but not quite black
    assets.rgba_gold        = rgba_color(255,240,128,255);          // gold-ish?
    assets.rgba_gold_bright = rgba_color(255,255,160,255);          // alternate text colour
    assets.rgba_collision_zone = rgba_color(255,0,0,64);            // collision zone colour
    assets.rgba_warning_zone   = rgba_color(255,255,0,64);          // collision warning zone colour
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

    assets.bmp_saint_patrick_right = load_bitmap("saint_patrick_right", "saint_patrick_right.png");     // Saint Patrick's sprite facing right
    assets.bmp_saint_patrick_up    = load_bitmap("saint_patrick_up",    "saint_patrick_up.png");        // Saint Patrick's sprite facing up
    assets.bmp_saint_patrick_left  = load_bitmap("saint_patrick_left",  "saint_patrick_left.png");      // Saint Patrick's sprite facing left
    assets.bmp_saint_patrick_down  = load_bitmap("saint_patrick_down",  "saint_patrick_down.png");      // Saint Patrick's sprite facing down

    assets.bmp_quartz = load_bitmap("quartz","quartz.png");                         // load quartz boulder sprite
    // load audio
    assets.mus_nathair_lark = load_music("mus_nathair_lark","nathair_lark.ogg");    // load main game music

    // return struct of resources
    return assets;
}


// func loop game music track
void tunes_loop(string track, double volume)
{
    // set music at specified volume
    set_music_volume(volume);

    // check to see if music has stopped playing...
    if(!music_playing())
    {  play_music(track);  }   // repeat the music track playback...
}


// func create new level
level level_new(string title, int sham_goal, string desc_0, string desc_1, string desc_2, string desc_3)
{
    // create a new level struct
    level stage = {};

    // add the title text to the level
    stage.title = title;

    // set the level's shamrock goal from the goal arg
    stage.shamrock_goal = sham_goal;

    // add the description text elements to the level decriptions array
    stage.desc[0] = desc_0;  // description 0
    stage.desc[1] = desc_1;  // description 1
    stage.desc[2] = desc_2;  // description 2
    stage.desc[3] = desc_3;  // description 3

    // return newly made level
    return stage;
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


// func get is position inside rectangular area? (slightly offset for top left corner)
bool point_in_rect_ish(double x, double y, double rx, double ry, double rw, double rh)
{
    // check to see if point is inside the quartz rock... 
    // (2 pixel padded range on upper left corner)
    bool in = false;    // make a variable for whether or not the point is inside the bounds

    // check to see if x position is in this rectangles bounds...
    if(x > rx-2  &&  x < rx+rw)
    {
        // check to see if y position is in this rectangles bounds...
        if(y > ry-2  &&  y < ry+rh)
        {  in = true;  }
    }

    return in;
}


// func get is position inside any quartz rocks?
bool point_in_quartz(game &scene, double x, double y)
{
    // return whether a point is inside a quartz rock
    // make variable for whether or not the point is inside a rock...
    bool in = false;

    // for all quartz rocks...
    for(int i=0; i<cMAX_QUARTZ; i++)
    {
        // get shorthand for this rock...
        quartz &rock = scene.rocks[i];

        // check if point is inside this rock...
        if(point_in_rect_ish(x, y, rock.pos.x, rock.pos.y, 64, 64))
        {  in = true;  }
    }

    // return whether the point was inside any rocks
    return in;
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


// func make Saint Patrick
saint_patrick saint_patrick_create(game &scene)
{
    // make struct for Saint Pat
    saint_patrick pat = {};

    // set Saint Pat's starting position
    pat.pos.x = 128;
    pat.pos.y = 128;

    // set Saint Pat's starting direction
    pat.dir = eDIR_DOWN;

    // set Saint Patrick's sprite
    pat.sprite = &scene.assets.bmp_saint_patrick_down;

    // set Saint Pat's starting values
    pat.present = false;                      // is Saint Pat present on this level?
    
    pat.frames_until_move = cFRAME_RATE *1;   // start Saint Pat's move speed to once per second
    pat.frames_this_cell = 0;                 // start Saint Pat as having not started moving

    // return the newly made Saint Patrick
    return pat;
}


// func reposition quartz boulder
void quartz_reposition(game &scene)
{
    // for all quartz rocks in the scene...
    for(int i=0; i<cMAX_QUARTZ; i++)
    {  scene.rocks[i].pos = cell_random();  }   // reposition all quartz rocks
}


// func make quartz boulder
quartz quartz_create(game &scene)
{
    // create a quartz boulder
    quartz rock = {};

    // get a random position for the quartz boulder
    rock.pos = cell_random();

    // return the quartz boulder
    return rock;
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

    // set Nathair's lives
    s.lives = 3;                // Nathair starts with 3 lives

    // set how many frames Nathair should flash for...
    s.flash_frames = 0;

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

    // if boulders are present in this scene...
    if(scene.has_quartz)
    {
        // make sure shamrock isn't inside a boulder...
        while(point_in_quartz(scene, pos.x, pos.y))
        {  pos = cell_random();  }
    }

    // write shamrock into game scene's shamrocks
    scene.shamrocks[scene.num_shamrocks] = {pos,true};
    // add 1 to the scene's shamrocks
    scene.num_shamrocks += 1;
}


// func get random cardinal direction
eDIR dir_get_random()
{
    int random = rnd(3);    // get random int between 0 and 3

    // return a random eDIR direction
    switch(random)
    {
        case 0:  return eDIR_RIGHT;  break;
        case 1:  return eDIR_UP;     break;
        case 2:  return eDIR_LEFT;   break;
        case 3:  return eDIR_DOWN;   break;
        default: return eDIR_RIGHT;  break;
    }
}


// func get reverse direction (used once), returns the reverse of an eDIR direction
eDIR dir_get_reverse(eDIR direction)
{
    // return the reverse of the direction
    switch(direction)
    {
        case eDIR_RIGHT:  return eDIR_LEFT;   break;
        case eDIR_UP:     return eDIR_DOWN;   break;
        case eDIR_LEFT:   return eDIR_RIGHT;  break;
        case eDIR_DOWN:   return eDIR_UP;     break;
        default:          return eDIR_RIGHT;  break;
    }
}


// func re-position Saint Patrick on random screen edge 
void saint_patrick_reposition(game &scene)
{
    // get shorthand for Saint Patrick
    saint_patrick &pat = scene.saint_pat;

    pat.dir = dir_get_random();     // get new random direction for Saint Patrick
    pat.pos = cell_random();        // get new random grid-aligned cell position for Saint Patrick

    // re-position Saint Patrick based on his new chosen direction
    switch(pat.dir)
    {
        case eDIR_RIGHT:  pat.pos.x = -256;           break;  // pat starts off screen left-side
        case eDIR_UP:     pat.pos.y = cSCENE_H +256;  break;  // pat starts off screen bottom-side
        case eDIR_LEFT:   pat.pos.x = cSCENE_W +256;  break;  // pat starts off screen right-side
        case eDIR_DOWN:   pat.pos.y = -256;           break;  // pat starts off screen top-side
    }
}


// func update Saint Patrick
void saint_patrick_update(game &scene)
{
    // update Saint Patrick over time
    // move him across the screen from one side to the other
    // when he leaves the screen, he should disappear for some time

    // get shorthand for Saint Patrick
    saint_patrick &pat = scene.saint_pat;

    // make sure Saint Patrick is faster on the final level...
    if(scene.stage == eLEVEL_SUMMIT)
    {
        pat.frames_until_move = cFRAME_RATE/6;     // sixth of a second per movement
    }
    else
    {
        pat.frames_until_move = cFRAME_RATE/3;     // third of a second per movement
    }

    // if Saint Patrick is currently present...
    if(pat.present)
    {
        // increment the number of frames Saint Patrick has been moving for on this space...
        pat.frames_this_cell += 1;

        // if Saint Patrick should move after a certain number of frames...
        if(pat.frames_this_cell > pat.frames_until_move)
        {
            // reset the number of frames Saint Pat has been moving for on this space...
            pat.frames_this_cell = 0;

            // move Saint Patrick along his movement direction
            switch(pat.dir)
            {
                case eDIR_RIGHT:  pat.pos.x += cCELL_SIZE;  break;  // move pat right
                case eDIR_UP:     pat.pos.y -= cCELL_SIZE;  break;  // move pat up
                case eDIR_LEFT:   pat.pos.x -= cCELL_SIZE;  break;  // move pat left
                case eDIR_DOWN:   pat.pos.y += cCELL_SIZE;  break;  // move pat down
            }

            // reposition Saint Patrick depending on his movement and direction
            if(pat.pos.x > cSCENE_W  &&  pat.dir == eDIR_RIGHT)
            {  saint_patrick_reposition(scene);  }

            if(pat.pos.y < -cCELL_SIZE  &&  pat.dir == eDIR_UP)
            {  saint_patrick_reposition(scene);  }

            if(pat.pos.x < -cCELL_SIZE  &&  pat.dir == eDIR_LEFT)
            {  saint_patrick_reposition(scene);  }

            if(pat.pos.y > cSCENE_H  &&  pat.dir == eDIR_DOWN)
            {  saint_patrick_reposition(scene);  }
        }
    }
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

    scene.halt = true;                      // start game halted to show intro
    scene.intro = true;                     // start game showing level intro
    scene.pause = false;                    // start game unpaused
    scene.gameover = false;                 // game over is false

    // create all the levels for the game...
    // "The Fields of Murrisk"
    scene.stages[eLEVEL_MURRISK] = level_new("The Fields of Murrisk", 20, 
        "Nathair begins her journey...", 
        "", "", "");

    // "The Base of The Reek"
    scene.stages[eLEVEL_REEKBASE] = level_new("The Base of The Reek", 30, 
        "Nathair has reached the base of the mountain...", 
        "Watch out for Saint Patrick!", "", "");

    // "The Shoulder of The Reek"
    scene.stages[eLEVEL_SHOULDER] = level_new("The Shoulder of The Reek", 40, 
        "Nathair has reached the shoulder of Crough Patrick...", 
        "Watch out for quartz boulders!", "", "");

    // "The Summit of Crough Patrick"
    scene.stages[eLEVEL_SUMMIT] = level_new("The Summit of Crough Patrick", 80, 
        "Nathair has reached The Summit of Crough Patrick!", 
        "Prepare to face the wrath of Saint Patrick!", 
        "Saint Patrick is very angry and now much faster!", "");

    // "The Summit of Crough Patrick"
    scene.stages[eLEVEL_FINAL] = level_new("Across All of Ireland", 99, 
        "Saint Patrick is chasing Nathair across Ireland!", 
        "Collect as many shamrocks as you can before you get caught!", 
        "Good luck!", "");

    scene.stage = eLEVEL_MURRISK;           // start game in the Fields of Murrisk

    scene.clock = 0;                        // set main gamer ticks to 0
    scene.time_to_update = 16;              // set default ticks to next update

    // make Nathair
    scene.nathair = snake_create(scene);                

    // make Saint Patrick
    scene.saint_pat = saint_patrick_create(scene);     
    scene.saint_pat.present = true; 

    // make quartz rocks
    // all quartz rocks start as not present this level
    // for all quartz rocks...
    for(int i=0; i<cMAX_QUARTZ; i++)
    {
        scene.rocks[i] = quartz_create(scene);
    }

    scene.num_shamrocks = 0;                    // start with 0 shamrocks
    shamrock_create(scene);                     // add a single shamrock to the scene

    // return the game scene
    return scene;                           
}


// func check if Nathair gets banished (check for game over)
void snake_vs_gameover(game &scene)
{
    // get shorthand for Nathair
    snake &s = scene.nathair;

    // check to see if game over... (when Nathair has no lives left)
    if(s.lives < 0)
    {
        // game over! -- halt the game and show the gameover screen
        // user later triggers starting a new game
        scene.halt = true;
        scene.gameover = true;
    }
}


// func Nathair loses a life...
void snake_ouch(game &scene)
{
    scene.nathair.lives -= 1;                       // reduce Nathair's lives by 1
    scene.nathair.flash_frames = cFRAME_RATE *2;    // set Nathair to flash for a number of frames (60 frames is 1 second)
    snake_vs_gameover(scene);                       // check for gameover...
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

    // test if Nathair recently was hurt...
    // ...she should be immune to injury while "flashing" after being recently hurt...
    if(s.flash_frames <= 0)
    {
        // test Nathair for collision with herself...
        // for each chunk of Nathair...
        for(int i=0; i<s.num_chunks; i++)
        {
            // get shorthand for this chunk
            chunk &ch = s.chunks[i];

            // check if Nathair's head is inside this chunk...
            if(point_in_rect_ish(s.pos.x, s.pos.y, ch.pos.x, ch.pos.y, 32, 32))
            {  snake_ouch(scene);  }    // you're in trouble now lassie!
        }
    }
}


// func Nathair vs Saint Patrick
void snake_vs_saint_patrick(game &scene)
{
    // do not run this code if Saint Patrick is not in this scene...
    if(!scene.saint_pat.present)
    {  return;  }

    // get shorthand for Nathair and Saint Patrick
    snake &s = scene.nathair;
    saint_patrick &pat = scene.saint_pat;

    // test if Nathair recently was hurt...
    // ...she should be immune to injury while "flashing" after being recently hurt...
    if(s.flash_frames <= 0)
    {
        // check to see if Nathair has collided with Saint Patrick...
        if(point_in_rect_ish(s.pos.x, s.pos.y, pat.pos.x, pat.pos.y, 64, 64))
        {  snake_ouch(scene);  }  // you're in trouble now lassie!
    }
}


// func Nathair vs quartz boulders
void snake_vs_quartz(game &scene)
{
    // do not run this code if the scene does not have quartz...
    if(!scene.has_quartz)
    {  return;  }

    // get shorthand for Nathair
    snake &s = scene.nathair;

    // test if Nathair recently was hurt...
    // ...she should be immune to injury while "flashing" after being recently hurt...
    if(s.flash_frames <= 0)
    {
        if(point_in_quartz(scene, s.pos.x, s.pos.y))
        {  snake_ouch(scene);  }   // you're in trouble now lassie!
    }
}


// func Nathair vs everything collision
void snake_vs_collision(game &scene)
{
    snake_vs_self(scene);           // check Nathair vs herself
    snake_vs_shamrocks(scene);      // check Nathair vs shamrocks
    snake_vs_saint_patrick(scene);  // check Nathair vs Saint Patrick
    snake_vs_quartz(scene);         // check Nathair vs quartz boulders
}


// func update game state step
void update(game &scene)
{
    process_events();           // process windows events
    snake &s = scene.nathair;   // get shorthand for Nathair

    // check if game over has occurred...
    if(scene.gameover)
    {
        // if user presses key to play again...
        if(input_play_toggle())
        {
            scene = game_new(scene.assets);
            return;
        }
    }

    // check if player presses pause game key... (and player has not lost)
    if(input_play_toggle() && !scene.gameover)
    {
        scene.halt  = !scene.halt;      // flip halted state
        scene.pause = scene.halt;       // set pause state AS halted state
        scene.intro = false;            // skip intro and start game 
    }

    // check to see if Saint Patrick should be present this level...
    if(scene.stage > eLEVEL_MURRISK)
    {  scene.saint_pat.present = true;  }
    else
    {  scene.saint_pat.present = false;  }

    // check to see if quartz boulders should be present this level...
    if(scene.stage > eLEVEL_REEKBASE)
    {  scene.has_quartz = true;  }
    else
    {  scene.has_quartz = false;  }

    // if game is halted...
    if(scene.halt)
    {  set_music_volume(.25);  }    // reduce voume of the music when game is halted

    // if game is unhalted...
    if(!scene.halt)
    {
        tunes_loop("mus_nathair_lark",1);   // loop the game music at full volume
        saint_patrick_update(scene);        // update Saint Patrick...

        // if Nathair's flash frames are more than 0...
        if(s.flash_frames > 0)
        {  s.flash_frames -= 1;  }          // reduce Nathair's flash frames by 1  

        // check if shamrocks goal reached...
        if(s.num_eaten >= scene.stages[scene.stage].shamrock_goal)
        {
            // Nathair goes to next level... (display the new level intro)
            scene.halt = true;                      // halt the game
            scene.intro = true;                     // set intro mode as true
            scene.nathair = snake_create(scene);    // overwrite Nathair with a new Nathair...

            saint_patrick_reposition(scene);        // reposition Saint Patrick...
            quartz_reposition(scene);               // reposition quartz rocks...

            // set the game's level as the next level based on the current level...
            switch(scene.stage)
            {
                case eLEVEL_MURRISK:   scene.stage = eLEVEL_REEKBASE;  break;   // goto "The Base of The Reek"
                case eLEVEL_REEKBASE:  scene.stage = eLEVEL_SHOULDER;  break;   // goto "The Shoulder of The Reek"
                case eLEVEL_SHOULDER:  scene.stage = eLEVEL_SUMMIT;    break;   // goto "The Summit of Crough Patrick"
                case eLEVEL_SUMMIT:    scene.stage = eLEVEL_FINAL;     break;   // goto final level
                case eLEVEL_FINAL:     scene.stage = eLEVEL_FINAL;     break;   // goto final level
            }
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
            snake_move(s);                  // move Nathair from keyboard input
            snake_vs_collision(scene);      // check Nathair's collision events
            scene.clock = 0;                // reset timer ticks
        }
    }
}


// func render centred text to screen
void render_text_centred(string text, color tcol, font &tfont, int fsize, int x, int y)
{
    int twidth = text_width(text, tfont, fsize);            // get the width of the text
    draw_text(text, tcol, tfont, fsize, x -(twidth/2), y);  // draw the text at the position, offset x by half the text width
}


// func render ground tiles
void render_ground_tiles(game &scene)
{
    // for each tile width in scene...
    for(int x=0; x<cSCENE_W; x+=128)
    {
        // for each tile height in scene...
        for(int y=0; y<cSCENE_H; y+=128)
        {  draw_bitmap(scene.assets.bmp_sward, x, y);  }  // render a tile of sward
    }
}


// func render quartz rocks
void render_quartz(game &scene)
{
    // do not run this code if quartz is not present in this scene
    if(!scene.has_quartz)
    {  return;  }

    // for all quartz rocks...
    for(int i=0; i<cMAX_QUARTZ; i++)
    {
        // get shorthand for this rock...
        quartz &rock = scene.rocks[i]; 

        draw_bitmap(scene.assets.bmp_quartz, rock.pos.x, rock.pos.y);  // render this quartz rock
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

    // if Nathair is flashing, only render her for most? of her frames
    // otherwise, if she isn't flashing, just render her
    if(s.flash_frames % cFRAME_FLASH_HIDE != 0  ||  s.flash_frames == 0)
    {
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
}


// func render Saint Patrick's movement warning zone
void render_saint_patrick_warnzone(game &scene)
{
    // get shorthand references
    resources &res = scene.assets;          // shorthand for resource assets
    saint_patrick &pat = scene.saint_pat;   // shorthand for Saint Patrick
    double &x = pat.pos.x;                  // shorthand for Saint Patrick's x coordinate
    double &y = pat.pos.y;                  // shorthand for Saint Patrick's y coordinate
    color &colour = res.rgba_warning_zone;  // shorthand for Saint Patrick's warning zone colour

    // render Saint Patrick's "collision zone"
    fill_rectangle(res.rgba_collision_zone, x, y, 64, 64);      

    // define length of Saint Pat's movement warning zone 
    int wlen = cCELL_SIZE*6;

    // render Saint Patrick differently depending on which way he's facing
    switch(pat.dir)
    {   
        case eDIR_RIGHT:  fill_rectangle(colour, x+64, y, wlen, 64);    break;   // Saint Pat facing right
        case eDIR_UP:     fill_rectangle(colour, x, y-wlen, 64, wlen);  break;   // Saint Pat head facing up
        case eDIR_LEFT:   fill_rectangle(colour, x-wlen, y, wlen, 64);  break;   // Saint Pat head facing left
        case eDIR_DOWN:   fill_rectangle(colour, x, y +64, 64, wlen);   break;   // Saint Pat head facing down
    }
}


// func render saint patrick
void render_saint_patrick(game &scene)
{
    // do not run this code if Saint Patrick is not present in the scene...
    if(!scene.saint_pat.present)
    {  return;  }

    // getshorthand for Saint Patrick and assets
    resources &res = scene.assets;                  // shorthand for resource assets
    saint_patrick &pat = scene.saint_pat;           // shorthand for Saint Patrick
    
    // draw Saint Patrick's warning zone area
    render_saint_patrick_warnzone(scene);

    // determine Saint Pat's image... set pointer to ref of the image
    switch(pat.dir)
    {   
        case eDIR_RIGHT:  pat.sprite = &res.bmp_saint_patrick_right;  break;   // Saint Pat facing right
        case eDIR_UP:     pat.sprite = &res.bmp_saint_patrick_up;     break;   // Saint Pat head facing up
        case eDIR_LEFT:   pat.sprite = &res.bmp_saint_patrick_left;   break;   // Saint Pat head facing left
        case eDIR_DOWN:   pat.sprite = &res.bmp_saint_patrick_down;   break;   // Saint Pat head facing down
    }

    // draw Saint Pat (deref the sprite pointer to use Saint Pat's image)
    draw_bitmap(*pat.sprite, pat.pos.x, pat.pos.y);
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
        {  draw_bitmap(scene.assets.bmp_shamrock, clover.pos.x, clover.pos.y);  }  // draw this shamrock
    }    
}


// func render heads up display
void render_hud(game &scene)
{
    // get shorthand for assets and Nathair
    resources &res = scene.assets;
    snake &s = scene.nathair;

    // draw a black rectangle for a user-interface strip / heads up display
    fill_rectangle(res.rgba_blue_dark, 0, cSCENE_H, cSCREEN_W, 64);

    // only render Nathair's lives on her visible flash frames...
    // ...or if she has no flash frames...
    if(s.flash_frames % cFRAME_FLASH_HIDE != 0  ||  s.flash_frames == 0)
    {
        // for each of Nathair's lives...
        for(int i=0; i<s.lives; i++)
        {
            // draw icon for this life of Nathair
            draw_bitmap(res.bmp_nathair_down, 8 +(i*36), cSCENE_H +12);
        }
    }

    // draw icon for shamrocks
    draw_bitmap(res.bmp_shamrock, 125, cSCENE_H +12);

    // draw text for shamrock score:
    draw_text(
        "Shamrocks: "+to_string(s.num_eaten) +" / " +to_string(scene.stages[scene.stage].shamrock_goal), 
        res.rgba_gold_bright, res.fnt_berenika, 24, 165, cSCENE_H +12);
}


// func render pause menu
void render_menu_pause(game &scene)
{
    // get shorthand for assets struct
    resources &res = scene.assets;

    // draw pause menu title to the screen
    render_text_centred("*  Game Paused  *", res.rgba_gold_bright, res.fnt_berenika, 48, cSCREEN_W_HALF, 100);

    // draw pause menu control hints text to the screen
    render_text_centred("Press [Space / Escape / P] to play", res.rgba_gold_bright, res.fnt_berenika, 24, cSCREEN_W_HALF, 175);
}


// func render game over menu
void render_menu_gameover(game &scene)
{
    // get shorthand for assets struct
    resources &res = scene.assets;

    // draw game over title to the screen
    render_text_centred("*  Game Over!  *", res.rgba_ui_text_gameover, res.fnt_berenika, 48, cSCREEN_W_HALF, 100);

    // draw game over menu flavour text to the screen
    render_text_centred("Nathair got tangled and banished by Saint Patrick!", res.rgba_ui_text_gameover, res.fnt_berenika, 24, cSCREEN_W_HALF, 175);

    // draw game over menu control hints text to the screen
    render_text_centred("Press [Space / Escape / P] to play again", res.rgba_ui_text_gameover, res.fnt_berenika, 24, cSCREEN_W_HALF, 225);
}


// func render intro for current game level
void render_intro(game &scene)
{
    resources &res = scene.assets;                  // get shorthand for assets struct
    level &lvl = scene.stages[scene.stage];         // get shorthand for the current level
    font &fnt = res.fnt_berenika;                   // get shorthand for font
    int x = cSCREEN_W_HALF;                         // use half screen width as x coordinate

    // draw level intro title to the screen 
    render_text_centred(lvl.title, res.rgba_gold_bright, fnt, 32, x, 50);

    // for each level intro description...
    for(int i=0; i<3; i++)
    {  render_text_centred(lvl.desc[i], res.rgba_gold, fnt, 24, x, 100 +(i*50));  }                                             // draw level intro description to the screen

    render_text_centred("Collect "+to_string(lvl.shamrock_goal) +" shamrocks to advance", res.rgba_gold, fnt, 24, x, 370);      // draw level intro goal to the screen
    render_text_centred(cSTRING_PLAY, res.rgba_gold_bright, fnt, 24, x, 420);                                                   // draw intro control hints text to the screen
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
    clear_screen(scene.assets.rgba_forest);     // clear screen with background colour

    render_ground_tiles(scene);                 // render sward tiles
    render_shamrocks(scene);                    // render all shamrocks to screen
    render_quartz(scene);                       // render quartz rocks...
    render_nathair(scene);                      // render Nathair to the scene
    render_saint_patrick(scene);                // render Saint Patrick to the scene
    
    // check if game is halted...
    if(scene.halt)
    {
        // draw the halted game overlay...
        render_halt_overlay(scene);

        // render a screen with text based on the game's state...
        if(scene.intro)     {render_intro(scene);}          // render intro for current level
        if(scene.pause)     {render_menu_pause(scene);}     // draw pause menu text
        if(scene.gameover)  {render_menu_gameover(scene);}  // draw game over text       
    }

    render_hud(scene);                              // render the heads up display
    refresh_screen(cFRAME_RATE);                    // refresh the screen at the game's frame rate
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