// Some partial implementation of standard JS stuff

// https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest

var XMLHttpRequest = function() {
    this.type = "POST";
    this.url = "";
    this.headers = {};
    this.onload = void null;
    this.onreadystatechange = void null;
    this.onerror = void null;

    this.responseText = null;
    this.status = 0;
    this.readyState = 0;
}

XMLHttpRequest.prototype.open = function(type, url) {
    this.type = type;
    this.url = url;
}

XMLHttpRequest.prototype.setRequestHeader = function(key, value) {
    this.headers[key] = value;
}

XMLHttpRequest.prototype.send = function(data) {
    this.responseText = null;
    this.status = 0;
    this.readyState = 0; // 0: request not initialized 

    curlAsyncSend(this, this.type, this.url, this.headers, data);
}

XMLHttpRequest.prototype._callFinalEvents = function() {
    this.readyState = 4; // 4: request finished and response is ready

    this._callEvent('onreadystatechange');

    if (this.status >= 200 && this.status < 400) {
        this._callEvent('onload');
    } else if (this.status >= 400) {
        this._callEvent('onerror');
    }
}

XMLHttpRequest.prototype._callEvent = function(event) {
    if (event in this) {
        var func = this[event];

        if (func !== void null) {
            if (typeof(func) === 'function') {
                func();
            } else {
                loggerWarning("Event defined but not callable! event:" + JSON.stringify(event,null,2) + ", this:" + JSON.stringify(this, null, 2));
            }
        }
    }
}
