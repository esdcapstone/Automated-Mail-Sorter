from fastapi import FastAPI
from app.routes.user import user
import uvicorn

app = FastAPI()

app.include_router(user)
# @app.get("/")
# async def root():
#     return {"message": "Hello World"}

# # Just for testing purpose
# # if __name__ == '__main__':
# 	uvicorn.run("main:app", host= '0.0.0.0', port= 8000, reload = True)	
