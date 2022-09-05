import cv2


def capture(picam2, filename: str):
    picam2.start_and_capture_file(filename, delay=0, show_preview=False)
    img = cv2.imread(filename)

    return img


def main():
    while True:
        input("Press any key to capture")
        img = capture("lol.jpg")


if __name__ == "__main__":
    main()
