//
//  ex1b.cpp
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
//

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "Model.h"
#include "ShaderIO.h"

/** Internal Definitions */

#define	WINDOW_SIZE         (600) // initial size of the window               //
#define	WINDOW_POS_X        (100) // initial X position of the window         //
#define	WINDOW_POS_Y        (100) // initial Y position of the window         //

#define RC_OK                 (0) // Everything went ok                       //
#define RC_INVALID_ARGUMENTS  (1) // Invalid arguments given to the program   //
#define RC_INPUT_ERROR        (2) // Invalid input to the program             //

#define	ARGUMENTS_PROGRAM     (0) // program name position on argv            //
#define	ARGUMENTS_INPUTFILE   (1) // given input file position on argv        //
#define	ARGUMENTS_REQUIRED    (2) // number of required arguments             //

/** Key definitions */

#define KEY_ANIMATE         ('a') // Key used to start animation              //
#define KEY_ESC            ('\e') // Key used to terminate the program - ESC  //
#define KEY_QUIT            ('q') // Key used to terminate the program        //
#define KEY_RESET           ('r') // Key used to reset the applied TX's	      //
#define KEY_RELOAD          ('l') // Key used to reload the shaders 	      //
#define KEY_WIREFRAME       ('w') // Key used to toggle wireframe rendering   //
#define KEY_PERSPECTIVE     ('p') // Key used to toggle perspective           //
#define KEY_NORMAL          ('n') // Key used to toggle normal mode           //
#define KEY_PHONG           ('1') // Key used to set lighting to phong        //
#define KEY_GOURAUD         ('2') // Key used to set lighting to gouraud      //
#define KEY_COLORFUL        ('3') // Key used to set lighting to colorful     //
#define KEY_MORE_SHININESS  ('=') // Key used to increase shininess           //
#define KEY_LESS_SHININESS  ('-') // Key used to decrease shininess           //
#define KEY_LESS_TEX_SCALE  ('s') // Key used to decrease texture scale       //
#define KEY_MORE_TEX_SCALE  ('d') // Key used to increase texture scale       //
#define KEY_LESS_TURB       ('f') // Key used to decrease turbulance magnitude//
#define KEY_MORE_TURB       ('g') // Key used to increase turbulance magnitude//
#define KEY_TOGGLE_TEX      ('t') // Key used to toggle tex mode              //

/** display callback */
void display(void);

/** window reshape callback  */
void windowResize(int width, int height);

/** keyboard callback  */
void keyboard(unsigned char key, int x, int y);

/** mouse click callback */
void mouse(int button, int state, int x, int y) ;

/** mouse dragging callback  */
void motion(int x, int y) ;

/** timer callback */
void timer(int value) ;

/** Global variables */

int     g_nFPS = 0, g_nFrames = 0;              // FPS and FPS Counter
int     g_dwLastFPS = 0;                        // Last FPS Check Time
bool    g_startAnimation = true;
bool    g_duringAnimation = false;

// A global variable for our model (a better practice would be to use a singletone that holds all model):
Model _model;

/** main function */
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <mesh filename>" << std::endl;
		exit(1);
	}
	std::string mesh_filename = argv[1];

	std::cout << "Starting ex4..." << std::endl;
	
	// Initialize GLUT
    glutInit(&argc, argv) ;
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE) ;
#elif defined WIN32
	glutInitContextVersion(3, 1);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif

    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
    glutCreateWindow("CG Ex4");
	
	// Initialize GLEW
    glewExperimental = GL_TRUE;
    int glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
        exit(1);
    }
	
#ifdef __APPLE__
    GLint sync = 1;
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &sync);
#endif
		
	// Set callback functions:
    glutDisplayFunc(display) ;
    glutReshapeFunc(windowResize) ;
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(100, timer, 0);   // uint millis int value
	


	// Init anything that can be done once and for all:
	if (! _model.init(mesh_filename))
	{
		std::cerr << "Failed to initialize model" << std::endl;
		return 1;
	}

	// Set clear color to black:
	glClearColor(0.0, 0.0, 0.0, 0.0);
		
	// Start events/drawing loop
	glutMainLoop();
	
	return 0;
}

void display(void)
{
	// Clear the screen buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Let the model to draw itself...
	_model.draw();
	
	// Swap those buffers so someone will actually see the results... //
    glutSwapBuffers();
}

// This method is called when the window is resized
void windowResize(int w, int h)
{
    // Update model to fit the new resolution
	_model.resize(w, h);
    
    // set the new viewport //
    glViewport(0, 0, w, h);
    
    // Refresh the display //
    glutPostRedisplay();
}

/********************************************************************
 * Function  :	keyboard
 * Arguments :	key : the key that was pressed
 *              x   : x value of the current mouse location
 *              y   : y value of the current mouse location
 * Returns   : n/a
 * Throws    : n/a
 *
 * Purpose   : This function handles all the keyboard input from the user.
 *             It supports terminating the application when the KEY_QUIT is pressed.
 *
 \******************************************************************/
void keyboard(unsigned char key, int x, int y)
{
    unsigned int lower_key = tolower(key);
	
    switch(lower_key)
    {
        case KEY_RESET:
            // reset to initial view of the object
            // For use in a future exercise
        	_model.reset();
            break;
        case KEY_RELOAD:
            // Reload the shading programs of the object
            // For use in a future exercise
            break;
        case KEY_WIREFRAME:
            // Toggle wireframe mode
            // For use in a future exercise
        	_model.switchPolygonMode();
            break;
        case KEY_ANIMATE:
            if (!g_duringAnimation) {
                g_startAnimation = true;
            }
            break;
        case KEY_PERSPECTIVE:
        	_model.switchPerspective();
        	break;
        case KEY_NORMAL:
        	_model.switchNormalMode();
        	break;
        case KEY_PHONG:
        	_model.setLightingMode(Model::PHONG);
        	break;
        case KEY_GOURAUD:
        	_model.setLightingMode(Model::GOURAUD);
        	break;
        case KEY_COLORFUL:
        	_model.setLightingMode(Model::COLORFUL);
        	break;
        case KEY_MORE_SHININESS:
        	_model.addShininess(1);
        	break;
        case KEY_LESS_SHININESS:
			_model.addShininess(-1);
			break;
		case KEY_MORE_TEX_SCALE:
			_model.addTexScale(1);
			break;
		case KEY_LESS_TEX_SCALE:
			_model.addTexScale(-1);
			break;
		case KEY_MORE_TURB:
			_model.addTurbCoeff(1);
			break;
		case KEY_LESS_TURB:
			_model.addTurbCoeff(-1);
			break;
		case KEY_TOGGLE_TEX:
			_model.toggleTexMode();
			break;
        case KEY_QUIT:
        case KEY_ESC:
            // Terminate the program:
            exit(RC_OK);
            break;
        default:
            std::cerr << "Key " << lower_key << " undefined\n";
            break;
    }
    
    return;
}

/********************************************************************
 * Function  :   mouse
 * Arguments :   button  : the button that was engaged in some action
 *               state   : the new state of that button
 *               x       : x value of the current mouse location
 *               y       : y value of the current mouse location
 * Returns   :   n/a
 * Throws    :   n/a
 *
 * Purpose   :   This function handles mouse actions.
 *
 \******************************************************************/
void mouse(int button, int state, int x, int y)
{
    _model.mouse_click(button, state == GLUT_DOWN, x, y);

    return;
}


/********************************************************************
 * Function  :   motion
 * Arguments :   x   : x value of the current mouse location
 *               y   : y value of the current mouse location
 * Returns   :   n/a
 * Throws    :   n/a
 *
 * Purpose   :   This function handles mouse dragging events.
 *
 \******************************************************************/
void motion(int x, int y)
{
	_model.mouse_move(x, y);
    return;
}

static const float animationDurationInFrames = 300;

void timer(int value) {
    /* Set the timer to be called again in X milli - seconds. */
    if (g_startAnimation)
	{
        value = 0;
        g_duringAnimation = true;
        g_startAnimation = false;
    }
    
    glutTimerFunc(25, timer, ++value);   // uint millis int value
    glutPostRedisplay();
    
    if (g_duringAnimation) {
        float t = (float)value / (float)animationDurationInFrames;
        if (t > 1) {
            g_duringAnimation = false;
            return;
        }
        
        //glutPostRedisplay();
    }
}
