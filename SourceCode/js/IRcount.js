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

    var IRCount_MESSAGE = [0x04, 0x31],
        MIN_READ_INTERVAL = 1000,
        MIN_RESPONSE_TIME = 30,
        RETRY_INTERVAL = 6000;

    var IRCountEvent = {
        READ: 'read',
        READ_ERROR: 'readError'
    };

    function IRCount(board, pin, pin1, pin2) {
        Module.call(this);

        this._type = 'IRCount';
        this._board = board;
        this._pin = pin;
        this._pin1 = pin1;
        this._pin2 = pin2;
        this._pin1count = null;
        this._pin2count = null;
        this._pin3count = null;
        this._lastRecv = null;
        this._readTimer = null;
        this._readCallback = function () {};

        this._board.on(BoardEvent.BEFOREDISCONNECT, this.stopRead.bind(this));
        this._messageHandler = onMessage.bind(this);
        this._board.on(BoardEvent.ERROR, this.stopRead.bind(this));
    }

    function onMessage(event) {
        var message = event.message;

        if (message[0] !== IRCount_MESSAGE[0] || message[1] !== IRCount_MESSAGE[1]) {
            return;
        } else {
            processIRCountData(this, message);
            console.log(message);
        }
    }

    function processIRCountData(self, data) {
        var str = '',
            i = 2,
            // MAX = data.length,
            dd = [];
            // d1;
            // while (i < data.length) {
                // d1 = data[i];
                // d1 && (str += (d1));
                // i += 1;
                // if ((i ) % MAX === 0) {
            dd.push(data[2]);
            dd.push(data[3]);
            dd.push(data[4]);
                    // str = '';
                    // }
                // }
            self._lastRecv = Date.now();
            self.emit(IRCountEvent.READ, dd[0], dd[1], dd[2]);
    }

    IRCount.prototype = proto = Object.create(Module.prototype, {
        constructor: {
            value: IRCount
        },

        pin1count: {
            get: function () {
                return this._pin1count;
            }
        },
        pin2count: {
            get: function () {
                return this._pin2count;
            }
        },
        pin3count: {
            get: function () {
                return this._pin3count;
            }
        },
    });

    proto.read = function (callback, interval) {
        var self = this,
            timer;

        self.stopRead();

        if (typeof callback === 'function') {
            self._readCallback = function (pin1count, pin2count, pin3count) {
                self._pin1count = pin1count;
                self._pin2count = pin2count;
                self._pin3count = pin3count;
                callback({
                    pin1count: pin1count,
                    pin2count: pin2count,
                    pin3count: pin3count,
                });
            };
            self._board.on(BoardEvent.SYSEX_MESSAGE, self._messageHandler);
            self.on(IRCountEvent.READ, self._readCallback);

            timer = function () {
                if (self._pin == null ){
                    self._pin = 0;
                }
                if (self._pin1 == null ){
                    self._pin1 = 0;
                }
                if (self._pin2 == null ){
                    self._pin2 = 0;
                }
                self._board.sendSysex(IRCount_MESSAGE[0], [IRCount_MESSAGE[1], self._pin, self._pin1, self._pin2]);
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
                    self._count = data.count;
                    setTimeout(function () {
                        resolve(data);
                    }, MIN_RESPONSE_TIME);
                });
            });
        }
    };

    proto.stopRead = function () {
        this.removeListener(IRCountEvent.READ, this._readCallback);
        this._board.removeListener(BoardEvent.SYSEX_MESSAGE, this._messageHandler);
        this._lastRecv = null;

        if (this._readTimer) {
            clearTimeout(this._readTimer);
            delete this._readTimer;
        }
    };

    scope.module.IRCountEvent = IRCountEvent;
    scope.module.IRCount = IRCount;
}));