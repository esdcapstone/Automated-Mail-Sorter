from sqlalchemy import create_engine, MetaData

engine = create_engine(
    'mysql+pymysql://sqluser:password@backend_db_1:3306/data')

meta = MetaData()
connection = engine.connect()
