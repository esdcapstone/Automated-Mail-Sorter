import cv2
import matplotlib.pyplot as plt
import torch
import torchvision.transforms.functional as F

from torchvision.transforms import InterpolationMode


def recognize_characters(model, img, debug=False):
    img = torch.from_numpy(img).permute(2, 0, 1)

    # Tranforms
    resized_im = F.resize(img, (28, 28), interpolation=InterpolationMode.BILINEAR)
    resized_im = F.rgb_to_grayscale(resized_im)
    resized_im = F.autocontrast(resized_im)
    resized_im = F.invert(resized_im)
    resized_im = resized_im.float() / 255.0
    resized_im = resized_im.unsqueeze(dim=0)

    if debug:
        cv2.imshow("transformed char", resized_im.detach().numpy()[0][0])
        cv2.waitKey(0)
        plt.imshow(resized_im[0][0].detach().numpy())
        plt.show()

    # Infer
    with torch.no_grad():
        res = model(resized_im)
        print(f"character detected: {chr(ord('a') + res.argmax(axis=1))}")

        return chr(ord('a') + res.argmax(axis=1))


def main():
    device = torch.device("cpu")
    torch_model = "pretrained/alphabets.pkl"

    model = torch.load(torch_model, map_location=device)
    model.eval()

    img = cv2.imread("boxed_0.jpg")
    recognize_characters(model, img)

    img = cv2.imread("boxed_1.jpg")
    recognize_characters(model, img)


if __name__ == "__main__":
    main()
