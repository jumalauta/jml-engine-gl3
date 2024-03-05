var SocketType = {
    "UDP": 1,
    "TCP": 0
}

var Socket = function() {
    this.ptr = socketNew();
}

Socket.prototype.delete = function() {
    socketDelete(this.ptr);
    this.ptr = void null;
}

Socket.prototype.setHost = function(host) {
    socketSetHost(this.ptr, host);
}

Socket.prototype.setPort = function(port) {
    socketSetPort(this.ptr, port);
}

Socket.prototype.setType = function(type) {
    socketSetType(this.ptr, type);
}

Socket.prototype.establishConnection = function() {
    return socketEstablishConnection(this.ptr);
}

Socket.prototype.closeConnection = function() {
    return socketCloseConnection(this.ptr);
}

Socket.prototype.sendData = function(array) {
    return socketSendData(this.ptr, array);
}

Socket.prototype.receiveData = function(array) {
    return socketReceiveData(this.ptr, array);
}
