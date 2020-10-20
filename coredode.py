import sensor, image, time
import json
import random
import math as m
from pyb import UART
uart = UART(3, 115200)
square_threshold   =(0, 91, -17, 23, 127, -64)
triangle_threshold   = (6, 91, -17, 23, 127, -27)
circle_threshold   = (0, 100, -128, 28, 127, -25)
sensor.reset()

sensor.set_pixformat(sensor.RGB565)

sensor.set_framesize(sensor.QQVGA)

sensor.skip_frames(time = 2000)

sensor.set_auto_gain(False) # must be turned off for color tracking

sensor.set_auto_whitebal(False) # must be turned off for color tracking

clock = time.clock()
#img = sensor.snapshot().lens_corr(1.8)
K=0.251322
K1=0.24868
K2=0.25231
circount=0
rectcount=0
tricount=0
while(True):
    clock.tick()
    img = sensor.snapshot()
    #for r in img.find_rects(roi=(55,20,30,30),threshold = 9000):#square
        #img.draw_rectangle(r.rect(), color = (0, 225, 0))
        #for p in r.corners(): img.draw_circle(p[0], p[1], 1, color = (0, 0, 0))
    if(circount>=1):
        circount=0
        rectcount=0
        tricount=0
        data_out1 = json.dumps("circle")
        uart.write(data_out1)
        print('you send:',data_out1)
    if(rectcount>=1):
        circount=0
        rectcount=0
        tricount=0
        data_out2 = json.dumps("square")
        uart.write(data_out2)
        print('you send:',data_out2)
    if(tricount>=1):
        circount=0
        rectcount=0
        tricount=0
        data_out3 = json.dumps("triangle")
        uart.write(data_out3)
        print('you send:',data_out3)
    for j in img.find_rects(roi=(55,20,30,30),threshold = 10000):
        img.draw_rectangle(j.rect(),color = (255,0,0))
        xc=0
        yc=0
        for p in j.corners():
            xc+=p[0]
            yc+=p[1]
            xc/=4
            yc/=4
            img.draw_circle(m.ceil(xc),m.ceil(yc),1,color=(0,255,0))
            blobs = img.find_blobs([square_threshold])
        if len(blobs) == 1:
        # Draw a rect around the blob.
             b = blobs[0]
        #img.draw_rectangle(b[0:4]) # rect
        #img.draw_cross(b[5], b[6]) # cx, cy
             Lm = (b[2]+b[3])/2
             length = K*Lm
             print("square",length)
           #print(Lm)
             uart.write("square")
             uart.write(str(length))
        rectcount+=1
    sum1=0
    for k in img.find_line_segments(roi=(70,30,30,20),merge_distance = 0,max_theta_diff = 10):
        img.draw_line(k.line(),color=(0,255,0))
        #print(k.theta())
        sum1+=k.theta()
    sum1-=90
    #print(sum1)
    if(sum1<190 and sum1>100):#sum1<200 and sum1>160
        blobs = img.find_blobs([triangle_threshold])
        if len(blobs) == 1:
        # Draw a rect around the blob.
            b = blobs[0]
        #img.draw_rectangle(b[0:4]) # rect
        #img.draw_cross(b[5], b[6]) # cx, cy
            Lm = (b[2]+b[3])/2
            length1 = K1*Lm
            uart.write("triangle")
            uart.write(str(length1))
            print("triangle",length1)
            #print(Lm)
        tricount+=1

    for c in img.find_circles(threshold =3500, x_margin = 10, y_margin = 10, r_margin = 10,r_min = 2, r_max = 100, r_step = 1):
        area = (c.x()-c.r(), c.y()-c.r(), 2*c.r(), 2*c.r())
        img.draw_rectangle(area, color = (0, 0, 225))
        blobs = img.find_blobs([circle_threshold])
        if len(blobs) == 1:
            # Draw a rect around the blob.
            b = blobs[0]
            #img.draw_rectangle(b[0:4]) # rect
            #img.draw_cross(b[5], b[6]) # cx, cy
            Lm = (b[2]+b[3])/2
            length2 = K2*Lm
            uart.write("circle")
            uart.write(str(length2))
            print("circle",length2)
        circount+=1
            #print(Lm)
    time.sleep(200)

    #output_str="%d\n" % (sum1)
