from enum import unique
from operator import index
from matplotlib.pyplot import prism
from sqlalchemy import Column, Boolean, Integer, String
from app.config.db import Base
MAX_EMAIL_LENGTH = 50
MAX_HASHED_PASSWORD_LENGTH = 250


class User(Base):
    __tablename__ = "users"
    id = Column(Integer, primary_key=True, index=True)
    email = Column(String(MAX_EMAIL_LENGTH), unique=True, index=True)
    hashed_password = Column(String(MAX_HASHED_PASSWORD_LENGTH))
    is_active = Column(Boolean, default=True)


# users = Table('users', meta,
#               Column('id', Integer, primary_key=True),
#               Column('name', String(255)),
#               Column('email', String(255)),
#               Column('password', String(255))
#               )

# meta.create_all(engine)
