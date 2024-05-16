# You can place your own status text in val[user_status] field in --data-raw.

curl 'http://boura.fit.cvut.cz/fakebook/static/ajax.php' \
  -H 'Accept: text/javascript, application/javascript, application/ecmascript, application/x-ecmascript, */*; q=0.01' \
  -H 'Accept-Language: sk-SK,sk;q=0.9,cs;q=0.8,en-US;q=0.7,en;q=0.6' \
  -H 'Cache-Control: no-cache' \
  -H 'Connection: keep-alive' \
  -H 'Content-Type: application/x-www-form-urlencoded' \
  -H 'Cookie: _ga_2LJG5SLB2T=GS1.2.1713886414.2.1.1713887542.0.0.0; _clck=ielgg%7C2%7Cfl8%7C0%7C1576; _ga_Z3PMC2SHMP=GS1.1.1715026184.2.0.1715026184.60.0.0; _ga=GA1.1.538259239.1713805324; _ga_54R78HKGNS=GS1.1.1715758550.6.1.1715758567.0.0.0; PHPSESSID=41c22fvhtf3ppp2j26hu5m3k6k; cored889visit=1715781334; cored889user_id=47; cored889user_hash=017cd2f37f0485b464fad78a322ad9990e5d9b681e7b3307ff; cored889last_login=1715851090' \
  -H 'Origin: http://boura.fit.cvut.cz' \
  -H 'Pragma: no-cache' \
  -H 'Referer: http://boura.fit.cvut.cz/fakebook/index.php?do=/profile-47/' \
  -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36' \
  -H 'X-Requested-With: XMLHttpRequest' \
  --data-raw '&core[ajax]=true&core[call]=user.updateStatus&core[security_token]=d4492e2099c22baa3d901a80a9722a5d&val[user_status]=Here%20you%20can%20place%20your%20text&val[group_id]=&val[action]=upload_photo_via_share&image[]=&val[link][url]=http%3A%2F%2F&val[status_info]=&val[connection][facebook]=0&val[connection][twitter]=0&val[privacy]=0&core[is_admincp]=0&core[is_user_profile]=1&core[profile_user_id]=47' \
  --insecure \
> /dev/null
