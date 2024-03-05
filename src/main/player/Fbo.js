var Fbo = function() {
    this.ptr = undefined;
    this.name = undefined;
    this.color = undefined;
}

Fbo.prototype.init = function(name) {
    var legacy = fboInit(name);

    this.name = name;
    this.ptr = legacy.ptr;

    if (legacy.color.ptr !== undefined) {
        this.color = new Image();
        this.color.ptr = legacy.color.ptr;
        this.color.id = legacy.color.id;
    }
}

Fbo.prototype.setStoreDepth = function(storeDepth) {
    fboStoreDepth(this.ptr, storeDepth === true ? 1 : 0);
}

Fbo.prototype.setDimensions = function(width, height) {
    fboSetDimensions(this.ptr, width, height);
}

Fbo.prototype.generateFramebuffer = function() {
    fboGenerateFramebuffer(this.ptr);
}

Fbo.prototype.setRenderDimensions = function(width, height) {
    fboSetRenderDimensions(this.ptr, width, height);
}

Fbo.prototype.bind = function() {
    fboBind(this.ptr);
}

Fbo.prototype.unbind = function() {
    fboUnbind(this.ptr);
}

Fbo.prototype.updateViewport = function() {
    fboUpdateViewport(this.ptr);
}

Fbo.prototype.bindTextures = function() {
    fboBindTextures(this.ptr);
}

Fbo.prototype.unbindTextures = function() {
    fboUnbindTextures(this.ptr);
}
