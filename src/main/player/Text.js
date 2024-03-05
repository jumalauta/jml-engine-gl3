var Text = function() {
}

Text.prototype.setFont = function(name) {
    setTextFont(name);
}

Text.prototype.setValue = function(text) {
    setDrawTextString(text);
}

Text.prototype.setDefaults = function() {
    setTextDefaults();
}

Text.prototype.setPivot = function(x, y, z) {
    setTextPivot(x, y, z);
}

Text.prototype.setRotation = function(degreesX, degreesY, degreesZ) {
    setTextRotation(degreesX, degreesY, degreesZ);
}

Text.prototype.setScale = function(x, y, z) {
    setTextSize(x, y, z);
}

Text.prototype.setPosition = function(x, y, z) {
    setTextPosition(x, y, z);
}

Text.prototype.setCenterAlignment = function(align) {
    setTextCenterAlignment(align);
}

Text.prototype.setColor = function(r, g, b, a) {
    setTextColor(r, g, b, a);
}

Text.prototype.setPerspective2d = function(perspective2d) {
    setTextPerspective3d(perspective2d === true ? 0 : 1);
}

Text.prototype.draw = function() {
    drawText();
}
