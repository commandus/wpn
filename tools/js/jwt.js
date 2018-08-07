
const vapidHelper = require('web-push');

const audience = 'https://updates.push.services.mozilla.com';
const subject = 'mailto:andrei.i.ivanov@gmail.com';
const publicKey = 'BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A';
const privateKey = '_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk';
const contentEncoding = 'aesgcm';
const exp = 1533621648;

const h = vapidHelper.getVapidHeaders(
  audience,
  subject,
  publicKey,
  privateKey,
  contentEncoding,
  exp
);

console.log(h);