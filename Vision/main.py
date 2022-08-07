import cv2
import json
import requests
import serial
import torch

from capture import capture
from datetime import datetime, timezone
from picamera2 import Picamera2
from recognize_chars import recognize_characters
from segment import segment_img
from text_detect import text_detect


IMG_NAME = "temp.jpg"
picam2 = Picamera2()


def main():
    # Load config
    try:
        with open("config.json", "r") as conf_file:
            config = json.load(conf_file)
            conf_file.close()
    except FileNotFoundError:
        print("Error: config.json not found") 
        exit(1)

    # Load the pre-trained EAST text detector
    print("[INFO] Loading EAST text detector...")
    net = cv2.dnn.readNet(config["east"])

    # Load torch model
    print("[INFO] Loading torch model alphabets.pkl...")
    model = torch.load(config["torch_model"], map_location=config["device"])
    model.eval()

    # Load serial communication for STM32
    try:
        ser = serial.Serial(
                "/dev/serial0",
                baudrate=9600,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )

        ser.flush()
        print("# Serial communication established #")

    except serial.Serial.SerialException:
        print("Unable to initialize serial port")
        exit(1)

    print("\n# All models loaded #\n")

    try:
        while True:
            # Block here
            #input("Press any key to take picture")
            stm32_cmd = ser.readline()
            print(stm32_cmd)

            if stm32_cmd != b"START\n":
                ser.write(b"ACK:ERR\n")
                continue
            else:
                ser.write(b"ACK:OK\n")

            # Request received
            print("Received request for image capture")
            img = capture(picam2, IMG_NAME)

            if img is None:
                print("Failed to capture image")
                continue

            start_time = datetime.now() # record time
            potential_regions = text_detect(net, img)
            province_found = False

            if len(potential_regions) > 0:
                # Found some text regions

                # Iterate over each region and try to segment
                for idx, region in enumerate(potential_regions):
                    print(f"region {idx}")
                    im, boxes = segment_img(region)

                    text = ""  # record each character in this string
                    for box in boxes:
                        im_boxed = im[box[1]:box[1] + box[3], box[0]:box[0] + box[2]]
                        text += recognize_characters(model, im_boxed)
                    text = text.upper() # alpha-2 codes are all caps

                    if check_province(text):
                        # Found a province
                        province_found = True
                        break
            else:
                print("No potential regions found")

            if province_found:
                # STM32
                stm32_text = f"Province:{text}\n".upper()
                ser.write(bytes(stm32_text, "utf-8"))

                stm32_cmd = ser.readline()
                print(f"STM32: {stm32_cmd}")

                if stm32_cmd != b"ACK:OK\n":
                    print("Did not receive OK from STM, retrying...")
                    continue

                # Backend
                req_status = send_to_web(
                    url=config["backend_url"],
                    data=json.dumps({
                        "province": text,
                        "timestamp": datetime.now(tz=timezone.utc).isoformat()
                    }
                ))

                if req_status != 200:
                    print(f"Got status {req_status} from server")
                else:
                    print("Posted result to backend server")
            else:
                # STM32
                stm32_text = "PROVINCE:UN\n"
                ser.write(bytes(stm32_text, "utf-8"))

                stm32_cmd = ser.readline()

                if stm32_cmd == b"ACK:UN\n":
                    # STM detected too many requests for same letter
                    # Classify this letter as unsorted
                    req_status = send_to_web(
                        url=config["backend_url"],
                        data=json.dumps({
                            "province": "UN",
                            "timestamp": datetime.now(tz=timezone.utc).isoformat()
                        }
                    ))
                    print("Letter classified as UNSORTED")
                elif stm32_cmd == b"ACK:OK\n":
                    # STM readjusted the letter, try again
                    print("Retrying in next iteration...")
                else:
                    print(f"Received unknown message from STM32: {stm32_cmd}")

            # Display timing info
            print(f"\n# Current iteration took : {(datetime.now() - start_time).total_seconds()}s #\n") # display time taken

    except KeyboardInterrupt:
        print("\nExiting...\n")
        

def send_to_web(url: str, data):
    print("here")
    try:
        r = requests.post(
            url,
            headers={
                "Content-Type": "application/json"
            },
            data=data
        )
    except Exception:
        print("Some error occured while sending request to backend")
        return 500

    return r.status_code


def check_province(alpha_2_code):
    all_provinces = [
        "AB", # Alberta
        "BC", # British Columbia
        "MB", # Manitoba
        "NB", # New Brunswick
        "NL", # Newfoundland and Labrador
        "NS", # Nova Scotia
        "ON", # Ontario
        "PE", # Prince Edward Island
        "QC", # Quebec
        "SK", # Saskatchewan
    ]

    return alpha_2_code in all_provinces


if __name__ == "__main__":
    main()
