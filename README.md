# uvReq-php

uvReq-php is multi-thread connections by cURL.

## Requirements

PHP 7.x  
cURL shard object  
liburl  

```
$ sudo apti install libcurl4-openssl-dev libuv-dev
```

## Building fastText for PHP

```
$ cd uvReq-php
$ phpize
$ ./configure
$ make -j $(nproc)
$ sudo make install
```

edit your php.ini and add:

```
extension=uvrequest.so
```

## Class synopsis

```php
uvRequest {
    public __construct ( [int try_number, int timeout, string ua, int sleep_time] )
    public array execute ( array urls )
    public array getErrors ( void )
}
```

## Table of Contents

[uvRequest::__construct](#__construct)  
[uvRequest::execute](#execute)  
[uvRequest::getErrors](#geterrors)  

-----

### <a name="__construct">uvRequest::__construct()

Instantiates a uvRequest object.

```php
$ua = 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:74.0) Gecko/20100101 Firefox/74.0';
$uvreq = new uvRequest(
    3,      // number of retries
    30,     // time out
    $ua,    // user agent
    2500    // sleep time
);
```

-----

### <a name="request">array uvRequest::execute(array urls)

Send an HTTP request and get a reply.

```php
$uvreq = new uvRequest();
$res = $uvreq->execute([
    'https://www.google.co.jp',
    'https://www.yahoo.co.jp',
    'https://www.bing.com/'
]);

print_r($res);
```

```
[
    [
        'url'=> 'https://www.google.co.jp',
        'reply'=> '...'
    ],
    [
        'url'=> 'https://www.yahoo.co.jp',
        'reply'=> '...'
    ],
    [
        'url'=> 'https://www.bing.com',
        'reply'=> '...'
    ],

]
```

-----

### <a name="geterrors">array uvRequest::getErrors()

Get the error URL.

```php
$uvreq = new uvRequest();
$res = $uvreq->execute([
    'https://www.google.co.jp',
    'https://www.yahoo.co.jp',
    'https://www.kekekekekekekekekeke.com/'
]);

$errors = $uvreq->getErrors();
print_r($errors);
```

```
[
    'https://www.kekekekekekekekekeke.com'
]
```

-----
### <a name="gdb">Check thread

```
$ gdb php
(gdb) r -f sample.php

Starting program: /usr/local/bin/php -f sample.php
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
[New Thread 0x7fffef2bd700 (LWP 21740)]
[New Thread 0x7fffeeabc700 (LWP 21741)]
[New Thread 0x7fffee2bb700 (LWP 21742)]
[New Thread 0x7fffedaba700 (LWP 21743)]
[New Thread 0x7fffecab8700 (LWP 21744)]
[New Thread 0x7fffdffff700 (LWP 21746)]
[New Thread 0x7fffed2b9700 (LWP 21745)]
[Thread 0x7fffed2b9700 (LWP 21745) exited]
[Thread 0x7fffecab8700 (LWP 21744) exited]
[Thread 0x7fffdffff700 (LWP 21746) exited]
[New Thread 0x7fffdffff700 (LWP 21747)]
[Thread 0x7fffdffff700 (LWP 21747) exited]
[New Thread 0x7fffdffff700 (LWP 21748)]
[Thread 0x7fffdffff700 (LWP 21748) exited]
```
