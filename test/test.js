'use strict';

const addon = require('bindings')('addon.node');
let count = 0;

addon.start(() => {
  count += 1;

  if (count >= 2 && count <= 3) {
    console.log('js: inside callback - will throw now');
    throw new Error('Hello, World!');
  } else {
    console.log('js: inside callback');
  }

  if (count === 5) {
    addon.dontKeepEventLoopAlive();
  }
});

addon.keepEventLoopAlive();

