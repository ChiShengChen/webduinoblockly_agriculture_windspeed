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

    var Wind_MESSAGE = [0x04, 0x35],
        MIN_READ_INTERVAL = 1000,
        MIN_RESPONSE_TIME = 30,
        RETRY_INTERVAL = 6000;

    var WindEvent = {
        READ: 'read',
        READ_ERROR: 'readError'
    };

    function Wind(board) {
        Module.call(this);

        this._type = 'Wind';
        this._board = board;
        this._Wind = null;
        this._WindVoltage = null;
        this._WindsensorVoltage = null;
        this._lastRecv = null;
        this._readTimer = null;
        this._readCallback = function () {};

        this._board.on(BoardEvent.BEFOREDISCONNECT, this.stopRead.bind(this));
        this._messageHandler = onMessage.bind(this);
        this._board.on(BoardEvent.ERROR, this.stopRead.bind(this));
    }

    function onMessage(event) {
        var message = event.message;

        if (message[0] !== Wind_MESSAGE[0] || message[1] !== Wind_MESSAGE[1]) {
            return;
        } else {
            processWindData(this, message);
        }
    }

    function processWindData(self, data) {
        var str = '',
            i = 2,
            MAX = 4,
            dd = [],
            d1;
            dd.push(data[2]);
            dd.push(data[3]);

            self._lastRecv = Date.now();
            self.emit(WindEvent.READ, dd[0], dd[1]/100);
    }

    Wind.prototype = proto = Object.create(Module.prototype, {
        constructor: {
            value: Wind
        },

        Wind: {
            get: function () {
                return this._Wind;
            }
        },
        WindVoltage: {
            get: function () {
                return this._WindVoltage;
            }
        },
    });

    proto.read = function (callback, interval) {
        var self = this,
            timer;

        self.stopRead();
        if (typeof callback === 'function') {
            self._readCallback = function (Wind, WindVoltage) {
                self._Wind = Wind;
                self._WindVoltage = WindVoltage;
                callback({
                    Wind: Wind,
                    WindVoltage: WindVoltage,
                });
            };
            self._board.on(BoardEvent.SYSEX_MESSAGE, self._messageHandler);
            self.on(WindEvent.READ, self._readCallback);

            timer = function () {
                self._board.sendSysex(Wind_MESSAGE[0], [Wind_MESSAGE[1]]);
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
                    self._Wind = data.Wind;
                    self._WindVoltage = data.WindVoltage;
                    setTimeout(function () {
                        resolve(data);
                    }, MIN_RESPONSE_TIME);
                });
            });
        }
    };

    proto.stopRead = function () {
        this.removeListener(WindEvent.READ, this._readCallback);
        this._board.removeListener(BoardEvent.SYSEX_MESSAGE, this._messageHandler);
        this._lastRecv = null;

        if (this._readTimer) {
            clearTimeout(this._readTimer);
            delete this._readTimer;
        }
    };

    scope.module.WindEvent = WindEvent;
    scope.module.Wind = Wind;
}));