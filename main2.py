import matplotlib.pyplot as plt
from nicegui import ui
import numpy as np
import plotly.graph_objects as go
from pymongo import MongoClient
import time


class MongoDBManager:

    def __init__(self, host='192.168.50.41', port=27017, db_name='pf'):
        # Initialize MongoDB client
        self.client = MongoClient(host, port)
        self.db = self.client[db_name]
        self.dataColl = self.db["dataMaker"]

    def insert_one(self, collection_name, data):
        """Insert one record into a MongoDB collection."""
        collection = self.db[collection_name]
        collection.insert_one(data)

    def insert_many(self, collection_name, data_list):
        """Insert multiple records into a MongoDB collection."""
        collection = self.db[collection_name]
        collection.insert_many(data_list)

    def find(self, collection_name, query=None, projection=None):
        """Query data from a MongoDB collection."""
        collection = self.db[collection_name]
        return collection.find(query, projection)

    def update_one(self, collection_name, filter_query, update_data):
        """Update one record in a MongoDB collection."""
        collection = self.db[collection_name]
        collection.update_one(filter_query, {'$set': update_data})

    def delete_one(self, collection_name, query):
        """Delete one record from a MongoDB collection."""
        collection = self.db[collection_name]
        collection.delete_one(query)

    def close(self):
        """Close MongoDB connection."""
        self.client.close()

    def updateData(self, points):
        cursor = self.dataColl.find().sort("TS", -1).limit(points)
        data = [doc for doc in cursor]
        # Extracting the data for plotting
        self.TS = np.flip(np.array([entry['TS'] for entry in data]))
        self.TS_date = []
        for item in self.TS:
            self.TS_date.append(time.strftime('%Y-%m-%d %H:%M:%s', time.localtime(item)))

        self.temp1 = np.flip(np.array([entry['therm1'] for entry in data]))
        self.temp2 = np.flip(np.array([entry['therm2'] for entry in data]))

        self.soil1a = np.flip(np.array([entry['soil1a'] for entry in data]))
        self.soil1b = np.flip(np.array([entry['soil1b'] for entry in data]))

        self.soil2a = np.flip(np.array([entry['soil2a'] for entry in data]))
        self.soil2b = np.flip(np.array([entry['soil2b'] for entry in data]))

dbSesh = MongoDBManager()
npts = 1
def update_plots() -> None:
    dbSesh.updateData(npts)
    tplot.push(dbSesh.TS, [dbSesh.temp1, dbSesh.temp2])
    s1plot.push(dbSesh.TS, [dbSesh.soil1a, dbSesh.soil1b])
    s2plot.push(dbSesh.TS, [dbSesh.soil2a, dbSesh.soil2b])

tplot = ui.line_plot(n=2, limit=1000, figsize=(10, 5), update_every=5) \
    .with_legend(['T1', 'T2'], loc='center left', ncol=1)
s1plot = ui.line_plot(n=2, limit=1000, figsize=(10,5), update_every=5) \
    .with_legend(['S1a', 'S1b'], loc='center left', ncol=1)
s2plot = ui.line_plot(n=2, limit=1000, figsize=(10,5), update_every=5) \
    .with_legend(['S1a', 'S1b'], loc='center left', ncol=1)

line_updates = ui.timer(0.1, update_plots, active=True)

ui.run()
