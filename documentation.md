# engine

## Table of contents

* [Supported platforms](#supported-platforms)
  * [Recommended setup](#recommended-setup)
* [Backwards incompatible changes](#backwards-incompatible-changes)
* [TODO](#todo)
* [Command line switches](#command-line-switches)
* [Demo tool mode](#demo-tool-mode)
  * [Keyboard bindings](#keyboard-bindings)
  * [Profiling and debugging](#profiling-and-debugging)
  * [File automatic reloading](#file-automatic-reloading)
  * [GNU Rocket integration](#gnu-rocket-integration)
  * [Exporting to video](#exporting-to-video)
  * [Tips and tricks](#tips-and-tricks)
* [Settings](#settings)
* [Shaders](#shaders)
  * [Shadertoy shader support](#shadertoy-shader-support)
  * [Uniform autobinding](#uniform-autobinding)
* [Supported file formats](#supported-file-formats)
  * [Music](#music)
  * [Images](#images)
  * [Videos](#videos)
  * [Fonts](#fonts)
  * [3D Meshes](#3d-meshes)
* [Demo scripting](#demo-scripting)
  * [Demo definitions](#demo-definitions)
  * [Scripting language](#scripting-language)
* [Bindings to JavaScript](#bindings-to-javascript)
  * [OpenGL](#opengl)
* [Copyrights and licensing](#copyrights-and-licensing)

## Supported platforms
* Windows
* Mac OS X
* Linux
  * Extra dependencies: SDL 2, libtheora, libogg, libvorbis, libassimp
  * NOTE: If you're using MESA libraries for OpenGL, you'd need version 17 or greater

### Recommended setup
* It's really up to the demo coder but roughly following is needed from engine's perspective:
  * 2 GHz+ 64bit processor with 2 or more cores
  * GPU with OpenGL 3.3+ support
  * 2 GB RAM

## Backwards incompatible changes
* Built-in fonts are no longer supported
* OpenGL 1.2/2.0 fixed pipeline changed to core 3.3
* Engine internal javascript bindings changed
* OpenGL fixed pipeline functions are deprecated
* 3DS is no longer supported 3D format, use COLLADA/OBJ instead
* Command line switches' syntax has changed
* 32bit and MorpOS operating systems are no longer supported
* Texture UVs are using CLAMP_TO_EDGE instead of REPEAT
* Canvas default dimensions are 1980x1280 ("Full HD"), instead of 1280x720
* Files are now only referred relative to "data/" directory root. old engine silently accepted "data/jee.png" and "jee.png" as same.

## TODO
* Some editor handling functionality (that should resemble Blender way-of-working)
  * Camera stuff
    * vasen nappi + osoita oikeeseen kohtaan = valitse objekti
    * hiiren rulla *TAI* ctrl+alt + vasen nappi + hiiren liikuttaminen = zoom in/out
    * alt + vasen nappi + hiiren liikuttaminen = rotate around origo
    * shift + alt + vasen nappi + hiiren liikuttaminen = liikutus kahdella akselilla
  * Object stuff
    * g + x/y/z + desimaaliluku (*TAI* hiiren liikutus) = liikuta objektia
    * r + x/y/z + desimaaliluku (*TAI* hiiren liikutus) = pyöritä objektia
    * s + x/y/z + desimaaliluku (*TAI* hiiren liikutus) = skaalaa objektia
  * TBD: Keyframe editing

* FIX COLLADA import
* 3D model special setups
* Implement WORLD vs. LOCAL vs. OBJECT 3d transformation support

* Fix memory handling and javascript bindings (currently very ugly and hacky)
* Better error handling
* Camera support
* Shadows
* COLLADA 3D animation
* global illumination
* video support improvements (fast rewinding)
* WebGL support
* Android support
* quad (4) and dolby surround (5.1) audio support
* multiple audio sources' support (sound effects)
* input device support (keyboard / mouse mainly)
* improved demo editor
* full shadertoy.com shader support (audio and missing image uniforms)
* javascript debugger

## Command line switches
* --help                                    Show help.
* --version                                 Show version.
* --log-file=&lt;file&gt;                   Set log output file. Defaults to STDOUT.
* --log-level=&lt;level&gt;                 Set minimum log level. Tool mode default is INFO(=2), otherwise WARNING(3)
* --settings-file=&lt;file&gt;              Load settings from specified location. By default engine tries to read ./settings.json on start.
* --project-path=&lt;path&gt;               Set project base path. Project path is location to data/ directory location.
* --show-menu=&lt;true|false&gt;            Show menu.
* --audio=&lt;true|false&gt;                Set audio on/off. Defined wheter audio should be muted or not.
* --resolution=&lt;widthxheight&gt;         Set window size. Canvas will be 1920x1280 regardless of the window size.
* --fullscreen=&lt;true|false&gt;           Set window fullscreen.
* --vertical-sync=&lt;true|false&gt;        Set vertical sync. Vertical sync in disabled in the tool mode.
* --tool=&lt;true|false&gt;                 Set demo tool usage.
* --editor=&lt;true|false&gt;               Set editor.
* --start-position=&lt;seconds&gt;          Set demo timer start position.
* --profiler=&lt;true|false&gt;             Enable EasyProfiler profiler.
* --profiler-listener=&lt;true|false&gt;    Enable listener vs. dump to file.

## Demo tool mode
### Keyboard bindings
* CTRL+HOME - Move demo timer to the begin (0:00)
* CTRL+END - Move demo timer to the end and pause demo
* CTRL+1 - Move demo timer backward one second
* CTRL+2 - Move demo timer forward one second
* CTRL+3 - Pause or unpause demo
* CTRL+5 - Deep refresh demo - Code, shader, script files and data resources (images, 3d objects) are refreshed
* CTRL+e - Editor mode on/off
* CTRL+f - Fullscreen on/off

### Profiling and debugging
* Engine supports easy profiler: https://github.com/yse/easy_profiler
* In tool mode log-level will be INFO, in normal mode WARNING, unless otherwise specified
* Log levels: 0 = TRACE, 1 = DEBUG, 2 = INFO, 3 = WARNING, 4 = ERROR, 5 = FATAL
* FATAL log entries will cause the engine to exit

### File automatic reloading
* All files (shaders, javascript, music, images, videos) are automatically reloaded on-the-fly
* Reloading is tracked based on file's modified timestamp and changes in the size of the file
* Reloading will start approximately 50-200 ms after there has been no changes in the file anymore
* Demo will be automatically paused for the time of reloading and unpaused when reload has ended
* Embedded files ("_embedded/*.*") are not reloaded as they are not expected to change.
* You may override the original embedded files by creating _embedded directory to data directory and placing file with identical name there.
  * Engine restart is needed for changes to take effect.

### Command addons
* [glslangvalidator](https://github.com/KhronosGroup/glslang/releases) is used to check shader code validity against the GLSL specifications. settings.json: gui.glslValidator
* [GNU diffutils](https://www.gnu.org/software/diffutils/) 'diff' command is used to compare new and old version of text files in case of errors. settings.json: gui.diff

### GNU Rocket integration
* In tool mode engine will on-start automatically attempt to connect to [GNU Rocket](https://github.com/kusma/rocket). You can also manually connect in editor.
* GNU Rocket integration requires data "sync" directory must be inside the data directory.
* If GNU Rocket server connection can't be established then player mode will be automatically used.
* XML file default naming convention is data/sync/sync.rocket
  * If not using XML, then track files must be located in following place and naming convention: data/sync/sync_&lt;TRACK NAME&gt;.track
* New track variables can be added in the editor. Shaders that contain float type uniforms with the new name will be reloaded via [Shader auto-binding](#uniform-autobinding)
  * If track is inside a tab, use "_" instead of ":"

### Exporting to video
* You can export to raw video in the editor (File -> Export Video), after raw video export you may use ffmpeg to convert the video and add audio
* YouTube Recommended upload encoding settings: https://support.google.com/youtube/answer/1722171?hl=en
* Audio: AAC-LC audio with high bitrate, stereo/5.1 and samplerate 48/96kHz
* Video: 16:9 MP4 H264 60 fps (/w nearly lossless quality)

* [ffmpeg](https://www.ffmpeg.org/) rawvideo to H264 AAC-LC MP4:
```
ffmpeg -f rawvideo -pixel_format rgb24 -video_size 1920x1080 -framerate 60 -i engine_rawvideo_1920x1080.rgb24 -i data/music.ogg -c:a aac -b:a 512k -framerate 60 -vcodec libx264 -crf 18 output.mp4
```
* Copy ogg video: ffmpeg -ss 00:00:10 -i 0807_Camera_Thrills_of_the_War_07_00_59_00_3mb.ogv -t 00:00:15 -c copy camera_thrills_clip.ogv
* Cut ogg video, scale (and maintain aspect ratio) and reencode without audio: ffmpeg -ss 00:02:29 -i Dividean1943.ogv -t 00:00:03 -vf scale=1280:-1 -vb 20M -an planning.ogv

### Tips and tricks

#### Audio file mass conversion
```
bash; for i in *.wav; do ffmpeg -i "$i" "${i%.*}.ogg"; done
```

#### Vector graphics to PNG
```
convert -density 300 -colorspace sRGB vector_file.eps -resize 5076x5106 -background transparent -units PixelsPerInch vector_file_rasterized.png

```

## Settings
* If passing command line switches is tedious then you can create a settings JSON file.
* On start engine will by-default try to read ./settings.json. You may define custom location in command line, example: --settings-file=settings.json
* You can save settings.json in editor mode: File -> Save Settings

Example settings.json file with current defaults (for Windows):
```JavaScript
{
    "audio": {
        "mute": false,
        "timeSource": false
    },
    "gui": {
        "editor": false,
        "fileModifyGracePeriod": 50,
        "logFile": "stdout.log",
        "profiler": false,
        "profilerListener": true,
        "projectPath": "",
        "startPosition": 0.0,
        "tool": false
    },
    "logger": {
        "duplicateLogGracePeriod": 1000,
        "logLevel": 3
    },
    "showMenu": true,
    "window": {
        "fullscreen": true,
        "height": 1080,
        "title": "AV experience",
        "verticalSync": true,
        "width": 1920
    }
}
```

Example settings.json file for demo making
```JavaScript
{
    "gui": {
        "editor": true,
        "logFile": "stdout.log",
        "profiler": false,
        "profilerListener": true,
        "projectPath": "../path/to/demos/data/",
        "tool": true
    },
    "logger": {
        "logLevel": 1
    },
    "showMenu": false,
    "window": {
        "fullscreen": false,
        "height": 1080,
        "verticalSync": false,
        "width": 1920
    }
}
```

## Shaders
* Shaders utilize GLSL 330 core by default on desktops, GLSL ES 2.0 otherwise

### Shadertoy shader support
* shadertoy.com shaders and uniforms are supported in-house
* pixel/fragment shader will be assumed as shadertoy.com shader if it contains following function: void mainImage( out vec4 fragColor, in vec2 fragCoord )
* shadertoy.com shaders will be bootstrapped with shadertoy.com header shader that contains necessary uniforms and void main() that handles mainImage function

### Uniform autobinding
Following uniforms will be attempted to be auto-binded, if uniform is available in the shader:
```
// engine internal uniforms:
uniform float      time;                    // Current time in seconds
uniform vec4       color;                   // Main color of the vertex
uniform mat4       mvp;                     // Model view project matrix
uniform sampler2D  texture0;                // Samplers for input textures i
uniform sampler2D  texture1;
uniform sampler2D  texture2;
uniform sampler2D  texture3;

// shadertoy.com related uniforms (will be auto-bind also in non-shadertoy.com shaders):
uniform vec3       iResolution;              // image/buffer          The viewport resolution (z is pixel aspect ratio, usually 1.0)
uniform float      iTime;                    // image/sound/buffer    Current time in seconds
uniform float      iTimeDelta;               // image/buffer          Time it takes to render a frame, in seconds
uniform int        iFrame;                   // image/buffer          Current frame
uniform float      iFrameRate;               // image/buffer          Number of frames rendered per second
uniform vec4       iDate;                    // image/buffer/sound    Year, month, day, time in seconds in .xyzw
uniform float      iSampleRate;              // image/buffer/sound    The sound sample rate (typically 44100)
uniform sampler2D  iChannel0;                // image/buffer/sound    Sampler for input textures i
uniform sampler2D  iChannel1;
uniform sampler2D  iChannel2;
uniform sampler2D  iChannel3;

// Sync / GNU Rocket uniforms
uniform float      <sync variable name>;    // If float type uniform with sync variable name exists, then sync data will be applied to it
```

## Supported file formats
### Music
* OGG vorbis
* Mono (1) and stereo (2) channels
* Varying bitrates
* User-defined sample rates
* Audio files with varying sample rates and channels are not supported

### Images
* PNG files (1/2/4/8/16 bit-per-channel) are supported

### Videos 
* OGV Theora video files

### Fonts
* TTF fonts are supported
* UTF-8 characters are supported
* Custom font file default path: data/font.ttf

### 3D Meshes
* OBJ, blender and collada (.dae) formats are supported to some extent (expect basic functionality)
* Collada format has basic keyframe animation support

## Demo scripting

* Demo scripting is based on [JSON](http://www.json.org/) and [JavaScript/ECMAScript 5](http://www.ecma-international.org/ecma-262/5.1/)
* Demo scripting consists of two major parts
  * [Demo definitions](#demo-definitions) - Demo static definitions such as resource files and setup information
  * [Scripting language](#scripting-language) - Actual script files

### Demo definitions

data/Demo.js contains the actual demo code.
data/script.json contains the main demo definition data. File is optional.

* music &lt;filename&gt; - Location to the main music file (OGG Vorbis), default "music.ogg"
* beatsPerMinute &lt;bpm&gt; - The average beats per minute - default value 100
* rowsPerBeat &lt;rpb&gt; - Rows per beat in GNU Rocket - default value 8
* length &lt;time in seconds&gt; - Demo total playing time, defaults to music file's total time
* targetFps &lt;fps&gt; - Target FPS, default is 500
* graphics
  * canvasWidth &lt;width&gt; - The 2D maximum width of the screen (default 1980)
  * canvasHeight &lt;height&gt; - The 2D maximum height of the screen (default 1280)
  * aspectRatio - The aspect ratio settings for screen and 3D, defaults to 16/9=1.77... aspect ratio
  * defaultTextureFilter &lt;integer&gt; - NEAREST(0), LINEAR(1), MIPMAP(2) - default MIPMAP
  * defaultTextureFormat &lt;integer&gt; - RGBA(0), RGB(1), DEPTH_COMPONENT(2) - default RGBA
  * defaultTextureWrap &lt;integer&gt; - REPEAT(0), MIRRORED_REPEAT(1), CLAMP_TO_EDGE(2), CLAMP_TO_BORDER(3) - default CLAMP_TO_EDGE
  * displayModes - Menu display mode options, defaults to end user's settings
  * maxActiveLightCount &lt;integer&gt; - Maximum supported lights, default 4 (not recommended to be changed...) 
  * maxTextureUnits &lt;integer&gt; - Maximum supported texture units, default 4 (not recommended to be changed...)
  * clearColor - Sets the main screen clear color
    * r &lt;double&gt; - red - default value 0.0
    * g &lt;double&gt; - green - default value 0.0
    * b &lt;double&gt; - blue - default value 0.0
    * a &lt;double&gt; - alpha - default value 0.0

Example script.json (Contains current defaults):
```JavaScript
{
    "beatsPerMinute": 100.0,
    "graphics": {
        "aspectRatio": 1.77777779102325,
        "canvasHeight": 1080.0,
        "canvasWidth": 1920.0,
        "clearColor": {
            "a": 0.0,
            "b": 0.0,
            "g": 0.0,
            "r": 0.0
        },
        "defaultTextureFilter": 2,
        "defaultTextureFormat": 0,
        "defaultTextureWrap": 2,
        "displayModes": [
            {
                "height": 1080,
                "width": 1920
            },
            {
                "height": 720,
                "width": 1280
            },
            {
                "height": 768,
                "width": 1024
            },
            {
                "height": 600,
                "width": 800
            },
            {
                "height": 480,
                "width": 640
            }
        ],
        "maxActiveLightCount": 4,
        "maxTextureUnits": 4
    },
    "length": -1.0,
    "rowsPerBeat": 8.0,
    "song": "music.ogg",
    "targetFps": 500.0
}
```

NOTE: In script.js it is possible to override default loading bar by defining function: Loader.drawLoadingBar = function(percent) { /*OpenGL functions to draw loaderbar goes here here*/ }

### Scripting language

* Here are the examples and documenation of the JSON based scripting language that can be used in the JavaScript files.

#### Scripting language reference
* Here's documented scripting language reference.
* Scripting language should primarily be defined in scene class' init() method.
* Script is rendered with method call: player.drawAnimation(this.loader.animationLayers);

##### addSync
* addSync defines a sync pattern that can be applied in the addAnimation definition
  * Sync point times redefine the time of specific animation primitive where sync is applied to.
  * When [GNU Rocket](https://github.com/kusma/rocket) is used for syncing it defines the sync timing and values
```JavaScript
//inhouse sync pattern
Sync.addSync([
{
     "name":"pattern1"              //name of the sync pattern that is referenced in the addAnimation method
    ,"start":"0:00"                 //relative start time of whole sync pattern
    ,"duration":"0:08"              //duration time of the whole sync pattern
    ,"end":"0:08"                   //relative end time of whole sync pattern
    ,"syncStartFunction":<function> //sync start function - initialized on every start of sync point
    ,"syncEndFunction":<function>   //sync end function - initialized on every end of sync point
    ,"syncRunFunction":<function>   //sync run function - initialized on every time sync point is active
    ,"pattern": [      //sync pattern sync point times
         {
             /*Sync pattern point 1*/
            ,"start":"0:00"                 //relative start time of the sync point
            ,"duration":"0:01"              //duration time of the sync point
            ,"end":"0:00"                   //relative start time of the sync point
            ,"syncStartFunction":<function> //sync start function - initialized on every start of sync point. Overrides parent syncStartFunction.
            ,"syncRunFunction":<function>   //sync run function - initialized on every time sync point is active. Overrides parent syncRunFunction.
        }
        ,{/*Sync pattern point 2*/}
        ,{/*Sync pattern point ...N*/}
    ]
}]);
```
```JavaScript
//GNU Rocket sync track
Sync.addSync([
{
     "name":"track.variable"        //name of the GNU Rocket sync track that is referenced in the addAnimation method
    ,"type":"rocket"                //Defines that sync pattern handled by GNU Rocket
    ,"syncStartFunction":<function> //sync start function - initialized once
    ,"syncRunFunction":<function>   //sync run function - initialized on every time sync point is active
}]);
```

##### addAnimation
* addAnimation method adds animation definition
```JavaScript
this.loader.addAnimation([
     {/*Animation JSON 1*/}
    ,{/*Animation JSON 2*/}
    ,{/*Animation JSON ...N*/}
]);
```

* To process all added animation you need to call Loader.processAnimation() (preferably in scene's init() or postInit())
* In case you have not defined postInit() function then Loader.processAnimation() will be automatically called after init() function.
```JavaScript
this.loader.processAnimation();
```

* To calculate and draw all processed animations you need to call Player.drawAnimation(Loader.animationLayers) (in scene's run())
* In case you have not defined run() function then Player.drawAnimation() will be automatically called.
```JavaScript
this.player.drawAnimation(this.loader.animationLayers);
```

##### Animation type static definitions

* Main animation types and non generic object related definitions
* Image & video (2D/3D)
```JavaScript
 "image": <PNG/OGV file path>
 {
     "canvasWidth":<width>            //the 2D screen relative width
    ,"canvasHeight":<height>          //the 2D screen relative height
    ,"perspective":<perspective>      //defines if the image should be rendered in 2D or 3D mode, default "2d"
  ,"align":<alignment>              //Sets the image alignment:
                                      //Constants.Align.CENTER - center alignment
                                      //Constants.Align.HORIZONTAL - horizontal / X-axis alignment
                                      //Constants.Align.VERTICAL - vertical / Y-axis alignment
                                      //Constants.Align.LEFT - left alignment
                                      //Constants.Align.RIGHT - right alignment
  ,"uv": {                          //Define UV coordinates of the image
     "uMin": 0.0
    ,"vMin": 0.0
    ,"uMax": 1.0
    ,"vMax": 1.0
  }
 }
```

  * Video parameters can be defined in image tag
```JavaScript
 "image": {
    "name": "video.ogv",
    "video": {
       "loop": <loop>    //1 = LOOP, 0 = do not loop. Default is 0.
      ,"fps": <fps>      //overrides videos normal fps. can be used for skipping frames etc...
      ,"speed": <speed>  //Can adjust the speed of playback. 1.0 is default.
    }
 }
```


* Text (2D/3D)
```JavaScript
 "text":
 {
     "string":<string>            //text that should be rendered
    ,"perspective":<perspective>  //defines if text should be rendered in 2D or 3D mode, default "2d"
  ,"name":<TTF font>            //font file that should be used for rendering the test. Defaults to default font, which is the first loaded font, presumably data/font.ttf
 }
 ,"clearDepthBuffer":<boolean>    //default false
 ,"align":<alignment>              //Sets the image alignment:
                                     //Constants.Align.CENTER - center alignment
                                     //Constants.Align.HORIZONTAL - horizontal / X-axis alignment
                                     //Constants.Align.VERTICAL - vertical / Y-axis alignment
                                     //Constants.Align.LEFT - left alignment
                                     //Constants.Align.RIGHT - right alignment
```

  * Object (3D)
```JavaScript
 "object":<3DS/OBJ file path or object name>  //name of the 3D object or path to .3ds/.obj file
,"shape":                                 //basic shape - for rendering default shapes
 {
    "type":<basic shape type>             //MATRIX (not object but for global transformation animations)
                                          //CUBE
                                          //CYLINDER
                                          //DISK
                                          //SPHERE
 }
,"objectFunction":<function>              //custom JavaScript object drawing function
,"clearDepthBuffer":<boolean>             //default false
,"fps":<decimal>                          //animation frames per second
,"frame":<decimal>                        //animation display constant frame
```

  * FBO (2D/3D)
```JavaScript
 "fbo":
 {
     "name":<fbo name> //name of the fbo - NOTE: fbo can be used in "image" animation by referring to image name "<fbo name>.color.fbo"
    ,"action":<action>
        //actions:
        //"begin" - start writing to FBO
        //"unbind" - end writing to FBO and return to the main screen rendering
        //"draw" - draw the FBO
        //"end" - perform "unbind" and "draw" operations
    ,"width":<width> //FBO texture width
    ,"height":<height> //FBO texture height
    ,"storeDepth":<true/false> //If FBO should store depth values as well. Default is false.
    "dimension":[ //adjust the FBO render quality by changing the render dimensions
        {
             "x":1.0 //FBO render width in percentage 0.0 - 1.0 - default is 1.0
            ,"y":1.0 //FBO render height in percentage 0.0 - 1.0 - default is 1.0
        }
    ]
 }
```

  * Passing single animation to FBO
```JavaScript
 //This shorthand could be useful to remove boilerplate when doing pass through shader FBOs etc...
 //You can use "passToFbo" in any of the animation types (image, object, fbo etc...)"
 "passToFbo":
 {
     "name":<fbo name> //name of the fbo where animation is saverd
    ,"beginLayer":<layer> //FBO start layer - default is animation's layer
    ,"endLayer":<layer> //FBO end layer - default is animation's layer
    ,"beginAction": <action> //FBO's first action - default is "begin"
    ,"endAction": <action> //FBO's last action - default is "unbind"
 }
```

  * Camera (3D)
    * There can be only one active camera in the main scene/FBO
```JavaScript
 "camera": <camera name>
 //where camera is looking at
 ,"target":[
     {
         "x":0.0 //camera look-at X, default 0
        ,"y":0.0 //camera look-at Y, default 0
        ,"z":0.0 //camera look-at Z, default 0
     }
     ,{/*target animation primitive...N*/}
 ]
 //camera's up vector
 ,"up":[
     {
         "x":0.0 //camera up vector X, default 0
        ,"y":1.0 //camera up vector Y, default 1
        ,"z":0.0 //camera up vector Z, default 0
     }
     ,{/*target animation primitive...N*/}
 ]
 //camera's perspective setup
 ,"perspective":[
    {
        "fov":45       //view Y angle in degrees, default 45
        ,"aspect":16/9 //aspect ratio, default 16:9
        ,"near":1      //near clipping plane
        ,"far":1000    //far clipping plane
    }
    ,{/*perspective animation primitive...N*/}
 ]
 ,"cameraRelativePosition": <object name> //lock camera's position to track 3d object main type
 ,"cameraRelativeTarget": <object name>   //lock camera's target(look-at) to track 3d object main type
```

* Time definitions
```JavaScript
"start":<time>              //start time, default is the scene's start time
"duration":<time>           //duration time, default is the scene's duration time
"end":<time>                //end time, default is the scene's end time
```

* Layer - defines the rendering order in alphabetical order
```JavaScript
"layer":<layer>      //layer, default value 1, layers can be in range 1 - 99999
```

* Custom functions
```JavaScript
,"initFunction":<function>  //custom JavaScript function that is called during addAnimation method processing
,"runFunction":<function>   //custom JavaScript function that is called every time animation is rendered
```

* Shader definitions
```JavaScript
"shader":
{
     "name":<shader name> //shader name as defined in script.js. alternatively you can directly reference to a single shader file ("name":"data/shader/file.fs") or multiple files ("name":["data/shader/file.fs","data/shader/file2.fs"])
  ,"programName":<program name> //program name, if nothing specified then program name will be the same as the last defined shader name
    ,"variable":[
         {
             "name":<name>   //uniform variable name as defined in the shader code
            ,"type":<type>   //uniform variable type (int or float) as defined in the shader code, default is "float"
            ,"value":[<value1>,<...valueN>} //1-4 values of the uniform as an array
        }
        ,{/*variable...N*/}
    ]
}
```

* Sync - apply defined sync pattern to the animation
  * Note: This notation is not mandatory for GNU Rocket tracks. If GNU Rocket track is used then stick with 0.0 - 1.0 value range as engine interprets the track strictly as progress percent.
* By default sync is applied to whole pattern but all or some can be excluded
```JavaScript
"sync":{
     "name":<patter name> //string name of the pattern that was defined in addSync method
    ,"all":<boolean>      //apply sync default value to all animation calculations - default value is true
    //single enable/disable animation types. Default values are based on "all" variable.
    ,"color":<boolean>
    ,"angle":<boolean>
    ,"position":<boolean>
    ,"scale":<boolean>
}
```

##### Animation primitives
* First animation primitive array element omits time definitions and uses defaults always
* All animation primitives support time definitions and arrays
* After first animation primitive, the primitives always inherit the values from previous primitive
* Non-time related animation variables support [JavaScript dynamic injection](#javascript-dynamic-injection)
```JavaScript
"animationPrimitive": [
    { //animation primitive 1
        "start":<time>     //start time, default is the animation block's start time
        ,"duration":<time> //duration time, default is the animation block's duration time
        ,"end":<time>      //end time, default is the animation block's end time
        /*more animation variables per animation primitive*/
    }
     ,{/*animation primitive 2*/}
     ,{/*animation primitive 3*/}
     /*...*/
     ,{/*animation primitive N*/}
]
```

```JavaScript
"scale": [
    {
         "x":1.0         //scale X value 1.0 = 100% - default is 1.0
        ,"y":1.0         //scale Y value 1.0 = 100% - default is 1.0
    ,"z":1.0         //scale Z value 1.0 = 100% - default is 1.0
    ,"uniform2d":1.0 //scale X & Y = uniform2d value - default undefined
    ,"uniform3d":1.0 //scale X, Y & Z = uniform2d value - default undefined
    }
]
```

```JavaScript
//in case of images:
// - if position is not given then image is aligned to center
// - 2d image's origo (x:0,y:0) is bottom-left corner so that image is centered to origo.
"position": [
    {
         "x":0.0  //position X - default is context specific but usually 0.0
        ,"y":0.0  //position Y - default is context specific but usually 0.0
        ,"z":0.0  //position Z - default is context specific but usually 0.0
    }
]
```

```JavaScript
//image and object pivot point
"pivot": [
  {
     "x":0.0  //position X - default is 0.0
    ,"y":0.0  //position Y - default is 0.0
    ,"z":0.0  //position Z - default is 0.0
  }
]
```

```JavaScript
"color": [
    {
         "r":255  //color red   - accepts values 0-255 - default is 255
        ,"g":255  //color green - accepts values 0-255 - default is 255
        ,"b":255  //color blue  - accepts values 0-255 - default is 255
        ,"a":255  //color alpha - accepts values 0-255 - default is 255
    }
]
```

```JavaScript
"angle": [
    {
         "degreesX":0  //3d angle degrees X - default is 0
        ,"degreesY":0  //3d angle degrees Y - default is 0
        ,"degreesZ":0  //2d/3d angle degrees Z - default is 0
        ,"x":1.0       //3d rotation vector X coordinate - default is 1.0
        ,"y":1.0       //3d rotation vector Y coordinate - default is 1.0
        ,"z":1.0       //3d rotation vector Z coordinate - default is 1.0
]
```

##### JavaScript dynamic injection
* Instead of normal JSON definition "variableName":1.0 or "variableName":getConstantValue() it's possible to inject dynamic JavaScript code with "{}" notation.
* Example: "variableName":"{return javaScriptVariable*Math.sin(getSceneTimeFromStart()/10.0);}"

#### Scripting language examples

##### 2D image animation examples
```JavaScript
//Move jml_fist.png from origo (x:0,y:0) to other end of the screen in 10 seconds
this.loader.addAnimation([{
     "start": "0:00", "duration": "5:00"
    ,"layer": 1, "image": "data/jml_fist.png"
    ,"position": [
          {"x":0, "y":0}
         ,{"duration":"0:10", "x":getScreenWidth(), "y":getScreenHeight()}
    ]
}]);
```
```JavaScript
//Play jml_fist.ogv animation and move from origo (x:0,y:0) to other end of the screen in 10 seconds
this.loader.addAnimation([{
   "start": "0:00", "duration": "5:00"
  ,"layer": 1, "image": "data/jml_fist.ogv"
  ,"position": [
      {"x":0, "y":0}
     ,{"duration":"0:10", "x":getScreenWidth(), "y":getScreenHeight()}
  ]
}]);
```
```JavaScript
//Wait 2 seconds and then:
//1) vertically "flip" jml_fist.png by scaling y from 1.0 to -1.0
//2) horizontally "flip" the image
//3) "flip" image back to normal
this.loader.addAnimation([{
     "start": "0:00", "duration": "5:00"
    ,"layer": 1, "image": "data/jml_fist.png"
    ,"scale": [
          {"x":1, "y":1}
         ,{"start":2, "duration":1, "x":1, "y":-1}
         ,{"duration":1, "x":-1, "y":-1}
         ,{"duration":1, "x":1, "y":1}
    ]
}]);
```
```JavaScript
//Rotate jml_fist.png clock-wise 360 degrees in 3 seconds
this.loader.addAnimation([{
     "start": "0:00", "duration": "5:00"
    ,"layer": 1, "image": "data/jml_fist.png"
    ,"angle": [
         {"degreesZ":0}
        ,{"duration":"0:03", "degreesZ":360}
    ]
}]);
```
```JavaScript
//Rotate jml_fist.png counter clock-wise 720 degrees pivoting from the center of screen in 10 seconds
this.loader.addAnimation([{
     "start": "0:00", "duration": "5:00"
    ,"layer": 1, "image": "data/jml_fist.png"
    ,"position": [{"x":getScreenWidth()/2, "y":0}]
  ,"pivot":[{"y":getScreenHeight()/2}]
    ,"angle": [
         {"degreesZ":0}
        ,{"duration":"0:10", "degreesZ":-360*2}
    ]
}]);
```
```JavaScript
//1) Make jml_fist.png completely black and transparent
//2) Transition the image from completely transparent to completely opaque in 2.5 seconds
//3) Transition the image from completely black to white in 5 seconds
this.loader.addAnimation([{
     "start": "0:00", "duration": "5:00"
    ,"layer": 1, "image": "data/jml_fist.png"
    ,"color": [
         {"r":0, "g":0, "b":0, "a":0}
        ,{"duration":"0:02.5", "a":255}
        ,{"duration":"0:05", "r":255, "g":255, "b":255}
    ]
}]);
```
```JavaScript
//Example where jml_fist.png is rotated and scaled down in a spiral-like manner. After the spiral it will be scaled up and faded out.
this.loader.addAnimation([{
     "start": "0:00", "duration": "5:00"
    ,"layer": 1, "image": "data/jml_fist.png"
    ,"position": [
         {"x":getScreenWidth()/2, "y":0}
        ,{"duration":"0:25","y":getScreenHeight()/2}
    ]
  ,"pivot": [
     {"y":getScreenHeight()/2}
    ,{"duration":"0:25", "y":0}
  ]
    ,"angle": [
         {} //use default values == 0.0
        ,{"duration":"0:25", "degreesZ":-360*5}
        ,{"duration":"0:50", "degreesZ":-360*25}
    ],
    "color": [
         {}
        ,{"start":"0:25", "duration":"0:30", "a":0}
    ],
    "scale": [
         {"x":2.5,"y":2.5}
        ,{"duration":"0:25", "x":1,"y":1}
        ,{"duration":"0:50", "x":20,"y":20}
    ]
}]);
```

##### Text drawing examples
Draw 2D text to the screen:
```JavaScript
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30"
    ,"layer": 100, "text":{"string":"Here is a 2D text string with\na line break!"}
    ,"scale": [
          {"x":0.5,"y":0.5}
    ]
    ,"color": [
          {"g":0,"b":0}
    ]
    ,"angle": [
           {"degreesZ":0}
          ,{"duration":"0:3", "degreesZ":360}
    ]
}]);
```

Draw 3D text to the screen:
```JavaScript
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30"
    ,"layer": 300, "text":{"string":"Here is a 3D text string!", "perspective":"3d"}
    ,"clearDepthBuffer":true
    ,"scale": [
          {"x":0.4,"y":0.4}
    ]
    ,"color": [
          {"r":0}
    ]
    ,"position": [
         {"x":-5,"y":0,"z":-10}
    ]
    ,"angle": [
           {"x":1,"y":1,"z":1}
          ,{"duration":"0:3", "degreesX":360,"degreesY":360,"degreesZ":360}
    ]
}]);
```

##### Create sync pattern and apply to animation
```JavaScript
//Create an inhouse sync pattern
Sync.addSync([
{
    "name":"pattern1"
    ,"start": "0:00", "duration": "0:08"
    ,"pattern": [
         {"start":"0:00","duration":"0:01"}
        ,{"start":"0:02","duration":"0:01"}
        ,{"start":"0:04","duration":"0:01"}
        ,{"start":"0:06","duration":"0:01"}
    ]
}]);
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30"
    ,"layer": 300, "text":{"string":"Here is a 3D text string!", "perspective":"3d"}
    ,"sync":{"name":"pattern1","color":false} //sync pattern applied to everything except color animation
    ,"clearDepthBuffer":true
    ,"scale": [
          {"x":0.6,"y":0.6}
    ]
    ,"color": [
           {"r":0,"a":0}
          ,{"duration":"0:05","a":255}
    ]
    ,"position": [
         {"x":-5,"y":0,"z":-10}
    ]
    ,"angle": [
           {"x":1,"y":1,"z":1}
          ,{"duration":"0:10", "degreesZ":360}
          ,{"duration":"0:10", "degreesY":360}
          ,{"duration":"0:10", "degreesX":360}
    ]
}]);
```

```JavaScript
//Create a GNU Rocket sync pattern
Sync.addSync([
{
    //use GNU Rocket's track "pattern1". Track file should be located here: "data/sync/sync_pattern1.track".
    "name":"pattern1", "type":"rocket"
}]);
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30"
    ,"layer": 300, "text":{"string":"Here is a 3D text string!", "perspective":"3d"}
    ,"sync":{"name":"pattern1","color":false} //sync pattern applied to everything except color animation
    ,"clearDepthBuffer":true
    ,"scale": [
          {"x":0.6,"y":0.6}
    ]
    ,"color": [
           {"r":0,"a":0}
          ,{"duration":"0:05","a":255}
    ]
    ,"position": [
         {"x":-5,"y":0,"z":-10}
    ]
    ,"angle": [
           {"x":1,"y":1,"z":1}
          ,{"duration":"0:10", "degreesZ":360}
          ,{"duration":"0:10", "degreesY":360}
          ,{"duration":"0:10", "degreesX":360}
    ]
}]);
```

```JavaScript
//Create a GNU Rocket sync pattern but apply only to one variable in the animation
Sync.addSync([
{
    //use GNU Rocket's track "vinyl.scratch". Track file should be located here: "data/sync/sync_vinyl.scratch.track".
    "name":"vinyl.scratch", "type":"rocket"
}]);
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30"
    ,"layer": 100, "image": "data/vinyl_label.png"
    ,"angle": [
         {"degreesZ":"{return Sync.getSyncValue('vinyl.scratch');}"}
    ]
}
]);
```

##### 3ds file play
```JavaScript
this.loader.addAnimation([{
     "start": 5.28, "duration": 1.76
    ,"layer": 7, "object": "1.3ds"
    ,"clearDepthBuffer": true
    ,"camera": "Camera01"
    ,"fps": 29
}]);
```

##### Draw and animate 3D shapes
```JavaScript
//3D animation works in similar way as with 2D
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30"
    ,"layer": 200, "object": "cube"
    ,"shape":{
        "type":"CUBE"
    },
    "position":[
         {}
        ,{"duration":"0:02","z":-5}
        ,{"duration":"0:01","x":1}
        ,{"duration":"0:01","x":-1}
        ,{"duration":"0:01","y":1}
        ,{"duration":"0:01","y":-1}
        ,{"duration":"0:01","x":0,"y":0}
    ],
    "scale":[
         {}
        ,{"duration":"0:02","x":0.5,"y":2.5}
    ]
    ,"angle":[
         {"x":1}
        ,{"duration":"0:10","degreesX":360,"x":1,"y":1,"z":1}
    ]
    ,"color":[
         {"a":0}
        ,{"duration":"0:01","a":255}
    ]
}]);
```

##### Setup camera
Look at origo (0,0,0) from z:10:
```JavaScript
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30", "camera": "cam1", "layer":200
     //where camera is located
     ,"position":[
        {"x":0,"y":0,"z":10}
     ]
     //where camera is looking at
     ,"target":[
        {"x":0,"y":0,"z":0}
     ]
     //camera's up vector
     ,"up":[
        {"x":0,"y":1,"z":0}
     ]
     //camera's perspective setup
     ,"perspective":[
        {"fov":45,"aspect":16/9,"near":1,"far":1000}
     ]
}]);
```

Track a 3D object from distance of z:-5:
```JavaScript
this.loader.addAnimation([
{
     "start": "0:00", "duration": "0:30", "camera": "cam1", "layer":200
     ,"position":[
        {"x":0,"y":0,"z":-5}
     ]
     ,"target":[
        {"x":0,"y":0,"z":0}
     ]
     //make camera position to track 3d object with name "cube"
     ,"cameraRelativePosition": "cube"
     //make camera target to track 3d object with name "cube"
     ,"cameraRelativeTarget": "cube"
}]);
```

##### FBO and fullscreen pixel shader example
```JavaScript
var deformationStart = "1:08";
var deformationDuration = "0:16";
//FBO start
this.loader.addAnimation([
{
     "start": deformationStart, "duration": deformationDuration
    ,"layer": 1
    ,"fbo":{"name":"fbo","action":"begin"}
},
//draw to FBO
{
     "start": deformationStart, "duration": deformationDuration
    ,"layer": 1, "image": "data/jml_fist.png"
}
//FBO end and apply pixel shader
{
     "start": deformationStart, "duration": deformationDuration
    ,"layer": 1
    ,"fbo":{"name":"fbo","action":"end"}
    ,"shader":{"name":"Deformation",
        "variable":[
             {"name":"time","value":["{return getSceneTimeFromStart()/10.0;}"]}
            ,{"name":"effect","type":"int","value":[4]}
        ]
    }
}]);
```

##### FBO and multipass pixel shader example
```JavaScript
this.loader.addAnimation([
//render contents rendered in layers 00001 - 00050 to FBO "fbo"
{
     "start": "0:00", "duration": "0:20"
    ,"layer": 1
    ,"fbo":{"name":"fbo","action":"begin"}
},
{
     "start": "0:00", "duration": "0:20"
    ,"layer": 50
    ,"fbo":{"name":"fbo","action":"unbind"}
},
//Render "fbo" via Glow shader (first pass) to "fbo2"
{
     "start": "0:00", "duration": "0:20"
    ,"layer": 51
    ,"fbo":{"name":"fbo2","action":"begin"}
},
{
     "start": "0:00", "duration": "0:20"
    ,"layer": 52
    ,"fbo":{"name":"fbo","action":"draw"}
    ,"shader":{"name":"Glow","variable":[
                 {"name":"direction","value":[0,1]}
                ,{"name":"alpha","value":[1.0]}
            ]}
},
{
     "start": "0:00", "duration": "0:20"
    ,"layer": 53
    ,"fbo":{"name":"fbo2","action":"unbind"}
},
//Draw the original "fbo" to screen
{
     "start": "0:00", "duration": "0:20"
    ,"layer": 54
    ,"image": "fbo.color.fbo"
    ,"color": [
             {"r":255,"g":0,"b":0}
        ]
},
//Draw "fbo2" via Glow shader (second pass) to screen
{
     "start": "0:00", "duration": "0:20"
    ,"layer": 55
    ,"fbo":{"name":"fbo2","action":"draw"}
    ,"shader":{"name":"Glow","variable":[
                 {"name":"direction","value":[1,0]}
                 ,{"name":"alpha","value":[0.4]}
            ]}
}
]);
```

##### Custom JavaScript functions
###### Simple example
Definition in the init method:
```JavaScript
    this.loader.addAnimation([
    {
         "start": "0:10", "end": "0:58"
        ,"layer": 4
        ,"initFunction":"{initFunction(animation);}"
        ,"runFunction":"{drawFunction(animation);}"
    }]);
```

###### Custom JavaScript 3d object handling function
Definition in the init method:
```JavaScript
this.loader.addAnimation([
{
     "start": "0:10", "end": "0:58", "object":"disco","shape":{"type":"CUSTOM"}
    ,"layer": 4
    ,"initFunction":"{initDiscoball(animation);}"
    ,"objectFunction":"{drawDiscoball(animation);}"
    ,"position":[
         {"z":-5}
        ,{"start":"0:55","duration":"0:03","y":4}
    ]
    ,"angle":[
        {"degreesX":90, "degreesZ":"{return getSceneTimeFromStart()*30;}"}
    ]
}]);
```

Actual code:
```JavaScript
// define variables outside functions if you want to, for example, calculate something in init and use in draw function
function initDiscoball(animation)
{
    // initialization code here
}

function drawDiscoball(animation)
{
  // drawing code here
}
```

## Bindings to JavaScript

### OpenGL
* WebGL 1: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API
* OpenGL ES 2.0: https://www.khronos.org/opengles/sdk/docs/reference_cards/OpenGL-ES-2_0-Reference-card.pdf
* Please note that desktop operating systems are using normal OpenGL, so crossplatform mobile / desktop applications should be aware of differences in what OpenGL actually supports

## Copyrights and licensing
* TBD
