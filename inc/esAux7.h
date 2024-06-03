/*
--------------------------------------------------
    James William Fletcher (github.com/mrbid)
        January 2024 - esAux7.h v7.0
--------------------------------------------------

    A pretty good color converter: https://www.easyrgb.com/en/convert.php

    Lambertian fragment shaders make a difference, but only if you normalise the
    vertNorm in the fragment shader. Most of the time you won't notice the difference.

    This focuses on using ML generated models from LUMA GENIE, MESHY.AI, or TRIPO3D.AI
    converted to vertex colors and shaded via shadeLambert().
    
    No Textures, No Phong, One view-space light with position, ambient and saturation control.

    Default: ambient = 0.648, saturate = 0.26 or 1.0
    
    https://registry.khronos.org/OpenGL-Refpages/es1.1/xhtml/
    https://registry.khronos.org/OpenGL/specs/es/2.0/GLSL_ES_Specification_1.00.pdf
    https://www.khronos.org/files/webgl/webgl-reference-card-1_0.pdf
*/
#ifndef AUX_H
#define AUX_H

//#define VERTEX_SHADE // uncomment for vertex shaded, default is pixel shaded
//#define MAX_MODELS 32 // uncomment to enable the use of esBindModel(id) and esRenderModel() or just esBindRender(id)
//#define GL_DEBUG // allows you to use esDebug(1); to enable OpenGL errors to the console.
                    // https://gen.glad.sh/ and https://glad.dav1d.de/ might help

// render state id's ~ ( just so you don't need to define them when using shadeLambert() or similar ) ~
GLint projection_id;
GLint modelview_id;
GLint position_id;
GLint normal_id;
GLint color_id;
GLint lightpos_id;
GLint ambient_id;
GLint saturate_id;
GLint opacity_id;
GLint lightness_id;

// ESModel ✨
typedef struct
{
    GLuint vid; // Vertex Array Buffer ID
    GLuint iid;	// Index Array Buffer ID
    GLuint cid;	// Colour Array Buffer ID
    GLuint nid;	// Normal Array Buffer ID
#ifdef MAX_MODELS
    GLuint itp; // Index Type (0:ubyte, 1:ushort, 2:uint)
    GLuint ni;  // Number of Indices
#endif
} ESModel;

// utility functions
GLuint  esRand(const GLuint min, const GLuint max);
GLfloat esRandFloat(const GLfloat min, const GLfloat max);
void    esBind(const GLenum target, GLuint* buffer, const void* data, const GLsizeiptr datalen, const GLenum usage);
void    esRebind(const GLenum target, GLuint* buffer, const void* data, const GLsizeiptr datalen, const GLenum usage);

// set shader pipeline: single color for whole object
void shadeFullbrightSolid(GLint* position, GLint* projection, GLint* modelview, GLint* color, GLint* lightness, GLint* opacity);
void shadeLambertSolid(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* ambient, GLint* saturate, GLint* opacity);

// set shader pipeline: color array for whole object
void shadeFullbright(GLint* position, GLint* projection, GLint* modelview, GLint* color, GLint* lightness, GLint* opacity);
void shadeLambert(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* ambient, GLint* saturate, GLint* opacity);

// misc
GLuint debugShader(GLuint shader_program);

// make the shader programs before you use them with shadeFunctions()
void makeAllShaders(); // don't be this lazy!
void makeFullbrightSolid();
void makeLambertSolid();
void makeFullbright();
void makeLambert();

//*************************************
// UTILITY CODE
//*************************************
GLuint esRand(const GLuint min, const GLuint max){return (rand()%(max+1-min))+min;}
GLfloat esRandFloat(const GLfloat min, const GLfloat max)
{
    static GLfloat rrndmax = 1.f/(GLfloat)RAND_MAX;
    return (((GLfloat)rand()) * rrndmax) * (max-min) + min;
}
void esBind(const GLenum target, GLuint* buffer, const void* data, const GLsizeiptr datalen, const GLenum usage)
{
    glGenBuffers(1, buffer);
    glBindBuffer(target, *buffer);
    glBufferData(target, datalen, data, usage);
}
void esRebind(const GLenum target, GLuint* buffer, const void* data, const GLsizeiptr datalen, const GLenum usage)
{
    glBindBuffer(target, *buffer);
    glBufferData(target, datalen, data, usage);
}
///
#ifdef GL_DEBUG
// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDebugMessageControl.xhtml
// https://registry.khronos.org/OpenGL-Refpages/es3/html/glDebugMessageControl.xhtml
// OpenGL ES 3.2 or OpenGL 4.3 and above only.
void GLAPIENTRY MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}
void esDebug(const GLuint state)
{
    if(state == 1)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, 0);
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    }
    else
    {
        glDisable(GL_DEBUG_OUTPUT);
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
}
#endif
/// just hiding this away down here, for the system to load and index models into GPU memory.
#ifdef MAX_MODELS // Once esLoadModel() is called just esBindModel(id) and esRenderModel() or just esBindRender(id)
    ESModel esModelArray[MAX_MODELS]; // just create new "sub - index arrays" of categories that index this master array
    uint esModelArray_index = 0;      // e.g; 0-10 index of fruit 3d models A-Z by name?
    uint esBoundModel = 0;
    void esBindModel(const uint id)
    {
        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);

        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].nid);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);

        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].cid);
        glVertexAttribPointer(color_id, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        glEnableVertexAttribArray(color_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esModelArray[id].iid);
        esBoundModel = id;
    }
    void esBindModelF(const uint id) // for Fullbright
    {
        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);

        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].cid);
        glVertexAttribPointer(color_id, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        glEnableVertexAttribArray(color_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esModelArray[id].iid);
        esBoundModel = id;
    }
    void esRenderModel()
    {
        glDrawElements(GL_TRIANGLES, esModelArray[esBoundModel].ni, esModelArray[esBoundModel].itp, 0);
    }
    /// above is; bind it, draw a few instances of it. ... below is ... bind it, draw it, draw something different.
    void esBindRender(const uint id)
    {
        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);

        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].nid);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);

        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].cid);
        glVertexAttribPointer(color_id, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        glEnableVertexAttribArray(color_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esModelArray[id].iid);

        glDrawElements(GL_TRIANGLES, esModelArray[id].ni, esModelArray[id].itp, 0);
    }
    void esBindRenderF(const uint id) // for Fullbright
    {
        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);

        glBindBuffer(GL_ARRAY_BUFFER, esModelArray[id].cid);
        glVertexAttribPointer(color_id, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        glEnableVertexAttribArray(color_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esModelArray[id].iid);

        glDrawElements(GL_TRIANGLES, esModelArray[id].ni, esModelArray[id].itp, 0);
    }
#define esLoadModel(x) \
	esBind(GL_ARRAY_BUFFER, &esModelArray[esModelArray_index].vid, x##_vertices, sizeof(x##_vertices[0]) * x##_numvert * 3, GL_STATIC_DRAW); \
	esBind(GL_ARRAY_BUFFER, &esModelArray[esModelArray_index].nid, x##_normals, sizeof(x##_normals[0]) * x##_numvert * 3, GL_STATIC_DRAW); \
	esBind(GL_ARRAY_BUFFER, &esModelArray[esModelArray_index].cid, x##_colors, sizeof(x##_colors[0]) * x##_numvert * 3, GL_STATIC_DRAW); \
	esBind(GL_ELEMENT_ARRAY_BUFFER, &esModelArray[esModelArray_index].iid, x##_indices, sizeof(x##_indices[0]) * x##_numind * 3, GL_STATIC_DRAW); \
	esModelArray[esModelArray_index].itp = x##_GL_TYPE; \
	esModelArray[esModelArray_index].ni = x##_numind * 3; \
	esModelArray_index++;
#endif // I like this system. It's amost frictionless and you can index what you like off it. 
// but you need the new ptf2.c program: https://gist.github.com/mrbid/35b1d359bddd9304c1961c1bf0fcb882
// or the newer PTF and PTO programs: https://github.com/mrbid/esAux7

//*************************************
// SHADER CODE
//*************************************
const GLchar* v0 = // ShadeFullbrightSolid() ]- vertex shader
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform float lightness;\n"
    "uniform vec3 color;\n"
    "uniform float opacity;\n"
    "attribute vec4 position;\n"
    "varying vec4 fragcolor;\n"
    "void main()\n"
    "{\n"
        "fragcolor = vec4(color*lightness, opacity);\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";
const GLchar* f0 = // ShadeFullbrightSolid() ]- fragment shader
    "#version 100\n"
    "precision highp float;\n"
    "varying vec4 fragcolor;\n"
    "void main()\n"
    "{\n"
        "gl_FragColor = fragcolor;\n"
    "}\n";
///
const GLchar* v01 = // ShadeFullbright() ]- vertex shader
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform float lightness;\n"
    "attribute vec3 color;\n"
    "uniform float opacity;\n"
    "attribute vec4 position;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "void main()\n"
    "{\n"
        "vertCol = color*lightness;\n"
        "vertOpa = opacity;\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";
const GLchar* f01 = // ShadeFullbright() ]- fragment shader
    "#version 100\n"
    "precision highp float;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "void main()\n"
    "{\n"
        "gl_FragColor = vec4(vertCol, vertOpa);\n"
    "}\n";
///
#ifdef VERTEX_SHADE // on to ! the fun stuff ;)
///
// solid color + normal array ]- vertex shader
const GLchar* v1 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform vec3 color;\n"
    "uniform float ambient;\n"
    "uniform float saturation;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "varying vec4 fragcolor;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vec3 vertNorm = normalize(vec3(modelview * vec4(normal, 0.0)));\n"
        "vec3 lightDir = normalize(lightpos - (vertPos4.xyz / vertPos4.w));\n"
        "fragcolor = vec4((color*ambient) + (color * min(max(dot(lightDir, vertNorm), 0.0), saturation)), opacity);\n"
        "gl_Position = projection * vertPos4;\n"
    "}\n";
// color array + normal array ]- vertex shader
const GLchar* v2 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform float ambient;\n"
    "uniform float saturation;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "attribute vec3 color;\n"
    "varying vec4 fragcolor;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vec3 vertNorm = normalize(vec3(modelview * vec4(normal, 0.0)));\n"
        "vec3 lightDir = normalize(lightpos - (vertPos4.xyz / vertPos4.w));\n"
        "fragcolor = vec4((color*ambient) + (color * min(max(dot(lightDir, vertNorm), 0.0), saturation)), opacity);\n"
        "gl_Position = projection * vertPos4;\n"
    "}\n";
const GLchar* f1 =  // fragment shader
    "#version 100\n"
    "precision highp float;\n"
    "varying vec4 fragcolor;\n"
    "void main()\n"
    "{\n"
        "gl_FragColor = fragcolor;\n"
    "}\n";
///
#else /////////////////////////////////////// PIXEL SHADED BELOW ///////////////////////////////////////
///
// solid color + normal array ]- vertex shader
const GLchar* v1 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform vec3 color;\n"
    "uniform float ambient;\n"
    "uniform float saturate;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertAmb;\n"
    "varying float vertSat;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vertPos = vertPos4.xyz / vertPos4.w;\n"
        "vertNorm = vec3(modelview * vec4(normal, 0.0));\n"
        "vertCol = color;\n"
        "vertAmb = ambient;\n"
        "vertSat = saturate;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "gl_Position = projection * vertPos4;\n"
    "}\n";
// color array + normal array ]- vertex shader
const GLchar* v2 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform float ambient;\n"
    "uniform float saturate;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "attribute vec3 color;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertAmb;\n"
    "varying float vertSat;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vertPos = vertPos4.xyz / vertPos4.w;\n"
        "vertNorm = vec3(modelview * vec4(normal, 0.0));\n"
        "vertCol = color;\n"
        "vertAmb = ambient;\n"
        "vertSat = saturate;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "gl_Position = projection * vertPos4;\n"
    "}\n";
const GLchar* f1 =  // fragment shader
    "#version 100\n"
    "precision highp float;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertAmb;\n"
    "varying float vertSat;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec3 lightDir = normalize(vlightPos - vertPos);\n"
        "float lambertian = min(max(dot(lightDir, normalize(vertNorm)), 0.0), vertSat);\n"
        "gl_FragColor = vec4((vertCol*vertAmb) + (vertCol*lambertian), vertOpa);\n"
    "}\n";

#endif
/// <><><> ///
GLuint shdFullbrightSolid;
GLint  shdFullbrightSolid_position;
GLint  shdFullbrightSolid_projection;
GLint  shdFullbrightSolid_modelview;
GLint  shdFullbrightSolid_color;
GLint  shdFullbrightSolid_opacity;
GLint  shdFullbrightSolid_lightness;
GLuint shdFullbright;
GLint  shdFullbright_position;
GLint  shdFullbright_projection;
GLint  shdFullbright_modelview;
GLint  shdFullbright_color;
GLint  shdFullbright_opacity;
GLint  shdFullbright_lightness;
GLuint shdLambertSolid;
GLint  shdLambertSolid_position;
GLint  shdLambertSolid_projection;
GLint  shdLambertSolid_modelview;
GLint  shdLambertSolid_lightpos;
GLint  shdLambertSolid_color;
GLint  shdLambertSolid_normal;
GLint  shdLambertSolid_ambient;
GLint  shdLambertSolid_saturate;
GLint  shdLambertSolid_opacity;
GLuint shdLambert;
GLint  shdLambert_position;
GLint  shdLambert_projection;
GLint  shdLambert_modelview;
GLint  shdLambert_lightpos;
GLint  shdLambert_color;
GLint  shdLambert_normal;
GLint  shdLambert_ambient;
GLint  shdLambert_saturate;
GLint  shdLambert_opacity;
/// <><><> ///
GLuint debugShader(GLuint shader_program)
{
    GLint linked;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &linked);
    if(linked == GL_FALSE) 
    {
        GLint infoLen = 0;
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = malloc(sizeof(char) * infoLen);
            if(infoLog != NULL)
            {
                glGetProgramInfoLog(shader_program, infoLen, NULL, infoLog);
                printf("!!! error linking shader !!!\n%s\n", infoLog);            
                free(infoLog);
            }
        }
        else
        {
            printf("!!! failed to link shader with no returned debug output !!!\n");
        }
        glDeleteProgram(shader_program);
        return linked;
    }
    return linked;
}
/// <><><> ///
void makeFullbrightSolid()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v0, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f0, NULL);
    glCompileShader(fragmentShader);

    shdFullbrightSolid = glCreateProgram();
        glAttachShader(shdFullbrightSolid, vertexShader);
        glAttachShader(shdFullbrightSolid, fragmentShader);
    glLinkProgram(shdFullbrightSolid);

    if(debugShader(shdFullbrightSolid) == GL_FALSE){return;}

    shdFullbrightSolid_position   = glGetAttribLocation(shdFullbrightSolid,  "position");
    
    shdFullbrightSolid_projection = glGetUniformLocation(shdFullbrightSolid, "projection");
    shdFullbrightSolid_modelview  = glGetUniformLocation(shdFullbrightSolid, "modelview");
    shdFullbrightSolid_color      = glGetUniformLocation(shdFullbrightSolid, "color");
    shdFullbrightSolid_opacity    = glGetUniformLocation(shdFullbrightSolid, "opacity");
    shdFullbrightSolid_lightness  = glGetUniformLocation(shdFullbrightSolid, "lightness");
}
void makeFullbright()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v01, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f01, NULL);
    glCompileShader(fragmentShader);

    shdFullbright = glCreateProgram();
        glAttachShader(shdFullbright, vertexShader);
        glAttachShader(shdFullbright, fragmentShader);
    glLinkProgram(shdFullbright);

    if(debugShader(shdFullbright) == GL_FALSE){return;}

    shdFullbright_position   = glGetAttribLocation(shdFullbright,  "position");
    shdFullbright_color      = glGetAttribLocation(shdFullbright,  "color");

    shdFullbright_projection = glGetUniformLocation(shdFullbright, "projection");
    shdFullbright_modelview  = glGetUniformLocation(shdFullbright, "modelview");
    shdFullbright_opacity    = glGetUniformLocation(shdFullbright, "opacity");
    shdFullbright_lightness  = glGetUniformLocation(shdFullbright, "lightness");
}
void makeLambertSolid()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v1, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f1, NULL);
    glCompileShader(fragmentShader);

    shdLambertSolid = glCreateProgram();
        glAttachShader(shdLambertSolid, vertexShader);
        glAttachShader(shdLambertSolid, fragmentShader);
    glLinkProgram(shdLambertSolid);

    if(debugShader(shdLambertSolid) == GL_FALSE){return;}

    shdLambertSolid_position   = glGetAttribLocation(shdLambertSolid,  "position");
    shdLambertSolid_normal     = glGetAttribLocation(shdLambertSolid,  "normal");
    
    shdLambertSolid_projection = glGetUniformLocation(shdLambertSolid, "projection");
    shdLambertSolid_modelview  = glGetUniformLocation(shdLambertSolid, "modelview");
    shdLambertSolid_lightpos   = glGetUniformLocation(shdLambertSolid, "lightpos");
    shdLambertSolid_color      = glGetUniformLocation(shdLambertSolid, "color");
    shdLambertSolid_ambient    = glGetUniformLocation(shdLambertSolid, "ambient");
    shdLambertSolid_saturate   = glGetUniformLocation(shdLambertSolid, "saturate");
    shdLambertSolid_opacity    = glGetUniformLocation(shdLambertSolid, "opacity");
}
void makeLambert()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v2, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f1, NULL);
    glCompileShader(fragmentShader);

    shdLambert = glCreateProgram();
        glAttachShader(shdLambert, vertexShader);
        glAttachShader(shdLambert, fragmentShader);
    glLinkProgram(shdLambert);

    if(debugShader(shdLambert) == GL_FALSE){return;}

    shdLambert_position   = glGetAttribLocation(shdLambert,  "position");
    shdLambert_normal     = glGetAttribLocation(shdLambert,  "normal");
    shdLambert_color      = glGetAttribLocation(shdLambert,  "color");
    
    shdLambert_projection = glGetUniformLocation(shdLambert, "projection");
    shdLambert_modelview  = glGetUniformLocation(shdLambert, "modelview");
    shdLambert_lightpos   = glGetUniformLocation(shdLambert, "lightpos");
    shdLambert_ambient    = glGetUniformLocation(shdLambert, "ambient");\
    shdLambert_saturate   = glGetUniformLocation(shdLambert, "saturate");
    shdLambert_opacity    = glGetUniformLocation(shdLambert, "opacity");
}
/// <><><> ///
void makeAllShaders()
{
    makeFullbrightSolid();
    makeFullbright();
    makeLambertSolid();
    makeLambert();
}
/// <><><> ///
void shadeFullbrightSolid(GLint* position, GLint* projection, GLint* modelview, GLint* color, GLint* lightness, GLint* opacity)
{
    *position = shdFullbrightSolid_position;
    *projection = shdFullbrightSolid_projection;
    *modelview = shdFullbrightSolid_modelview;
    *color = shdFullbrightSolid_color;
    *opacity = shdFullbrightSolid_opacity;
    *lightness = shdFullbrightSolid_lightness;
    glUseProgram(shdFullbrightSolid);
}
void shadeFullbright(GLint* position, GLint* projection, GLint* modelview, GLint* color, GLint* lightness, GLint* opacity)
{
    *position = shdFullbright_position;
    *projection = shdFullbright_projection;
    *modelview = shdFullbright_modelview;
    *color = shdFullbright_color;
    *opacity = shdFullbright_opacity;
    *lightness = shdFullbright_lightness;
    glUseProgram(shdFullbright);
}
void shadeLambertSolid(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* ambient, GLint* saturate, GLint* opacity)
{
    *position = shdLambertSolid_position;
    *projection = shdLambertSolid_projection;
    *modelview = shdLambertSolid_modelview;
    *lightpos = shdLambertSolid_lightpos;
    *color = shdLambertSolid_color;
    *normal = shdLambertSolid_normal;
    *ambient = shdLambertSolid_ambient;
    *saturate = shdLambertSolid_saturate;
    *opacity = shdLambertSolid_opacity;
    glUseProgram(shdLambertSolid);
}
void shadeLambert(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* ambient, GLint* saturate, GLint* opacity)
{
    *position = shdLambert_position;
    *projection = shdLambert_projection;
    *modelview = shdLambert_modelview;
    *lightpos = shdLambert_lightpos;
    *color = shdLambert_color;
    *normal = shdLambert_normal;
    *ambient = shdLambert_ambient;
    *saturate = shdLambert_saturate;
    *opacity = shdLambert_opacity;
    glUseProgram(shdLambert);
}//    <>  / / /
/// <><><> ///
/// <><> ///
/// <> ///
 //////`
#endif
