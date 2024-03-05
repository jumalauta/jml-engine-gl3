var Model = function() {
    this.ptr = undefined;
    this.filename = undefined;
    this.camera = 'Camera 01';
    this.fps = 0;
    this.clearDepthBuffer = true;
}

Model.prototype.load = function(filename) {
    var legacy = loadObject(filename);
    this.filename = filename;
    this.ptr = legacy.ptr;
}

Model.prototype.setCameraName = function(cameraName) {
    this.cameraName = cameraName;
}

Model.prototype.setFps = function(fps) {
    this.fps = fps;
}

Model.prototype.setClearDepthBuffer = function(boolean) {
    this.clearDepthBuffer = boolean;
}

Model.prototype.setLighting = function(boolean) {
    loggerDebug("useObjectLighting not implemented");
    //useObjectLighting(this.ptr, boolean === true ? 1 : 0);
}

Model.prototype.setSimpleColors = function(boolean) {
    loggerDebug("useSimpleColors not implemented");
    //useSimpleColors(this.ptr, boolean === true ? 1 : 0);
}

Model.prototype.setCamera = function(boolean) {
    loggerDebug("useObjectCamera not implemented");
    //useObjectCamera(this.ptr, boolean === true ? 1 : 0);
}

Model.prototype.setPosition = function(x, y, z) {
    setObjectPosition(this.ptr, x, y, z);
}

Model.prototype.setPivot = function(x, y, z) {
    setObjectPivot(this.ptr, x, y, z);
}

Model.prototype.setRotation = function(degreesX, degreesY, degreesZ, x, y, z) {
    setObjectRotation(this.ptr, degreesX, degreesY, degreesZ, x, y, z);
}

Model.prototype.setScale = function(x, y, z) {
    setObjectScale(this.ptr, x, y, z);
}

Model.prototype.setNodePosition = function(nodeName, x, y, z) {
    setObjectNodePosition(this.ptr, nodeName, x, y, z);
}

Model.prototype.setNodeRotation = function(nodeName, degreesX, degreesY, degreesZ, x, y, z) {
    setObjectNodeRotation(this.ptr, nodeName, degreesX, degreesY, degreesZ, x, y, z);
}

Model.prototype.setNodeScale = function(nodeName, x, y, z) {
    setObjectNodeScale(this.ptr, nodeName, x, y, z);
}

Model.prototype.setColor = function(r, g, b, a) {
    setObjectColor(this.ptr, r/255, g/255, b/255, a/255);
}

Model.prototype.draw = function() {
    drawObject(this.ptr, this.cameraName, this.fps, this.clearDepthBuffer === true ? 1 : 0);
}
