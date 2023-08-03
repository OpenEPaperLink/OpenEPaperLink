from PIL import Image
from bitarray import bitarray
import os

"""
helper for loading pictures from the filesystem.
"""


class PicFS:
    def __init__(self):
        # caches preload and -converted image-data for faster retrieval
        # this is actually quite critical, as RQB (AP BlockRequests) must
        # be responded to very timely (within a couple dozen milliseconds)
        self.data_cache = {}

    def _get_path(self, image_path):
        # TODO: make picture path better / configurable
        return "cache/" + image_path

    def has_image(self, image_path):
        return os.path.exists(self._get_path(image_path))

    def get_image(self, image_path, data_type):
        """
        gets the given image from cache (if present)
        or loads it otherwise.
        TODO: add a mechanism to dispose of stale entries,
              as they accumulate memory indefinitely right now
        """
        mtime = os.path.getmtime(self._get_path(image_path))
        key = (image_path, data_type, mtime)
        if key in self.data_cache:
            return self.data_cache[key]
        data = self.load_image(image_path, data_type)
        self.data_cache[key] = data
        return data
    
    def remove_image(self, image_path):
        """
        removes the given image from cache (if present)
        """
        for key in list(self.data_cache.keys()):
            if key[0] == image_path:
                del self.data_cache[key]
        os.remove(self._get_path(image_path))

    def load_image(self, image_path, data_type):
        if data_type == 0x20:
            # just black
            colors = [(0x00, 0x00, 0x00)]
        elif data_type == 0x21:
            colors = [(0x00, 0x00, 0x00), (0xFF, 0x00, 0x00)]
        elif data_type == 0x03:
            # firmware update
            colors = []
        else:
            raise "no such data_type " + str(data_type)
            
        if data_type == 0x03:
            with open(self._get_path(image_path), 'rb') as file:
                return bytearray(file.read())
        else:

            image = Image.open(self._get_path(image_path))
            width, height = image.size
            pixel_colors = bitarray()

            # the tag picture format is:
            # - one full frame per color
            # - each frame consist of $height columns of $with pixels
            #   Note: so the image is rotated by 90-deg
            # - each column (line from src image) is transferred in reverse
            # - each pixel is one bit (the epaper only knows "WHITE" (0) or "COLOR" (1))
            # due to the below algorithm, all colors, except for exact matches of the defined
            # colors, are ignored (remain white)
            for c in colors:  # one frame per color
                for x in range(width - 1, -1, -1):  # transfer lines (columns in output) in reverse
                    for y in range(height):
                        pixel = image.getpixel((x, y))
                        is_similar = abs(pixel[0] - c[0]) < 128 \
                                    and abs(pixel[1] - c[1]) < 128 \
                                    and abs(pixel[2] - c[2]) < 128
                        # each pixel is on bit (True or False)
                        pixel_colors.append(is_similar)
            return pixel_colors.tobytes()  # bitarray handles packing of 8-booleans to one byte
