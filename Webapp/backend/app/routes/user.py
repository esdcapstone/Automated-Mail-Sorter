from fastapi import APIRouter
from app.models.user import users
from app.config.db import connection
from app.schemas.user import User
user = APIRouter()

@user.get('/')
async def fetchUsers():
    return connection.execute(users.select()).fetchall()

@user.get('/{id}')
async def fetchSingleUsers(id: int):
    return connection.execute(users.select().where(users.c.id == id)).first()

@user.post('/')
async def createUsers(user: User):
    connection.execute(users.insert().values(
        name = user.name,
        email = user.email,
        password = user.password
    ))
    return connection.execute(users.select()).fetchall()

@user.put('/{id}')
async def updateUser(id: int, user: User):
    connection.execute(users.update().values(
        name = user.name,
        email = user.email,
        password = user.password
    ).where(users.c.id == id))
    return  connection.execute(users.select()).fetchall()

   
@user.delete('/{id}')
async def deleteUser(id: int):
    connection.execute(users.delete().where(users.c.id == id))
    return connection.execute(users.select()).fetchall()