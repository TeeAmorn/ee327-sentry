# source: https://docs.opencv.org/4.x/d7/d00/tutorial_meanshift.html
import numpy as np
import cv2 as cv
import argparse
cap = cv.VideoCapture(0) #set to zero for video capture, else input an image path

# take first frame of the video
ret,frame = cap.read()

# setup initial location of window
x, y, w, h = 300, 200, 100, 50 # simply hardcoded the values
track_window = (x, y, w, h)

# set up the ROI for tracking (region of interest)
roi = frame[y:y+h, x:x+w] # Arbitrary hard coded values
hsv_roi =  cv.cvtColor(roi, cv.COLOR_BGR2HSV) #HSV = Hue Saturation Value, focus on Hue only for meanshift
mask = cv.inRange(hsv_roi, np.array((0., 60.,32.)), np.array((180.,255.,255.))) # Eliminate low light values. Not really sure what mask means though
roi_hist = cv.calcHist([hsv_roi],[0],mask,[180],[0,180])
cv.normalize(roi_hist,roi_hist,0,255,cv.NORM_MINMAX)

# Setup the termination criteria, either 10 iteration or move by at least 1 pt
# Terminate as in terminating the algorithm, not the program
# Doing EPS | Count makes the algorithm terminate on whichever condition happens first
# The last two values are the values at which the termination happens
term_crit = ( cv.TERM_CRITERIA_EPS | cv.TERM_CRITERIA_COUNT, 10, 1 ) 

while(1):
    ret, frame = cap.read()
    if ret == True: # ret = true means that a frame was captured. False means no frame captured so something went wrong
        hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
        dst = cv.calcBackProject([hsv],[0],roi_hist,[0,180],1)

        # apply meanshift to get the new location
        ret, track_window = cv.meanShift(dst, track_window, term_crit) # No idea why rectangle value is needed?

        # Draw it on image
        x,y,w,h = track_window
        img2 = cv.rectangle(frame, (x,y), (x+w,y+h), 255,2)
        cv.imshow('img2',img2)

        # Is esc key is pressed, stop the program
        k = cv.waitKey(30) & 0xff
        if k == 27:
            break
    else:
        break