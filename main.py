import serial
import time
import json

class ArduinoComm:
    def __init__(self, port, baudrate=9600):
        self.ser = serial.Serial(port, baudrate)
        time.sleep(2)  # Give some time for the connection to be established

    def send(self, command, data=None):
        msg = {"command": command, "data": data}
        encoded_msg = json.dumps(msg).encode()
        self.ser.write(encoded_msg)
        return self.receive_response()

    def receive_response(self):
        raw_data = self.ser.readline().decode().strip()
        response = json.loads(raw_data)
        return response

    def close(self):
        self.ser.close()

from pymongo import MongoClient

class MongoDBManager:

    def __init__(self, host='DESKTOP-M8400H1', port=27017, db_name='pf'):
        # Initialize MongoDB client
        self.client = MongoClient(host, port)
        self.db = self.client[db_name]

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


if __name__ == "__main__":
    # Example usage
    db_manager = MongoDBManager()

    # Insert data
    db_manager.insert_one("users", {"name": "John Doe", "email": "john.doe@example.com"})

    # Query data
    results = db_manager.find("users", {"name": "John Doe"})
    for r in results:
        print(r)

    # Update data
    db_manager.update_one("users", {"name": "John Doe"}, {"email": "new.email@example.com"})

    # Close connection
    db_manager.close()


if __name__ == "__main__":
    dataMaker = ArduinoComm("COM5")
    pumpControl = ArduinoComm("COM6")

    #For now, just going to have

    # Sending a command to Arduino
    response = arduino.send(command="getAll", data=None)
    print(response)  # Should print the time (or an error if there's one)

    arduino.close()
