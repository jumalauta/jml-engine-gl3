function debugErrorPrint(msg) { loggerError(msg); }
function debugWarningPrint(msg) { loggerWarning(msg); }
function debugInfoPrint(msg) { loggerInfo(msg); }
function debugPrint(msg) { loggerDebug(msg); }
function threadWaitAsyncCalls() { }

function loadObjectBasicShape(a,b) { loggerWarning("loadObjectBasicShape not implemented"); }
function useObjectLighting(ptr, value) { loggerWarning("useObjectLighting not implemented"); }
function useSimpleColors(ptr, value) { loggerWarning("useSimpleColors not implemented"); }
function useObjectCamera(ptr, value) { loggerWarning("useObjectCamera not implemented"); }
function fboStoreDepth(ptr, value) { loggerWarning("fboStoreDepth not implemented"); }
function fboSetDimensions(ptr, value, value2) { loggerWarning("fboSetDimensions not implemented"); }
function useObjectLighting(ptr, value) { loggerWarning("useObjectLighting not implemented"); }
function useSimpleColors(ptr, value) { loggerWarning("useSimpleColors not implemented"); }
function useObjectCamera(ptr, value) { loggerWarning("useObjectCamera not implemented"); }
function fboSetDimensions(ptr, width, height) { loggerWarning("fboSetDimensions not implemented"); }
function timerSetTargetFps(fps) { loggerWarning("timerSetTargetFps not implemented"); }
function addPlayerEffect(name, reference) { loggerWarning("addPlayerEffect not implemented"); }
function addPlayerScene(name1, name, effect, startTime, durationTime) { loggerWarning("addPlayerScene not implemented"); }
function convertTimeToSeconds(start) { loggerWarning("convertTimeToSeconds not implemented"); }
function fboSetRenderDimensions(ptr, x, y) { loggerWarning("fboSetRenderDimensions not implemented"); }
function getObjectFromMemory(lightRelativePosition) { loggerWarning("getObjectFromMemory not implemented"); }
function getObjectFromMemory(cameraRelativeTarget) { loggerWarning("getObjectFromMemory not implemented"); }
function getObjectFromMemory(cameraRelativePosition) { loggerWarning("getObjectFromMemory not implemented"); }

function lightSetPositionObject(index) { loggerWarning("lightSetPositionObject not implemented"); }
function lightSetPositionObject(index, ptr) { loggerWarning("lightSetPositionObject not implemented"); }
function setCameraPositionObject() { loggerWarning("setCameraPositionObject not implemented"); }
function setCameraPositionObject(ptr) { loggerWarning("setCameraPositionObject not implemented"); }
function setCameraTargetObject() { loggerWarning("setCameraTargetObject not implemented"); }
function setCameraTargetObject(ptr) { loggerWarning("setCameraTargetObject not implemented"); }

var Constants = function()
{
};

Constants.Align = {
    'NONE': 0 ,
    'CENTER': 1 ,
    'HORIZONTAL': 2 ,
    'VERTICAL': 3 ,
    'LEFT': 4 ,
    'RIGHT': 5
};

// FIXME: Is Settings needed anymore?

var Settings = function()
{
};

Settings.sceneVariables = {};

Settings.processVariables = function(scene)
{
    /*if (scene.hasOwnProperty('variables'))
    {
        Settings.sceneVariables[scene.name] = scene.variables;

        for (var variableName in scene.variables)
        {
            var variable = scene.variables[variableName];
            addTwVariable(scene.name, variableName, variable.type, variable.definition);
        }
    }*/
};

Settings.processDemoScript = function()
{
    /*Settings.sceneVariables = {};

    if (Settings.demoScript === void null) {
        loggerDebug("Settings.demoScript not initialized, no script.js? Initializing default");

        Settings.demoScript = {
            "clearColor": {
                "r": 0.0,
                "g": 0.0,
                "b": 0.0,
                "a": 0.0
            },
            "beatsPerMinute": 100,
            "rowsPerBeat": 8
        }
    }

    if (Settings.demoScript.music === void null) {
        Settings.demoScript.music = "music.ogg";
    }
    setPlaylistMusic(Settings.demoScript.music);

    var demoLength = Settings.demoScript.totalTime;
    if (demoLength !== undefined) {
        setPlaylistLength(demoLength);
    }

    //jsSetUseInput(Settings.demoScript.useInput === true ? 1 : 0);

    if (Settings.demoScript.screen !== void null)
    {
        setScreenDimensions(Settings.demoScript.screen.width, Settings.demoScript.screen.height);

        if (Settings.demoScript.screen.aspectRatio !== void null)
        {
            setWindowScreenAreaAspectRatio(Settings.demoScript.screen.aspectRatio.width, Settings.demoScript.screen.aspectRatio.height);
        }
    }

    if (Settings.demoScript.jumalauta === true)
    {
        windowSetTitle('Jumalauta!');
    }

    if (Settings.demoScript.fps !== void null)
    {
        timerSetTargetFps(Settings.demoScript.fps);
    }

    if (Settings.demoScript.window !== void null)
    {
        if (Settings.demoScript.window.title !== void null)
        {
            windowSetTitle(Settings.demoScript.window.title);
        }
    }

    if (Settings.demoScript.clearColor !== void null)
    {
        var color = Settings.demoScript.clearColor;
        setClearColor(color.r, color.g, color.b, color.a);
    }

    if (Settings.demoScript.beatsPerMinute !== void null)
    {
        timerSetBeatsPerMinute(Settings.demoScript.beatsPerMinute);
    }

    if (Settings.demoScript.rowsPerBeat !== void null)
    {
        syncEditorSetRowsPerBeat(Settings.demoScript.rowsPerBeat);
    }

    if (Settings.demoScript.resolutions !== void null)
    {
        for (var i = 0; i < Settings.demoScript.resolutions.length; i++)
        {
            setMenuResolution(i, Settings.demoScript.resolutions[i].width + 'x' + Settings.demoScript.resolutions[i].height);

            if (Settings.demoScript.resolutions[i].selected === true)
            {
                setMenuComponentSelected(i);
            }
        }
    }*/
};

Settings.processPlayer = function()
{
    /*//Assume that there's data/js/Demo.js existing if no effects are defined
    if (Settings.demoScript.effects === void null)
    {
        Settings.demoScript.effects = [
            {
                'name': 'Demo',
                'reference': 'data/js/Demo.js'
            }
        ];
    }

    for (var effectI = 0; effectI < Settings.demoScript.effects.length; effectI++)
    {
        var effect = Settings.demoScript.effects[effectI];
        if (effect.skip === true)
        {
            continue;
        }

        addPlayerEffect(effect.name, effect.reference);
    }

    //Assume that there's effect "Demo" if no scenes are defined
    if (Settings.demoScript.scenes === void null)
    {
        Settings.demoScript.scenes = [
            {
                'name': 'Demo',
                'effect': 'Demo'
            }
        ];
    }

    for (var sceneI = 0; sceneI < Settings.demoScript.scenes.length; sceneI++)
    {
        var scene = Settings.demoScript.scenes[sceneI];

        if (scene.skip === true)
        {
            continue;
        }

        if (scene.startTime === void null)
        {
            scene.startTime = '0:00';
        }
        if (scene.durationTime === void null)
        {
            scene.durationTime = Settings.demoScript.totalTime;
        }

        addPlayerScene('', scene.name, scene.effect, scene.startTime, scene.durationTime);
        Settings.processVariables(scene);

        if (!scene.hasOwnProperty('effect'))
        {
            for (var subSceneI = 0; subSceneI < scene.scenes.length; subSceneI++)
            {
                var subScene = scene.scenes[subSceneI];
                if (subScene.skip === true)
                {
                    continue;
                }

                addPlayerScene(scene.name, subScene.name, subScene.effect, subScene.startTime, subScene.durationTime);
                Settings.processVariables(subScene);
            }
        }
    }*/
}
