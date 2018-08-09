var http = require('http');
// store a reference to the original request function
const originalRequest = http.request; 
http.request = function wrapMethodRequest(req) {
  console.log(req);
  return originalRequest.apply(this, arguments);
}

var webPush = require('web-push');
webPush.setVapidDetails(
  'mailto:andrei.i.ivanov@gmail.com',
  'BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A',
  '_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk'
);


// chrome
var pushSubscription = {
  endpoint: 'https://fcm.googleapis.com/fcm/send/dAob7IITAjk:APA91bF0cuvHE-p8lbpdayCdiGJzNWNfu53Bd6O29cnpbz_8nzZyRHw-ChTo2o43KjGmmlk7T1GjwdaymttN_fjtp7x8gX9jC5yN2F2fPdbVkBp6IXV8wlKCvDyXZy8qDs1ObLlcl6Odic1Wu11oQIJLSZ8Df9cFEA',
  keys:{
    p256dh:"BCjbINwYThS2ZAOKseAmax6ENhfEhqKmMMlUyzdZm2qQy_vA3tMRlOF9ReMAcvsefwHhqSt3WUyCjbUzQ53PSLw", 
    auth:"Jc9t4JP9tonIwjzXty-WxQ"
  }
};


/*
// firefox
var pushSubscription = {
  endpoint: 'https://updates.push.services.mozilla.com/wpush/v2/gAAAAABbZ7cIJuyrIqApNuZd0AVjSSrYk5Cef5cI29-g8iRpHvFZzvqO6bI0ymUcf1tJpvg0lCIF7GxAbU7yg7EMXUh6c4MKaFPsSEsLzC7Mlb1JyIAMz5Wf0orVg15A2OD9dBCCUwbol78DdinNpwz-ExA67dH7InfiUDeYZS6QmVNXaPhzpGo',
  keys:{
    p256dh:"BBpYsgvCmjRZTlwQ__nWoeaLwuqxVc9Eg-GSloPxQdvVxapVybJKJMns8IMkYQUDiLBrnXp-qFugkPBq3fOncvY", 
    auth:"4SgZbJVmKUP56tJ39wcWPw"
  }
};
*/

var payload = JSON.stringify(
  {
    notification: {
      title: 'Title',
      body: 'Body',
      icon: 'https://commandus.com/favicon.ico',
      click_action: 'https://commandus.com/'
    } 
  }
);

var options = {
  contentEncoding: 'aesgcm'
};

webPush.sendNotification(
  pushSubscription,
  payload,
  options
).then(
  value => console.log(value), 
  error => console.log(error)
);
