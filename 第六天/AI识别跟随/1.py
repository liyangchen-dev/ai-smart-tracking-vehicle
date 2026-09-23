import torch
from ultralytics import YOLO
import cv2
import cvzone
import math
# from deep_sort_realtime.deepsort_tracker import DeepSort
import os
import numpy as np
from sort import Sort
import pandas as pd
from utils import get_angle, get_control_byte
import socket
from queue import Queue
import threading
import traceback
import datetime
import copy
import time
import argparse
import configparser
import sys

os.environ['KMP_DUPLICATE_LIB_OK'] = 'TRUE'  # 允许重复导入 OpenCV 库的问题

ThreadingList = []
signal_queue = Queue()
frame_queue = Queue()
filter_dict = {
    1: [1, 1, 2, 3, 4, 5],
    2: [2, 2, 4, 6, 8, 10],
    3: [3, 3, 6, 9, 12, 15],
    5: [5, 5, 10, 15, 20, 25]
}


def receive_frame(cap=None):
    i = 0
    while True:
        success, img = cap.read()
        if success:
            i += 1
            if i % 2 != 0:
                continue
            frame_queue.put(img)


def log_direction(log_path, str1):
    with open(log_path, "a") as f:
        f.write(str1 + '\n')


def convert_command(client_socket=None, log_path=None):
    while True:
        try:
            if signal_queue.empty():
                time.sleep(0.001)
                continue
            else:
                signal = signal_queue.get(block=False)
                print('signal: ', signal)
                try:
                    if signal[0] in ['左转', '右转']:
                        data_bytes = get_control_byte(signal=signal[0])
                        # print(data_bytes, type(data_bytes), data_bytes.hex(), type(data_bytes.hex()))
                        client_socket.sendall(data_bytes)
                        str1 = '{}  ----------{}----------'.format(str(datetime.datetime.now()), signal[0])
                        log_direction(log_path, str1)
                        time.sleep(0.2)
                        signal_2 = '停止'
                        data_bytes = get_control_byte(signal=signal_2)
                        # print(data_bytes, type(data_bytes), data_bytes.hex(), type(data_bytes.hex()))
                        client_socket.sendall(data_bytes)
                        str2 = '{}  ----------{}----------'.format(str(datetime.datetime.now()), signal_2)
                        log_direction(log_path, str2)
                    if signal[0] in ['停止']:
                        data_bytes = get_control_byte(signal=signal[0])
                        # print(data_bytes, type(data_bytes), data_bytes.hex(), type(data_bytes.hex()))
                        client_socket.sendall(data_bytes)
                        str2 = '{}  ----------{}----------'.format(str(datetime.datetime.now()), signal[0])
                        log_direction(log_path, str2)
                    if signal[0] in ['前进', '后退']:
                        if len(list(set(signal))) == 1:
                            data_bytes = get_control_byte(signal=signal[0])
                            # print(data_bytes, type(data_bytes), data_bytes.hex(), type(data_bytes.hex()))
                            client_socket.sendall(data_bytes)
                            str2 = '{}  ----------{}----------'.format(str(datetime.datetime.now()), signal[0])
                            log_direction(log_path, str2)
                        else:
                            data_bytes = get_control_byte(signal=signal[0])
                            # print(data_bytes, type(data_bytes), data_bytes.hex(), type(data_bytes.hex()))
                            client_socket.sendall(data_bytes)
                            str2 = '{}  ----------{}----------'.format(str(datetime.datetime.now()), signal[0])
                            log_direction(log_path, str2)
                            time.sleep(0.05)
                            # time.sleep(0.15)
                            signal_2 = '停止'
                            data_bytes = get_control_byte(signal=signal_2)
                            # print(data_bytes, type(data_bytes), data_bytes.hex(), type(data_bytes.hex()))
                            client_socket.sendall(data_bytes)
                            str2 = '{}  ----------{}----------'.format(str(datetime.datetime.now()), signal_2)
                            log_direction(log_path, str2)
                except:
                    print(traceback.format_exc())
        except:
            print(traceback.format_exc())


def main_fun(model_params=None):
    filter_nums = int(model_params.get('filter_nums'))
    params = filter_dict[filter_nums]
    # 640, 480   960, 540
    width, height = int(model_params.get('width')), int(model_params.get('height'))
    s1 = round(float(model_params.get('s1')), 4)
    s2 = round(float(model_params.get('s2')), 4)
    d1 = round(float(model_params.get('d1')), 4)

    i = 0
    status_list = []
    queue_list = []
    tracker_id = None
    model = model_params.get('model')
    tracker = model_params.get('tracker')

    cap = cv2.VideoCapture(0)



    while True:
        # if frame_queue.empty():
        #     time.sleep(0.001)
        #     continue
        if (1<0):
            pass
        else:
            success, frame = cap.read()
            # img = cv2.imread('images/2.jpg')
            img = frame

            i += 1
            img = cv2.resize(img, (width, height))
            with torch.no_grad():
                results = model(img, stream=True)
            detections = np.empty((0, 6))
            for r in results:
                boxes = r.boxes
                print('len(boxes): ', len(boxes))
                for box in boxes:
                    x1, y1, x2, y2 = box.xyxy[0]
                    x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                    conf = math.ceil((box.conf[0] * 100)) / 100
                    cls = int(box.cls[0])
                    currentClass = model.names[cls]
                    if currentClass in ["person"]:
                        currentArray = np.array([x1, y1, x2, y2, conf, cls])
                        detections = np.vstack((detections, currentArray))
                classes_array = detections[:, -1:]
                resultsTracker = tracker.update(detections)
                # resultsTracker = detections
                try:
                    resultsTracker = np.hstack((resultsTracker, classes_array))
                except ValueError:
                    classes_array = classes_array[:resultsTracker.shape[0], :]
                    resultsTracker = np.hstack((resultsTracker, classes_array))
                # resultsTracker = detections
                detail_list = []
                if len(resultsTracker) == 0:
                    continue
                for result in resultsTracker:
                    x1, y1, x2, y2, id, cls = result
                    x1, y1, x2, y2, id, cls = int(x1), int(y1), int(x2), int(y2), int(id), int(cls)
                    w, h = x2 - x1, y2 - y1
                    detail = {
                        'x1': x1,
                        'y1': y1,
                        'x2': x2,
                        'y2': y2,
                        'id': id,
                        'cls': cls,
                        'w': w,
                        'h': h,
                        's': w * h,
                        'cx': x1 + w // 2,
                        'cy': y1 + h // 2
                    }
                    detail_list.append(detail)

                df = pd.DataFrame(detail_list)
                if tracker_id is None:
                    max_s_index = int(df.sort_values(by='s', ascending=False).index[0])
                    tracker_index = max_s_index
                    tracker_id = df[df.index == tracker_index]['id'].item()
                else:
                    id_flag = df['id'].isin([tracker_id]).any()
                    if id_flag:
                        tracker_id_index = df[df['id'] == tracker_id].index[0]
                        tracker_index = tracker_id_index
                    else:
                        max_s_index = int(df.sort_values(by='s', ascending=False).index[0])
                        tracker_index = max_s_index
                        tracker_id = df[df.index == tracker_index]['id'].item()
                x1 = detail_list[tracker_index]['x1']
                y1 = detail_list[tracker_index]['y1']
                w = detail_list[tracker_index]['w']
                h = detail_list[tracker_index]['h']
                cx = detail_list[tracker_index]['cx']
                s = detail_list[tracker_index]['s']

                list_sum = 0
                print('status_list: ', status_list)
                if len(status_list) == params[0]:
                    list_sum = sum(status_list)
                    status_list = []
                print('s: ', s)
                if cx > (width / 2) + width * d1:
                    status_list.append(3)
                    if list_sum == params[3]:
                        queue_list.append('右转')
                elif cx < (width / 2) - width * d1:
                    status_list.append(4)
                    if list_sum == params[4]:
                        queue_list.append('左转')
                else:
                    if s > width * height * s2:  # 180000 122800   0.4
                        status_list.append(1)
                        if list_sum == params[1]:
                            queue_list.append('后退')
                    elif s < width * height * s1:  # 130000 76800
                        status_list.append(2)
                        if list_sum == params[2]:
                            queue_list.append('前进')
                    else:
                        status_list.append(5)
                        if list_sum == params[5]:
                            queue_list.append('停止')
                print('queue_list: ', queue_list)
                if len(queue_list) == 2:
                    temp_list = copy.deepcopy(queue_list)
                    signal_queue.put(temp_list)
                    queue_list.pop(0)

                cvzone.cornerRect(img, (x1, y1, w, h), l=12, rt=4, colorR=(255, 0, 255))
            cv2.imshow('car following', img)
            cv2.waitKey(1)


def run():
    cf = configparser.ConfigParser()
    cf.read("./config.ini", encoding='utf-8')
    weights_path = cf.get("YOLOWeights", "weights_path")
    log_path = cf.get("LogConfig", "log_path")
    filter_nums = cf.get("ModelConfig", "filter_nums")
    width = cf.get("ModelConfig", "width")
    height = cf.get("ModelConfig", "height")
    s1 = cf.get("ModelConfig", "s1")
    s2 = cf.get("ModelConfig", "s2")
    d1 = cf.get("ModelConfig", "d1")

    # client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # server_address = (tcp_server, tcp_port)
    # client_socket.connect(server_address)

    # cap = cv2.VideoCapture(camera_addr)

    model = YOLO(weights_path)
    tracker = Sort(max_age=20, min_hits=3, iou_threshold=0.3)
    model_params = {
        'model': model,
        'tracker': tracker,
        'filter_nums': filter_nums,
        'width': width,
        'height': height,
        's1': s1,
        's2': s2,
        'd1': d1
    }

    main_fun(model_params)


if __name__ == '__main__':
    run()
