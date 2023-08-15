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

if __name__ == "__main__":
    arduino = ArduinoComm("COM6")  # Replace "COM4" with your port

    # Sending a command to Arduino
    response = arduino.send(command="getTime", data=None)
    print(response)  # Should print the time (or an error if there's one)

    arduino.close()
