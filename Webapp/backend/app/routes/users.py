
from typing import List
from fastapi import APIRouter, Depends, HTTPException
import app.models.user_model as user_model
import app.schemas.user_schema as user_schema
from sqlalchemy.orm import Session
from app.config.db import SessionLocal, engine
import app.utils.user_utils as user_utils

user_model.Base.metadata.create_all(bind=engine)

users = APIRouter(prefix="/users", tags=["users"])

# Dependency


def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


@users.get('/', response_model=List[user_schema.User])
async def fetchUsers(skip: int = 0, limit: int = 100, db: Session = Depends(get_db)):
    all_users = user_utils.get_users(db, skip=skip, limit=limit)
    return all_users


@users.get('/{id}', response_model=user_schema.User)
async def fetchSingleUsers(id: int, db: Session = Depends(get_db)):
    db_user = user_utils.get_user_by_id(db, user_id=id)
    if db_user is None:
        raise HTTPException(status_code=404, detail="User not found")
    return db_user


@users.post('/', response_model=user_schema.User)
async def createUsers(user: user_schema.UserCreate, db: Session = Depends(get_db)):
    db_user = user_utils.get_user_by_email(db, email=user.email)
    if db_user:
        raise HTTPException(status_code=400, detail="Email already registered")
    return user_utils.create_user(db=db, user=user)


# @users.put('/{id}')
# async def updateUser(id: int, user: User):
#     connection.execute(users.update().values(
#         name=user.name,
#         email=user.email,
#         password=user.password
#     ).where(users.c.id == id))
#     return connection.execute(users.select()).fetchall()

# @users.delete('/{id}', response_model=user_schema.User)
# async def deleteUser(id: int, db: Session = Depends(get_db)):

#     connection.execute(users.delete().where(users.c.id == id))
#     return connection.execute(users.select()).fetchall()
