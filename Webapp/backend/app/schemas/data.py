from pydantic import BaseModel


class DataEntry(BaseModel):
    distance: str
    timestamp: str
