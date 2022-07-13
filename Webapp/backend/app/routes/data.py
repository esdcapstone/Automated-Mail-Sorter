from matplotlib.pyplot import connect
from fastapi import APIRouter
from app.config.db import connection
from app.schemas.data import DataEntry
from app.models.data import dataEntries

data = APIRouter(
    prefix='/data',
    tags=['data']
)


@data.get('/')
async def fetchData():
    return connection.execute(dataEntries.select()).fetchall()


# @data.get('/{id}')
# async def fetchEntry()
#     return connection.execute(dataEntries.select().where(dataEntries.c.id == id)).first()


@data.post('/')
async def createEntry(data: DataEntry):
    connection.execute(dataEntries.insert().values(
        province=data.province,
        timestamp=data.timestamp
    ))
    return connection.execute(dataEntries.select()).fetchall()
