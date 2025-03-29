import sounddevice as sd
import json
import vosk
import serial
import re

# Arduino serial port (adjust as needed)
arduino_port = "/dev/ttyACM0"  # Use '/dev/ttyUSB0' on Linux
baud_rate = 9600

# Setup Vosk
model = vosk.Model("./vosk-model-small-en-us-0.15/")
recognizer = vosk.KaldiRecognizer(model, 16000)

# Connect to Arduino
arduino = serial.Serial(arduino_port, baud_rate)
print("Connected to Arduino")

hot_keywords = ["hot", "really hot", "steaming", "boiling", "warm", "toasty"]
cold_keywords = ["cold", "freezing", "chilly", "frosty", "ice", "cool"]

def send_to_arduino(command):
    print(f"Sending: {command}")
    arduino.write((command + "\n").encode())

def recognize_speech():
    with sd.RawInputStream(samplerate=16000, blocksize=8000, dtype='int16', channels=1) as stream:
        print("Listening for temperature feedback...")
        while True:
            data = stream.read(4000)
            if recognizer.AcceptWaveform(data[0]):
                result = json.loads(recognizer.Result())
                text = result.get("text", "").lower()
                print("Recognized:", text)

                if any(re.search(rf"\b{word}\b", text) for word in hot_keywords):
                    send_to_arduino("true")
                elif any(re.search(rf"\b{word}\b", text) for word in cold_keywords):
                    send_to_arduino("false")
                else:
                    print("No relevant temperature feedback detected.")

try:
    recognize_speech()
except KeyboardInterrupt:
    print("Exiting...")
finally:
    arduino.close()

