# from datetime import date
# from matplotlib.pyplot import connect
import pytz
import traceback
from typing import List

from sqlalchemy.orm import Session
from fastapi import APIRouter, Depends, HTTPException
from app.config.db import SessionLocal, engine
from app.schemas.data_schema import DataEntry
from app.utils import data_utils
import app.models.data_model as data_model
from datetime import datetime, tzinfo
data_model.Base.metadata.create_all(bind=engine)

data = APIRouter(
    prefix='/data',
    tags=['data']
)


def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


@data.get('/', response_model=List[DataEntry])
async def fetchData(skip: int = 0, limit: int = 100, db: Session = Depends(get_db)):
    allData = data_utils.get_data(db, skip=skip, limit=limit)
    return allData


@data.post('/', response_model=DataEntry)
async def postData(data: DataEntry, db: Session = Depends(get_db)):
    dt_object = data.timestamp.replace(tzinfo=pytz.utc)
    print(dt_object)
    try:
        timestamp = datetime.isoformat(data.timestamp)
        print(timestamp)
    except:
        print(traceback.print_exc())
        raise HTTPException(status_code=400, detail="Invalid timestamp format")

    return data_utils.createDataEntry(db=db, dataEntry=data)
# async def fetchData():
#     return connection.execute(dataEntries.select()).fetchall()
# @data.get('/{id}')
# async def fetchSingleEntry(id: int):
#     return connection.execute(dataEntries.select().where(dataEntries.c.id == id)).first()
# @data.post('/')
# async def createEntry(data: DataEntry):
#     # If timestamp in correct format
#     timestamp = data.timestamp
#     print(datetime.fromisoformat(timestamp))
#        except:
#             return
#         # Insert into database and return 200 Ok
#         # Otherwise
#         # Return 400
#         connection.execute(dataEntries.insert().values(
#             distance=data.distance,
#             timestamp=data.timestamp
#         ))
#         return connection.execute(dataEntries.select()).fetchall()
# @data.put('/{id}')
# async def updateEntry(id: int, dataEntry: DataEntry):
#     connection.execute(dataEntries.update().values(
#         distance=dataEntry.distance,
#         timestamp=dataEntry.timestamp
#     ).where(dataEntries.c.id == id))
#     return connection.execute(dataEntries.select()).fetchall()
# @data.delete('/{id}')
# async def deleteUser(id: int):
#     connection.execute(dataEntries.delete().where(dataEntries.c.id == id))
#     return connection.execute(dataEntries.select()).fetchall()
