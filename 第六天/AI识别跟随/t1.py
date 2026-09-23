import cv2

# 视频流地址
camera_url = "http://192.168.1.1:8080/?action=stream"

# 创建视频捕捉对象
cap = cv2.VideoCapture(camera_url)
if not cap.isOpened():
    print("无法打开相机")
    exit()

while True:
    # 逐帧读取
    ret, frame = cap.read()

    if not ret:
        print("无法读取视频流")
        break

    # 显示视频帧
    cv2.imshow('Camera Feed', frame)

    # 按 'q' 键退出
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 释放捕捉对象和关闭窗口
cap.release()
cv2.destroyAllWindows()