var Image = function() {
    this.ptr = undefined;
    this.id = undefined;
    this.filename = undefined;
    this.width = undefined;
    this.height = undefined;
}

Image.prototype.load = function(filename) {
    this.filename = filename;
    var legacy = imageLoadImage(filename);
    this.ptr = legacy.ptr;
    this.id = legacy.id;
    this.width = legacy.width;
    this.height = legacy.height;
}

Image.prototype.setBlendFunc = function(src, dst) {
    loggerWarning("setBlendFunc not implemented");
}

Image.prototype.setPerspective2d = function(perspective2d) {
    setTexturePerspective3d(this.ptr, perspective2d === true ? 0 : 1);
}

Image.prototype.setCanvasDimensions = function(width, height) {
    loggerWarning("setCanvasDimensions not implemented");
    //setTextureCanvasDimensions(this.ptr, width, height);
}

Image.prototype.setUvDimensions = function(width, height) {
    loggerWarning("setTextureUvDimensions not implemented");
    //setTextureUvDimensions(this.ptr, uMin, vMin, uMax, vMax);
}

Image.prototype.setUnitTexture = function(index, ptr) {
    setTextureUnitTexture(this.ptr, index, ptr);
}

Image.prototype.setPivot = function( x, y, z) {
    loggerWarning("setTexturePivot not implemented");
    //setTexturePivot(this.ptr, x, y, z);
}

Image.prototype.setRotation = function(degreesX, degreesY, degreesZ, x, y, z) {
    setTextureRotation(this.ptr, degreesX, degreesY, degreesZ, x, y, z);
}

Image.prototype.setScale = function(x, y, z) {
    setTextureScale(this.ptr, x, y, z);
}

Image.prototype.setPosition = function(x, y, z) {
    setTexturePosition(this.ptr, x, y, z);
}

Image.prototype.setCenterAlignment = function(align) {
    setTextureCenterAlignment(this.ptr, align);
}

Image.prototype.setColor = function(r, g, b, a) {
    setTextureColor(this.ptr, r, g, b, a);
}

Image.prototype.setSizeToScreenSize = function() {
    setTextureSizeToScreenSize(this.ptr);
}

Image.prototype.setDefaults = function() {
    //loggerWarning("setTextureDefaults not implemented");
    //setTextureDefaults(this.ptr);
}

Image.prototype.draw = function() {
    drawTexture(this.ptr);
}
