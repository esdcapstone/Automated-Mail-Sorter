from pydantic import BaseModel


class UserBase(BaseModel):
    email: str

# For creation


class UserCreate(UserBase):
    password: str

# For returning/reading


class User(UserBase):
    id: int
    is_active: bool

    class Config:
        orm_mode = True
