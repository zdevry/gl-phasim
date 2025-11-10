from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

with Image.open('assets/textsheet.png') as im:
    with im.convert('L') as lim:
        npa = np.asarray(lim)

bs = npa.tobytes()

with open('assets/textsheet_960x120_R8.bin', 'wb') as f:
    f.write(bs)
