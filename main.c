/*
    James William Fletcher ( github.com/mrbid )
        May 2024

    maybe reality is a cinema of little electrons projecting
    their most novel shared idea of reality into the void
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define uint GLuint
#define sint GLint

#ifdef WEB
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#endif

#define GLAD_GL_IMPLEMENTATION
#include "inc/gl.h"
#define GLFW_INCLUDE_NONE
#include "inc/glfw3.h"
#define fTime() (float)glfwGetTime()

//#define GL_DEBUG
#define MAX_MODELS 7 // hard limit, be aware and increase if needed
#include "inc/esAux7.h"
#include "inc/matvec.h"

#include "inc/res.h"
#include "assets/high/sky.h"    //0
#include "assets/high/water.h"  //1
#include "assets/high/boat.h"   //2
#include "assets/high/tux.h"    //3
#include "assets/high/rod.h"    //4
#include "assets/high/float.h"  //5
#include "assets/high/splash.h" //6


//*************************************
// globals
//*************************************
const char appTitle[]="Tux Fishing";
GLFWwindow* window;
uint winw=1024, winh=768;
float t=0.f, dt=0.f, lt=0.f, fc=0.f, lfct=0.f, aspect;

// render state
mat projection, view, model, modelview;

// camera vars
uint focus_cursor = 1;
double sens = 0.003f;
double mx,my,lx,ly;
float xrot = d2PI;
float yrot = 1.3f;
float zoom = -3.3f;

// game vars
#define FAR_DISTANCE 16.f
uint ks[2];
float woff = 0.f;
float pr = 0.f;

//*************************************
// game functions
//*************************************
void resetGame(uint mode)
{
    //

    glfwSetTime(0.0);
}

//*************************************
// utility functions
//*************************************
void timestamp(char* ts)
{
    const time_t tt = time(0);
    strftime(ts, 16, "%H:%M:%S", localtime(&tt));
}
void updateModelView()
{
    mMul(&modelview, &model, &view);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
}

//*************************************
// update & render
//*************************************
void main_loop()
{
//*************************************
// core logic
//*************************************
    fc++;
    glfwPollEvents();
    t = fTime();
    dt = t-lt;
    lt = t;

#ifdef WEB
    EmscriptenPointerlockChangeEvent e;
    if(emscripten_get_pointerlock_status(&e) == EMSCRIPTEN_RESULT_SUCCESS)
    {
        if(focus_cursor == 0 && e.isActive == 1)
        {
            glfwGetCursorPos(window, &lx, &ly);
        }
        focus_cursor = e.isActive;
    }
#endif

//*************************************
// game logic
//*************************************

    // inputs
    if(ks[0] == 1){pr -= 1.f*dt;}
    if(ks[1] == 1){pr += 1.f*dt;}

    // water offset
    woff = sinf(t*0.42f);

    // camera
    if(focus_cursor == 1)
    {
        glfwGetCursorPos(window, &mx, &my);

        xrot += (float)((lx-mx)*sens);
        yrot += (float)((ly-my)*sens);

        if(yrot > 1.5f){yrot = 1.5f;}
        if(yrot < 0.5f){yrot = 0.5f;}

        lx = mx, ly = my;
    }

    mIdent(&view);
    mSetPos(&view, (vec){0.f, -0.13f, zoom});
    mRotate(&view, yrot, 1.f, 0.f, 0.f);
    mRotate(&view, xrot, 0.f, 0.f, 1.f);

//*************************************
// render
//*************************************

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ///

    // render sky
    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &lightness_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
    glUniform1f(lightness_id, 1.f);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&view.m[0][0]);
    esBindRenderF(0);
    
    // render water
    mIdent(&model);
    mSetPos(&model, (vec){0.f, 0.f, 0.f});
    mScale(&model, 1.f, 1.f, woff);
    updateModelView();
    esBindRenderF(1);

    // shade lambert
    shadeLambert(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &ambient_id, &saturate_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
    glUniform1f(ambient_id, 0.4f);
    glUniform1f(saturate_id, 0.5f);

    // render boat
    mIdent(&model);
    mSetPos(&model, (vec){0.f, 0.f, woff*-0.026f});
    updateModelView();
    esBindRender(2);

    // render tux
    mIdent(&model);
    mSetPos(&model, (vec){0.f, 0.f, woff*-0.026f});
    mRotZ(&model, pr);
    updateModelView();
    esBindRender(3);

    // render rod
    mIdent(&model);
    mSetPos(&model, (vec){0.f, 0.f, 0.125378f+(woff*-0.026f)});
    mRotZ(&model, pr);
    updateModelView();
    esBindRender(4);

    ///

    // display render
    glfwSwapBuffers(window);
}

//*************************************
// input
//*************************************
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        if(     key == GLFW_KEY_LEFT  || key == GLFW_KEY_A){ks[0]=1;}
        else if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_D){ks[1]=1;}
        else if(key == GLFW_KEY_F) // show average fps
        {
            if(t-lfct > 2.0)
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] FPS: %g\n", strts, fc/(t-lfct));
                lfct = t;
                fc = 0;
            }
        }
        else if(key == GLFW_KEY_R) // reset game
        {
            resetGame(0);
        }
        else if(key == GLFW_KEY_ESCAPE)
        {
            focus_cursor = 0;
#ifndef WEB
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwGetCursorPos(window, &lx, &ly);
#endif
        }
    }
    else if(action == GLFW_RELEASE)
    {
        if(     key == GLFW_KEY_LEFT  || key == GLFW_KEY_A){ks[0]=0;}
        else if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_D){ks[1]=0;}
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(yoffset < 0.0){zoom += 0.06f * zoom;}else{zoom -= 0.06f * zoom;}
    if(zoom > -0.73f){zoom = -0.73f;}else if(zoom < -5.f){zoom = -5.f;}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(focus_cursor == 0)
        {
            focus_cursor = 1;
#ifndef WEB
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(window, &lx, &ly);
#endif
        }
        else
        {
            focus_cursor = 0;
#ifndef WEB
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwGetCursorPos(window, &lx, &ly);
#endif
        }
    }
}
void window_size_callback(GLFWwindow* window, int width, int height)
{
    winw = width, winh = height;
    glViewport(0, 0, winw, winh);
    aspect = (float)winw / (float)winh;
    mIdent(&projection);
    mPerspective(&projection, 30.0f, aspect, 0.01f, FAR_DISTANCE);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
}
#ifdef WEB
EM_BOOL emscripten_resize_event(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
{
    winw = uiEvent->documentBodyClientWidth;
    winh = uiEvent->documentBodyClientHeight;
    window_size_callback(window, winw, winh);
    emscripten_set_canvas_element_size("canvas", winw, winh);
    return EM_FALSE;
}
#endif

//*************************************
// process entry point
//*************************************
int main(int argc, char** argv)
{
#ifdef WEB
    focus_cursor = 0;
#endif

    // allow custom msaa level
    int msaa = 16;
    if(argc >= 2){msaa = atoi(argv[1]);}

    // help
    printf("----\n");
    printf("James William Fletcher (github.com/mrbid)\n");
    printf("%s - 3D Fishing Game, with 55 species of fish!\n", appTitle);
    printf("----\n");
#ifndef WEB
    printf("One command line argument, msaa 0-16.\n");
    printf("e.g; ./tuxfishing 16\n");
    printf("----\n");
#endif
    printf("AD/LR = Rotate\n"); // to be continued...
    printf("F = FPS to console.\n");
    printf("R = Reset game.\n");
    printf("----\n");
    printf("All assets where generated using LUMA GENIE (https://lumalabs.ai/genie).\n");
    printf("----\n");
    printf("Tux made by Andy Cuccaro\n");
    printf("https://andycuccaro.gumroad.com/\n");
    printf("----\n");
    printf("Fishing Rod made by Shedmon\n");
    printf("https://sketchfab.com/shedmon\n");
    printf("----\n");
    printf("%s\n", glfwGetVersionString());
    printf("----\n");

    // init glfw
    if(!glfwInit()){printf("glfwInit() failed.\n"); exit(EXIT_FAILURE);}
#ifdef WEB
    double width, height;
    emscripten_get_element_css_size("body", &width, &height);
    winw = (uint)width, winh = (uint)height;
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, msaa);
    window = glfwCreateWindow(winw, winh, appTitle, NULL, NULL);
    if(!window)
    {
        printf("glfwCreateWindow() failed.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());
#ifndef WEB
    glfwSetWindowPos(window, (desktop->width/2)-(winw/2), (desktop->height/2)-(winh/2)); // center window on desktop
    glfwGetCursorPos(window, &lx, &ly);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync

    // set icon
    glfwSetWindowIcon(window, 1, &(GLFWimage){16, 16, (unsigned char*)icon_image});

//*************************************
// bind vertex and index buffers
//*************************************
    register_sky();
    register_water();
    register_boat();
    register_tux();
    register_rod();
    register_float();
    register_splash();

//*************************************
// configure render options
//*************************************
    makeLambert();
    makeFullbright();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.f, 0.f, 0.f, 0.f);

    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &lightness_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
    window_size_callback(window, winw, winh);

#ifdef GL_DEBUG
    esDebug(1);
#endif

//*************************************
// execute update / render loop
//*************************************

    // init
    srand(time(0));
    srandf(time(0));
    t = fTime();
    lfct = t;

    // game init
    resetGame(0);

    // loop
#ifdef WEB
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_FALSE, emscripten_resize_event);
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while(!glfwWindowShouldClose(window)){main_loop();}
#endif

    // done
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
