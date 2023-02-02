import scrapy
import sqlite3
import csv
from scrapy.linkextractors import LinkExtractor

def append(url, title, desc):
    file = "crawler.db"
    with sqlite3.connect(file) as db:
        c = db.cursor()
        c.execute("INSERT INTO crawles (url, title, desc) VALUES (?, ?, ?)", (url, title, desc))
        db.commit()


class PoliteSpider(scrapy.Spider):
    name = "PoliteSpider"

    with open('top-1m.csv', newline='') as f:
        reader = csv.reader(f)
        data = list(reader)
        start_urls = []
        for i in range(0, len(data)):
            start_urls.append("https://" + data[i][1])

    link_extractor = LinkExtractor()

    def parse(self, response):
        for link in self.link_extractor.extract_links(response):
            yield scrapy.Request(link.url, callback=self.parse)

        append(response.url, response.css("title::text").get(),
               response.css("meta[name=description]::attr(content)").get())
