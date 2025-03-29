import paho.mqtt.client as mqtt
import serial
import time

#MQTT Settings 

MQTT_BROKER = "broker.emqx.io"
MQTT_PORT = 1883
MQTT_TOPIC = "makeohio/computervision"
MQTT_USERNAME = "funniestman7"
MQTT_PASSWORD = "makerohio7"

SERIAL_PORT = '/dev/ttyACM0'  # Change this to your serial port
SERIAL_BAUDRATE = 9600  # Change this to your baud rate

arduino = None

def setup_serial():
    global arduino
    try:
        arduino = serial.Serial(SERIAL_PORT, SERIAL_BAUDRATE, timeout=1)
        time.sleep(2)  # Wait for serial connection to establish
        print(f"Connected to Arduino on {SERIAL_PORT}")
    except Exception as e:
        print(f"Error connecting to Arduino: {e}")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(MQTT_TOPIC)
    else:
        print(f"Failed to connect, return code {rc}")

# Callback when a message is received from the MQTT broker
def on_message(client, userdata, msg):
    try:
        value = msg.payload.decode()
        print(f"Received MQTT message: {value}")
        
        # Send the value to Arduino via serial
        if arduino and arduino.is_open:
            arduino.write(f"{value}\n".encode())
            print(f"Sent to Arduino: {value}")
        else:
            print("Serial connection not available")
            
    except Exception as e:
        print(f"Error processing message: {e}")

def main():
    # Setup serial connection
    setup_serial()
    
    # Setup MQTT client
    client = mqtt.Client()
    #client.on_connect = on_connect
    client.on_message = on_message
    
    # Set username and password
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    
    # Connect to MQTT broker
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
    except Exception as e:
        print(f"Error connecting to MQTT broker: {e}")
        return
    
    # Start the loop
    client.loop_start()
    

if __name__ == "__main__":
    main()
