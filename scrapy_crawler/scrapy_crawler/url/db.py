"""
Wrapper for using a SQLite3 database to store a list because I like making things harder to follow.
Author: Mineinjava#1475
"""
import sqlite3


def create_db(name):
    with sqlite3.connect(name) as db:
        db.execute("CREATE TABLE crawles (url TEXT, title TEXT, desc TEXT)")
        db.commit()


if __name__ == '__main__':
    name = input("Name of the database (including .db): ")
    create_db(name)
