+(function (window, webduino) {
  'use strict';
  window.getWindspeed = function(board, pin) {
    return new webduino.module.windspeed(board, board.getAnalogPin(pin));
  }
}(window, window.webduino));
