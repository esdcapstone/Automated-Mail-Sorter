from datetime import date
from matplotlib.pyplot import connect
from fastapi import APIRouter
from app.config.db import connection
from app.schemas.data import DataEntry
from app.models.data import dataEntries
from datetime import datetime

data = APIRouter(
    prefix='/data',
    tags=['data']
)


@data.get('/')
async def fetchData():
    return connection.execute(dataEntries.select()).fetchall()


@data.get('/{id}')
async def fetchSingleEntry(id: int):
    return connection.execute(dataEntries.select().where(dataEntries.c.id == id)).first()


@data.post('/')
async def createEntry(data: DataEntry):
    # If timestamp in correct format
    #timestamp = data.timestamp
    #print(datetime.fromisoformat(timestamp))

    # Insert into database and return 200 Ok
    # Otherwise
    # Return 400

    connection.execute(dataEntries.insert().values(
        distance=data.distance,
        timestamp=data.timestamp
    ))
    return connection.execute(dataEntries.select()).fetchall()


@data.put('/{id}')
async def updateEntry(id: int, dataEntry: DataEntry):
    connection.execute(dataEntries.update().values(
        distance=dataEntry.distance,
        timestamp=dataEntry.timestamp
    ).where(dataEntries.c.id == id))
    return connection.execute(dataEntries.select()).fetchall()


@data.delete('/{id}')
async def deleteUser(id: int):

    connection.execute(dataEntries.delete().where(dataEntries.c.id == id))
    return connection.execute(dataEntries.select()).fetchall()
