import app.models.data_model as data_model
import app.schemas.data_schema as data_schema
from sqlalchemy.orm import Session


# Get all data points after offset and upto a limit
def get_data(db: Session, skip: int = 0, limit: int = 100):
    return db.query(data_model.DataEntry).offset(skip).limit(limit).all()

# Get all data points by id


def get_data_by_id(db: Session, data_id: int):
    return db.query(data_model.DataEntry).filter(data_model.DataEntry.id == data_id).first()

# Create new data entry


def createDataEntry(db: Session, dataEntry: data_schema.DataEntry):
    data = data_model.DataEntry(
        timestamp=dataEntry["timestamp"],
        province=dataEntry["province"]
    )
    db.add(data)
    db.commit()
    db.refresh(data)
    return data
