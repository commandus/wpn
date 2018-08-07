var webPush = require('web-push');
webPush.setGCMAPIKey('AIzaSyAyz-oVjsfRNK53XA6o9DVHN9ZNAaHOfxw');
webPush.setVapidDetails(
  'mailto:andrei.i.ivanov@gmail.com',
  'BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A',
  '_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk'
);

var pushSubscription = {
  endpoint: 'https://fcm.googleapis.com/fcm/send/dAob7IITAjk:APA91bF0cuvHE-p8lbpdayCdiGJzNWNfu53Bd6O29cnpbz_8nzZyRHw-ChTo2o43KjGmmlk7T1GjwdaymttN_fjtp7x8gX9jC5yN2F2fPdbVkBp6IXV8wlKCvDyXZy8qDs1ObLlcl6Odic1Wu11oQIJLSZ8Df9cFEA',
  keys:{
    p256dh:"BJ4Kwfg9qJxiQy1RlDX0W-gr5X2sy-K-jinKt_ZNbCLUPWiG0EZrull0SO1_KfRQfntJnFfbHiuCCve7GQuDQTE", 
    auth:"aKEEhoc1tAYueXqBm_frBA"
  }
};

var payload = 'Here is a payload!';

var options = {
  // gcmAPIKey: 'AIzaSyAyz-oVjsfRNK53XA6o9DVHN9ZNAaHOfxw',
  TTL: 60
};

webPush.sendNotification(
  pushSubscription,
  payload,
  options
).then(
  value => console.log(value), 
  error => console.log(error)
);
