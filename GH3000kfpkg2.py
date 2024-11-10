import sensor
import image
import time
import KPU as kpu
import os

# Initialize the camera
sensor.reset()
sensor.set_pixformat(sensor.RGB565)  # Set pixel format to RGB565
sensor.set_framesize(sensor.QVGA)    # Set frame size to QVGA (320x240)
sensor.set_brightness(2)             # Increase brightness (-3 to 3, 2 is a moderate value)
sensor.set_contrast(1)               # Adjust contrast (-3 to 3, 1 is moderate)
sensor.set_saturation(1)
# Enable face detection
sensor.set_vflip(1)                  # Flip the image if necessary (adjust for your camera)
sensor.set_hmirror(1)                # Mirror the image if necessary
sensor.run(1)  # Start the sensor
sensor.skip_frames(time=2000)  # Give the sensor time to stabilize

# Load the KPU task (model) from flash memory
task = kpu.load(0x300000)

# Define anchors for YOLO detection (pre-trained model)
anchor = (1.889, 2.5245, 2.9465, 3.94056, 3.99987, 5.3658, 5.155437, 6.92275, 6.718375, 9.01025)
a = kpu.init_yolo2(task, 0.5, 0.3, 5, anchor)  # Initialize YOLO with thresholds

# Function to pad the time components with leading zeros if necessary
def pad_zero(value):
    value_str = str(value)
    if len(value_str) == 1:  # Check if it's a single digit
        return "0" + value_str
    return value_str

# Function to generate a timestamp in dd.mm.yy_HH.mm.ss format
def get_formatted_time():
    current_time = time.localtime()  # Get the local time (tuple format)
    day = pad_zero(current_time[2])  # Day
    month = pad_zero(current_time[1])  # Month
    year = pad_zero(current_time[0] % 100)  # Year (last two digits)
    hour = pad_zero(current_time[3])  # Hour
    minute = pad_zero(current_time[4])  # Minute
    second = pad_zero(current_time[5])  # Second
    return day + "." + month + "." + year + "_" + hour + "." + minute + "." + second

# Function to save image to SD card with formatted filename
def save_image_to_sd(img):
    filename = get_formatted_time() + ".jpg"  # Generate filename in dd.mm.yy_HH.mm.ss format
    path = "/sd/" + filename  # Set the file path
    img.save(path)  # Save the image as a JPEG file
    print("Image saved as " + path)

# Main loop
while True:
    img = sensor.snapshot()  # Capture an image

    if img:  # Check if an image is captured successfully
        code = kpu.run_yolo2(task, img)  # Run YOLO face detection

        if code:  # If a face is detected
            for i in code:
                print("Face detected:", i)  # Print face detection result to serial
                img.draw_rectangle(i.rect())  # Draw rectangle around detected face
                x, y, w, h = i.rect()  # Get face coordinates
                print("Face at (x: {}, y: {}, w: {}, h: {})".format(x, y, w, h))  # Send face coordinates to serial

            # Save image immediately upon face detection
            save_image_to_sd(img)  # Save the image to the SD card

        else:
            print("No face detected in the image")

    else:
        print("Failed to capture image")  # Debugging message if image capture fails

# Deinitialize the KPU task (clean up)
a = kpu.deinit(task)
