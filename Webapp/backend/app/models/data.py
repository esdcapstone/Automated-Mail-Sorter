from sqlalchemy.sql.sqltypes import Integer, String
from sqlalchemy import Table, Column
from app.config.db import meta, engine


dataEntries = Table('dataEntry', meta,
                    Column('id', Integer, primary_key=True),
                    Column('distance', String(255)),
                    Column('timestamp', String(255))
                    )

meta.create_all(engine)
