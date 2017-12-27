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

    var BME280_MESSAGE = [0x04, 0x32],
        MIN_READ_INTERVAL = 1000,
        MIN_RESPONSE_TIME = 30,
        RETRY_INTERVAL = 6000;

    var BME280Event = {
        READ: 'read',
        READ_ERROR: 'readError'
    };

    function BME280(board) {
        Module.call(this);
        // --------宣告--------- 
        this._type = 'BME280';
        this._board = board;
        this._humidity = null;
        this._temperature = null;
        this._pressure = null;
        this._altitude = null;
        this._lastRecv = null;
        this._readTimer = null;
        this._readCallback = function () {};

        this._board.on(BoardEvent.BEFOREDISCONNECT, this.stopRead.bind(this));
        this._messageHandler = onMessage.bind(this);
        this._board.on(BoardEvent.ERROR, this.stopRead.bind(this));
    }

    function onMessage(event) {
        var message = event.message;

        if (message[0] !== BME280_MESSAGE[0] || message[1] !== BME280_MESSAGE[1]) {
            return;
        } else {
            processBME280Data(this, message);
        }
    }
    // 收Firmata送來的值
    function processBME280Data(self, data) {
        var str = '',
            i = 2,
            MAX = 4,
            dd = [],
            d1;
            while (i < (data.length)) {
                d1 = data[i];
                d1 && (str += (d1 - 48));
                i += 1;

                if ((i - 2) % MAX === 0) {
                    dd.push(parseFloat(str));
                    str = '';
                }
            }

            self._lastRecv = Date.now();
            self.emit(BME280Event.READ, dd[0]/100, dd[1]/100, dd[2]/100, (dd[3]));
    }

    BME280.prototype = proto = Object.create(Module.prototype, {
        constructor: {
            value: BME280
        },

        temperature: {
            get: function () {
                return this._temperature;
            }
        },

        humidity: {
            get: function () {
                return this._humidity;
            }
        },

        altitude: {
            get: function () {
                return this._altitude;
            }
        },

        pressure: {
            get: function () {
                return this._pressure;
            }
        },        
    });

    proto.read = function (callback, interval) {
        var self = this,
            timer;

        self.stopRead();

        if (typeof callback === 'function') {
            self._readCallback = function (temperature, humidity, altitude, pressure) {
                self._temperature = temperature;
                self._humidity = humidity;
                self._altitude = altitude;
                self._pressure = pressure;
                callback({
                    temperature: temperature,
                    humidity: humidity,
                    altitude: altitude,
                    pressure: pressure
                });
            };
            self._board.on(BoardEvent.SYSEX_MESSAGE, self._messageHandler);
            self.on(BME280Event.READ, self._readCallback);

            timer = function () {
                self._board.sendSysex(BME280_MESSAGE[0], [BME280_MESSAGE[1]]);
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
                    self._temperature = data.temperature;
                    self._altitude = data.altitude;
                    self._pressure = data.pressure;
                    setTimeout(function () {
                        resolve(data);
                    }, MIN_RESPONSE_TIME);
                });
            });
        }
    };

    proto.stopRead = function () {
        this.removeListener(BME280Event.READ, this._readCallback);
        this._board.removeListener(BoardEvent.SYSEX_MESSAGE, this._messageHandler);
        this._lastRecv = null;

        if (this._readTimer) {
            clearTimeout(this._readTimer);
            delete this._readTimer;
        }
    };

    scope.module.BME280Event = BME280Event;
    scope.module.BME280 = BME280;
}));