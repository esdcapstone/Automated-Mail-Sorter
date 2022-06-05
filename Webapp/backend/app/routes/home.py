from fastapi import APIRouter, status, Request


home = APIRouter(
    prefix = '/home',
    tags = ['home']
)

@home.get('/', status_code = status.HTTP_200_OK)
async def getHomepage(request: Request):
    return 

