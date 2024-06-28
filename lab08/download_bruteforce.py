import time
import requests
from pathlib import Path


SAVE_DIR = Path(__file__).parent
CLUB_URL = 'http://boura.fit.cvut.cz/nightclub/download.php'
LINKS = [
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/Common-PHP-Filenames.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/Apache.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/ApacheTomcat.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/CommonBackdoors-PHP.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/Logins.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/PHP.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/Passwords.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/Randomfiles.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/UnixDotfiles.fuzz.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/common.txt',
    'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/versioning_metafiles.txt',
]


def _main(name_source):
    names = requests.get(name_source).text.strip().split('\n')

    for idx, name in enumerate(names, start=1):
        # req = requests.get(CLUB_URL, params={'file': f'../{name}'})
        req = requests.get(CLUB_URL, params={'file': f'../extra/{name}'})

        if req.text:
            save_file = SAVE_DIR / name
            print(f'Found: {name}, saving to: {save_file}')

            with open(save_file, 'w') as file:
                file.write(req.text)

        if idx % 1000 == 0:
            print(f'Completed: {idx}/{len(names)}')


if __name__ == '__main__':
    for link in LINKS:
        _main(link)
