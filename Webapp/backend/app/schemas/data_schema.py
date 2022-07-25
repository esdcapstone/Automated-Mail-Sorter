from pydantic import BaseModel
import datetime


class DataEntry(BaseModel):
    province: str
    timestamp: datetime.datetime

    class Config:
        orm_mode = True
