#include "ScriptEngineDuktape.h"

#include "Script.h"
#include "logger/logger.h"

#include "Settings.h"

#include "EnginePlayer.h"

#include "io/MemoryManager.h"
#include "io/NetworkManager.h"
#include "io/Curl.h"
#include "io/Socket.h"
#include "io/MidiManager.h"
#include "math/MathUtils.h"
#include "math/TransformationMatrix.h"

#include "ui/Window.h"
#include "ui/Gui.h"
#include "ui/Input.h"
#include "ui/menu/Menu.h"
#include "audio/Audio.h"
#include "graphics/Graphics.h"
#include "graphics/Image.h"
#include "graphics/video/VideoFile.h"
#include "graphics/model/Model.h"
#include "graphics/Fbo.h"
#include "graphics/model/TexturedQuad.h"
#include "graphics/Shader.h"
#include "graphics/ShaderProgram.h"
#include "graphics/ShaderProgramOpenGl.h"
#include "graphics/Font.h"
#include "graphics/Text.h"
#include "graphics/Texture.h"
#include "graphics/TextureOpenGl.h"

#include "graphics/LightManager.h"
#include "graphics/Camera.h"

#include "sync/Sync.h"
#include "sync/SyncRocket.h"

#include <assert.h>
#include "GL/gl3w.h"

#define DUKWEBGL_IMPLEMENTATION
#include "dukwebgl.h"

#include <memory>
#include <future>
#include <chrono>

static std::vector<std::unique_ptr<TexturedQuad>> texturedQuads; // FIXME: Don't like this...

// TODO: Implement JavaScript debugger - https://github.com/svaarala/duktape/blob/master/doc/debugger.rst

#define duk_gl_push_opengl_constant_property(ctx, opengl_constant) \
    duk_push_uint((ctx), (opengl_constant)); \
    duk_put_prop_string((ctx), -2, #opengl_constant)

#define NOP_LEGACY_CAPABILITY GL_TEXTURE_2D
static const unsigned int GL_POINT_SMOOTH = NOP_LEGACY_CAPABILITY;
static const unsigned int GL_LINE_STIPPLE = NOP_LEGACY_CAPABILITY;
static const unsigned int GL_CURRENT_BIT = NOP_LEGACY_CAPABILITY;

#define duk_push_constant_property(ctx, type, constant_name, constant) \
    duk_push_##type((ctx), (constant)); \
    duk_put_prop_string((ctx), -2, constant_name)

static void setOpenGlConstants(duk_context *ctx) {

    duk_gl_push_opengl_constant_property(ctx, GL_TEXTURE_2D);
    duk_gl_push_opengl_constant_property(ctx, GL_POINT_SMOOTH);
    duk_gl_push_opengl_constant_property(ctx, GL_LINE_STIPPLE);
    duk_gl_push_opengl_constant_property(ctx, GL_CURRENT_BIT);

    duk_gl_push_opengl_constant_property(ctx, GL_BLEND);
    duk_gl_push_opengl_constant_property(ctx, GL_LINE_SMOOTH);

    duk_gl_push_opengl_constant_property(ctx, GL_POINTS);
    duk_gl_push_opengl_constant_property(ctx, GL_LINE_STRIP);
    duk_gl_push_opengl_constant_property(ctx, GL_LINE_LOOP);
    duk_gl_push_opengl_constant_property(ctx, GL_LINES);
    duk_gl_push_opengl_constant_property(ctx, GL_TRIANGLE_STRIP);
    duk_gl_push_opengl_constant_property(ctx, GL_TRIANGLES);

    duk_gl_push_opengl_constant_property(ctx, GL_LINE_SMOOTH_HINT);
    duk_gl_push_opengl_constant_property(ctx, GL_POLYGON_SMOOTH_HINT);
    duk_gl_push_opengl_constant_property(ctx, GL_TEXTURE_COMPRESSION_HINT);
    duk_gl_push_opengl_constant_property(ctx, GL_FRAGMENT_SHADER_DERIVATIVE_HINT);
    duk_gl_push_opengl_constant_property(ctx, GL_FASTEST);
    duk_gl_push_opengl_constant_property(ctx, GL_NICEST);
    duk_gl_push_opengl_constant_property(ctx, GL_DONT_CARE);

    //NOT OPENGL CONSTANTS BUT MIMIC VALUES
    duk_push_constant_property(ctx, uint, "POINTS", GL_POINTS);
    duk_push_constant_property(ctx, uint, "LINE_STRIP", GL_LINE_STRIP);
    duk_push_constant_property(ctx, uint, "LINE_LOOP", GL_LINE_LOOP);
    duk_push_constant_property(ctx, uint, "LINES", GL_LINES);
    duk_push_constant_property(ctx, uint, "TRIANGLE_STRIP", GL_TRIANGLE_STRIP);
    duk_push_constant_property(ctx, uint, "TRIANGLES", GL_TRIANGLES);
}

ScriptEngine& ScriptEngine::getInstance() {
    static ScriptEngineDuktape scriptEngine;
    return scriptEngine;
}

ScriptEngineDuktape::ScriptEngineDuktape() {
    ctx = NULL;
    stackTraceCalled = 0;
}

ScriptEngineDuktape::~ScriptEngineDuktape() {

}

static Text text = Text("");

static int duk_getScreenWidth(duk_context *ctx)
{
    duk_push_number(ctx, Settings::demo.graphics.canvasWidth);

    return 1;
}

static int duk_getScreenHeight(duk_context *ctx)
{
    duk_push_number(ctx, Settings::demo.graphics.canvasHeight);

    return 1;
}

static int duk_settingsWindowSetWindowDimensions(duk_context *ctx)
{
    unsigned int width = duk_get_uint(ctx, 0);
    unsigned int height = duk_get_uint(ctx, 1);

    Settings::window.setWindowDimensions(width, height);

    return 0;
}

static int duk_settingsLoadSettingsFromString(duk_context *ctx)
{
    const char *data = duk_get_string(ctx, 0);

    Settings::loadSettingsFromString(data);

    return 0;
}

static int duk_settingsDemoLoadDemoSettingsFromString(duk_context *ctx)
{
    const char *data = duk_get_string(ctx, 0);

    Settings::demo.loadDemoSettingsFromString(data);

    return 0;
}

static int duk_settingsDemoSaveDemoSettings(duk_context *ctx)
{
    Settings::demo.saveDemoSettings();

    return 0;
}

static int duk_settingsSaveSettings(duk_context *ctx)
{
    Settings::saveSettings();

    return 0;
}

static int duk_inputSetUserExit(duk_context *ctx)
{
    Input& input = Input::getInstance();
    bool userExit = duk_require_boolean(ctx, 0) == 1 ? true : false;
    input.setUserExit(userExit);

    return 0;
}


static int duk_inputIsUserExit(duk_context *ctx)
{
    Input& input = Input::getInstance();
    duk_push_boolean(ctx, input.isUserExit() ? 1 : 0);

    return 1;
}

static int duk_inputPollEvents(duk_context *ctx)
{
    Input& input = Input::getInstance();
    input.pollEvents();

    return 0;
}


static int duk_inputGetPressedKeyMap(duk_context *ctx)
{
    Input& input = Input::getInstance();
    std::map<std::string, bool> keyMap = input.getPressedKeyMap();

    duk_idx_t key_obj = duk_push_object(ctx);
    for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
        duk_push_boolean(ctx, it->second ? 1 : 0);
        duk_put_prop_string(ctx, key_obj, it->first.c_str());
    }

    return 1;
}


static int duk_menuSetQuit(duk_context *ctx)
{
    Menu& menu = Menu::getInstance();
    bool quit = duk_require_boolean(ctx, 0) == 1 ? true : false;
    menu.setQuit(quit);

    return 0;
}

static int duk_menuGetWidth(duk_context *ctx)
{
    Menu& menu = Menu::getInstance();
    duk_push_number(ctx, menu.getWidth());

    return 1;
}

static int duk_menuGetHeight(duk_context *ctx)
{
    Menu& menu = Menu::getInstance();
    duk_push_number(ctx, menu.getHeight());

    return 1;
}

static int duk_timerSetBeatsPerMinute(duk_context *ctx)
{
    double bpm = (double)duk_get_number(ctx, 0);
    Timer &timer = EnginePlayer::getInstance().getTimer();
    timer.setBeatsPerMinute(bpm);

    return 0;
}

static int duk_timerGetBeatsPerMinute(duk_context *ctx)
{
    Timer &timer = EnginePlayer::getInstance().getTimer();
    duk_push_number(ctx, static_cast<double>(timer.getBeatsPerMinute()));

    return 1;
}

static int duk_timerGetBeatInSeconds(duk_context *ctx)
{
    Timer &timer = EnginePlayer::getInstance().getTimer();
    duk_push_number(ctx, static_cast<double>(timer.getSecondsPerBeat()));

    return 1;
}


static int duk_setTextPivot(duk_context *ctx)
{
    double x = (double)duk_get_number(ctx, 0);
    double y = (double)duk_get_number(ctx, 1);
    double z = (double)duk_get_number(ctx, 2);

    //setTextPivot(x, y, z);

    return 0;
}

static int duk_setTextRotation(duk_context *ctx)
{
    double x = (double)duk_get_number(ctx, 0);
    double y = (double)duk_get_number(ctx, 1);
    double z = (double)duk_get_number(ctx, 2);

    //setTextRotation(x, y, z);
    text.setAngle(x, y, z);

    return 0;
}

static int duk_setTextColor(duk_context *ctx)
{
    double r = (double)duk_get_number(ctx, 0)/255.0;
    double g = (double)duk_get_number(ctx, 1)/255.0;
    double b = (double)duk_get_number(ctx, 2)/255.0;
    double a = (double)duk_get_number(ctx, 3)/255.0;

    text.setColor(r,g,b,a);
    Graphics::getInstance().setColor(Color(r, g, b, a));

    return 0;
}

static int duk_setTextSize(duk_context *ctx)
{
    double x = (double)duk_get_number(ctx, 0);
    double y = (double)duk_get_number(ctx, 1);
    double z = (double)duk_get_number(ctx, 2);

    text.setScale(x, y, z);

    return 0;
}

static int duk_setTextDefaults(duk_context *ctx)
{
    //setTextDefaults();

    return 0;
}

static int duk_setTextCenterAlignment(duk_context *ctx)
{
    int center = (int)duk_get_int(ctx, 0);

    //setTextCenterAlignment(center);

    return 0;
}

static int duk_setTextFont(duk_context *ctx)
{
    const char* fontName = (const char*)duk_get_string(ctx, 0);

    Font *font = MemoryManager<Font>::getInstance().getResource(fontName);
    if (font == NULL) {
        loggerError("Font not found! '%s'", fontName);
        return 0;
    }

    if (!font->isLoaded()) {
        if (!font->load()) {
            loggerWarning("Could not load font '%s'", fontName);
        }
    }


    text.setFont(font);

    //setTextFont(font);

    return 0;
}


static int duk_setTextPosition(duk_context *ctx)
{
    double x = (double)duk_get_number(ctx, 0);
    double y = (double)duk_get_number(ctx, 1);
    double z = (double)duk_get_number(ctx, 2);

    text.setPosition(x, y, z);

    //setTextPosition(x, y, z);

    return 0;
}

static int duk_setDrawTextString(duk_context *ctx)
{
    const char *textString = (const char*)duk_get_string(ctx, 0);

    text.setText(textString);
    //setDrawTextString(text);

    return 0;
}

static int duk_setTextPerspective3d(duk_context *ctx)
{
    //drawText2d();
    unsigned int perspective3d = (unsigned int)duk_get_uint(ctx, 0);

    text.setPerspective2d( perspective3d == 0 ? true : false );

    return 0;
}

static int duk_drawText(duk_context *ctx)
{
    //drawText3d();

    text.draw();

    return 0;
}



static duk_idx_t duk_push_mesh_object(duk_context *ctx, Mesh *mesh)
{
    assert(ctx != NULL);

    duk_idx_t mesh_obj = duk_push_object(ctx);
    
    if (mesh == NULL)
    {
        return mesh_obj;
    }

    duk_push_pointer(ctx, (void*)mesh);
    duk_put_prop_string(ctx, mesh_obj, "ptr");

    return mesh_obj;
}

static int duk_meshNew(duk_context *ctx)
{
    Mesh* mesh = new Mesh();

    duk_push_mesh_object(ctx, mesh);

    return 1;
}

static int duk_meshDelete(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);

    delete mesh;

    return 0;
}

static int duk_meshMaterialSetTexture(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 1);
    unsigned int unit = duk_get_uint(ctx, 2);

    Texture *texture = tex->getTexture();
    loggerTrace("Setting texture 0x%p to mesh 0x%p unit %u", texture, mesh, unit);
    if (mesh->getMaterial() == NULL) {
        loggerTrace("Mesh has no material, setting some");
        mesh->setMaterial(new Material(), true);
    }

    mesh->getMaterial()->setTexture(texture, unit);

    return 0;
}

static int duk_meshSetFaceDrawType(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    unsigned int mode = duk_get_uint(ctx, 1);
    FaceType faceType = FaceType::TRIANGLES;

    switch(mode) {
        //FIXME: New types shouldn't depend on OpenGL stuff
        case GL_TRIANGLES:
        default:
            break;
        case GL_TRIANGLE_STRIP:
            faceType = FaceType::TRIANGLE_STRIP;
            break;
        case GL_LINES:
            faceType = FaceType::LINES;
            break;
        case GL_LINE_LOOP:
            faceType = FaceType::LINE_LOOP;
            break;
        case GL_LINE_STRIP:
            faceType = FaceType::LINE_STRIP;
            break;
        case GL_POINTS:
            faceType = FaceType::POINTS;
            break;
    }

    mesh->setFaceDrawType(faceType);

    return 0;  // no return value
}

static int duk_meshGenerate(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    mesh->generate();
    mesh->print();
    return 0;
}

static int duk_meshDraw(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    double begin = duk_get_number(ctx, 1);
    double end = duk_get_number(ctx, 2);
    //loggerInfo("meshDraw! 0x%p", mesh);
    //mesh->print();
    mesh->draw(begin, end);

    return 0;
}

static int duk_meshClear(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    mesh->clear();

    return 0;
}

static int duk_meshAddColor(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    double r = duk_get_number(ctx, 1);
    double g = duk_get_number(ctx, 2);
    double b = duk_get_number(ctx, 3);
    double a = duk_get_number(ctx, 4);

    mesh->addColor(r, g, b, a);

    return 0;  // no return value
}

static int duk_meshAddIndex(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    unsigned int index = (unsigned int)duk_get_uint(ctx, 1);

    mesh->addIndex(index);

    return 0;  // no return value
}

static int duk_meshAddVertex(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    double x = duk_get_number(ctx, 1);
    double y = duk_get_number(ctx, 2);
    double z = duk_get_number(ctx, 3);

    mesh->addVertex(x, y, z);

    return 0;  // no return value
}

static int duk_meshAddTexCoord(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    double u = duk_get_number(ctx, 1);
    double v = duk_get_number(ctx, 2);

    mesh->addTexCoord(u, v);

    return 0;  // no return value
}

static int duk_meshAddNormal(duk_context *ctx)
{
    Mesh *mesh = (Mesh*)duk_get_pointer(ctx, 0);
    double x = duk_get_number(ctx, 1);
    double y = duk_get_number(ctx, 2);
    double z = duk_get_number(ctx, 3);

    mesh->addNormal(x, y, z);

    return 0;  // no return value
}

static duk_idx_t duk_push_shader_program_object(duk_context *ctx, ShaderProgram *shaderProgram)
{
    assert(ctx != NULL);

    duk_idx_t shaderProgram_obj = duk_push_object(ctx);
    
    if (shaderProgram == NULL)
    {
        return shaderProgram_obj;
    }

    duk_push_pointer(ctx, (void*)shaderProgram);
    duk_put_prop_string(ctx, shaderProgram_obj, "ptr");
    duk_push_string(ctx, (const char*)shaderProgram->getName().c_str());
    duk_put_prop_string(ctx, shaderProgram_obj, "name");
    //duk_push_uint(ctx, shaderProgram->id);
    //duk_put_prop_string(ctx, shaderProgram_obj, "id");
    duk_push_int(ctx, 1);
    duk_put_prop_string(ctx, shaderProgram_obj, "ok");

    return shaderProgram_obj;
}

static duk_idx_t duk_push_shader_object(duk_context *ctx, Shader *shader)
{
    assert(ctx != NULL);

    duk_idx_t shader_obj = duk_push_object(ctx);
    
    if (shader == NULL)
    {
        return shader_obj;
    }

    duk_push_pointer(ctx, (void*)shader);
    duk_put_prop_string(ctx, shader_obj, "ptr");
    duk_push_string(ctx, (const char*)shader->getFilePath().c_str());
    duk_put_prop_string(ctx, shader_obj, "name");
    duk_push_string(ctx, (const char*)shader->getFilePath().c_str());
    duk_put_prop_string(ctx, shader_obj, "filename");
    //duk_push_uint(ctx, shader->id);
    //duk_put_prop_string(ctx, shader_obj, "id");
    duk_push_int(ctx, 1);
    duk_put_prop_string(ctx, shader_obj, "ok");

    return shader_obj;
}

static int duk_shaderProgramLoad(duk_context *ctx)
{
    const char* name = (const char*)duk_get_string(ctx, 0);

    //debugPrintf("shaderProgramLoad(%s)",name);
    MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
    ShaderProgram *shaderProgram = shaderProgramMemory.getResource(name);

    duk_push_shader_program_object(ctx, shaderProgram);

    return 1;
}

static int duk_getShaderProgramFromMemory(duk_context *ctx)
{
    const char* name = (const char*)duk_get_string(ctx, 0);

    MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
    ShaderProgram *shaderProgram = shaderProgramMemory.getResource(name);

    // TODO: get linked checking is a hack for legacy compatibility. should be removed once other stuff is fixed
    if (shaderProgram->isLinked())
    {
        duk_push_shader_program_object(ctx, shaderProgram);        
    }
    else
    {
        duk_idx_t shaderProgram_obj = duk_push_object(ctx);
        return shaderProgram_obj;
    }

    return 1;
}


static int duk_shaderLoad(duk_context *ctx)
{
    const char* shaderName = (const char*)duk_get_string(ctx, 0);
    const char* shaderFilename = (const char*)duk_get_string(ctx, 1);

    MemoryManager<Shader>& shaderMemory = MemoryManager<Shader>::getInstance();
    Shader *shader = shaderMemory.getResource(shaderFilename);

    if (shader == NULL || shader->getError()) {
        shader = NULL;
    } else {
        if (! shader->isLoaded()) {
            shader->load();
        }
    }

    duk_push_shader_object(ctx, shader);

    return 1;
}

static int duk_shaderProgramAddShaderByName(duk_context *ctx)
{
    const char* shaderProgramName = (const char*)duk_get_string(ctx, 0);
    const char* shaderName = (const char*)duk_get_string(ctx, 1);

    //debugPrintf("shaderProgramAddShaderByName(%s,%s)",shaderProgramName, shaderName);
    //shaderProgramAddShaderByName(shaderProgramName, shaderName);

    MemoryManager<Shader>& shaderMemory = MemoryManager<Shader>::getInstance();
    Shader *shader = shaderMemory.getResource(shaderName);

    MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
    ShaderProgram *shaderProgram = shaderProgramMemory.getResource(shaderProgramName);
    shaderProgram->addShader(shader);

    return 0;
}

static int duk_shaderProgramAttachAndLink(duk_context *ctx)
{
    ShaderProgram *shaderProgram = (ShaderProgram*)duk_get_pointer(ctx, 0);

    //shaderProgramAttachAndLink(shaderProgram);
    shaderProgram->link();

    return 0;
}


static int duk_disableShaderProgram(duk_context *ctx)
{
    //disableShaderProgram();
    //glUseProgram(0);
    ShaderProgram *shaderProgram = (ShaderProgram*)duk_get_pointer(ctx, 0);
    shaderProgram->unbind();

    return 0;
}

static int duk_activateShaderProgram(duk_context *ctx)
{
    const char* name = (const char*)duk_get_string(ctx, 0);

    //activateShaderProgram(name);
    MemoryManager<ShaderProgram>& shaderProgramMemory = MemoryManager<ShaderProgram>::getInstance();
    ShaderProgram *shaderProgram = shaderProgramMemory.getResource(name);
    shaderProgram->bind();

    return 0;
}

static int duk_shaderProgramUse(duk_context *ctx)
{
    ShaderProgram *shaderProgram = (ShaderProgram*)duk_get_pointer(ctx, 0);

    //shaderProgramUse(shaderProgram);
    shaderProgram->bind();

    return 0;
}

static int duk_getUniformLocation(duk_context *ctx)
{
    const char* variable = (const char*)duk_get_string(ctx, 0);

    duk_push_uint(ctx, ShaderProgramOpenGl::getUniformLocation(variable));

    return 1;
}

static int duk_glUniformf(duk_context *ctx)
{
    int argc = duk_get_top(ctx);
    if(argc<2 || argc>5)
    {
        loggerWarning("Argument count invalid! count:'%d'", argc);
        return 0;
    }

    unsigned int uniformLocation = (unsigned int)duk_get_uint(ctx, 0);
    float value1 = 0.0f;
    float value2 = 0.0f;
    float value3 = 0.0f;
    float value4 = 0.0f;

    switch(argc)
    {
        case 5:
            value4 = (float)duk_to_number(ctx, 4);
        case 4:
            value3 = (float)duk_to_number(ctx, 3);
        case 3:
            value2 = (float)duk_to_number(ctx, 2);
        case 2:
            value1 = (float)duk_to_number(ctx, 1);
        default:
            break;
    }

    switch(argc)
    {
        case 5:
            glUniform4f(uniformLocation, value1, value2, value3, value4);
            break;
        case 4:
            glUniform3f(uniformLocation, value1, value2, value3);
            break;
        case 3:
            glUniform2f(uniformLocation, value1, value2);
            break;
        case 2:
            glUniform1f(uniformLocation, value1);
            break;
        default:
            break;
    }

    return 0;
}

static int duk_glUniformi(duk_context *ctx)
{
    int argc = duk_get_top(ctx);
    if(argc<2 || argc>5)
    {
        loggerWarning("Argument count invalid! count:'%d'", argc);
        return 0;
    }

    unsigned int uniformLocation = (unsigned int)duk_get_uint(ctx, 0);
    int value1 = 0;
    int value2 = 0;
    int value3 = 0;
    int value4 = 0;

    switch(argc)
    {
        case 5:
            value4 = (int)duk_to_int(ctx, 4);
        case 4:
            value3 = (int)duk_to_int(ctx, 3);
        case 3:
            value2 = (int)duk_to_int(ctx, 2);
        case 2:
            value1 = (int)duk_to_int(ctx, 1);
        default:
            break;
    }

    switch(argc)
    {
        case 5:
            glUniform4i(uniformLocation, value1, value2, value3, value4);
            break;
        case 4:
            glUniform3i(uniformLocation, value1, value2, value3);
            break;
        case 3:
            glUniform2i(uniformLocation, value1, value2);
            break;
        case 2:
            glUniform1i(uniformLocation, value1);
            break;
        default:
            break;
    }

    return 0;
}

static int duk_setPerspective3d(duk_context *ctx)
{
    unsigned int perspective3d = (int)duk_get_int(ctx, 0);

    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    if (perspective3d == 1) {
        transformationMatrix.perspective3d();
    } else {
        transformationMatrix.perspective2d();
    }

    return 0;
}


static int duk_setObjectScale(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    float x = (float)duk_get_number(ctx, 1);
    float y = (float)duk_get_number(ctx, 2);
    float z = (float)duk_get_number(ctx, 3);
    
    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    transformationMatrix.scale(x, y, z);

    return 0;
}

static int duk_setObjectPosition(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    float x = (float)duk_get_number(ctx, 1);
    float y = (float)duk_get_number(ctx, 2);
    float z = (float)duk_get_number(ctx, 3);
    
    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    transformationMatrix.translate(x, y, z);

    return 0;
}

static int duk_setObjectPivot(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    float x = (float)duk_get_number(ctx, 1);
    float y = (float)duk_get_number(ctx, 2);
    float z = (float)duk_get_number(ctx, 3);
    
    //setObjectPivot(object, x, y, z);
    loggerWarning("NOT IMPLEMENTED");

    return 0;
}

static int duk_setObjectRotation(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    float degreesX = (float)duk_get_number(ctx, 1);
    float degreesY = (float)duk_get_number(ctx, 2);
    float degreesZ = (float)duk_get_number(ctx, 3);
    float x = (float)duk_get_number(ctx, 4);
    float y = (float)duk_get_number(ctx, 5);
    float z = (float)duk_get_number(ctx, 6);
    
    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    //TODO: is this ok?!?!
    transformationMatrix.rotateX(degreesX*x);
    transformationMatrix.rotateY(degreesY*y);
    transformationMatrix.rotateZ(degreesZ*z);

    //setObjectRotation(object, degreesX, degreesY, degreesZ, x, y, z);

    return 0;
}

static int duk_setObjectNodeScale(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    const char *nodeName = duk_get_string(ctx, 1);
    float x = (float)duk_get_number(ctx, 2);
    float y = (float)duk_get_number(ctx, 3);
    float z = (float)duk_get_number(ctx, 4);
    
    Mesh *mesh = model->getMesh(std::string(nodeName));
    if (mesh) {
        mesh->setScale(x, y, z);
    } else {
        loggerWarning("Node name not recognized! node:'%s', model:'%s'", nodeName, model->getFilePath().c_str());
        return 0;
    }

    return 0;
}

static int duk_setObjectNodePosition(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    const char *nodeName = duk_get_string(ctx, 1);
    float x = (float)duk_get_number(ctx, 2);
    float y = (float)duk_get_number(ctx, 3);
    float z = (float)duk_get_number(ctx, 4);
    
    Mesh *mesh = model->getMesh(std::string(nodeName));
    if (mesh) {
        mesh->setTranslate(x, y, z);
    } else {
        loggerWarning("Node name not recognized! node:'%s', model:'%s'", nodeName, model->getFilePath().c_str());
        return 0;
    }

    return 0;
}

static int duk_setObjectNodeRotation(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    const char *nodeName = duk_get_string(ctx, 1);
    float degreesX = (float)duk_get_number(ctx, 2);
    float degreesY = (float)duk_get_number(ctx, 3);
    float degreesZ = (float)duk_get_number(ctx, 4);
    float x = (float)duk_get_number(ctx, 5);
    float y = (float)duk_get_number(ctx, 6);
    float z = (float)duk_get_number(ctx, 7);
    
    Mesh *mesh = model->getMesh(std::string(nodeName));
    if (mesh) {
        //TODO: is this ok?!?!
        mesh->setRotate(degreesX*x, degreesY*y, degreesZ*z);
    } else {
        loggerWarning("Node name not recognized! node:'%s', model:'%s'", nodeName, model->getFilePath().c_str());
        return 0;
    }

    return 0;
}

static int duk_setObjectColor(duk_context *ctx)
{
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    float r = (float)duk_get_number(ctx, 1);
    float g = (float)duk_get_number(ctx, 2);
    float b = (float)duk_get_number(ctx, 3);
    float a = (float)duk_get_number(ctx, 4);
    
    //setObjectColor(object, r, g, b, a);
    loggerWarning("NOT IMPLEMENTED");

    return 0;
}

#define bindCFunctionToJs(cFunction, argumentCount) \
  duk_push_c_function(ctx, duk_##cFunction, argumentCount); \
  duk_put_prop_string(ctx, -2, #cFunction)

#define bindNewCFunctionToJs(cFunction, argumentCount) \
  duk_push_c_function(ctx, _duk_##cFunction, argumentCount); \
  duk_put_prop_string(ctx, -2, #cFunction)

static duk_idx_t duk_push_empty_object(duk_context *ctx)
{
    assert(ctx != NULL);

    duk_idx_t tex_obj = duk_push_object(ctx);

    return tex_obj;
}

static duk_idx_t duk_push_texture_object(duk_context *ctx, TexturedQuad* texturedQuad)
{
    assert(ctx != NULL);

    duk_idx_t tex_obj = duk_push_object(ctx);
    
    if (texturedQuad == NULL)
    {
        return tex_obj;
    }

    duk_push_pointer(ctx, (void*)texturedQuad);
    duk_put_prop_string(ctx, tex_obj, "ptr");

    duk_push_number(ctx, texturedQuad->getWidth());
    duk_put_prop_string(ctx, tex_obj, "width");
    duk_push_number(ctx, texturedQuad->getHeight());
    duk_put_prop_string(ctx, tex_obj, "height");

    //duk_push_string(ctx, image->getFilePath().c_str());
    //duk_put_prop_string(ctx, tex_obj, "name");
    /*duk_push_number(ctx, tex->x);
    duk_put_prop_string(ctx, tex_obj, "x");
    duk_push_number(ctx, tex->y);
    duk_put_prop_string(ctx, tex_obj, "y");    
    duk_push_number(ctx, tex->z);
    duk_put_prop_string(ctx, tex_obj, "z");    
    duk_push_uint(ctx, tex->id);
    duk_put_prop_string(ctx, tex_obj, "id");
    duk_push_int(ctx, tex->perspective3d);
    duk_put_prop_string(ctx, tex_obj, "perspective3d");*/
    duk_push_uint(ctx, dynamic_cast<TextureOpenGl*>(texturedQuad->getTexture())->getId());
    duk_put_prop_string(ctx, tex_obj, "id");

    return tex_obj;
}

static int duk_loadObject(duk_context *ctx)
{
    const char *filename = duk_get_string(ctx, 0);

    MemoryManager<Model>& memory = MemoryManager<Model>::getInstance();
    Model *model = memory.getResource(std::string(filename));

    duk_idx_t obj = duk_push_object(ctx);
    
    if (model == NULL || model->getError()) {
        return 1;
    }

    if (!model->isLoaded() || model->modified()) {
        model->load();
    }

    if (model->isLoaded()) {
        duk_push_pointer(ctx, (void*)model);
        duk_put_prop_string(ctx, obj, "ptr");
        duk_push_string(ctx, filename);
        duk_put_prop_string(ctx, obj, "filename");    
    }
    return 1;
}

static int duk_drawObject(duk_context *ctx)
{
    int argc = duk_get_top(ctx);
    assert(argc > 0);
    
    Model *model = (Model*)duk_get_pointer(ctx, 0);
    const char *displayCamera = "Camera01";
    double displayFrame = 0.0;
    int clear = 0;

    if (argc > 1)
    {
        displayCamera = (const char*)duk_get_string(ctx, 1);
    }
    if (argc > 2)
    {
        displayFrame = (double)duk_get_number(ctx, 2);
    }
    if (argc > 3)
    {
        clear = (unsigned int)duk_get_int(ctx, 3);
    }
    
    //drawObject(object_ptr, displayCamera, displayFrame, clear);

    model->draw();

    return 0;
}

//getWindowScreenAreaAspectRatio
static int duk_getCameraAspectRatio(duk_context *ctx)
{
    Camera& camera = EnginePlayer::getInstance().getActiveCamera();

    duk_push_number(ctx, camera.getAspectRatio());

    return 1;
}

static int duk_setCameraPerspective(duk_context *ctx)
{
    double cfov = (double)duk_get_number(ctx, 0);
    double caspect = (double)duk_get_number(ctx, 1);
    double cnear = (double)duk_get_number(ctx, 2);
    double cfar = (double)duk_get_number(ctx, 3);

    Camera& camera = EnginePlayer::getInstance().getActiveCamera();
    camera.setHorizontalFov(cfov);
    camera.setAspectRatio(caspect);
    camera.setClipPlaneNear(cnear);
    camera.setClipPlaneFar(cfar);

    return 0;
}

static int duk_setCameraPosition(duk_context *ctx)
{
    double x = (double)duk_get_number(ctx, 0);
    double y = (double)duk_get_number(ctx, 1);
    double z = (double)duk_get_number(ctx, 2);

    Camera& camera = EnginePlayer::getInstance().getActiveCamera();
    camera.setPosition(x, y, z);

    return 0;
}

static int duk_setCameraLookAt(duk_context *ctx)
{
    double x = (double)duk_get_number(ctx, 0);
    double y = (double)duk_get_number(ctx, 1);
    double z = (double)duk_get_number(ctx, 2);

    Camera& camera = EnginePlayer::getInstance().getActiveCamera();
    camera.setLookAt(x, y, z);

    return 0;
}

static int duk_setCameraUpVector(duk_context *ctx)
{
    double x = (double)duk_get_number(ctx, 0);
    double y = (double)duk_get_number(ctx, 1);
    double z = (double)duk_get_number(ctx, 2);

    Camera& camera = EnginePlayer::getInstance().getActiveCamera();
    camera.setUp(x, y, z);

    return 0;
}

static int duk_lightSetAmbientColor(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);
    double r = (double)duk_get_number(ctx, 1) / 255.0;
    double g = (double)duk_get_number(ctx, 2) / 255.0;
    double b = (double)duk_get_number(ctx, 3) / 255.0;
    double a = (double)duk_get_number(ctx, 4) / 255.0;

    Light& light = LightManager::getInstance().getLight(lightIndex);
    light.setAmbient(r, g, b, a);

    return 0;
}

static int duk_lightSetDiffuseColor(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);
    double r = (double)duk_get_number(ctx, 1) / 255.0;
    double g = (double)duk_get_number(ctx, 2) / 255.0;
    double b = (double)duk_get_number(ctx, 3) / 255.0;
    double a = (double)duk_get_number(ctx, 4) / 255.0;

    Light& light = LightManager::getInstance().getLight(lightIndex);
    light.setDiffuse(r, g, b, a);

    return 0;
}

static int duk_lightSetSpecularColor(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);
    double r = (double)duk_get_number(ctx, 1) / 255.0;
    double g = (double)duk_get_number(ctx, 2) / 255.0;
    double b = (double)duk_get_number(ctx, 3) / 255.0;
    double a = (double)duk_get_number(ctx, 4) / 255.0;

    Light& light = LightManager::getInstance().getLight(lightIndex);
    light.setSpecular(r, g, b, a);

    return 0;
}

static int duk_lightSetPosition(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);
    double x = (double)duk_get_number(ctx, 1);
    double y = (double)duk_get_number(ctx, 2);
    double z = (double)duk_get_number(ctx, 3);

    Light& light = LightManager::getInstance().getLight(lightIndex);
    light.setPosition(x, y, z);

    return 0;
}

static int duk_lightSetDirection(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);
    double x = (double)duk_get_number(ctx, 1);
    double y = (double)duk_get_number(ctx, 2);
    double z = (double)duk_get_number(ctx, 3);

    Light& light = LightManager::getInstance().getLight(lightIndex);
    light.setDirection(x, y, z);

    return 0;
}

static int duk_lightSetOn(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);

    //FIXME: hacky shit again...
    LightManager::getInstance().setActiveLightCount(lightIndex + 1);

    return 0;
}


static int duk_lightSetOff(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);

    //FIXME: hacky shit again...
    LightManager::getInstance().setActiveLightCount(lightIndex);

    return 0;
}

static int duk_lightSetType(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);
    unsigned int type = static_cast<unsigned int>(duk_get_uint(ctx, 1));

    LightType lightType;
    switch(type) {
        case 2:
            lightType = LightType::POINT;
            break;
        case 3:
            lightType = LightType::SPOT;
            break;
        default:
        case 1:
            lightType = LightType::DIRECTIONAL;
            break;
    }

    Light& light = LightManager::getInstance().getLight(lightIndex);
    light.setType(lightType);

    return 0;
}

static int duk_lightSetGenerateShadowMap(duk_context *ctx)
{
    unsigned int lightIndex = (unsigned int)duk_get_uint(ctx, 0);
    bool generateShadowMap = static_cast<unsigned int>(duk_get_uint(ctx, 1)) == 1 ? true : false;

    Light& light = LightManager::getInstance().getLight(lightIndex);
    light.setGenerateShadowMap(generateShadowMap);

    return 0;
}

static int duk_audioLoad(duk_context *ctx)
{
    const char *filename = duk_get_string(ctx, 0);

    Audio::getInstance().load(filename);

    return 0;
}

static int duk_audioPlay(duk_context *ctx)
{
    const char *filename = duk_get_string(ctx, 0);

    Audio::getInstance().play(filename);

    return 0;
}


static int duk_audioSetDuration(duk_context *ctx)
{
    const char *filename = duk_get_string(ctx, 0);
    double duration = duk_get_number(ctx, 1);

    Audio::getInstance().setDuration(filename, duration);

    return 0;
}

static int duk_audioStop(duk_context *ctx)
{
    Audio::getInstance().stop();

    return 0;
}


static int duk_imageLoadImage(duk_context *ctx);
static int duk_videoLoad(duk_context *ctx)
{
    return duk_imageLoadImage(ctx);
}

static int duk_videoSetStartTime(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double start = duk_get_number(ctx, 1);

    VideoFile* video = reinterpret_cast<VideoFile*>(tex->getParent());
    video->setStartTime(start);

    return 0;
}

static int duk_videoSetSpeed(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double speed = duk_get_number(ctx, 1);

    VideoFile* video = reinterpret_cast<VideoFile*>(tex->getParent());
    video->setSpeed(speed);

    return 0;
}

static int duk_videoSetFps(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double fps = duk_get_number(ctx, 1);

    VideoFile* video = reinterpret_cast<VideoFile*>(tex->getParent());
    video->setFps(fps);

    return 0;
}

static int duk_videoSetLoop(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    bool loop = static_cast<unsigned int>(duk_get_uint(ctx, 1)) == 1 ? true : false;

    VideoFile* video = reinterpret_cast<VideoFile*>(tex->getParent());
    video->setLoop(loop);

    return 0;
}


static int duk_videoSetLength(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double length = duk_get_number(ctx, 1);

    VideoFile* video = reinterpret_cast<VideoFile*>(tex->getParent());
    video->setLength(length);
loggerInfo("tried to set length %.2f", length);
    return 0;
}

static int duk_videoPlay(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    VideoFile* video = reinterpret_cast<VideoFile*>(tex->getParent());
    video->play();

    return 0;
}

static int duk_videoDraw(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    VideoFile* video = reinterpret_cast<VideoFile*>(tex->getParent());
    video->draw();

    return 0;
}

extern Texture *fftTexture; //FIXME

static int duk_imageLoadImage(duk_context *ctx)
{
    std::string filename = std::string(duk_get_string(ctx, 0));

    std::unique_ptr<TexturedQuad> texturedQuad = nullptr;

    MemoryManager<Image>& memory = MemoryManager<Image>::getInstance();
    Image *image = memory.getResource(filename);
    if (image == NULL) {
        image = MemoryManager<VideoFile>::getInstance().getResource(filename);
    }

    if (image == NULL) {
        std::size_t nameIndex = filename.find_last_of(".");
        if (nameIndex != std::string::npos && filename.substr(nameIndex) == ".fbo") {
            bool useDepthTexture = false;
            nameIndex = filename.find_first_of(".");
            if (nameIndex != std::string::npos && filename.substr(nameIndex) == ".depth.fbo") {
                useDepthTexture = true;
            }

            std::string fboName = filename.substr(0, nameIndex);

            MemoryManager<Fbo>& fboMemory = MemoryManager<Fbo>::getInstance();
            Fbo *fbo = fboMemory.getResource(fboName);
            if (fbo->getColorTexture() == NULL) {
                fbo->generate();
            }

            Texture *fboTexture = fbo->getColorTexture();
            if (useDepthTexture) {
                if (fbo->getDepthTexture() == NULL) {
                    fbo->generate();
                }
                fboTexture = fbo->getDepthTexture();
            }

            loggerTrace("Image from fbo! filename:'%s', fboName:'%s', ptr:0x%p", filename.c_str(), fboName.c_str(), fbo);
            if (fbo != NULL && fboTexture != NULL) {
                texturedQuad = std::unique_ptr<TexturedQuad>(TexturedQuad::newInstance(fbo, fboTexture));
            } else {
                loggerWarning("Couldn't find FBO! '%s'", filename.c_str());
            }
        } else if (filename == "fft0") { //FIXME: unit number support
            if (fftTexture) {
                loggerTrace("Image from FFT! filename:'%s'", filename.c_str());
                texturedQuad = std::unique_ptr<TexturedQuad>(TexturedQuad::newInstance(Settings::demo.graphics.canvasWidth, Settings::demo.graphics.canvasHeight));
                texturedQuad.get()->setTexture(fftTexture, 0);
            } else {
                loggerWarning("Image from FFT can't be created, FFT texture not initialized! filename:'%s'", filename.c_str());
            }
        }
    } else if (!image->isLoaded() || image->modified()) {
        image->load();
    }

    if (!texturedQuad) {
        if (image != NULL) {
            texturedQuad = std::unique_ptr<TexturedQuad>(TexturedQuad::newInstance(image));
        } else {
            loggerError("Funky situation: image NULL. filename:'%s'", filename.c_str());
        }
    }

    if (texturedQuad) {
        texturedQuad->init();
        duk_push_texture_object(ctx, texturedQuad.get());
        texturedQuads.push_back(std::move(texturedQuad));
    } else {
        loggerError("Error in initializing image '%s'", filename.c_str());
        duk_push_empty_object(ctx);
    }
    
    return 1;
}

static int duk_setTextureSizeToScreenSize(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);

    double requestedWidth = Settings::demo.graphics.canvasWidth;
    double requestedHeight = Settings::demo.graphics.canvasHeight;

    if (tex->getWidth() != requestedWidth || tex->getHeight() != requestedHeight) {
        tex->setDimensions(requestedWidth, requestedHeight);
        tex->init();
    }

    return 0;
}

static int duk_setTextureCenterAlignment(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    int alignmentNumeric = (int)duk_get_int(ctx, 1);
    Alignment alignment;
    switch(alignmentNumeric) {
        case 1:
        default:
            alignment = Alignment::CENTERED;
            break;
        case 2:
            alignment = Alignment::HORIZONTAL;
            break;
        case 3:
            alignment = Alignment::VERTICAL;
            break;
    }

    
    tex->setAlignment(alignment);

    return 0;
}

static int duk_setTexturePerspective3d(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    unsigned int perspective3d = (int)duk_get_int(ctx, 1);

    //setTexturePerspective3d(tex, perspective3d);
    if (perspective3d == 1) {
        tex->setPerspective2d(false);
    } else {
        tex->setPerspective2d(true);
    }

    return 0;
}

static int duk_drawTexture(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    
    tex->draw();

    return 0;
}

static int duk_setTextureRotation(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double degreesX = (double)duk_get_number(ctx, 1);
    double degreesY = (double)duk_get_number(ctx, 2);
    double degreesZ = (double)duk_get_number(ctx, 3);
    //double x = (double)duk_get_number(ctx, 4);
    //double y = (double)duk_get_number(ctx, 5);
    //double z = (double)duk_get_number(ctx, 6);

    tex->setAngle(degreesX, degreesY, degreesZ);

    //setTextureRotation(tex, degreesX, degreesY, degreesZ, x, y, z);

    return 0;
}

static int duk_setTextureUnitTexture(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    unsigned int unit = (unsigned int)duk_get_uint(ctx, 1);
    TexturedQuad *texDst = (TexturedQuad*)duk_get_pointer(ctx, 2);

    //setTextureUnitTexture(tex, unit, texDst);
    tex->setTexture(texDst->getTexture(), unit);

    return 0;
}

static int duk_setTextureScale(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double x = (double)duk_get_number(ctx, 1);
    double y = (double)duk_get_number(ctx, 2);
    double z = (double)duk_get_number(ctx, 3);

    tex->setScale(x, y, z);

    return 0;
}

static int duk_setTexturePosition(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double x = (double)duk_get_number(ctx, 1);
    double y = (double)duk_get_number(ctx, 2);
    double z = (double)duk_get_number(ctx, 3);

    //setTexturePosition(tex, x, y, z);
    tex->setPosition(x, y, z);

    return 0;
}

static int duk_setTextureColor(duk_context *ctx)
{
    TexturedQuad *tex = (TexturedQuad*)duk_get_pointer(ctx, 0);
    double r = (double)duk_get_number(ctx, 1) / 255.0;
    double g = (double)duk_get_number(ctx, 2) / 255.0;
    double b = (double)duk_get_number(ctx, 3) / 255.0;
    double a = (double)duk_get_number(ctx, 4) / 255.0;

    //setTexturePosition(tex, x, y, z);
    tex->setColor(r, g, b, a);

    return 0;
}

static duk_idx_t duk_push_fbo_object(duk_context *ctx, Fbo *fbo, TexturedQuad* texturedQuad)
{
    assert(ctx != NULL);

    duk_idx_t fbo_obj = duk_push_object(ctx);

    if (fbo == NULL || fbo->getColorTexture() == NULL)
    {
        return fbo_obj;
    }

    duk_push_pointer(ctx, fbo);
    duk_put_prop_string(ctx, fbo_obj, "ptr");
    /*duk_push_int(ctx, fbo->getWidth());
    duk_put_prop_string(ctx, fbo_obj, "width");
    duk_push_int(ctx, fbo->getHeight());
    duk_put_prop_string(ctx, fbo_obj, "height");*/
    /*duk_push_uint(ctx, fbo->id);
    duk_put_prop_string(ctx, fbo_obj, "id");*/
    if (texturedQuad != NULL && texturedQuad->isGenerated())
    {
        duk_push_texture_object(ctx, texturedQuad);
    } else {
        duk_push_empty_object(ctx);
    }

    duk_put_prop_string(ctx, fbo_obj, "color");
    /*if (fbo->depth)
    {
        duk_push_texture_object(ctx, fbo->depth);
        duk_put_prop_string(ctx, fbo_obj, "depth");
    }*/

    return fbo_obj;
}

static std::map<std::string, TexturedQuad*> texturedQuadMap;

static int duk_fboInit(duk_context *ctx)
{
    const char *name = duk_get_string(ctx, 0);

    MemoryManager<Fbo>& fboMemory = MemoryManager<Fbo>::getInstance();
    Fbo *fbo = fboMemory.getResource(std::string(name));
    if (fbo->getColorTexture() == NULL) {
        fbo->generate();
        //fboTexturedQuad = std::unique_ptr<TexturedQuad>(TexturedQuad::newInstance(fbo));
        //fboTexturedQuad->init();
        //texturedQuadMap[fbo->getName()] = fboTexturedQuad;
    } /*else {
        fboTexturedQuad = texturedQuadMap[fbo->getName()];
    }*/

    std::unique_ptr<TexturedQuad> fboTexturedQuad = std::unique_ptr<TexturedQuad>(TexturedQuad::newInstance(fbo));

    if (fboTexturedQuad) {
        fboTexturedQuad->init();
        duk_push_fbo_object(ctx, fbo, fboTexturedQuad.get());
        texturedQuads.push_back(std::move(fboTexturedQuad));
    } else {
        loggerWarning("Textured quad should not be NULL! fbo:%s", fbo->getName().c_str());
        duk_push_empty_object(ctx);
    }
    
    return 1;
}

static int duk_fboBind(duk_context *ctx)
{
    int argc = duk_get_top(ctx);
    if (argc > 0) {
        Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);
        fbo->bind();
    } else {
        loggerWarning("CAN'T UNBIND FBO");
    }

//    fboBind(fbo);

    return 0;
}
static int duk_fboUnbind(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);
    fbo->unbind();

    return 0;
}

/*static int duk_fboDeinit(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);

    fboDeinit(fbo);

    return 0;
}

static int duk_fboStoreDepth(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);
    int storeDepth = (int)duk_get_int(ctx, 1);

    fboStoreDepth(fbo, storeDepth);

    return 0;
}*/

/*static int duk_fboSetDimensions(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);
    unsigned int width = (unsigned int)duk_get_uint(ctx, 1);
    unsigned int height = (unsigned int)duk_get_uint(ctx, 2);

    //fboSetDimensions(fbo, width, height);

    return 0;
}*/

static int duk_fboGenerateFramebuffer(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);
    //fbo->generate();

    //FIXME: MEMORY LEAK HERE
    //TexturedQuad* texturedQuad = TexturedQuad::newInstance(fbo);
    //texturedQuad->init();

    //FIXME: always true...
    duk_push_uint(ctx, 1);

    return 1;
}

/*static int duk_fboSetRenderDimensions(duk_context *ctx)
{
    fbo_t* fbo = (fbo_t*)duk_get_pointer(ctx, 0);
    double widthPercent = (double)duk_get_number(ctx, 1);
    double heightPercent = (double)duk_get_number(ctx, 2);

    fboSetRenderDimensions(fbo, widthPercent, heightPercent);

    return 0;
}*/
static int duk_fboGetWidth(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);

    duk_push_int(ctx, fbo->getWidth());

    return 1;
}
static int duk_fboGetHeight(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);

    duk_push_int(ctx, fbo->getHeight());

    return 1;
}

static int duk_fboUpdateViewport(duk_context *ctx)
{
    Graphics& graphics = Graphics::getInstance();
    graphics.clear();

    return 0; 

    /*Fbo* fbo = NULL;
    int argc = duk_get_top(ctx);
    if (argc > 0)
    {
        fbo = (Fbo*)duk_get_pointer(ctx, 0);
        graphics.setViewport(0, 0, fbo->getWidth(), fbo->getHeight());
    } else {
        graphics.setViewport();
    }


    return 0;*/
}
static int duk_fboBindTextures(duk_context *ctx)
{
    Fbo* fbo = NULL;
    int argc = duk_get_top(ctx);
    if (argc > 0)
    {
        fbo = (Fbo*)duk_get_pointer(ctx, 0);
        fbo->textureBind();
    } else {
        loggerWarning("CAN'T UNBIND FBO TEXTURES");
    }

    return 0;
}
static int duk_fboUnbindTextures(duk_context *ctx)
{
    Fbo* fbo = (Fbo*)duk_get_pointer(ctx, 0);
    fbo->textureUnbind();

    return 0;
}

double interpolateSmoothStep(double p, double a, double b)
{
    double x = getClamp((interpolateLinear(p, a, b) - a)/(b - a), 0.0, 1.0);
    return x*x*(3 - 2*x);
}

double interpolateSmootherStep(double p, double a, double b)
{
    double x = getClamp((interpolateLinear(p, a, b) - a)/(b - a), 0.0, 1.0);
    return x*x*x*(x*(x*6 - 15) + 10);
}

static int duk_interpolateLinear(duk_context *ctx)
{
    double p = (double)duk_get_number(ctx, 0);
    double a = (double)duk_get_number(ctx, 1);
    double b = (double)duk_get_number(ctx, 2);

    duk_push_number(ctx, (double)interpolateLinear(p, a, b));

    return 1;
}

static int duk_interpolateSmootherStep(duk_context *ctx)
{
    double p = (double)duk_get_number(ctx, 0);
    double a = (double)duk_get_number(ctx, 1);
    double b = (double)duk_get_number(ctx, 2);

    duk_push_number(ctx, (double)interpolateSmootherStep(p, a, b));

    return 1;
}

static int duk_interpolateSmoothStep(duk_context *ctx)
{
    double p = (double)duk_get_number(ctx, 0);
    double a = (double)duk_get_number(ctx, 1);
    double b = (double)duk_get_number(ctx, 2);

    duk_push_number(ctx, (double)interpolateSmoothStep(p, a, b));

    return 1;
}

static int duk_interpolate(duk_context *ctx)
{
    double p = getClamp((double)duk_get_number(ctx, 0), 0.0, 1.0);
    double a = (double)duk_get_number(ctx, 1);
    double b = (double)duk_get_number(ctx, 2);
    int type = (int)duk_get_int(ctx, 3);
    
    double value = 0.0;
    switch(type)
    {
        case 2:
            value = interpolateSmootherStep(p, a, b);
            break;

        case 1:
            value = interpolateSmoothStep(p, a, b);
            break;

        case 0:
        default:
            value = interpolateLinear(p, a, b);
            break;
    }
    duk_push_number(ctx, value);

    return 1;
}

static int _duk_random(duk_context *ctx)
{
    if (duk_get_top(ctx) > 0)
    {
        srand((unsigned int)duk_get_uint(ctx, 0));
    }

    duk_push_number(ctx, (double)((rand()%10000)/10000.0));
    return 1;
}

static int duk_getSceneStartTime(duk_context *ctx)
{
    //FIXME: redundant function?
    duk_push_number(ctx, 0.0);
    
    return 1;
}

static int duk_getSceneEndTime(duk_context *ctx)
{
    //FIXME: redundant function?
    duk_push_number(ctx, Settings::demo.length);
    
    return 1;
}

static int duk_getSceneTimeFromStart(duk_context *ctx)
{
    Timer &timer = EnginePlayer::getInstance().getTimer();
    duk_push_number(ctx, timer.getTimeInSeconds());
    
    return 1;
}

static int duk_getSceneProgressPercent(duk_context *ctx)
{
    Timer &timer = EnginePlayer::getInstance().getTimer();

    duk_push_number(ctx, timer.getTimeInSeconds() / Settings::demo.length);
    
    return 1;
}


//FIXME: Bad ad-hoc kludge. Make this better.
static int resourceCount = 0;
static int resourceLoadCount = 0;
static int duk_setResourceCount(duk_context *ctx)
{
    int resource_count = (int)duk_get_int(ctx, 0);

    resourceCount += resource_count;

    return 0;
}

static int duk_notifyResourceLoaded(duk_context *ctx)
{
    resourceLoadCount++;

    EnginePlayer& enginePlayer = EnginePlayer::getInstance();
    enginePlayer.setProgress(resourceLoadCount/static_cast<double>(resourceCount));
    enginePlayer.mainScreenDraw();

    if (resourceLoadCount >= resourceCount) {
        // reset loader after done
        resourceCount = 0;
        resourceLoadCount = 0;
    }

    return 0;
}

static duk_ret_t duk_setPlaylistMusic(duk_context *ctx)
{
    Settings::demo.song = duk_get_string(ctx, 0);
    return 0;
}

static duk_ret_t duk_setPlaylistLength(duk_context *ctx)
{
    //FIXME: should support also humanreadable time strings
    Settings::demo.length = static_cast<double>(atoi(duk_get_string(ctx, 0)));
    return 0;
}

static duk_ret_t duk_windowSetTitle(duk_context *ctx)
{
    const char *windowTitle = duk_get_string(ctx, 0);

    EnginePlayer& player = EnginePlayer::getInstance();
    Window* window = player.getWindow(WindowType::CURRENT);
    if (window != NULL) {
        window->setTitle(windowTitle);
    }

    return 0;
}

static duk_ret_t duk_setScreenDimensions(duk_context *ctx)
{
    float width = static_cast<float>(duk_get_number(ctx, 0));
    float height = static_cast<float>(duk_get_number(ctx, 1));

    Settings::demo.graphics.setCanvasDimensions(width, height);

    return 0;
}

static duk_ret_t duk_setWindowScreenAreaAspectRatio(duk_context *ctx)
{
    float width = static_cast<float>(duk_get_number(ctx, 0));
    float height = static_cast<float>(duk_get_number(ctx, 1));

    Settings::demo.graphics.aspectRatio = width / height;

    return 0;
}

static duk_ret_t duk_setClearColor(duk_context *ctx)
{
    double r = duk_get_number(ctx, 0);
    double g = duk_get_number(ctx, 1);
    double b = duk_get_number(ctx, 2);
    double a = duk_get_number(ctx, 3);

    Graphics& graphics = Graphics::getInstance();
    graphics.setClearColor(Color(r, g, b, a));

    return 0;
}


static int duk_graphicsHandleErrors(duk_context *ctx)
{
    Graphics& graphics = Graphics::getInstance();

    int result = 0;
    if (graphics.handleErrors()) {
        result = 1;        
    }

    duk_push_int(ctx, result);

    return 1;
}

static int duk_getDisplayModes(duk_context *ctx)
{
    Gui& gui = Gui::getInstance();

    std::vector<DisplayMode>& displayModes = gui.getDisplayModes();
    if (displayModes.empty()) {
        displayModes = Settings::demo.graphics.displayModes;
    }

    duk_idx_t arr_idx = duk_push_array(ctx);
    for(size_t i = 0; i < displayModes.size(); i++) {
        DisplayMode& displayMode = displayModes[i];

        duk_idx_t display_mode_obj = duk_push_object(ctx);
        duk_push_number(ctx, displayMode.width);
        duk_put_prop_string(ctx, display_mode_obj, "width");
        duk_push_number(ctx, displayMode.height);
        duk_put_prop_string(ctx, display_mode_obj, "height");

        duk_put_prop_index(ctx, arr_idx, i);
    }

    return 1;
}

static int duk_getAudioDevices(duk_context *ctx)
{
    Audio& audio = EnginePlayer::getInstance().getAudio();

    std::vector<std::string> audioDevices = audio.getOutputDevices();

    duk_idx_t arr_idx = duk_push_array(ctx);
    for(size_t i = 0; i < audioDevices.size(); i++) {
        const char *audioDevice = audioDevices[i].c_str();
        duk_push_string(ctx, audioDevice);
        duk_put_prop_index(ctx, arr_idx, i);
    }

    return 1;
}

static int duk_setScriptError(duk_context *ctx)
{
    bool error = static_cast<unsigned int>(duk_get_uint(ctx, 0)) == 1 ? true : false;

    for (auto it : MemoryManager<Script>::getInstance().getResources()) {
        File* file = it.second;
        if (file->getFileScope() != FileScope::CONSTANT) {
            file->setError(error);
        }
    }

    return 0;
}

static int duk_setLoggerPrintState(duk_context *ctx)
{
    const char* printState = (const char*)duk_get_string(ctx, 0);
    setLoggerPrintState(printState);

    return 0;
}

static duk_ret_t duk_loggerTrace(duk_context *ctx)
{
    loggerTrace("%s", duk_get_string(ctx, 0));
    return 0;
}
static duk_ret_t duk_loggerDebug(duk_context *ctx)
{
    loggerDebug("%s", duk_get_string(ctx, 0));
    return 0;
}
static duk_ret_t duk_loggerInfo(duk_context *ctx)
{
    loggerInfo("%s", duk_get_string(ctx, 0));
    return 0;
}
static duk_ret_t duk_loggerWarning(duk_context *ctx)
{
    loggerWarning("%s", duk_get_string(ctx, 0));
    return 0;
}
static duk_ret_t duk_loggerError(duk_context *ctx)
{
    loggerError("%s", duk_get_string(ctx, 0));
    return 0;
}
static duk_ret_t duk_loggerFatal(duk_context *ctx)
{
    loggerFatal("%s", duk_get_string(ctx, 0));
    return 0;
}

static int duk_evalFile(duk_context *ctx)
{
    const char* filePath = duk_get_string(ctx, 0);

    MemoryManager<Script>& scriptMemory = MemoryManager<Script>::getInstance();
    Script *script = scriptMemory.getResource(std::string(filePath));
    if (script != NULL) {
        script->load();
    }

    return 0;  // no return value
}

/*
    glPushMatrix();
    perspective2dBegin(getScreenWidth(), getScreenHeight());
    glTranslatef(getScreenWidth()/2,getScreenWidth()/2,0);
    glRotatef(30,0,0,-1);
    glTranslatef(-getScreenWidth()/2,-getScreenWidth()/2,0);
    var columns = 12;
    var w = getScreenWidth()/8.0;
    var h = getScreenWidth()+400;
    var x = 0;
    var y = -400;

    var fade = Utils.calculateProgress(animation.start+animation.warningStripFadeStart, animation.warningStripFadeDuration);
    for(var i = 0; i < columns; i++)
    {
        var fadePositionY = 0;
        if (i%2 == 0)
        {
            fadePositionY = h*fade;
            glColor3ub(50,50,50);
        }
        else
        {
            fadePositionY = 1-h*fade;
            glColor3ub(0xE8,0xBF,0x28); //nuclear yellowish
        }

        glBegin(GL_TRIANGLES);
        glVertex2f(x,y+fadePositionY);
        glVertex2f(x,y+h+fadePositionY);
        glVertex2f(x+w,y+h+fadePositionY);

        glVertex2f(x+w,y+h+fadePositionY);
        glVertex2f(x,y+h+fadePositionY);
        glVertex2f(x+w,y+fadePositionY);
        glEnd();

        x += w;
    }
    glColor3f(1,1,1);
    perspective2dEnd();
    glPopMatrix();
*/
static int duk_perspective2dBegin(duk_context *ctx)
{
    double w = duk_get_number(ctx, 0);
    double h = duk_get_number(ctx, 1);

    TransformationMatrix::getInstance().perspective2d(w, h);

    return 0;  // no return value
}

static int duk_perspective2dEnd(duk_context *ctx)
{
    TransformationMatrix::getInstance().perspective3d();

    return 0;  // no return value
}

static int duk_glPushMatrix(duk_context *ctx)
{
    TransformationMatrix::getInstance().push();

    return 0;  // no return value
}

static int duk_glPopMatrix(duk_context *ctx)
{
    TransformationMatrix::getInstance().pop();

    return 0;  // no return value
}

static int duk_glPushAttrib(duk_context *ctx)
{
    unsigned int attrib = duk_get_uint(ctx, 0);
    if (attrib != GL_CURRENT_BIT) {
        loggerWarning("Invalid attrib push %u", attrib);
        return 0;
    }

    Graphics::getInstance().pushState();

    return 0;  // no return value
}

static int duk_glPopAttrib(duk_context *ctx)
{
    Graphics::getInstance().popState();

    return 0;  // no return value
}

static int duk_glTranslatef(duk_context *ctx)
{
    double x = duk_get_number(ctx, 0);
    double y = duk_get_number(ctx, 1);
    double z = duk_get_number(ctx, 2);

    TransformationMatrix::getInstance().translate(x, y, z);

    return 0;  // no return value
}

static int duk_glRotatef(duk_context *ctx)
{
    double degrees = duk_get_number(ctx, 0);
    double x = -duk_get_number(ctx, 1);
    double y = -duk_get_number(ctx, 2);
    double z = -duk_get_number(ctx, 3);

    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    if (x != 0.0) { 
        transformationMatrix.rotateX(x * degrees);
    }

    if (y != 0.0) { 
        transformationMatrix.rotateY(y * degrees);
    }

    if (z != 0.0) { 
        transformationMatrix.rotateZ(z * degrees);
    }

    return 0;  // no return value
}

static int duk_glColor4f(duk_context *ctx)
{
    double r = duk_get_number(ctx, 0);
    double g = duk_get_number(ctx, 1);
    double b = duk_get_number(ctx, 2);
    double a = duk_get_number(ctx, 3);

    Graphics::getInstance().setColor(Color(r, g, b, a));

    return 0;  // no return value
}

static int duk_glColor3f(duk_context *ctx)
{
    double r = duk_get_number(ctx, 0);
    double g = duk_get_number(ctx, 1);
    double b = duk_get_number(ctx, 2);
    double a = 1.0;

    Graphics::getInstance().setColor(Color(r, g, b, a));

    return 0;  // no return value
}

static int duk_glColor4ub(duk_context *ctx)
{
    double r = duk_get_number(ctx, 0) / 255.0;
    double g = duk_get_number(ctx, 1) / 255.0;
    double b = duk_get_number(ctx, 2) / 255.0;
    double a = duk_get_number(ctx, 3) / 255.0;

    Graphics::getInstance().setColor(Color(r, g, b, a));

    return 0;  // no return value
}

static int duk_glColor3ub(duk_context *ctx)
{
    double r = duk_get_number(ctx, 0) / 255.0;
    double g = duk_get_number(ctx, 1) / 255.0;
    double b = duk_get_number(ctx, 2) / 255.0;
    double a = 1.0;

    Graphics::getInstance().setColor(Color(r, g, b, a));

    return 0;  // no return value
}

static Mesh mesh;

static int duk_glBegin(duk_context *ctx)
{
    unsigned int mode = duk_get_uint(ctx, 0);
    FaceType faceType = FaceType::TRIANGLES;

    switch(mode) {
        case GL_TRIANGLES:
        default:
            break;
        case GL_TRIANGLE_STRIP:
            faceType = FaceType::TRIANGLE_STRIP;
            break;
        case GL_LINES:
            faceType = FaceType::LINES;
            break;
        case GL_LINE_LOOP:
            faceType = FaceType::LINE_LOOP;
            break;
        case GL_LINE_STRIP:
            faceType = FaceType::LINE_STRIP;
            break;
        case GL_POINTS:
            faceType = FaceType::POINTS;
            break;
    }

    mesh.begin(faceType);

    return 0;  // no return value
}

static int duk_glEnd(duk_context *ctx)
{
    Graphics::getInstance().setColor(Color(1, 1, 1, 1));
    mesh.end();

    return 0;  // no return value
}

static void addColorToVertex() {
    Color& color = Graphics::getInstance().getColor();
    mesh.addColor(color.r, color.g, color.b, color.a);
}

static int duk_glVertex3f(duk_context *ctx)
{
    double x = duk_get_number(ctx, 0);
    double y = duk_get_number(ctx, 1);
    double z = duk_get_number(ctx, 2);

    mesh.addVertex(x, y, z);
    addColorToVertex();


    return 0;  // no return value
}

static int duk_glVertex2f(duk_context *ctx)
{
    double x = duk_get_number(ctx, 0);
    double y = duk_get_number(ctx, 1);

    mesh.addVertex(x, y);
    addColorToVertex();

    return 0;  // no return value
}

static int duk_glTexCoord2f(duk_context *ctx)
{
    double u = duk_get_number(ctx, 0);
    double v = duk_get_number(ctx, 1);

    mesh.addTexCoord(u, v);

    return 0;  // no return value
}

static int duk_glNormal3f(duk_context *ctx)
{
    double x = duk_get_number(ctx, 0);
    double y = duk_get_number(ctx, 1);
    double z = duk_get_number(ctx, 2);

    mesh.addNormal(x, y, z);

    return 0;  // no return value
}

static int duk_glPointSize(duk_context *ctx)
{
    double size = duk_get_number(ctx, 0);

    glPointSize(size);

    return 0;  // no return value
}

static int duk_glLineWidth(duk_context *ctx)
{
    double size = duk_get_number(ctx, 0);

    /*GLint aliasedRange[2], smoothRange[2];
    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, aliasedRange);
    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, smoothRange);

    loggerWarning("glLineWidth(%.2f); %d-%d, %d-%d", size, aliasedRange[0], aliasedRange[1], smoothRange[0], smoothRange[1]);*/
    glLineWidth(size);

    return 0;  // no return value
}

static int duk_glEnable(duk_context *ctx)
{
    unsigned int capability = duk_get_uint(ctx, 0);
    if (capability == NOP_LEGACY_CAPABILITY) {
        return 0;
    }

    glEnable(capability);

    return 0;  // no return value
}

static int duk_glDisable(duk_context *ctx)
{
    unsigned int capability = duk_get_uint(ctx, 0);
    if (capability == NOP_LEGACY_CAPABILITY) {
        return 0;
    }

    glDisable(capability);

    return 0;  // no return value
}

static int duk_glBindTexture(duk_context *ctx)
{
    unsigned int target = duk_get_uint(ctx, 0);
    unsigned int texture = duk_get_uint(ctx, 1);

    glBindTexture(target, texture);

    return 0;  // no return value
}

static int duk_glHint(duk_context *ctx)
{
    unsigned int target = duk_get_uint(ctx, 0);
    unsigned int mode = duk_get_uint(ctx, 1);
    if (target == NOP_LEGACY_CAPABILITY) {
        return 0;
    }

    glHint(target, mode);

    return 0;  // no return value
}


struct CurlCall {
public:
    CurlCall();
    void *jsHeapAddress;
    Curl curl;
};

CurlCall::CurlCall() {
    jsHeapAddress = NULL;
}

static std::vector<std::future<CurlCall*>> curlFutures = std::vector<std::future<CurlCall*>>();
static int duk_processFutures(duk_context *ctx)
{
    auto it = curlFutures.begin();
    while (it != curlFutures.end())
    {
        if (!it->valid()) {
            delete (*it).get();
            it = curlFutures.erase(it);
            continue;
        }

        std::future_status status = it->wait_for(std::chrono::seconds(0));

        if (status == std::future_status::ready) {
            CurlCall *curlCall = (*it).get();
            void *ptrOld = curlCall->jsHeapAddress;

            duk_push_heapptr(ctx, ptrOld);

            std::string data = curlCall->curl.getData();
            duk_push_string(ctx, data.c_str());
            duk_put_prop_string(ctx, -2, "responseText");

            duk_push_uint(ctx, static_cast<unsigned int>(curlCall->curl.getHttpStatus()));
            duk_put_prop_string(ctx, -2, "status");

            duk_push_string(ctx, "_callFinalEvents");
            duk_call_prop(ctx, -2, 0);

            // heap can be freed now regarding the pointer

            delete curlCall;
            it = curlFutures.erase(it);
        } else {
            // in progress (deferred / timeout), do not remove
            ++it;
        }
    }

    return 0;
}

static int duk_curlAsyncSend(duk_context *ctx)
{
    void *ptr = duk_get_heapptr(ctx, 0);

    const char *type = (const char*)duk_get_string(ctx, 1);
    const char *url = (const char*)duk_get_string(ctx, 2);

    std::map<std::string, std::string> headers;
    if (duk_is_object(ctx, 3)) {
        duk_enum(ctx, 3, 0);
        while (duk_next(ctx, -1, 1)) {
            std::string key = duk_to_string(ctx, -2);
            std::string value = duk_to_string(ctx, -1);
            headers[key] = value;
            duk_pop_2(ctx);
        }
        duk_pop(ctx);
    }

    const char *data = (const char*)duk_get_string(ctx, 4);

    curlFutures.push_back(
        std::async([](void *ptr, std::string type, std::string url, std::map<std::string, std::string> headers, std::string data){
            RequestType requestType = Curl::getRequestType(type);

            CurlCall *curlCall = new CurlCall();
            curlCall->curl.open(requestType, url);
            for(const auto &header : headers) {
                curlCall->curl.setRequestHeader(header.first, header.second);
            }
            curlCall->curl.send(data);

            curlCall->jsHeapAddress = ptr;

            return curlCall;
        }, ptr, std::string(type), std::string(url), headers, std::string(data))
    );

    return 0;
}

static int duk_drawTest(duk_context *ctx)
{
    double fade = duk_get_number(ctx, 0); // Utils.calculateProgress(animation.start+animation.warningStripFadeStart, animation.warningStripFadeDuration);

    TransformationMatrix& transformationMatrix = TransformationMatrix::getInstance();
    transformationMatrix.perspective2d();
    transformationMatrix.translate(Settings::demo.graphics.canvasWidth/2, Settings::demo.graphics.canvasHeight/2, 0);

    float radius = 92;
    Mesh m = Mesh();
    int CIRCLE_PRECISION = 10;
    //glFinish();
    //m.begin(FaceType::POINTS);
    m.clear();
    m.setFaceDrawType(FaceType::LINE_LOOP);
    

    glFinish();
    for (int i=0; i < CIRCLE_PRECISION; i++)
    {
        //glColor3f(1,i/CIRCLE_PRECISION,0);
        float alpha = (i+1)/(double)CIRCLE_PRECISION*0.5+0.5;
        float angle = ((i+1)/(double)CIRCLE_PRECISION)*2*3.14159f;
        //glVertex2f(Math.cos(angle) * radius, Math.sin(angle) * radius);
        m.addVertex(cos(angle) * radius, sin(angle) * radius);
        if (i%2==0) {
            m.addColor(1,0,0,alpha);
        } else {
            m.addColor(0,1,0,alpha);
        }
        //glFinish();
    }
    //glFinish();
    m.generate();
    m.print();
    m.draw();
    //m.end();
    glFinish();

    transformationMatrix.perspective3d();

    /*transformationMatrix.push();
    transformationMatrix.perspective2d();

    transformationMatrix.translate(Settings::demo.graphics.canvasWidth/2.0, Settings::demo.graphics.canvasHeight/2.0, 0);
    transformationMatrix.rotateZ(30);
    transformationMatrix.translate(-Settings::demo.graphics.canvasWidth/2.0, -Settings::demo.graphics.canvasHeight/2.0, 0);

    int columns = 12;
    double w = Settings::demo.graphics.canvasWidth / 8.0;
    double h = Settings::demo.graphics.canvasWidth + 400;
    double x = -100;
    double y = -400;

    static Mesh mesh = Mesh(); // = Mesh::getInstance();
    for(int i = 0; i < columns; i++)
    {
        double fadePositionY = 0;
        if (i%2 == 0)
        {
            fadePositionY = h*fade;
            //glColor3ub(50,50,50);
            Graphics::getInstance().setColor(Color(50/255.,50/255.,50/255.,1));
        }
        else
        {
            fadePositionY = 1-h*fade;
            Graphics::getInstance().setColor(Color(0xE8/255.,0xBF/255.,0x28/255.,1));
            //glColor3ub(0xE8,0xBF,0x28); //nuclear yellowish
        }

        mesh.begin(FaceType::TRIANGLES);
        mesh.addVertex(x,y+fadePositionY);
        mesh.addVertex(x,y+h+fadePositionY);
        mesh.addVertex(x+w,y+h+fadePositionY);

        mesh.addVertex(x+w,y+h+fadePositionY);
        mesh.addVertex(x,y+fadePositionY);
        mesh.addVertex(x+w,y+fadePositionY);

        mesh.end();

        x += w;
    }
    //glColor3f(1,1,1);
    Graphics::getInstance().setColor(Color());

    transformationMatrix.perspective3d();
    transformationMatrix.pop();*/

    return 0;  // no return value
}

/*
    glPushMatrix();
    perspective2dBegin(getScreenWidth(), getScreenHeight());
    glTranslatef(getScreenWidth()/2,getScreenWidth()/2,0);
    glRotatef(30,0,0,-1);
    glTranslatef(-getScreenWidth()/2,-getScreenWidth()/2,0);
    var columns = 12;
    var w = getScreenWidth()/8.0;
    var h = getScreenWidth()+400;
    var x = 0;
    var y = -400;

    var fade = Utils.calculateProgress(animation.start+animation.warningStripFadeStart, animation.warningStripFadeDuration);
    for(var i = 0; i < columns; i++)
    {
        var fadePositionY = 0;
        if (i%2 == 0)
        {
            fadePositionY = h*fade;
            glColor3ub(50,50,50);
        }
        else
        {
            fadePositionY = 1-h*fade;
            glColor3ub(0xE8,0xBF,0x28); //nuclear yellowish
        }

        glBegin(GL_QUADS);
        glVertex2f(x,y+fadePositionY);
        glVertex2f(x,y+h+fadePositionY);
        glVertex2f(x+w,y+h+fadePositionY);
        glVertex2f(x+w,y+fadePositionY);
        glEnd();

        x += w;
    }
    glColor3f(1,1,1);
    perspective2dEnd();
    glPopMatrix();
*/

static int duk_syncEditorSetRowsPerBeat(duk_context *ctx)
{
    int rpb = (int)duk_get_int(ctx, 0);

    SyncRocket& sync = dynamic_cast<SyncRocket&>(EnginePlayer::getInstance().getSync());
    sync.setRowsPerBeat(static_cast<double>(rpb));

    return 0;
}

static int duk_syncEditorGetRowsPerBeat(duk_context *ctx)
{
    SyncRocket& sync = dynamic_cast<SyncRocket&>(EnginePlayer::getInstance().getSync());

    duk_push_int(ctx, static_cast<int>(sync.getRowsPerBeat()));

    return 1;
}

static int duk_syncEditorGetTrack(duk_context *ctx)
{
    const char *trackName = (const char*)duk_get_string(ctx, 0);
    SyncRocket& sync = dynamic_cast<SyncRocket&>(EnginePlayer::getInstance().getSync());
    void *ptr = (void*)sync.getSyncTrack(trackName);

    duk_idx_t sync_track_obj = duk_push_object(ctx);
    duk_push_pointer(ctx, ptr);
    duk_put_prop_string(ctx, sync_track_obj, "ptr");
    duk_push_string(ctx, trackName);
    duk_put_prop_string(ctx, sync_track_obj, "name");
    
    return 1;
}

static int duk_syncEditorGetTrackCurrentValue(duk_context *ctx)
{
    void *trackPointer = (void*)duk_get_pointer(ctx, 0);

    SyncRocket& sync = dynamic_cast<SyncRocket&>(EnginePlayer::getInstance().getSync());
    
    double value = sync.getSyncTrackCurrentValue(trackPointer);
    duk_push_number(ctx, (double)value);

    return 1;
}

void bindJsSyncEditorFunctions(duk_context *ctx)
{
    bindCFunctionToJs(syncEditorSetRowsPerBeat, 1);
    bindCFunctionToJs(syncEditorGetRowsPerBeat, 0);
    bindCFunctionToJs(syncEditorGetTrack, 1);
    bindCFunctionToJs(syncEditorGetTrackCurrentValue, 1);
}

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

static int duk_imgui_Combo(duk_context *ctx)
{
    const char *name = duk_get_string(ctx, 0);

    duk_get_prop_string(ctx, 1, "v");
    int v = duk_to_int(ctx, -1);
    duk_pop(ctx);

    const char *items_separated_by_zeros = duk_get_string(ctx, 2);
    int height_in_items = -1;

    ImGui::Combo(name, &v, items_separated_by_zeros, height_in_items);

    duk_push_int(ctx, v);
    duk_put_prop_string(ctx, 1, "v");

    return 0;
}

static int duk_imgui_Begin(duk_context *ctx)
{
    const char *name = duk_get_string(ctx, 0);

    // backwards compatibility with ImGui 1.53 vs. 1.72b
    if (name == NULL || strcmp(name, "") != 0) {
        name = "window";
    }

    duk_get_prop_string(ctx, 1, "v");
    int v = duk_to_boolean(ctx, -1);
    duk_pop(ctx);
    bool ref = v == 1 ? true : false;

    int flags = duk_get_int(ctx, 2);

    ImGui::Begin(name, &ref, flags);

    duk_push_boolean(ctx, ref ? 1 : 0);
    duk_put_prop_string(ctx, 1, "v");

    return 0;
}

static int duk_imgui_InputText(duk_context *ctx)
{
    const char *label = duk_get_string(ctx, 0);

    duk_get_prop_string(ctx, 1, "v");
    const char* v = duk_to_string(ctx, -1);
    duk_pop(ctx);
    duk_get_prop_string(ctx, 1, "size");
    unsigned int size = duk_to_uint(ctx, -1) + 1;
    duk_pop(ctx);

    char *inputData = new char[size];
    snprintf(inputData, size, "%s", v);

    ImGui::InputText(label, inputData, size);

    duk_push_string(ctx, inputData);
    duk_put_prop_string(ctx, 1, "v");

    delete [] inputData;

    return 0;
}


static int duk_imgui_Checkbox(duk_context *ctx)
{
    const char *label = duk_get_string(ctx, 0);

    duk_get_prop_string(ctx, 1, "v");
    int v = duk_to_boolean(ctx, -1);
    duk_pop(ctx);

    bool ref = v == 1 ? true : false;    
    ImGui::Checkbox(label, &ref);

    duk_push_boolean(ctx, ref ? 1 : 0);
    duk_put_prop_string(ctx, 1, "v");

    return 0;
}

static int duk_imgui_SetNextWindowPos(duk_context *ctx)
{
    duk_get_prop_string(ctx, 0, "x");
    double x = duk_to_number(ctx, -1);
    duk_pop(ctx);
    duk_get_prop_string(ctx, 0, "y");
    double y = duk_to_number(ctx, -1);
    duk_pop(ctx);

    unsigned int conditions = duk_get_uint(ctx, 1);

    ImGui::SetNextWindowPos(ImVec2(x, y), conditions);

    return 0;
}

static int duk_imgui_SetNextWindowSize(duk_context *ctx)
{
    duk_get_prop_string(ctx, 0, "x");
    double x = duk_to_number(ctx, -1);
    duk_pop(ctx);
    duk_get_prop_string(ctx, 0, "y");
    double y = duk_to_number(ctx, -1);
    duk_pop(ctx);

    unsigned int conditions = duk_get_uint(ctx, 1);

    ImGui::SetNextWindowSize(ImVec2(x, y), conditions);

    return 0;
}

static int duk_imgui_SetWindowFontScale(duk_context *ctx)
{
    double scale = duk_get_number(ctx, 0);

    ImGui::SetWindowFontScale(scale);

    return 0;
}


static int duk_imgui_PushItemWidth(duk_context *ctx)
{
    double value = duk_get_number(ctx, 0);

    ImGui::PushItemWidth(value);

    return 0;
}

static int duk_imgui_PopItemWidth(duk_context *ctx)
{
    ImGui::PopItemWidth();

    return 0;
}

static int duk_imgui_BeginGroup(duk_context *ctx)
{
    ImGui::BeginGroup();

    return 0;
}

static int duk_imgui_EndGroup(duk_context *ctx)
{
    ImGui::EndGroup();

    return 0;
}

static int duk_imgui_Separator(duk_context *ctx)
{
    ImGui::Separator();

    return 0;
}

static int duk_imgui_PushStyleVar(duk_context *ctx)
{
    int idx = duk_get_int(ctx, 0);
    double value = duk_get_number(ctx, 1);

    ImGui::PushStyleVar(idx, value);

    return 0;
}

static int duk_imgui_Button(duk_context *ctx)
{
    const char *value = duk_get_string(ctx, 0);

    bool ret = ImGui::Button(value);

    duk_push_boolean(ctx, ret ? 1 : 0);

    return 1;
}

static int duk_imgui_GetKeyIndex(duk_context *ctx)
{
    ImGuiKey key = duk_get_int(ctx, 0);

    int userKey = ImGui::GetKeyIndex(key);

    duk_push_int(ctx, userKey);

    return 1;
}

static int duk_imgui_IsKeyPressed(duk_context *ctx)
{
    int userKeyIndex = duk_get_int(ctx, 0);
    bool repeat = duk_get_boolean(ctx, 1);

    bool ret = ImGui::IsKeyPressed(userKeyIndex, repeat);

    duk_push_boolean(ctx, ret ? 1 : 0);

    return 1;
}

static int duk_imgui_SameLine(duk_context *ctx)
{
    ImGui::SameLine();

    return 0;
}

static int duk_imgui_End(duk_context *ctx)
{
    ImGui::End();

    return 0;
}

static int duk_imgui_PopStyleVar(duk_context *ctx)
{
    ImGui::PopStyleVar();

    return 0;
}

static int duk_imgui_Render(duk_context *ctx)
{
    ImGui::Render();

    return 0;
}

#define duk_push_imgui_constant_property(ctx, imgui_constant, imgui_value) \
    duk_push_int((ctx), (imgui_value)); \
    duk_put_prop_string((ctx), -2, #imgui_constant)

#define bindImGuiCFunctionToJs(cFunction, argumentCount) \
  duk_push_c_function(ctx, duk_imgui_##cFunction, argumentCount); \
  duk_put_prop_string(ctx, -2, #cFunction)

void bindImGuiModule(duk_context *ctx)
{
    duk_push_object(ctx);

    duk_push_object(ctx);
    duk_push_imgui_constant_property(ctx, Always, ImGuiCond_Always);
    duk_put_prop_string(ctx, -2, "Cond");

    // backwards compatibility with ImGui 1.53 vs. 1.72b
    duk_push_object(ctx);
    duk_push_imgui_constant_property(ctx, Always, ImGuiCond_Always);
    duk_put_prop_string(ctx, -2, "SetCond");

    duk_push_object(ctx);
    duk_push_imgui_constant_property(ctx, WindowRounding, ImGuiStyleVar_WindowRounding);
    duk_put_prop_string(ctx, -2, "StyleVar");

    duk_push_object(ctx);
    duk_push_imgui_constant_property(ctx, NoSavedSettings, ImGuiWindowFlags_NoSavedSettings);
    duk_push_imgui_constant_property(ctx, NoTitleBar, ImGuiWindowFlags_NoTitleBar);
    duk_push_imgui_constant_property(ctx, NoResize, ImGuiWindowFlags_NoResize);
    duk_push_imgui_constant_property(ctx, NoMove, ImGuiWindowFlags_NoMove);
    duk_push_imgui_constant_property(ctx, NoCollapse, ImGuiWindowFlags_NoCollapse);
    duk_put_prop_string(ctx, -2, "WindowFlags");

    duk_push_object(ctx);
    duk_push_imgui_constant_property(ctx, Tab, ImGuiKey_Tab);
    duk_push_imgui_constant_property(ctx, LeftArrow, ImGuiKey_LeftArrow);
    duk_push_imgui_constant_property(ctx, RightArrow, ImGuiKey_RightArrow);
    duk_push_imgui_constant_property(ctx, UpArrow, ImGuiKey_UpArrow);
    duk_push_imgui_constant_property(ctx, DownArrow, ImGuiKey_DownArrow);
    duk_push_imgui_constant_property(ctx, PageUp, ImGuiKey_PageUp);
    duk_push_imgui_constant_property(ctx, PageDown, ImGuiKey_PageDown);
    duk_push_imgui_constant_property(ctx, Home, ImGuiKey_Home);
    duk_push_imgui_constant_property(ctx, End, ImGuiKey_End);
    duk_push_imgui_constant_property(ctx, Delete, ImGuiKey_Delete);
    duk_push_imgui_constant_property(ctx, Backspace, ImGuiKey_Backspace);
    duk_push_imgui_constant_property(ctx, Enter, ImGuiKey_Enter);
    duk_push_imgui_constant_property(ctx, Escape, ImGuiKey_Escape);
    duk_push_imgui_constant_property(ctx, A, ImGuiKey_A);
    duk_push_imgui_constant_property(ctx, C, ImGuiKey_C);
    duk_push_imgui_constant_property(ctx, V, ImGuiKey_V);
    duk_push_imgui_constant_property(ctx, X, ImGuiKey_X);
    duk_push_imgui_constant_property(ctx, Y, ImGuiKey_Y);
    duk_push_imgui_constant_property(ctx, Z, ImGuiKey_Z);
    duk_push_imgui_constant_property(ctx, COUNT, ImGuiKey_COUNT);
    duk_put_prop_string(ctx, -2, "Key");

    bindImGuiCFunctionToJs(SetNextWindowPos, 2);
    bindImGuiCFunctionToJs(SetNextWindowSize, 2);
    bindImGuiCFunctionToJs(PushItemWidth, 1);
    bindImGuiCFunctionToJs(PopItemWidth, 0);
    bindImGuiCFunctionToJs(BeginGroup, 0);
    bindImGuiCFunctionToJs(EndGroup, 0);
    bindImGuiCFunctionToJs(Separator, 0);
    bindImGuiCFunctionToJs(PushStyleVar, 2);
    bindImGuiCFunctionToJs(SetWindowFontScale, 1);

    bindImGuiCFunctionToJs(Button, 1);
    bindImGuiCFunctionToJs(InputText, 2);
    bindImGuiCFunctionToJs(Checkbox, 2);
    bindImGuiCFunctionToJs(Begin, 3);
    bindImGuiCFunctionToJs(Combo, 3);

    bindImGuiCFunctionToJs(GetKeyIndex, 1);
    bindImGuiCFunctionToJs(IsKeyPressed, 2);

    bindImGuiCFunctionToJs(SameLine, 0);
    bindImGuiCFunctionToJs(End, 0);
    bindImGuiCFunctionToJs(PopStyleVar, 0);
    bindImGuiCFunctionToJs(Render, 0);
    
    duk_put_prop_string(ctx, -2, "ImGui");
}

static int duk_socketNew(duk_context *ctx)
{
    Socket* sock = new Socket();
    duk_push_pointer(ctx, static_cast<void*>(sock));

    return 1;
}

static int duk_socketDelete(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));
    if (sock) {
        delete sock;
    }

    return 0;
}

static int duk_socketSetHost(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));
    const char *host = duk_get_string(ctx, 1);
    sock->setHost(host);

    return 0;
}

static int duk_socketSetPort(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));
    unsigned short port = static_cast<unsigned short>(duk_get_uint(ctx, 1));
    sock->setPort(port);

    return 0;
}

static int duk_socketSetType(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));
    unsigned int type = duk_get_uint(ctx, 1);
    SocketType sockType = SocketType::TCP;
    switch(type) {
        case 1:
            sockType = SocketType::UDP;
            break;
        case 0:
        default:
            sockType = SocketType::TCP;
            break;
    }
    sock->setType(sockType);

    return 0;
}

static int duk_socketEstablishConnection(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));

    bool ret = sock->establishConnection();

    duk_push_boolean(ctx, ret ? 1 : 0);
    return 1;
}

static int duk_socketCloseConnection(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));

    bool ret = sock->closeConnection();

    duk_push_boolean(ctx, ret ? 1 : 0);
    return 1;
}

static int duk_socketSendData(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));
    duk_size_t arraySize = 0;
    void *array = duk_get_buffer_data(ctx, 1, &arraySize);

    bool ret = sock->sendData(array, (size_t)arraySize);

    duk_push_boolean(ctx, ret ? 1 : 0);
    return 1;
}

static int duk_socketReceiveData(duk_context *ctx)
{
    Socket* sock = static_cast<Socket*>(duk_get_pointer(ctx, 0));
    duk_size_t arraySize = 0;
    void *array = duk_get_buffer_data(ctx, 1, &arraySize);

    bool ret = sock->receiveData(array, (size_t)arraySize);

    duk_push_boolean(ctx, ret ? 1 : 0);
    return 1;
}

static int duk_midiManagerAddMidiController(duk_context *ctx)
{
    MidiManager &midiManager = MidiManager::getInstance();
    midiManager.addMidiController();

    return 0;
}

static int duk_midiManagerGetMidiControllers(duk_context *ctx)
{
    MidiManager &midiManager = MidiManager::getInstance();
    std::vector<MidiController*> midiControllers = midiManager.getMidiControllers();

    duk_idx_t arr_idx = duk_push_array(ctx);
    for(size_t i = 0; i < midiControllers.size(); i++) {
        MidiController* midiController = midiControllers[i];

        duk_idx_t midi_controller_obj = duk_push_object(ctx);

        duk_push_pointer(ctx, static_cast<void*>(midiController));
        duk_put_prop_string(ctx, midi_controller_obj, "ptr");

        duk_put_prop_index(ctx, arr_idx, i);
    }

    return 1;
}


static int duk_midiManagerPollEvents(duk_context *ctx)
{
    MidiManager &midiManager = MidiManager::getInstance();
    std::vector<MidiController*> midiControllers = midiManager.getMidiControllers();

    duk_idx_t arr_idx = duk_push_array(ctx);

    size_t array_i = 0;

    for(MidiController* midiController : midiControllers) {

        std::vector<MidiEvent> events = midiController->pollEvents();
        if (!events.empty()) {
            for(MidiEvent event : events) {
                std::string eventJsonData = event.serialize();
                duk_push_string(ctx, eventJsonData.c_str());
                duk_json_decode(ctx, -1);
                duk_put_prop_index(ctx, arr_idx, array_i);

                array_i++;
            }
        }
    }

    return 1;
}



/*
MidiManager:
    MidiController* addMidiController();
    std::vector<MidiController*> getMidiControllers();

MidiController:
    static MidiController* newInstance();
    virtual ~MidiController() {};
    virtual bool init() = 0;
    virtual bool exit() = 0;
    virtual std::vector<MidiPort> listAvailablePorts() = 0;
    virtual bool connect(unsigned int portNumber) = 0;
    virtual bool disconnect() = 0;
    virtual void addEvent(const MidiEvent& event) = 0;
    virtual void addEvent( double deltaTime, std::vector< unsigned char > *message );
    virtual std::vector<MidiEvent> pollEvents();
protected:
    MidiPort port;
    std::vector<MidiEvent> events;
};
*/

static GLuint dukwebgl_get_object_id_uint2(duk_context *ctx, duk_idx_t obj_idx) {
    GLuint ret = 0;

    /* everything else than object assumed null */
    if (duk_is_object(ctx, obj_idx)) {
        duk_get_prop_string(ctx, obj_idx, "_id");
        ret = (GLuint)duk_to_uint(ctx, -1);
        duk_pop(ctx);
    }

    return ret;
}
static GLint dukwebgl_get_object_id_int2(duk_context *ctx, duk_idx_t obj_idx) {
    GLint ret = 0;

    /* everything else than object assumed null */
    if (duk_is_object(ctx, obj_idx)) {
        duk_get_prop_string(ctx, obj_idx, "_id");
        ret = (GLint)duk_to_int(ctx, -1);
        duk_pop(ctx);
    }

    return ret;
}
static GLfloat fix[16] = {0.0010416667209938169,0,0,0,0,0.0018518518190830946,0,0,0,0,1,0,-1,-1,0,1};
static duk_ret_t duk_MATRIX4FV(duk_context *ctx) {
    GLint location = dukwebgl_get_object_id_int2(ctx, 0);
    GLboolean transpose = (GLboolean)(duk_get_boolean(ctx, 1) == 1 ? GL_TRUE : GL_FALSE);
    duk_size_t count = 0;
    const GLfloat *value = (const GLfloat *)duk_get_buffer_data(ctx, 2, &count);

    count = 16; value = fix;

    loggerInfo("MATRIXUNIFORM:%d,%d,%zu,%p",location,transpose,count,value);
    glUniformMatrix4fv (location, 1, transpose, value);
    //glUniformMatrix4fv(uniformId, 1, GL_FALSE, TransformationMatrix::getInstance().getProjectionMatrix());
    return 0;
}



void ScriptEngineDuktape::bindFunctions()
{
    // WebGL bindings
    dukwebgl_bind(ctx);

    duk_push_global_object(ctx);

    bindImGuiModule(ctx);

    bindJsSyncEditorFunctions(ctx);

    bindCFunctionToJs(MATRIX4FV, 0);

    bindCFunctionToJs(midiManagerPollEvents, 0);

    bindCFunctionToJs(drawTest, 1);
    bindCFunctionToJs(curlAsyncSend, 5);
    bindCFunctionToJs(processFutures, 0);

    bindCFunctionToJs(socketNew, 0);
    bindCFunctionToJs(socketDelete, 1);
    bindCFunctionToJs(socketSetHost, 2);
    bindCFunctionToJs(socketSetPort, 2);
    bindCFunctionToJs(socketSetType, 2);
    bindCFunctionToJs(socketEstablishConnection, 1);
    bindCFunctionToJs(socketCloseConnection, 1);
    bindCFunctionToJs(socketSendData, 2);
    bindCFunctionToJs(socketReceiveData, 2);

    bindCFunctionToJs(graphicsHandleErrors, 0);
    bindCFunctionToJs(getDisplayModes, 0);
    bindCFunctionToJs(getAudioDevices, 0);
    bindCFunctionToJs(setScriptError, 1);
    bindCFunctionToJs(setLoggerPrintState, 1);
    bindCFunctionToJs(loggerTrace, 1);
    bindCFunctionToJs(loggerDebug, 1);
    bindCFunctionToJs(loggerInfo, 1);
    bindCFunctionToJs(loggerWarning, 1);
    bindCFunctionToJs(loggerError, 1);
    bindCFunctionToJs(loggerFatal, 1);

    bindCFunctionToJs(meshNew, 0);
    bindCFunctionToJs(meshDelete, 1);
    bindCFunctionToJs(meshMaterialSetTexture, 3);
    bindCFunctionToJs(meshSetFaceDrawType, 2);
    bindCFunctionToJs(meshGenerate, 1);
    bindCFunctionToJs(meshDraw, 3);
    bindCFunctionToJs(meshClear, 1);
    bindCFunctionToJs(meshAddColor, 5);
    bindCFunctionToJs(meshAddIndex, 2);
    bindCFunctionToJs(meshAddVertex, 4);
    bindCFunctionToJs(meshAddTexCoord, 3);
    bindCFunctionToJs(meshAddNormal, 4);

    bindCFunctionToJs(perspective2dBegin, 2);
    bindCFunctionToJs(perspective2dEnd, 0);
    bindCFunctionToJs(glTexCoord2f, 2);
    bindCFunctionToJs(glNormal3f, 3);
    bindCFunctionToJs(glPointSize, 1);
    bindCFunctionToJs(glLineWidth, 1);
    bindCFunctionToJs(glEnable, 1);
    bindCFunctionToJs(glDisable, 1);
    bindCFunctionToJs(glBindTexture, 2);
    bindCFunctionToJs(glHint, 2);
    bindCFunctionToJs(glPushMatrix, 0);
    bindCFunctionToJs(glPopMatrix, 0);
    bindCFunctionToJs(glPushAttrib, 1);
    bindCFunctionToJs(glPopAttrib, 0);
    bindCFunctionToJs(glTranslatef, 3);
    bindCFunctionToJs(glRotatef, 4);
    bindCFunctionToJs(glColor4f, 4);
    bindCFunctionToJs(glColor3f, 3);
    bindCFunctionToJs(glColor4ub, 4);
    bindCFunctionToJs(glColor3ub, 4);
    bindCFunctionToJs(glBegin, 1);
    bindCFunctionToJs(glEnd, 0);
    bindCFunctionToJs(glVertex2f, 2);
    bindCFunctionToJs(glVertex3f, 3);


    bindCFunctionToJs(getScreenWidth, 0);
    bindCFunctionToJs(getScreenHeight, 0);

    bindCFunctionToJs(settingsWindowSetWindowDimensions, 2);
    bindCFunctionToJs(settingsLoadSettingsFromString, 1);
    bindCFunctionToJs(settingsDemoLoadDemoSettingsFromString, 1);
    bindCFunctionToJs(settingsDemoSaveDemoSettings, 0);
    bindCFunctionToJs(settingsSaveSettings, 0);

    bindCFunctionToJs(inputSetUserExit, 1);
    bindCFunctionToJs(inputIsUserExit, 0);
    bindCFunctionToJs(inputPollEvents, 0);
    bindCFunctionToJs(inputGetPressedKeyMap, 0);
    bindCFunctionToJs(menuSetQuit, 1);
    bindCFunctionToJs(menuGetWidth, 0);
    bindCFunctionToJs(menuGetHeight, 0);

    bindCFunctionToJs(timerSetBeatsPerMinute, 1);
    bindCFunctionToJs(timerGetBeatsPerMinute, 0);
    bindCFunctionToJs(timerGetBeatInSeconds, 0);
    
    bindCFunctionToJs(setTextPivot, 3);
    bindCFunctionToJs(setTextRotation, 3);
    bindCFunctionToJs(setTextColor, 4);
    bindCFunctionToJs(setTextSize, 3);
    bindCFunctionToJs(setTextDefaults, 0);
    bindCFunctionToJs(setTextFont, 1);
    bindCFunctionToJs(setTextPosition, 3);
    bindCFunctionToJs(setTextCenterAlignment, 1);
    bindCFunctionToJs(setDrawTextString, 1);
    bindCFunctionToJs(setTextPerspective3d, 1);
    bindCFunctionToJs(drawText, 0);

    //engine function binding
    bindCFunctionToJs(shaderProgramLoad, 1);
    bindCFunctionToJs(getShaderProgramFromMemory, 1);
    bindCFunctionToJs(shaderLoad, 2);
    bindCFunctionToJs(shaderProgramAddShaderByName, 2);
    bindCFunctionToJs(shaderProgramAttachAndLink, 1);

    //OpenGL external function binding
    bindCFunctionToJs(getUniformLocation, 1);
    bindCFunctionToJs(glUniformf, DUK_VARARGS);
    bindCFunctionToJs(glUniformi, DUK_VARARGS);
    bindCFunctionToJs(disableShaderProgram, 1);
    bindCFunctionToJs(activateShaderProgram, 1);
    bindCFunctionToJs(shaderProgramUse, 1);

    bindCFunctionToJs(evalFile, 1);
    //bindJsMiscellaneousFunctions(ctx);

    bindCFunctionToJs(setPlaylistMusic, 1);
    bindCFunctionToJs(setPlaylistLength, 1);
    bindCFunctionToJs(windowSetTitle, 1);
    bindCFunctionToJs(setScreenDimensions, 2);
    bindCFunctionToJs(setWindowScreenAreaAspectRatio, 2);

    bindCFunctionToJs(setClearColor, 4);

    bindCFunctionToJs(setPerspective3d, 1);

    bindCFunctionToJs(loadObject, 1);
    bindCFunctionToJs(drawObject, DUK_VARARGS);
    bindCFunctionToJs(setObjectScale, 4);
    bindCFunctionToJs(setObjectPosition, 4);
    bindCFunctionToJs(setObjectPivot, 4);
    bindCFunctionToJs(setObjectRotation, 7);
    bindCFunctionToJs(setObjectColor, 5);

    bindCFunctionToJs(setObjectNodeScale, 5);
    bindCFunctionToJs(setObjectNodePosition, 5);
    bindCFunctionToJs(setObjectNodeRotation, 8);

    bindCFunctionToJs(getCameraAspectRatio, 0);
    bindCFunctionToJs(setCameraPerspective, 4);
    bindCFunctionToJs(setCameraPosition, 3);
    bindCFunctionToJs(setCameraLookAt, 3);
    bindCFunctionToJs(setCameraUpVector, 3);

    bindCFunctionToJs(lightSetAmbientColor, 5);
    bindCFunctionToJs(lightSetDiffuseColor, 5);
    bindCFunctionToJs(lightSetSpecularColor, 5);

    bindCFunctionToJs(lightSetPosition, 4);
    bindCFunctionToJs(lightSetDirection, 4);
    bindCFunctionToJs(lightSetOn, 1);
    bindCFunctionToJs(lightSetOff, 1);
    bindCFunctionToJs(lightSetGenerateShadowMap, 2);
    bindCFunctionToJs(lightSetType, 2);

    bindCFunctionToJs(audioLoad, 1);
    bindCFunctionToJs(audioPlay, 1);
    bindCFunctionToJs(audioSetDuration, 2);
    bindCFunctionToJs(audioStop, 0);

    bindCFunctionToJs(videoLoad, 1);
    bindCFunctionToJs(videoSetStartTime, 2);
    bindCFunctionToJs(videoSetSpeed, 2);
    bindCFunctionToJs(videoSetFps, 2);
    bindCFunctionToJs(videoSetLoop, 2);
    bindCFunctionToJs(videoSetLength, 2);
    bindCFunctionToJs(videoPlay, 1);
    bindCFunctionToJs(videoDraw, 1);

    bindCFunctionToJs(imageLoadImage, 1);
    bindCFunctionToJs(getSceneStartTime, 0);
    bindCFunctionToJs(getSceneEndTime, 0);
    bindCFunctionToJs(getSceneTimeFromStart, 0);
    bindCFunctionToJs(getSceneProgressPercent, 0);
    bindCFunctionToJs(setResourceCount, 1);
    bindCFunctionToJs(notifyResourceLoaded, 0);

    bindCFunctionToJs(setTextureSizeToScreenSize, 1);
    bindCFunctionToJs(setTextureCenterAlignment, 2);
    //bindCFunctionToJs(setTextureDefaults, 1);
    bindCFunctionToJs(setTexturePerspective3d, 2);
    bindCFunctionToJs(setTextureRotation, 7);
    bindCFunctionToJs(setTextureUnitTexture, 3);
    bindCFunctionToJs(setTextureScale, 4);
    bindCFunctionToJs(setTexturePosition, 4);
    bindCFunctionToJs(setTextureColor, 5);
    bindCFunctionToJs(drawTexture, 1);

    bindCFunctionToJs(fboInit, 1);
    bindCFunctionToJs(fboBind, DUK_VARARGS);
    bindCFunctionToJs(fboUnbind, 1);
    //bindCFunctionToJs(fboDeinit, 1);
    //bindCFunctionToJs(fboStoreDepth, 2);
    //bindCFunctionToJs(fboSetDimensions, 3);
    bindCFunctionToJs(fboGenerateFramebuffer, 1);
    //bindCFunctionToJs(fboSetRenderDimensions, 3);
    bindCFunctionToJs(fboGetWidth, 1);
    bindCFunctionToJs(fboGetHeight, 1);
    bindCFunctionToJs(fboUpdateViewport, DUK_VARARGS);
    bindCFunctionToJs(fboBindTextures, DUK_VARARGS);
    bindCFunctionToJs(fboUnbindTextures, 1);

    bindCFunctionToJs(interpolateLinear, 3);
    bindCFunctionToJs(interpolateSmoothStep, 3);
    bindCFunctionToJs(interpolateSmootherStep, 3);
    bindCFunctionToJs(interpolate, DUK_VARARGS);
    
    bindNewCFunctionToJs(random, DUK_VARARGS);

    setOpenGlConstants(ctx);

    duk_pop(ctx);
        
    //duk_gl_push_opengl_bindings(ctx);
}

bool ScriptEngineDuktape::evalString(const char *string)
{
    bool success = true;
    duk_push_string(ctx, string);
    duk_int_t returnValue = duk_peval(ctx);
    if (returnValue != DUK_EXEC_SUCCESS)
    {
        success = false;
        loggerError("eval failed for '%s': %s\n", string, duk_safe_to_string(ctx, -1));
        //windowSetTitle("JS ERROR");
    }
    duk_pop(ctx);

    return success;

    /*if (returnValue != DUK_EXEC_SUCCESS && !stackTraceCalled)
    {
        evalString("Utils.debugPrintStackTrace();");
        stackTraceCalled = 1;
    }*/
}

bool ScriptEngineDuktape::evalScript(Script &file)
{
    bool success = true;
    duk_push_string(ctx, reinterpret_cast<const char *>(file.getData()));
    duk_int_t returnValue = duk_peval(ctx);
    if (returnValue != DUK_EXEC_SUCCESS)
    {
        success = false;
        loggerError("Error in '%s': %s\n", file.getFilePath().c_str(), duk_safe_to_string(ctx, -1));
        //windowSetTitle("JS ERROR");
    }
    duk_pop(ctx);

    file.setError(!success);

    return success;

    /*bool success = true;
    duk_int_t returnValue = duk_peval_file(ctx, file.getFilePath().c_str());
    if (returnValue != DUK_EXEC_SUCCESS)
    {
        success = false;
        loggerError("Error in '%s': %s\n", file.getFilePath().c_str(), duk_safe_to_string(ctx, -1));
        //windowSetTitle("JS ERROR");
    }
    duk_pop(ctx);

    return success;*/
    /*if (returnValue != DUK_EXEC_SUCCESS && !stackTraceCalled)
    {
        evalString("Utils.debugPrintStackTrace();");
        stackTraceCalled = 1;
    }*/
}

bool ScriptEngineDuktape::init()
{
    loggerDebug("Initializing scripting.");
    ctx = duk_create_heap_default();
    if (!ctx) {
        loggerFatal("Failed to create heap.");
        return false;
    }

    bindFunctions();

    //preInitEngine();

    return true;
}

void ScriptEngineDuktape::synchronizeSettings() {
    std::string jsonString = "var Settings = " + Settings::serialize();
    evalString(jsonString.c_str());

    jsonString = "Settings.demo = " + Settings::demo.serialize();
    loggerTrace("Synchronizing settings: %s", jsonString.c_str());
    evalString(jsonString.c_str());
}

bool ScriptEngineDuktape::callClassMethod(const char *className, const char *methodName, const char *effectClassName)
{
    duk_push_global_object(ctx);
    duk_push_string(ctx, className);
    duk_get_prop(ctx, -2);
    duk_push_string(ctx, methodName);
    duk_get_prop(ctx, -2);
    duk_push_string(ctx, effectClassName);
    
    duk_int_t returnValue = duk_pcall(ctx, 1); //calls: class.method("effectClassName")

    if (returnValue != DUK_EXEC_SUCCESS)
    {
        loggerError("eval failed for '%s.%s(\"%s\")': %s\n", className, methodName, effectClassName, duk_safe_to_string(ctx, -1));
        //windowSetTitle("JS ERROR");
    }
    duk_pop_n(ctx, 3);
    
    /*if (returnValue != DUK_EXEC_SUCCESS && !stackTraceCalled)
    {
        evalString("Utils.debugPrintStackTrace();");
        stackTraceCalled = 1;
        return false;
    }*/

    return true;
}

void ScriptEngineDuktape::garbageCollect() {
    texturedQuads.clear(); // FIXME: not a good location but probably quite senseful for refreshing

    //Duktape instructs to call garbage collection twice
    duk_gc(ctx, 0);
    duk_gc(ctx, 0);
}

bool ScriptEngineDuktape::exit()
{
    loggerDebug("Deinitializing scripting.");

    texturedQuads.clear();

    duk_destroy_heap(ctx);

    return true;
}

/*

// TODO: Demo engine specific stuff. Separate to different class?
void ScriptEngineDuktape::preInitEngine()
{
    if (fileExists("engine.js"))
    {
        evalScript("engine.js"); //hack to enable --demoPath argument usage without needing to add engine.js to the data directory
    }
    else
    {
        evalScript("js/engine.js");
    }
    evalString("Settings.processDemoScript();");
}

void ScriptEngineDuktape::initEngine()
{
    garbageCollect();
    preInitEngine();
    evalString("Settings.processPlayer();");
}

bool ScriptEngineDuktape::callClassMethod(const char *class, const char *method, const char *effectClassName)
{
    duk_push_global_object(ctx);
    duk_push_string(ctx, class);
    duk_get_prop(ctx, -2);
    duk_push_string(ctx, method);
    duk_get_prop(ctx, -2);
    duk_push_string(ctx, effectClassName);
    
    duk_int_t returnValue = duk_pcall(ctx, 1); //calls: class.method("effectClassName")

    if (returnValue != DUK_EXEC_SUCCESS)
    {
        loggerError("eval failed for '%s.%s(\"%s\")': %s\n", class, method, effectClassName, duk_safe_to_string(ctx, -1));
        //windowSetTitle("JS ERROR");
    }
    duk_pop_n(ctx, 3);
    
    if (returnValue != DUK_EXEC_SUCCESS && !stackTraceCalled)
    {
        evalString("Utils.debugPrintStackTrace();");
        stackTraceCalled = 1;
        return false;
    }

    return true;
}

*/
