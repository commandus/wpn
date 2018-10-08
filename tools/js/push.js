var http = require('http');
// store a reference to the original request function
const originalRequest = http.request; 
http.request = function wrapMethodRequest(req) {
  console.log(req);
  return originalRequest.apply(this, arguments);
}
var webPush = require('web-push');
/*
webPush.setVapidDetails(
  'mailto:andrei.i.ivanov@gmail.com',
  'BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A',
  '_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk'
);
*/

/*
webPush.setVapidDetails(
  'mailto:andrei.i.ivanov@gmail.com',
  'BM97-HP_Pw_RIrkp1mwVaYTEgR21Pl4PD1QYzDGYi5o7mp-YB6Cr9Pbz7_D7l3r5Zb4Ji-pLKubCza_lE4SsZIA',
  'tt8Cgw4QEZwrr7CN2d-4ITxaAygiQ7j5kL6uzz9P-Sg'
);
*/

webPush.setVapidDetails(
  'mailto:andrei.i.ivanov@gmail.com',
  'BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A',
  '_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk'
);

// chrome
/*
var pushSubscription = {
  endpoint: 'https://fcm.googleapis.com/fcm/send/fsvJsFvUpvE:APA91bFLUUr0Owxupb1AqRZ_DE5AfVta35Hm2SAbczaGEQF6PgtEbhI0_ZWArirhcbioKakGPPR5lq4plQBm6QJazCKTiuQvRE1ptidKLq6S2y7h_89spPRi_E9ncJS59A5knRnKSxRh1T6TzJKizW739bWAQm7KKg',
  keys:{
    p256dh:"BK0nI6BHSAM7yhv-5TUybvgzUSePr95RUkvNwDw3D7EsG_p0XpbbXnnY7PcwVWv9-v-17dDvD1mK7n4LWXcgSnM", 
    auth:"yMBwZZsKcENvwyeAux8FVg"
  }
};
*/


// firefox
/*
var pushSubscription = {
  endpoint: 'https://updates.push.services.mozilla.com/wpush/v2/gAAAAABbZ7cIJuyrIqApNuZd0AVjSSrYk5Cef5cI29-g8iRpHvFZzvqO6bI0ymUcf1tJpvg0lCIF7GxAbU7yg7EMXUh6c4MKaFPsSEsLzC7Mlb1JyIAMz5Wf0orVg15A2OD9dBCCUwbol78DdinNpwz-ExA67dH7InfiUDeYZS6QmVNXaPhzpGo',
  keys:{
    p256dh:"BBpYsgvCmjRZTlwQ__nWoeaLwuqxVc9Eg-GSloPxQdvVxapVybJKJMns8IMkYQUDiLBrnXp-qFugkPBq3fOncvY", 
    auth:"4SgZbJVmKUP56tJ39wcWPw"
  }
};
*/

// firefox
var pushSubscriptionFirefox = {
  endpoint: 'https://updates.push.services.mozilla.com/wpush/v2/gAAAAABburDAsGO8igS8MAF2pwRlidA-nijrdrTVGDbN00aNWiYqWDfEivBo7WZ6Bj1NVeeoIsg7uti7n8ssstB4t2nHZZHt6XYWPUpoKXVyQmgtev-L95TM-qNqhbEFPVCo08B3nW1OdWLpM0fccKnrDQ4sTxnCEoG8ZE3SyyQKWoIBrkRh9bY',
  keys:{
    p256dh: 'BBImS4WgrcgEqZUwKAz3u1cg5e68ee2dvrIivrdvqcM52YN8YQ4cIPUFfyzLf2d95pqjPO3prBsEXP5vGZI6lYU',
    auth: '3UsND6oZbYoUJlcOpWGgpA'
  }
};

// chrome
var pushSubscriptionChrome = {
  endpoint: 'https://fcm.googleapis.com/fcm/send/cxy8w75xPcI:APA91bGYNQyc_A3pKME-piWvKgev-8D8XtgBZ3-8IQFO0J1NJJ3WS86GYyJ7vZhrOlKyCwnGg8R1FSVv3OL5rEpRziEMObQ9y__GlsEC5zMMtFgrBpr7NN6X-abziVdJA_IWipOM2_Pb',
  keys:{
    p256dh: 'BAryHD8i_Qz7b2PZFYbUn9U8Ob8bqM3GgOQuezJ9OlmQ9DzuqDtZ08UIvFRmRMF_Zuc87aGHJwBmez-vKXndBrE',
    auth: '1nlG9tMOBn_SYYgtqPE7vw'
  }
};

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
  contentEncoding: 'aes128gcm'
  //contentEncoding: 'aesgcm'
};

webPush.sendNotification(
  pushSubscriptionFirefox,
  payload,
  options
).then(
  value => console.log(value), 
  error => console.log(error)
);

webPush.sendNotification(
  pushSubscriptionChrome,
  payload,
  options
).then(
  value => console.log(value), 
  error => console.log(error)
);
