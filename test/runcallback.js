'use strict';

const addon = require('bindings')('addon.node');

setTimeout(() => {
  console.log('This will not be logged to the console');
  console.log('Due to the exception below');
}, 2000)

addon.runCallback((str) => {
  console.log('js: runCallback callback');
  throw new Error('Error in callback');
});

