import requests
from PIL import Image, ImageDraw, ImageFont

mac = "00000197E5CB3B38"   # destination mac address
dither = 0   # set dither to 1 is you're sending photos etc
apip = "192.168.178.192"   # ip address of your access point

# Create a new paletted image with indexed colors
image = Image.new('P', (296, 128))

# Define the color palette (white, black, red)
palette = [
    255, 255, 255,  # white
    0, 0, 0,        # black
    255, 0, 0       # red
]

# Assign the color palette to the image
image.putpalette(palette)

# Initialize the drawing context
draw = ImageDraw.Draw(image)

# Define the text lines
line1 = 'OpenEPaperLink'
line2 = 'Demo image'

# Define the fonts and sizes
font_line1 = ImageFont.truetype('arial.ttf', size=36)  # Change the font file and size as per your preference
font_line2 = ImageFont.truetype('arial.ttf', size=16)  # Change the font file and size as per your preference

# Calculate the text bounding boxes to get the text widths and heights
text_bbox_line1 = draw.textbbox((0, 0), line1, font=font_line1)
text_bbox_line2 = draw.textbbox((0, 0), line2, font=font_line2)

# Calculate the text positions to center the lines horizontally
text_position_line1 = ((image.width - (text_bbox_line1[2] - text_bbox_line1[0])) // 2, 20)
text_position_line2 = ((image.width - (text_bbox_line2[2] - text_bbox_line2[0])) // 2, 80)

# Write the text on the image
draw.text(text_position_line1, line1, fill=2, font=font_line1)  # Use palette index 1 for black color
draw.text(text_position_line2, line2, fill=1, font=font_line2)  # Use palette index 2 for red color

# Convert the image to 24-bit RGB
rgb_image = image.convert('RGB')

# Save the image as JPEG with maximum quality
image_path = 'output.jpg'
rgb_image.save(image_path, 'JPEG', quality="maximum")

# Prepare the HTTP POST request
url = "http://" + apip + "/imgupload"
payload = {"dither": dither, "mac": mac}  # Additional POST parameter
files = {"file": open(image_path, "rb")}  # File to be uploaded

# Send the HTTP POST request
response = requests.post(url, data=payload, files=files)

# Check the response status
if response.status_code == 200:
    print("Image uploaded successfully!")
else:
    print("Failed to upload the image.")

