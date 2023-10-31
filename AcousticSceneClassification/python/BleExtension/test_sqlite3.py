import sqlite3

con = sqlite3.connect("asc.db")
cur = con.cursor()
res = cur.execute("SELECT * FROM test")
print(res.fetchone())