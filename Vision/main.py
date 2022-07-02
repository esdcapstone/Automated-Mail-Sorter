import cv2
import torch
import torchvision.transforms.functional as F

from segment import segment_img
from text_detect import text_detect


cam = cv2.VideoCapture(0)
cam.set(cv2.CAP_PROP_FRAME_WIDTH, 2560) # img width
cam.set(cv2.CAP_PROP_FRAME_HEIGHT, 1440)    # img height

# Transfer model onto cpu
device = torch.device("cpu")
model = torch.load("pretrained/alphabets.pkl", map_location=device)


def main():
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

            potential_regions = text_detect(img)

            for idx, region in enumerate(potential_regions):
                print(f"region {idx}")
                im, boxes = segment_img(region)

                for box in boxes:
                    im_boxed = im[box[1]:box[1] + box[3], box[0]:box[0] + box[2]]
                    recognize_characters(im_boxed)

                print()
    except KeyboardInterrupt:
        print("Exiting...")
        cam.release()
        

def send_to_controller(msg):
    print(msg)


def send_to_web(msg):
    print(msg)


def recognize_characters(img):
    img = torch.from_numpy(img).permute(2, 0, 1)

    # Tranforms
    resized_im = F.pad(img, padding=5, padding_mode="edge")
    resized_im = F.resize(resized_im, (28, 28))
    resized_im = F.rgb_to_grayscale(resized_im)
    resized_im = resized_im / 255.0
    resized_im = resized_im.unsqueeze(dim=0)

    # Infer
    with torch.no_grad():
        model.eval()
        res = model(resized_im)
        print(f"character detected: {chr(ord('a') + res.argmax(axis=1))}")


if __name__ == "__main__":
    main()