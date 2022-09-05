# Vision
Code for handling detection and character recognition on Raspberry Pi for Mail Sorting.

# Requirements
- PyTorch >= 1.11.0
- OpenCV >= 4.5.5

# How?
1. Capture image.
2. Detect text regions.
3. Iterate over detected text regions and segment each region.
4. Iterate over segmented images and detect characters.

# Credits
- PyImageSearch *[OpenCV EAST Text Detection](https://pyimagesearch.com/2018/08/20/opencv-text-detection-east-text-detector/)*
- Kaggle *[A-Z Handwritten Dataset](https://www.kaggle.com/datasets/sachinpatel21/az-handwritten-alphabets-in-csv-format)*
- minhthangdang *[CharactersSegmentationRecognition](https://github.com/minhthangdang/CharactersSegmentationRecognition)*