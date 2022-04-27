# Haar source: https://towardsdatascience.com/face-detection-in-2-minutes-using-opencv-python-90f89d7c0f81 
# KCF source: https://learnopencv.com/object-tracking-using-opencv-cpp-python/ 

import cv2
import sys
import numpy as np

# Multiple cascade classifiers can be used at once 
# On the opencv site they combine a face and eye classifier
# Additional classifiers can also be found on OpenCV's repo: https://github.com/opencv/opencv/tree/master/data/haarcascades
# Load the cascade
face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

# Create KCF Tracker
tracker = cv2.TrackerKCF_create()

# Read video
video = cv2.VideoCapture(0) # 0 means stream from webcam

# Exit if video not opened.
if not video.isOpened():
    print("Could not open video")
    sys.exit()

# Read first frame.
ok, frame = video.read()
while not ok:
    # If no frame captured continue to loop until we capture a frame
    ok, frame = video.read()
    print('Frame not captured!')
    k = cv2.waitKey(1) & 0xff
    if k == 27 : # If the ESC key is pressed while still trying to read frame, just crash
        sys.exit()

# Instead of manually selecting a ROI, use a Haar Cascade to define the ROI
# Convert to grayscale, this Cascade only works when image is in grayscale (Haar calculates pixel using binary values)
gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
# Detect the faces
# TODO: Combine multiple cascade (side face profile)
faces = face_cascade.detectMultiScale(gray, 1.1, 4)
# Faces are captured and stored as an array within array (IE a 2D array is initialized)
# First dimension determines which face to choose, second dimension provides dimensions of face

while len(faces) == 0: 
    # If no faces are detected, continue to process frames until a face is found
    ok, frame = video.read()
    while not ok:
        ok, frame = video.read()
        print('Frame not captured!')
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.1, 4)

bbox = faces[0] # Define ROI as the first detected face

# Initialize tracker with first frame and bounding box
ok = tracker.init(frame, bbox)

# frame_counter = 0 # Keep track of frames to determine when to reinit ROI
while True:
    # Read a new frame
    ok, frame = video.read()
    frame_counter += 1
    while not ok:
        # If no frame captured continue to loop until we capture a frame
        ok, frame = video.read()
        print('Frame not captured!')
        break

    # Start timer
    timer = cv2.getTickCount()

    # Update tracker
    ok, bbox = tracker.update(frame)

    # Calculate Frames per second (FPS)
    fps = cv2.getTickFrequency() / (cv2.getTickCount() - timer);

    # Draw bounding box
    if ok:
        # Tracking success
        p1 = (int(bbox[0]), int(bbox[1]))
        p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
        cv2.rectangle(frame, p1, p2, (255,0,0), 2, 1)
    else :
        # Tracking failure
        cv2.putText(frame, "Tracking failure detected", (100,80), cv2.FONT_HERSHEY_SIMPLEX, 0.75,(0,0,255),2)
        
        # Upon tracking failure, use Haar to find a new ROI 
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        faces = face_cascade.detectMultiScale(gray, 1.1, 4)
        if len(faces) == 0:
            # If no faces are detected, simply continue to loop to load in the next frame
            # Could also infinitely loop here and contiue to process frames until a face is found
            pass
        else:
            bbox = faces[0] # Define ROI as the first detected face
            frame_counter = 0 # Reset frame counter
            # Initialize tracker with first frame and bounding box
            tracker = cv2.TrackerKCF_create()
            ok = tracker.init(frame, bbox)

    ### BELOW IS AN ALTERNATE METHOD FOR TRACKER REINITIALIZATION ###
    # Below method is not as good for tracking the same person if two people are in frame 
    # # If we have processed 10 frames, reinitialize the ROI
    # if frame_counter >= 10:
    #     # cv2.putText(frame, "Reinitializing", (100,110), cv2.FONT_HERSHEY_SIMPLEX, 0.75,(255,165,0),2)
    #     # Uncomment above if you want an idea of how often we reinitialize
    #     gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    #     faces = face_cascade.detectMultiScale(gray, 1.1, 4)
    #     if len(faces) == 0:
    #         # If no faces are detected, simply continue to loop to load in the next frame
    #         # Could also infinitely loop here and contiue to process frames until a face is found
    #         pass
    #     else:
    #         bbox = faces[0] # Define ROI as the first detected face
    #         frame_counter = 0 # Reset frame counter
    #         # Initialize tracker with first frame and bounding box
    #         tracker = cv2.TrackerKCF_create()
    #         ok = tracker.init(frame, bbox)
    #################################################################

    # Display FPS on frame
    cv2.putText(frame, "FPS : " + str(int(fps)), (100,50), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (50,170,50), 2);
    # Display result
    cv2.imshow("Tracking", frame)
    # Exit if ESC pressed
    k = cv2.waitKey(1) & 0xff
    if k == 27 : break