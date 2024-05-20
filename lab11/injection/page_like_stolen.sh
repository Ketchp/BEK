curl 'http://boura.fit.cvut.cz/fakebook/static/ajax.php' \
  -H 'Accept: text/javascript, application/javascript, application/ecmascript, application/x-ecmascript, */*; q=0.01' \
  -H 'Accept-Language: sk-SK,sk;q=0.9,cs;q=0.8,en-US;q=0.7,en;q=0.6' \
  -H 'Cache-Control: no-cache' \
  -H 'Connection: keep-alive' \
  -H 'Content-Type: application/x-www-form-urlencoded' \
  -H 'Cookie: _ga_54R78HKGNS=GS1.1.1712999948.2.0.1712999948.0.0.0; _ga=GA1.1.977767694.1712485344; PHPSESSID=8j50hpm43emulgaf0k2m4eo123; cored889visit=1716223691; cored889user_id=33; cored889user_hash=29733578d23454a4fb9f5514b1fb6516feb706fe5d7e87925e; cored889last_login=1716223740' \
  -H 'Origin: http://boura.fit.cvut.cz' \
  -H 'Pragma: no-cache' \
  -H 'Referer: http://boura.fit.cvut.cz/fakebook/index.php?do=/pages/2/' \
  -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36' \
  -H 'X-Requested-With: XMLHttpRequest' \
  --data-raw '&core[ajax]=true&core[call]=like.add&type_id=pages&item_id=2&core[security_token]=d4492e2099c22baa3d901a80a9722a5d&core[is_admincp]=0&core[is_user_profile]=0&core[profile_user_id]=0' \
  --insecure \
> /dev/null
