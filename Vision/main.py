import cv2
import json
import torch

from datetime import datetime
from segment import segment_img
from text_detect import text_detect
from recognize_chars import recognize_characters


cam = cv2.VideoCapture(0)
cam.set(cv2.CAP_PROP_FRAME_WIDTH, 2560)     # img width
cam.set(cv2.CAP_PROP_FRAME_HEIGHT, 1440)    # img height


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

    print("\n# All models loaded #\n")

    try:
        while True:
            # Block here
            input("Press any key to take picture")

            # Request received
            print("Received request for image capture")
            cv2_ret, img = cam.read()
            #img = cv2.imread("full.jpg")

            if not cv2_ret:
                # Could not read image, send this to controller
                print("Failed to read")
                continue

            start_time = datetime.now() # record time
            potential_regions = text_detect(net, img)
            province_found = False

            if len(potential_regions) == 0:
                print("No potential regions found")
                continue

            for idx, region in enumerate(potential_regions):
                print(f"region {idx}")
                im, boxes = segment_img(region)

                text = ""  # record each character in this string
                for box in boxes:
                    im_boxed = im[box[1]:box[1] + box[3], box[0]:box[0] + box[2]]
                    text += recognize_characters(model, im_boxed)
                text = text.upper() # alpha-2 codes are all caps

                if check_province(text):
                    # Found
                    # TODO: send message to controller and backend
                    print(f"\nProvince: {text}\n")
                    province_found = True

            if not province_found:
                print("No text region with a province found")

            # Reset params and display info
            province_found = False
            print(f"\n# All operations took: {(datetime.now() - start_time).total_seconds()}s #\n") # display time taken

    except KeyboardInterrupt:
        print("\nExiting...\n")
        cam.release()
        

def send_to_controller(msg):
    print(msg)


def send_to_web(msg):
    print(msg)


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