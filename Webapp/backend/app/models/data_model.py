from email.headerregistry import ParameterizedMIMEHeader
from sqlalchemy.sql.sqltypes import Integer, String
from sqlalchemy import Table, Column, TIMESTAMP
from app.config.db import Base
from datetime import datetime
from app.config.db import Base

MAX_PROVINCE_LENGTH = 2


class DataEntry(Base):
    __tablename__ = 'data'
    id = Column(Integer, primary_key=True, index=True)
    province = Column(String(MAX_PROVINCE_LENGTH), nullable=False)
    timestamp = Column(TIMESTAMP, default=datetime.utcnow, nullable=False)
