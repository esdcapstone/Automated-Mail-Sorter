from fastapi import FastAPI, WebSocket
from app.routes.users import users
from app.routes.data import data
import uvicorn

app = FastAPI()

app.include_router(users)
app.include_router(data)


# @app.get("/")
# async def root():
#     return {"message": "Hello World"}

# # Just for testing purpose
# # if __name__ == '__main__':
# 	uvicorn.run("main:app", host= '0.0.0.0', port= 8000, reload = True)
