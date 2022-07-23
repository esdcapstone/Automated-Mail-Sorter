import cv2
import numpy as np
import functools


def segment_img(image):
    # Read the image and convert to grayscale
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    
    # Apply Gaussian blurring and thresholding 
    # to reveal the characters on the license plate
    blurred = cv2.GaussianBlur(gray, (3, 3), 0)
    thresh = cv2.adaptiveThreshold(blurred, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY_INV, 45, 15)
    
    # Perform connected components analysis on the thresholded image and
    # initialize the mask to hold only the components we are interested in
    _, labels = cv2.connectedComponents(thresh)
    mask = np.zeros(thresh.shape, dtype="uint8")
    
    # Set lower bound and upper bound criteria for characters
    total_pixels = image.shape[0] * image.shape[1]
    lower = total_pixels // 80 # heuristic param, can be fine tuned if necessary
    upper = total_pixels // 5 # heuristic param, can be fine tuned if necessary
    
    # Loop over the unique components
    for (i, label) in enumerate(np.unique(labels)):
        # If this is the background label, ignore it
        if label == 0:
            continue
        
        # Otherwise, construct the label mask to display only connected component
        # for the current label
        labelMask = np.zeros(thresh.shape, dtype="uint8")
        labelMask[labels == label] = 255
        numPixels = cv2.countNonZero(labelMask)
    
        # If the number of pixels in the component is between lower bound and upper bound, 
        # add it to our mask
        if numPixels > lower and numPixels < upper:
            mask = cv2.add(mask, labelMask)
    
    # Find contours and get bounding box for each contour
    cnts, _ = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    boundingBoxes = [cv2.boundingRect(c) for c in cnts]
    
    # Sort the bounding boxes from left to right, top to bottom
    # sort by Y first, and then sort by X if Ys are similar
    def compare(rect1, rect2):
        if abs(rect1[1] - rect2[1]) > 10:
            return rect1[1] - rect2[1]
        else:
            return rect1[0] - rect2[0]
    
    boundingBoxes = sorted(boundingBoxes, key=functools.cmp_to_key(compare))
    
    boxes = []

    for box in boundingBoxes:
        boxes.append(box)

    return image, sorted(boxes)


def main():
    img = cv2.imread("detected_0.jpg")

    _, boxes = segment_img(img)

    for idx, box in enumerate(boxes):
        im_boxed = img[box[1]:box[1] + box[3], box[0]:box[0] + box[2]]
        cv2.imwrite(f"boxed_{idx}.jpg", im_boxed)


if __name__ == "__main__":
    main()