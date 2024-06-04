/*
    James William Fletcher ( github.com/mrbid )
        June 2024
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
#define MAX_MODELS 60 // hard limit, be aware and increase if needed
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

#include "assets/high/fish/a0.h"     //7
#include "assets/high/fish/a1.h"     //8
#include "assets/high/fish/a2.h"     //9
#include "assets/high/fish/a3.h"     //10
#include "assets/high/fish/a4.h"     //11
#include "assets/high/fish/a5.h"     //12
#include "assets/high/fish/a6.h"     //13
#include "assets/high/fish/a7.h"     //14
#include "assets/high/fish/a8.h"     //15
#include "assets/high/fish/a9.h"     //16
#include "assets/high/fish/a10.h"    //17
#include "assets/high/fish/a11.h"    //18
#include "assets/high/fish/a12.h"    //19
#include "assets/high/fish/a13.h"    //20
#include "assets/high/fish/a14.h"    //21

#include "assets/high/fish/b0.h"     //22
#include "assets/high/fish/b1.h"     //23
#include "assets/high/fish/b2.h"     //24
#include "assets/high/fish/b3.h"     //25
#include "assets/high/fish/b4.h"     //26
#include "assets/high/fish/b5.h"     //27
#include "assets/high/fish/b6.h"     //28
#include "assets/high/fish/b7.h"     //29
#include "assets/high/fish/b8.h"     //30
#include "assets/high/fish/b9.h"     //31
#include "assets/high/fish/b10.h"    //32
#include "assets/high/fish/b11.h"    //33
#include "assets/high/fish/b12.h"    //34

#include "assets/high/fish/c0.h"     //35
#include "assets/high/fish/c1.h"     //36
#include "assets/high/fish/c2.h"     //37
#include "assets/high/fish/c3.h"     //38
#include "assets/high/fish/c4.h"     //39
#include "assets/high/fish/c5.h"     //40
#include "assets/high/fish/c6.h"     //41
#include "assets/high/fish/c7.h"     //42
#include "assets/high/fish/c8.h"     //43
#include "assets/high/fish/c9.h"     //44
#include "assets/high/fish/c10.h"    //45
#include "assets/high/fish/c11.h"    //46

#include "assets/high/fish/d0.h"     //47
#include "assets/high/fish/d1.h"     //48
#include "assets/high/fish/d2.h"     //49
#include "assets/high/fish/d3.h"     //50
#include "assets/high/fish/d4.h"     //51
#include "assets/high/fish/d5.h"     //52
#include "assets/high/fish/d6.h"     //53
#include "assets/high/fish/d7.h"     //54
#include "assets/high/fish/d8.h"     //55
#include "assets/high/fish/d9.h"     //56
#include "assets/high/fish/d10.h"    //57
#include "assets/high/fish/d11.h"    //58

#include "assets/high/fish/e1.h"     //59


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
uint ks[2]; // is rotate key pressed toggle
uint cast = 0; // is casting toggle
uint caught = 0; // total fish caught
float woff = 0.f; // wave offset
float pr = 0.f; // player rotation (yaw)
float rodr = 0.f; // fishing rod rotation (pitch)
vec fp = (vec){0.f, 0.f, 0.f}; // float position

float frx=0.f, fry=0.f, frr=0.f; // float return direction
int hooked = -1; // is a fish hooked, if so, its the ID of the fish.
float next_wild_fish = 0.f; // time for next wild fish discovery
int last_fish[2]={0};
uint lfi=0;
float winning_fish = 0.f;
uint winning_fish_id = 0;

float shoal_x[3]; // position of shoal
float shoal_y[3]; // position of shoal
uint shoal_lfi[3];// last fish id that jumped
float shoal_nt[3];// next shoal jump time
float shoal_r1[3];// jump rots
float shoal_r2[3];
float shoal_r3[3];

float caught_list[53]={0};


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
// game functions
//*************************************
uint ratioCaught()
{
    uint r = 0;
    for(uint i=0; i<53; i++){if(caught_list[i] == 1){r++;}}
    return r;
}
void rndShoalPos(uint i)
{
    const float ra = esRandFloat(-PI, PI);
    const float rr = esRandFloat(2.3f, 3.6f);
    shoal_x[i] = sinf(ra)*rr;
    shoal_y[i] = cosf(ra)*rr;
    shoal_lfi[i] = (int)roundf(esRandFloat(7.f, 59.f));
    shoal_nt[i] = t + esRandFloat(6.5f, 16.f);
}
void resetGame(uint mode)
{
    cast=0;
    pr=0.f;
    rodr=0.f;
    fp=(vec){0.f, 0.f, 0.f};
    frx=0.f;
    fry=0.f;
    frr=0.f;
    hooked=-1;
    last_fish[0]=-1;
    last_fish[1]=-1;
    lfi=0;
    winning_fish=0.f;
    winning_fish_id=0;
    next_wild_fish=t+esRandFloat(23.f,180.f);
    caught=0;
    //for(uint i=0; i<53; i++){caught_list[i]=0;}
    memset(&caught_list[0], 0x00, sizeof(float)*53);
    rndShoalPos(0);
    rndShoalPos(1);
    rndShoalPos(2);
    if(mode == 1)
    {
        char strts[16];
        timestamp(&strts[0]);
        printf("[%s] Game Reset.\n", strts);
    }
    glfwSetWindowTitle(window, appTitle);
}
float getWaterHeight(float x, float y)
{
    const uint imax = water_numvert*3;
    int ci = -1;
    float cid = 9999.f;
    for(uint i=0; i < imax; i+=3)
    {
        const float xm = water_vertices[i]   - x;
        const float ym = water_vertices[i+1] - y;
        const float nd = xm*xm + ym*ym;
        if(nd < cid)
        {
            ci = i;
            cid = nd;
        }
    }
    if(ci != -1)
    {
        return water_vertices[ci+2];
    }
    return woff;
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
    if(hooked == -1)
    {
        if(ks[0] == 1){pr -= 1.6f*dt;fp=(vec){0.f, 0.f, 0.f};}
        if(ks[1] == 1){pr += 1.6f*dt;fp=(vec){0.f, 0.f, 0.f};}
        if(cast == 1)
        {
            if(rodr < 2.f){rodr += 1.5f*dt;}
            const float trodr = (rodr+0.23f)*1.65f;
            frx = sinf(pr+d2PI), fry = cosf(pr+d2PI), frr = -d2PI+pr;
            fp.x = frx*trodr, fp.y = fry*trodr;
            fp.z = getWaterHeight(fp.x, fp.y);
        }
        else{if(rodr > 0.f){rodr -= 9.f*dt;}}
    }

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

    // glEnable(GL_BLEND);
    // glUniform1f(opacity_id, 0.5f);
    // mIdent(&model);
    // mSetPos(&model, (vec){0.f, 0.f, 0.01f});
    // mScale(&model, 1.f, 1.f, woff);
    // updateModelView();
    // esBindRenderF(1);
    // glDisable(GL_BLEND);

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

    // render last catch(es)
    if(last_fish[0] != -1)
    {
        mIdent(&model);
        mSetPos(&model, (vec){0.f, -0.14f, 0.04f+(woff*-0.026f)});
        updateModelView();
        esBindRender(last_fish[0]);
    }
    if(last_fish[1] != -1)
    {
        mIdent(&model);
        mSetPos(&model, (vec){0.02f, 0.2f, 0.05f+(woff*-0.026f)});
        mRotZ(&model, 90.f*DEG2RAD);
        updateModelView();
        esBindRender(last_fish[1]);
    }

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
    mRotX(&model, rodr);
    updateModelView();
    esBindRender(4);

    // render float
    if(fp.x != 0.f || fp.y != 0.f || fp.z != 0.f)
    { 
        // is a fish hooked?
        if(hooked != -1)
        {
            // reel it in
            rodr = 0.8f;
            const float rs = 0.32f*dt;
            const float n1 = -fp.x*0.3f*dt;
            const float n2 = -frx*rs;
            const float o1 = -fp.y*0.3f*dt;
            const float o2 = -fry*rs;
            const float x1 = n2+o2, x2 = n1+o1;
            if(x1*x1 < x2*x2)
            {
                fp.x += n1;
                fp.y += o1;
            }
            else
            {
                fp.x += n2;
                fp.y += o2;
            }
            fp.z = getWaterHeight(fp.x, fp.y);
            // fp.x += -frx*rs;
            // fp.y += -fry*rs;
            // const float wh = getWaterHeight(fp.x, fp.y);
            //if(fabsf(wh-fp.z) > 0.03f){fp.z=wh;} // lol
            if(vMag(fp) < 0.1f)
            {
                winning_fish = t+4.f;
                winning_fish_id = hooked;
                fp = (vec){0.f, 0.f, 0.f};
                last_fish[lfi] = hooked;
                if(++lfi > 1){lfi=0;}
                caught_list[hooked-7] = 1;
                hooked = -1;
                caught++;
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] Fish Caught: %u (%u/53)\n", strts, caught, ratioCaught());
                char tmp[256];
                sprintf(tmp, "Tux 🐟 %u (%u/53) 🐟 Fishing", caught, ratioCaught());
                glfwSetWindowTitle(window, tmp);
            }
            else
            {
                // render fish
                mIdent(&model);
                mSetPos(&model, (vec){fp.x, fp.y, fp.z*woff});
                mRotZ(&model, frr);
                updateModelView();
                esBindRender(hooked);
            }
        }
        else
        {
            if(t > next_wild_fish)
            {
                const float rc = esRandFloat(0.f, 100.f);
                if(rc < 50.f)     {hooked = (int)roundf(esRandFloat( 7.f, 21.f));}
                else if(rc < 80.f){hooked = (int)roundf(esRandFloat(22.f, 34.f));}
                else if(rc < 90.f){hooked = (int)roundf(esRandFloat(35.f, 46.f));}
                else if(rc < 97.f){hooked = (int)roundf(esRandFloat(47.f, 58.f));}
                else{hooked = 59;}
                //hooked = (int)roundf(esRandFloat(7.f, 59.f));
                next_wild_fish = t + esRandFloat(23.f, 180.f);
            }

            if(cast == 1){glEnable(GL_BLEND);glUniform1f(opacity_id, 0.5f);}
            mIdent(&model);
            mSetPos(&model, (vec){fp.x, fp.y, fp.z*woff});
            updateModelView();
            esBindRender(5);
            if(cast == 1){glDisable(GL_BLEND);}
        }
    }

    // render jumping fish
    for(uint i=0; i<3; i++)
    {
        if(hooked == -1)
        {
            const float xm = fp.x - shoal_x[i];
            const float ym = fp.y - shoal_y[i];
            const float nd = xm*xm + ym*ym;
            if(nd < 0.3f)
            {
                //printf("[%u] %f %f\n", i, nd, shoal_nt[i]);
                if(shoal_nt[i]-t < -4.5f){hooked = shoal_lfi[i];}
            }
        }

        if(shoal_nt[i]-t < -11.f){rndShoalPos(i);}

        const float d = shoal_nt[i]-t;
        if(d < 0.f && d >= -1.5f)
        {
            const float z = -0.03f+(0.33f*(fabsf(d)/1.5f));
            const float wah = (getWaterHeight(shoal_x[i], shoal_y[i])*woff)-0.016f;

            mIdent(&model);
            mSetPos(&model, (vec){shoal_x[i], shoal_y[i], wah});
            mRotZ(&model, t*0.3f);
            updateModelView();
            esBindRender(6);

            mIdent(&model);
            mSetPos(&model, (vec){shoal_x[i], shoal_y[i], z});
            shoal_r1[i] += esRandFloat(0.1f, 0.6f)*dt;
            shoal_r2[i] += esRandFloat(0.1f, 0.6f)*dt;
            shoal_r3[i] += esRandFloat(0.1f, 0.6f)*dt;
            mRotX(&model, shoal_r1[i]);
            mRotY(&model, shoal_r2[i]);
            mRotZ(&model, shoal_r3[i]);
            updateModelView();
            esBindRender(shoal_lfi[i]);
        }
        else if(d > -2.5f && d < -1.5f)
        {
            const float wah = (getWaterHeight(shoal_x[i], shoal_y[i])*woff)-0.016f;

            mIdent(&model);
            mSetPos(&model, (vec){shoal_x[i], shoal_y[i], wah});
            mRotZ(&model, t*0.3f);
            updateModelView();
            esBindRender(6);

            mIdent(&model);
            mSetPos(&model, (vec){shoal_x[i], shoal_y[i], 0.3f});
            shoal_r1[i] += esRandFloat(0.1f, 0.6f)*dt;
            shoal_r2[i] += esRandFloat(0.1f, 0.6f)*dt;
            shoal_r3[i] += esRandFloat(0.1f, 0.6f)*dt;
            mRotX(&model, shoal_r1[i]);
            mRotY(&model, shoal_r2[i]);
            mRotZ(&model, shoal_r3[i]);
            updateModelView();
            esBindRender(shoal_lfi[i]);
        }
        else if(d > -5.5f && d < -2.5f)
        {
            const float z = 0.3f-(0.303f*(fabsf(d+2.5f)/1.5f));
            const float wah = (getWaterHeight(shoal_x[i], shoal_y[i])*woff)-0.016f;

            glEnable(GL_BLEND);
            glUniform1f(opacity_id, d+4.5f);
            mIdent(&model);
            mSetPos(&model, (vec){shoal_x[i], shoal_y[i], wah});
            mRotZ(&model, t*0.3f);
            updateModelView();
            esBindRender(6);
            glDisable(GL_BLEND);

            mIdent(&model);
            mSetPos(&model, (vec){shoal_x[i], shoal_y[i], z});
            shoal_r1[i] += esRandFloat(0.1f, 0.6f)*dt;
            shoal_r2[i] += esRandFloat(0.1f, 0.6f)*dt;
            shoal_r3[i] += esRandFloat(0.1f, 0.6f)*dt;
            mRotX(&model, shoal_r1[i]);
            mRotY(&model, shoal_r2[i]);
            mRotZ(&model, shoal_r3[i]);
            updateModelView();
            esBindRender(shoal_lfi[i]);
        }
    }

    // render winning fish
    if(winning_fish > t)
    {
        const float d = winning_fish - t;
        if(d < 1.f)
        {
            glEnable(GL_BLEND);
            glUniform1f(opacity_id, d);
            mIdent(&model);
            mSetPos(&model, (vec){0.f, 0.f, 0.37f});
            mScale1(&model, 3.f);
            mRotZ(&model, t*2.1f);
            updateModelView();
            esBindRender(winning_fish_id);
            glDisable(GL_BLEND);
        }
        else
        {
            mIdent(&model);
            mSetPos(&model, (vec){0.f, 0.f, 0.37f});
            mScale1(&model, 3.f);
            mRotZ(&model, t*2.1f);
            updateModelView();
            esBindRender(winning_fish_id);
        }
    }

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
        else if(key == GLFW_KEY_SPACE)                     { cast=1;
                                                             next_wild_fish = t + esRandFloat(23.f, 180.f);}
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
        // else if(key == GLFW_KEY_E)
        // {
        //     //shoal_nt[0] = t;
        //     //hooked = (int)roundf(esRandFloat(7.f, 59.f));
        // }
        // else if(key == GLFW_KEY_R) // reset game
        // {
        //     resetGame(1);
        // }
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
        else if(key == GLFW_KEY_SPACE)                     { cast=0;}
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
    printf("%s - 3D Fishing Game, with 53 species of fish!\n", appTitle);
    printf("----\n");
#ifndef WEB
    printf("One command line argument, msaa 0-16.\n");
    printf("e.g; ./tuxfishing 16\n");
    printf("----\n");
#endif
    printf("Mouse = Rotate Camera, Scroll = Zoom Camera\n");
    printf("W,A / Arrows = Move Rod Cast Direction\n");
    printf("Space = Cast Rod, the higher the rod when you release space the farther the lure launches.\n");
    printf("If you see a fish jump out of the water throw a lure after it and you will catch it straight away.\n");
    printf("F = FPS to console.\n");
    //printf("R = Reset game.\n");
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

    register_a0();
    register_a1();
    register_a2();
    register_a3();
    register_a4();
    register_a5();
    register_a6();
    register_a7();
    register_a8();
    register_a9();
    register_a10();
    register_a11();
    register_a12();
    register_a13();
    register_a14();

    register_b0();
    register_b1();
    register_b2();
    register_b3();
    register_b4();
    register_b5();
    register_b6();
    register_b7();
    register_b8();
    register_b9();
    register_b10();
    register_b11();
    register_b12();

    register_c0();
    register_c1();
    register_c2();
    register_c3();
    register_c4();
    register_c5();
    register_c6();
    register_c8();
    register_c9();
    register_c10();
    register_c11();

    register_d0();
    register_d1();
    register_d2();
    register_d3();
    register_d4();
    register_d5();
    register_d6();
    register_d7();
    register_d8();
    register_d9();
    register_d10();
    register_d11();

    register_e1();

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
