import sqlite3
import datetime
import asyncio
import json
from websockets.server import serve

DBNAME = "leaderboard"

con = sqlite3.connect("leaderboard.db")
cur = con.cursor()

connections = []

res = cur.execute("SELECT name FROM sqlite_master")
try:
    res = cur.execute(f"CREATE TABLE {DBNAME}(name, time, difficulty, date)")
    print("Created leaderboard")
except sqlite3.OperationalError:
    print("Loading leaderboard")


def post_score(name, time, difficulty):
    cur.execute(
        f"INSERT INTO {DBNAME} VALUES ('{name}', {time}, '{difficulty}', '{datetime.datetime.now()}')")
    con.commit()

def get_scores(difficulty):
    return cur.execute(
        f"SELECT name, min(time) FROM {DBNAME} WHERE difficulty = '{difficulty}' GROUP BY name ORDER BY time"
    ).fetchmany(20)

async def handle(websocket):
    connections.append(websocket)
    async for message in websocket:
        print(message)
        ns = json.loads(message)
        if len(ns) > 1:
            post_score(ns[0][:10], ns[1], ns[2])
            scores = get_scores(ns[2])
            for c in connections:
                try:
                    await c.send(json.dumps(scores))
                except:
                    connections.remove(c)
        else:
            await websocket.send(json.dumps(get_scores(ns[0])))
    connections.remove(websocket)
        

async def main():
    async with serve(handle, "localhost", 8764):
        await asyncio.Future()

asyncio.run(main())

con.close()
