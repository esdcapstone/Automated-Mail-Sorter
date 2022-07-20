import app.models.user_model as user_model
import app.schemas.user_schema as user_schema
from sqlalchemy.orm import Session
from passlib.context import CryptContext
# Get user by id


def get_user_by_id(db: Session, user_id: int):
    return db.query(user_model.User).filter(user_model.User.id == user_id).first()


# Get user by email
def get_user_by_email(db: Session, email: str):
    return db.query(user_model.User).filter(user_model.User.email == email).first()

# Get all users after offset and upto a limit


def get_users(db: Session, skip: int = 0, limit: int = 100):
    return db.query(user_model.User).offset(skip).limit(limit).all()


# Encrypt the password using bcrypt algorithm
pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

# Verify if password is correct


def verify_password(plain_password, hashed_password):
    return pwd_context.verify(plain_password, hashed_password)


def get_password_hash(plain_password):
    return pwd_context.hash(plain_password)


# Create user
def create_user(db: Session, user: user_schema.UserCreate):
    hashed_password = get_password_hash(user.password)
    print(hashed_password)
    db_user = user_model.User(
        email=user.email, hashed_password=hashed_password)
    db.add(db_user)
    db.commit()
    db.refresh(db_user)
    return db_user
