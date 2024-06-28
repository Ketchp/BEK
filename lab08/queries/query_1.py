from pathlib import Path


with open(Path(__file__).parent / 'query_1.md') as file:
    data = file.read().partition('[')[-1].partition(']')[0].split('"')[1::2]

data = [row.split(',') for row in data]

print(set(row[1] for row in data))
# {'bek', 'information_schema'}

print(*(row for row in data if 'bek' in row), sep='\n')
# tables: 'accounts', 'performers', 'stories'
