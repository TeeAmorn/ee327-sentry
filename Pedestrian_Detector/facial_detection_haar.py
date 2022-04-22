# Source: https://towardsdatascience.com/face-detection-in-2-minutes-using-opencv-python-90f89d7c0f81
# OpenCV Classifier source: https://github.com/opencv/opencv/tree/master/data/haarcascades
import cv2

# Multiple cascade classifiers can apparently be used at once, on the opencv site they combine a face and eye classifier
# Load the cascade
face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
# Upper body haarcascade (very finnicky, also named face cascade so I don't have to rename other vars)
#face_cascade = cv2.CascadeClassifier('haarcascade_upperbody.xml') 

# To capture video from webcam. 
cap = cv2.VideoCapture(0)
# To use a video file as input 
# cap = cv2.VideoCapture('filename.mp4')

while True:
    # Read the frame
    _, img = cap.read()
    # Convert to grayscale, this Cascade only works when image is in grayscale for some reason
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Detect the faces
    faces = face_cascade.detectMultiScale(gray, 1.1, 4)
    # Draw the rectangle around each face
    for (x, y, w, h) in faces:
        cv2.rectangle(img, (x, y), (x+w, y+h), (255, 0, 0), 2)
    # Display
    cv2.imshow('img', img)
    # Stop if escape key is pressed
    k = cv2.waitKey(30) & 0xff
    if k==27:
        break
# Release the VideoCapture object
cap.release()