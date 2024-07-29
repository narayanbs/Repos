from concurrent.futures import ThreadPoolExecutor

urls = ["x.com", "planetf1.com", "ndtv.com"]


def scrape_site(url):
    res = f"{url} was scraped!"
    return res


with ThreadPoolExecutor(max_workers=4) as executor:
    results = executor.map(scrape_site, urls)

for result in results:
    print(result)
