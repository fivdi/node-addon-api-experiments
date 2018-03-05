'use strict';

const addon = require('bindings')('addon.node');

addon.start(() => {
  console.log('js: start callback');
  throw new Error('Error in callback');
});

addon.keepEventLoopAlive();

