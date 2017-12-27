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

    var UVsensor_MESSAGE = [0x04, 0x33],
        MIN_READ_INTERVAL = 1000,
        MIN_RESPONSE_TIME = 30,
        RETRY_INTERVAL = 6000;

    var UVsensorEvent = {
        READ: 'read',
        READ_ERROR: 'readError'
    };

    function UVsensor(board) {
        Module.call(this);

        this._type = 'UVsensor';
        this._board = board;
        this._UVsensor = null;
        this._UVsensorVoltage = null;
        this._lastRecv = null;
        this._readTimer = null;
        this._readCallback = function () {};

        this._board.on(BoardEvent.BEFOREDISCONNECT, this.stopRead.bind(this));
        this._messageHandler = onMessage.bind(this);
        this._board.on(BoardEvent.ERROR, this.stopRead.bind(this));
    }

    function onMessage(event) {
        var message = event.message;

        if (message[0] !== UVsensor_MESSAGE[0] || message[1] !== UVsensor_MESSAGE[1]) {
            return;
        } else {
            processUVsensorData(this, message);
        }
    }

    function processUVsensorData(self, data) {
        var str = '',
            i = 2,
            MAX = 4,
            dd = [],
            d1;
            // d2;
            console.log(data);
            // while (i < data.length) {
                // d1 = data[2];
                // d2 = data[3];
                // d2 = data[3];
                // d1 && (str += (d1));
                // i += 1;
                // if ((i ) % MAX === 0) {
            dd.push(data[2]);
            dd.push(data[3]);
                    // dd.push(d2);
                    // str = '';
                    // }
                // }

            self._lastRecv = Date.now();
            self.emit(UVsensorEvent.READ, dd[0], dd[1]/100);
    }

    UVsensor.prototype = proto = Object.create(Module.prototype, {
        constructor: {
            value: UVsensor
        },

        UVsensor: {
            get: function () {
                return this._UVsensor;
            }
        },

        UVsensorVoltage: {
            get: function () {
                return this._UVsensorVoltage;
            }
        }
    });

    proto.read = function (callback, interval) {
        var self = this,
            timer;

        self.stopRead();
        if (typeof callback === 'function') {
            self._readCallback = function (UVsensor, UVsensorVoltage) {
                self._UVsensor = UVsensor;
                self._UVsensorVoltage = UVsensorVoltage;
                callback({
                    UVsensor: UVsensor,
                    UVsensorVoltage: UVsensorVoltage
                });
            };
            self._board.on(BoardEvent.SYSEX_MESSAGE, self._messageHandler);
            self.on(UVsensorEvent.READ, self._readCallback);

            timer = function () {
                self._board.sendSysex(UVsensor_MESSAGE[0], [UVsensor_MESSAGE[1]]);
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
                    self._UVsensor = data.UVsensor;
                    self._UVsensorVoltage = data.UVsensorVoltage;
                    setTimeout(function () {
                        resolve(data);
                    }, MIN_RESPONSE_TIME);
                });
            });
        }
    };

    proto.stopRead = function () {
        this.removeListener(UVsensorEvent.READ, this._readCallback);
        this._board.removeListener(BoardEvent.SYSEX_MESSAGE, this._messageHandler);
        this._lastRecv = null;

        if (this._readTimer) {
            clearTimeout(this._readTimer);
            delete this._readTimer;
        }
    };

    scope.module.UVsensorEvent = UVsensorEvent;
    scope.module.UVsensor = UVsensor;
}));