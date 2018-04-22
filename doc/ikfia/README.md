# Firebase

## Ключи

Идентификатор отправителя

246829423295

Ключ сервера

AAAAOXguGr8:APA91bHurXbFqZxxlwKsdCQP3D0Zu6Btugw8zJ-M7CUUYxcDVoBh0oGz2LeetfTy_idy2KjGJH9p4HGbGhj5A6ANYR4J-xJJTM7TfsJrcbYqX5s6a8AcZlPrnYncuo-VkWHte9UpUvw5

Устаревший ключ сервера

AIzaSyAyz-oVjsfRNK53XA6o9DVHN9ZNAaHOfxw

## Тестовый хостинг

https://ikfia.wpn.commandus.com/

### FTP

Имя пользователя FTP: wpn@commandus.com

FTP-сервер: ftp.commandus.com

FTP & явный порт FTPS:  21

### Вебдиск

Address:

commandus.com

Port:

2078

Username:

wpn@commandus.com

SSL Enabled: включен

## Консоль

Аккаунт ikfia3@gmail.com

Пароль 30Негритят

Проект ikfia-wpn

## Web 
<script src="https://www.gstatic.com/firebasejs/4.13.0/firebase.js"></script>
<script>
  // Initialize Firebase
  var config = {
    apiKey: "AIzaSyBfUt1N5aabh8pubYiBPKOq9OcIoHv_41I",
    authDomain: "ikfia-wpn.firebaseapp.com",
    databaseURL: "https://ikfia-wpn.firebaseio.com",
    projectId: "ikfia-wpn",
    storageBucket: "",
    messagingSenderId: "246829423295"
  };
  firebase.initializeApp(config);
</script>



## Android

Открыв вид Проект в Android Studio, перейдете в корневой каталог своего проекта.

Переместите файл google-services.json в корневой каталог модуля для приложений Android (app).

### Gradle

Плагин сервисов Google для Gradle загружает google-services.json.

Чтобы использовать плагин, измените файлы build.gradle.

Файл build.gradle уровня проекта (<project>/build.gradle):

```
buildscript {
  dependencies {
    // Add this line
    classpath 'com.google.gms:google-services:3.2.0'
  }
}
```

Файл build.gradle уровня приложения (<project>/<app-module>/build.gradle):

```
dependencies {
  // Add this line
  compile 'com.google.firebase:firebase-core:12.0.1'
}

...

// Add to the bottom of the file
apply plugin: 'com.google.gms.google-services'
```

сервис Analytics включен по умолчанию help_outline

Нажмите кнопку Sync now (Синхронизовать), которая появится на панели в интегрированной среде разработки.


### Android NDK C++

Скачать и установить в проекте Firebase SDK

https://firebase.google.com/docs/cpp/setup?authuser=2


## Отправка уведомлений из консоли

https://console.firebase.google.com/u/2/project/ikfia-wpn/notification

