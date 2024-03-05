var Menu = function() {
    this.input = new Input();

    this.mute = {"v":Settings.audio.mute};
    this.fullscreen = {"v":Settings.window.fullscreen};
    this.displayModeSelect = void null;

    this.displayModes = getDisplayModes();

    this.displayModeList = "";
    for (var i = 0; i < this.displayModes.length; i++) {
        var dm = this.displayModes[i];
        if (!dm) {
            continue;
        }

        this.displayModeList += dm.width + "x" + dm.height + "\0";

        if (dm.width == Settings.demo.graphics.canvasWidth && dm.height == Settings.demo.graphics.canvasHeight) {
            this.displayModeSelect = {"v": i};
        }
    }
    this.displayModeList += "\0";

    if (this.displayModeSelect === void null) {
        this.displayModeSelect = {"v": 0};
    }

    this.audioDevices = getAudioDevices();

    this.audioDeviceList = "";
    for (var i = 0; i < this.audioDevices.length; i++) {
        var audioDevice = this.audioDevices[i];
        if (!audioDevice) {
            continue;
        }

        this.audioDeviceList += audioDevice + "\0";
    }
    this.audioDeviceList += "\0";
    this.audioDeviceSelect = {"v": 0};
}
Menu.prototype.setQuit = function(quit) {
    menuSetQuit(quit);
}
Menu.prototype.getWidth = function() {
    return menuGetWidth();
}
Menu.prototype.getHeight = function() {
    return menuGetHeight();
}
Menu.prototype.render = function() {
    ImGui.SetNextWindowPos({"x":0, "y":0}, ImGui.Cond.Always);
    ImGui.SetNextWindowSize({"x":this.getWidth(), "y":this.getHeight()}, ImGui.Cond.Always);

    ImGui.PushStyleVar(ImGui.StyleVar.WindowRounding, 0);
    ImGui.Begin("menu", {"v":true},
      ImGui.WindowFlags.NoSavedSettings
      | ImGui.WindowFlags.NoTitleBar
      | ImGui.WindowFlags.NoResize
      | ImGui.WindowFlags.NoMove
      | ImGui.WindowFlags.NoCollapse);
    var flags = ImGui.WindowFlags.NoResize;

    ImGui.SetWindowFontScale(3.0);

    ImGui.Combo("display mode", this.displayModeSelect, this.displayModeList);

    ImGui.Combo("audio device", this.audioDeviceSelect, this.audioDeviceList);

    ImGui.Checkbox("fullscreen", this.fullscreen);
    ImGui.SameLine();
    ImGui.Checkbox("mute", this.mute);

    if (ImGui.Button("Start") || ImGui.IsKeyPressed(ImGui.GetKeyIndex(ImGui.Key.Enter), true)) {
        var displayMode = this.displayModes[this.displayModeSelect.v];

        Settings.audio.mute = this.mute.v;
        Settings.audio.device = this.audioDevices[this.audioDeviceSelect.v];
        Settings.window.fullscreen = this.fullscreen.v;

        var userOptions = {
            "displayMode": displayMode,
            "mute": Settings.audio.mute,
            "device": Settings.audio.device,
            "fullscreen": Settings.window.fullscreen
        };
        loggerInfo("User menu options: " + JSON.stringify(userOptions, null, 2));

        // Send Settings modified in JS to backend
        settingsLoadSettingsFromString(JSON.stringify(Settings, null, 2));
        settingsDemoLoadDemoSettingsFromString(JSON.stringify(Settings.demo, null, 2));

        settingsWindowSetWindowDimensions(displayMode.width, displayMode.height);

        this.setQuit(false); // i.e. let's continue and show the demo
        this.input.setUserExit(true);
    }
    ImGui.SameLine();
    if (ImGui.Button("Exit") || ImGui.IsKeyPressed(ImGui.GetKeyIndex(ImGui.Key.Escape), true)) {
        this.input.setUserExit(true);
    }

    ImGui.End();
    ImGui.PopStyleVar();

    ImGui.Render();
}
