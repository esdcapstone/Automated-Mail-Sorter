from pydantic import BaseModel


class DataEntry(BaseModel):
    province: str
    timestamp: str
