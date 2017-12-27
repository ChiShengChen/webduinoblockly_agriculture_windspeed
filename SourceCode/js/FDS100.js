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

    var FDS100_MESSAGE = [0x04, 0x30],
        MIN_READ_INTERVAL = 1000,
        MIN_RESPONSE_TIME = 30,
        RETRY_INTERVAL = 6000;

    var FDS100Event = {
        READ: 'read',
        READ_ERROR: 'readError'
    };

    function FDS100(board) {
        Module.call(this);

        this._type = 'FDS100';
        this._board = board;
        this._humidity = null;
        this._lastRecv = null;
        this._readTimer = null;
        this._readCallback = function () {};

        this._board.on(BoardEvent.BEFOREDISCONNECT, this.stopRead.bind(this));
        this._messageHandler = onMessage.bind(this);
        this._board.on(BoardEvent.ERROR, this.stopRead.bind(this));
    }

    function onMessage(event) {
        var message = event.message;

        if (message[0] !== FDS100_MESSAGE[0] || message[1] !== FDS100_MESSAGE[1]) {
            return;
        } else {
            processFDS100Data(this, message);
        }
    }

    function processFDS100Data(self, data) {
        var str = '',
            i = 2,
            MAX = 4,
            dd = [],
            d1;
        while (i < data.length) {
            d1 = data[i];
            d1 && (str += (d1 - 48));
            i += 1;

            if ((i - 1) % MAX === 0) {
                dd.push((((parseInt(str)-204.0)*100.0)/(1024.0-204.0)).toFixed(2));
                str = '';
            }
        }

        self._lastRecv = Date.now();
        self.emit(FDS100Event.READ, dd[0]);
    }

    FDS100.prototype = proto = Object.create(Module.prototype, {
        constructor: {
            value: FDS100
        },

        humidity: {
            get: function () {
                return this._humidity;
            }
        },
    });

    proto.read = function (callback, interval) {
        var self = this,
            timer;

        self.stopRead();

        if (typeof callback === 'function') {
            self._readCallback = function (humidity, temperature) {
                self._humidity = humidity;
                callback({
                    humidity: humidity,
                });
            };
            self._board.on(BoardEvent.SYSEX_MESSAGE, self._messageHandler);
            self.on(FDS100Event.READ, self._readCallback);

            timer = function () {
                self._board.sendSysex(FDS100_MESSAGE[0], [FDS100_MESSAGE[1]]);
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
                    self._humidity = data.humidity;
                    setTimeout(function () {
                        resolve(data);
                    }, MIN_RESPONSE_TIME);
                });
            });
        }
    };

    proto.stopRead = function () {
        this.removeListener(FDS100Event.READ, this._readCallback);
        this._board.removeListener(BoardEvent.SYSEX_MESSAGE, this._messageHandler);
        this._lastRecv = null;

        if (this._readTimer) {
            clearTimeout(this._readTimer);
            delete this._readTimer;
        }
    };

    scope.module.FDS100Event = FDS100Event;
    scope.module.FDS100 = FDS100;
}));