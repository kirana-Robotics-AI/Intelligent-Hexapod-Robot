import cv2
import numpy as np
import onnxruntime as ort
import serial

YOLO = "models/yolov5s.onnx"
NAMES = "models/coco.names"
IP = "http://YOUR_IP:8080/video"

ser = serial.Serial("COM3",115200)

classes = open(NAMES).read().splitlines()
session = ort.InferenceSession(YOLO)
input_name = session.get_inputs()[0].name

cap = cv2.VideoCapture(IP)

while True:
    ret, frame = cap.read()
    if not ret:
        continue

    img = cv2.resize(frame,(640,640))
    img = cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
    img = img.astype(np.float32)/255.0
    img = np.transpose(img,(2,0,1))[None]

    pred = session.run(None,{input_name:img})[0]

    persons = 0

    for det in pred[0]:
        if det[4] < 0.4:
            continue

        cls = np.argmax(det[5:])
        label = classes[cls]

        if label=="person":
            persons += 1

        cv2.putText(frame,label,(50,50),
                    cv2.FONT_HERSHEY_SIMPLEX,1,(0,255,0),2)

    # ===== DECISION =====
    if persons>0:
        ser.write(b'F\n')   # move forward
    else:
        ser.write(b'S\n')   # stop

    cv2.imshow("Robot",frame)

    if cv2.waitKey(1)==ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
