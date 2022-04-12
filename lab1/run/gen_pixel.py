try:
    from PIL import Image
except ImportError:
    import Image
import sys
if len(sys.argv)<3:
    print('Usage: python3 %s input.img output.pixel' % (sys.argv[0]))
    sys.exit()

img = Image.open(sys.argv[1])
im_gray = img.convert('L')
width, height = im_gray.size
pixels = list(im_gray.getdata())
pixels = [pixels[i * width:(i + 1) * width] for i in range(height)]
print((height, width))

output_f = open(sys.argv[2], 'w+')
output_f.write("%d %d\n" % (height, width))

for line in pixels:
    output_f.write(" ".join(str(pixel) for pixel in line) + '\n')

output_f.close()
