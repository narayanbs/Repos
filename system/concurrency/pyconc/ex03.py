import concurrent.futures
import time


def task(n):
    return n**2


with concurrent.futures.ThreadPoolExecutor(max_workers=2) as executor:
    results = [executor.submit(task, i) for i in range(10234, 10241)]

    for future in concurrent.futures.as_completed(results):
        result = future.result()
        print(result)
