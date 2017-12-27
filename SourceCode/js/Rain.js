/*===== Must have =====*/
+(function (factory) {
    if (typeof exports === 'undefined') {
        factory(webduino || {});
    } else {
        module.exports = factory;
    }
}(function (scope) {
    'use strict';
    var proto;
  /*===== Must have =====*/

  /*===== 開始加入全域變數 =====*/
    var Module = scope.Module,
        BoardEvent = scope.BoardEvent,
        proto;

    var Rain_MESSAGE = [0x04, 0x34],
        MIN_READ_INTERVAL = 1000,
        MIN_RESPONSE_TIME = 30,
        RETRY_INTERVAL = 6000;

    var RainEvent = {
        READ: 'read',
        READ_ERROR: 'readError'
    };

    function Rain(board) {
        Module.call(this);

        this._type = 'Rain';
        this._board = board;
        this._Rain = null;
        this._lastRecv = null;
        this._readTimer = null;
        this._readCallback = function () {};

        this._board.on(BoardEvent.BEFOREDISCONNECT, this.stopRead.bind(this));
        this._messageHandler = onMessage.bind(this);
        this._board.on(BoardEvent.ERROR, this.stopRead.bind(this));
    }

    function onMessage(event) {
        var message = event.message;

        if (message[0] !== Rain_MESSAGE[0] || message[1] !== Rain_MESSAGE[1]) {
            return;
        } else {
            processRainData(this, message);
        }
    }

    function processRainData(self, data) {
        var str = '',
            i = 2,
            MAX = data.length,
            dd = [],
            d1;
            // while (i < data.length) {
                // d1 = data[i];
                // d1 && (str += (d1));
                // i += 1;
                // if ((i - 2) % MAX === 0) {
            dd.push(data[2]);
                    // str = '';
                // }
            // }

            self._lastRecv = Date.now();
            self.emit(RainEvent.READ, dd[0]/10);
    }

    Rain.prototype = proto = Object.create(Module.prototype, {
        constructor: {
            value: Rain
        },

        Rain: {
            get: function () {
                return this._Rain;
            }
        },
    });

    proto.read = function (callback, interval) {
        var self = this,
            timer;

        self.stopRead();

        if (typeof callback === 'function') {
            self._readCallback = function (Rain) {
                self._Rain = Rain;
                callback({
                    Rain: Rain
                });
            };
            self._board.on(BoardEvent.SYSEX_MESSAGE, self._messageHandler);
            self.on(RainEvent.READ, self._readCallback);

            timer = function () {
                self._board.sendSysex(Rain_MESSAGE[0], [Rain_MESSAGE[1]]);
                if (interval) {
                    interval = Math.max(interval, MIN_READ_INTERVAL);
                    if (self._lastRecv === null || Date.now() - self._lastRecv < 5 * interval) {
                        self._readTimer = setTimeout(timer, interval);
                    } else {
                        self.stopRead();
                        setTimeout(function () {
                            self.read(callback, interval);
                        }, RETRY_INTERVAL);
                    }
                }
            };

            timer();
        } else {
            return new Promise(function (resolve, reject) {
                self.read(function (data) {
                    self._Rain = data.Rain;
                    setTimeout(function () {
                        resolve(data);
                    }, MIN_RESPONSE_TIME);
                });
            });
        }
    };

    proto.stopRead = function () {
        this.removeListener(RainEvent.READ, this._readCallback);
        this._board.removeListener(BoardEvent.SYSEX_MESSAGE, this._messageHandler);
        this._lastRecv = null;

        if (this._readTimer) {
            clearTimeout(this._readTimer);
            delete this._readTimer;
        }
    };

    scope.module.RainEvent = RainEvent;
    scope.module.Rain = Rain;
}));