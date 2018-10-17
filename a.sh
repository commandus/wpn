#!/bin/sh
# -p, --private-key VAPID private key
#     93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk
# -k, --public-key VAPID public key
#     BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A 
# -d, --p256dh Recipient's endpoint p256dh
#     BHVRcPq9Mf8Ci2T3YwknrvSB2r-0qypk_
# -a, --auth Recipient's endpoint auth
#     Hx5DZb93lE98ub6KYRdveg 

#Sender
private=_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk
public=BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A

# Recipient Firefox
registrationFF=gAAAAABburDAsGO8igS8MAF2pwRlidA-nijrdrTVGDbN00aNWiYqWDfEivBo7WZ6Bj1NVeeoIsg7uti7n8ssstB4t2nHZZHt6XYWPUpoKXVyQmgtev-L95TM-qNqhbEFPVCo08B3nW1OdWLpM0fccKnrDQ4sTxnCEoG8ZE3SyyQKWoIBrkRh9bY
p256dhFF=BBImS4WgrcgEqZUwKAz3u1cg5e68ee2dvrIivrdvqcM52YN8YQ4cIPUFfyzLf2d95pqjPO3prBsEXP5vGZI6lYU
authFF=3UsND6oZbYoUJlcOpWGgpA

# Recipient Chrome
registrationCH=cxy8w75xPcI:APA91bGYNQyc_A3pKME-piWvKgev-8D8XtgBZ3-8IQFO0J1NJJ3WS86GYyJ7vZhrOlKyCwnGg8R1FSVv3OL5rEpRziEMObQ9y__GlsEC5zMMtFgrBpr7NN6X-abziVdJA_IWipOM2_Pb
p256dhCH=BAryHD8i_Qz7b2PZFYbUn9U8Ob8bqM3GgOQuezJ9OlmQ9DzuqDtZ08UIvFRmRMF_Zuc87aGHJwBmez-vKXndBrE
authCH=1nlG9tMOBn_SYYgtqPE7vw

# Recipient wpnr chrome
registrationWR=eM9Se1TKu0I:APA91bHW8_08w8Zsc1G-EAvPL1SCOF_X1WUo8IvHkJmhamaIlOOmxChx9MVkRSEsL6F1rBw8UlzQOqgKd0G90feFI8Al9fgivr2xtD3qNfgJRspy5AqMaoV95UnTijaD7LffT9jJomUX
p256dhWR=BOv3pO9cZ8O7aENzybIY3TsdemSk41a9e_OeC23vMrAah3NTHYutDdMXbWV3yFTGFzFPVgtl5eR-gXtpdJmJef0
authWR=pGZrBtdtWGpyUdMQ_hFHgw

# Recipient wpnr ff
registrationWR=eM9Se1TKu0I:APA91bHW8_08w8Zsc1G-EAvPL1SCOF_X1WUo8IvHkJmhamaIlOOmxChx9MVkRSEsL6F1rBw8UlzQOqgKd0G90feFI8Al9fgivr2xtD3qNfgJRspy5AqMaoV95UnTijaD7LffT9jJomUX
p256dhWR=BOv3pO9cZ8O7aENzybIY3TsdemSk41a9e_OeC23vMrAah3NTHYutDdMXbWV3yFTGFzFPVgtl5eR-gXtpdJmJef0
authWR=pGZrBtdtWGpyUdMQ_hFHgw

#     Hx5DZb93lE98ub6KYRdveg 
# ./wpn -m -f "andrei.i.ivanov@gmail.com" -t "hi there" -b "body message" -i https://commandus.com/favicon.ico -l https://commandus.com/ \
# -p $private -k $public -d $p256dh -a $auth -vvv https://fcm.googleapis.com/fcm/send/$registration
 
#./wpnw -f "andrei.i.ivanov@gmail.com" -b "body message" -t "hi there" -i "https://commandus.com/favicon.ico" -l "https://commandus.com/" \
#-r $registrationFF -d $p256dhFF -a $authFF -vvv -k $private -K $public -p firefox

#./wpnw -f "andrei.i.ivanov@gmail.com" -b "body message" -t "hi there" -i "https://commandus.com/favicon.ico" -l "https://commandus.com/" \
#-r $registrationCH -d $p256dhCH -a $authCH -vvv -k $private -K $public -p chrome

./wpnw -f "andrei.i.ivanov@gmail.com" -b "body message" -t "hi there" -i "https://commandus.com/favicon.ico" -l "https://commandus.com/" \
-r $registrationWR -d $p256dhWR -a $authWR -vvv -k $private -K $public -p chrome
