import requests
from concurrent.futures import ThreadPoolExecutor

HOST = "http://localhost:3490"

def worker(i):
    path = f"{HOST}/uploads/test{i}.txt"

    try:
        r = requests.delete(path)
        print(i, r.status_code)
    except Exception as e:
        print(i, e)

with ThreadPoolExecutor(max_workers=100) as pool:
    pool.map(worker, range(1000))