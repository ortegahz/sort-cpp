import torch
import glob
import os
import sys
import cv2

import numpy as np

sys.path.append('/media/manu/kingstop/workspace/demo')
from models.experimental import attempt_load
from utils.datasets import LoadStreams, LoadImages
from utils.general import (
    check_img_size, non_max_suppression, apply_classifier, scale_coords,
    xyxy2xywh, plot_one_box, strip_optimizer, set_logging)
from utils.torch_utils import select_device, load_classifier, time_synchronized


def letterbox(img, new_shape=(640, 640), color=(114, 114, 114), auto=True, scaleFill=False, scaleup=True):
    # Resize image to a 32-pixel-multiple rectangle https://github.com/ultralytics/yolov3/issues/232
    shape = img.shape[:2]  # current shape [height, width]
    if isinstance(new_shape, int):
        new_shape = (new_shape, new_shape)

    # Scale ratio (new / old)
    r = min(new_shape[0] / shape[0], new_shape[1] / shape[1])
    if not scaleup:  # only scale down, do not scale up (for better test mAP)
        r = min(r, 1.0)

    # Compute padding
    ratio = r, r  # width, height ratios
    new_unpad = int(round(shape[1] * r)), int(round(shape[0] * r))
    dw, dh = new_shape[1] - new_unpad[0], new_shape[0] - new_unpad[1]  # wh padding
    if auto:  # minimum rectangle
        dw, dh = np.mod(dw, 64), np.mod(dh, 64)  # wh padding
    elif scaleFill:  # stretch
        dw, dh = 0.0, 0.0
        new_unpad = (new_shape[1], new_shape[0])
        ratio = new_shape[1] / shape[1], new_shape[0] / shape[0]  # width, height ratios

    dw /= 2  # divide padding into 2 sides
    dh /= 2

    if shape[::-1] != new_unpad:  # resize
        img = cv2.resize(img, new_unpad, interpolation=cv2.INTER_LINEAR)
    top, bottom = int(round(dh - 0.1)), int(round(dh + 0.1))
    left, right = int(round(dw - 0.1)), int(round(dw + 0.1))
    img = cv2.copyMakeBorder(img, top, bottom, left, right, cv2.BORDER_CONSTANT, value=color)  # add border
    return img, ratio, (dw, dh)


dir_imgs = '/media/manu/kingstop/workspace/sort/mot_benchmark/train/ETH-Pedcross2/img1'
path_det_out = '/media/manu/kingstop/workspace/sort/data/train/ETH-Pedcross2/det/det.txt'
name_window = 'show detections'
len_imgs = 3716
b_show = False

weights = ['/home/manu/tmp/yolov5s_e300_ceil_relua_rfocus_synbn_weightse300/weights/best.pt', ]
device = torch.device('cuda:0')
conf_thres = 0.5
iou_thres = 0.5
classes = None
agnostic_nms = False
half = True
imgsz = 640

print('detect init start ...')
# Load model
model = attempt_load(weights, map_location=device)  # load FP32 model
imgsz = check_img_size(imgsz, s=model.stride.max())  # check img_size
if half:
    model.half()  # to FP16
print('detect init done')

if b_show:
    cv2.namedWindow(name_window, cv2.WINDOW_NORMAL)
    cv2.resizeWindow(name_window, 960, 540)

fh = open(path_det_out, mode='w')

for i in range(len_imgs):
    idx_f = i + 1
    path_img = os.path.join(dir_imgs, f'{idx_f:06d}.jpg')
    frame = cv2.imread(path_img)

    img = letterbox(frame, new_shape=imgsz)[0]
    img = img[:, :, ::-1].transpose(2, 0, 1)  # BGR to RGB, to 3x416x416
    img = np.ascontiguousarray(img)
    img = torch.from_numpy(img).to(device)
    img = img.half() if half else img.float()  # uint8 to fp16/32
    img /= 255.0  # 0 - 255 to 0.0 - 1.0
    if img.ndimension() == 3:
        img = img.unsqueeze(0)

    pred = model(img, augment=False)[0]
    pred = non_max_suppression(pred, conf_thres, iou_thres, classes=classes, agnostic=agnostic_nms)
    det = pred[0]

    if det is not None:
        det = det[det[:, -1] == 0]  # pick certain class
        det[:, :4] = scale_coords(img.shape[2:], det[:, :4], frame.shape).round()
        det = det[:, 0:5].detach().cpu().numpy()

        # plot
        for d in det:
            bbox = d[0:4]
            box = bbox.astype(int)
            if b_show:
                cv2.rectangle(frame, (box[0], box[1]), (box[2], box[3]), (0, 255, 0), 2)
            line = '%d,-1,%f,%f,%f,%f,%f,-1,-1,-1\n' % (idx_f, box[0], box[1], box[2]-box[0], box[3]-box[1], d[4])
            fh.write(line)
            print(line)

    if b_show:
        cv2.imshow(name_window, frame)
        if cv2.waitKey(40) & 0xFF == ord('q'):
            break

if b_show:
    cv2.destroyAllWindows()

fh.close()
