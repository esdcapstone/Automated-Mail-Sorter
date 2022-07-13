FROM python:3
COPY . /usr/src/app
WORKDIR /usr/src/app
RUN pip install -r requirements.txt
CMD ["uvicorn", "main:app",  "--reload", "--host", "0.0.0.0", "--port", "8000" ]
