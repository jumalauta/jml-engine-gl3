var Input = function() {
}

Input.prototype.setUserExit = function(userExit) {
    inputSetUserExit(userExit);
}

Input.prototype.isUserExit = function() {
    return inputIsUserExit();
}

Input.prototype.pollEvents = function() {
    return inputPollEvents();
}

Input.prototype.getPressedKeyMap = function() {
    return inputGetPressedKeyMap();
}

