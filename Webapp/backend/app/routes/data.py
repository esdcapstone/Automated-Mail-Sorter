# from datetime import date
# from matplotlib.pyplot import connect
from datetime import timezone
import pytz
import traceback
from typing import List

from sqlalchemy.orm import Session
from fastapi import APIRouter, Depends, HTTPException, WebSocket, WebSocketDisconnect
from app.config.db import SessionLocal, engine
from app.schemas.data_schema import DataEntry
from app.utils import data_utils
import app.models.data_model as data_model
from datetime import datetime, tzinfo
data_model.Base.metadata.create_all(bind=engine)

websocketFlag = 0
dataGlobal = None
data = APIRouter(
    prefix='/data',
    tags=['data']
)

validProvinces = ["AB", "BC", "MB", "NB", "NL", "NS", "ON", "PE", "QC", "SK"]


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


class ConnectionManager:
    def __init__(self):
        self.active_connections: List[WebSocket] = []

    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.active_connections.append(websocket)

    def disconnect(self, websocket: WebSocket):
        self.active_connections.remove(websocket)

    async def send_personal_message(self, message: str, websocket: WebSocket):
        await websocket.send_text(message)

    async def broadcast(self, message: str):
        for connection in self.active_connections:
            await connection.send_text(message)


manager = ConnectionManager()


@data.post('/', response_model=DataEntry)
async def postData(data: DataEntry, db: Session = Depends(get_db)):

    try:
        # Conversion of timestamp sent by client to UTC. At the same time it
        # Checks for any error in the iso format of timestamp sent by client
        timestamp = data.timestamp.astimezone(timezone.utc).isoformat()
        print(timestamp)
        # Check if the data that is posted has valid province
        if(data.province in validProvinces):
            print(data.timestamp)
            # Create new data to save with UTC time
            dataToSave = {"province": data.province, "timestamp": timestamp}
        else:
            raise ValueError("Invalid province")
    except:
        print(traceback.print_exc())
        # Raise exception for bad format
        raise HTTPException(status_code=400, detail="Invalid timestamp format")
    await manager.broadcast(f"{data.province}")
    return data_utils.createDataEntry(db=db, dataEntry=dataToSave)

# Web socket route for receiving province data real-time


@data.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    # Wait till websocket connection is accepted by client
    await manager.connect(websocket)
    # When connection gets accepted, keep on awaiting for receiving and sending data
    while True:
        try:
            data = await websocket.receive_text()
            await manager.broadcast(f" says: {data}")
        except WebSocketDisconnect:
            manager.disconnect(websocket)
            await manager.broadcast(f"Someone left")

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
