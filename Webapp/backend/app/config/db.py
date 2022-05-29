from sqlalchemy import create_engine, MetaData

engine = create_engine('mysql+pymysql://root:example@localhost:3306/users')

meta = MetaData()
connection = engine.connect()
