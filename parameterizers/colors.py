

def rgb_2_triple(color):
    """
    #ffff00 -> (1, 1, 0)
    """
    c_str = (color[1:3], color[3:5], color[5:7])
    c = lambda h: int(h, 16) / 255
    return (c(c_str[0]), c(c_str[1]), c(c_str[2]))


def triple_2_rgb(color):
    """
    (1, 1, 0) -> #ffff00
    """
    def c(h):
        h = int(h * 255)
        if h < 16:
            return "0" + hex(h)[2:]
        else:
            return hex(h)[2:]
    return "#" + c(color[0]) + c(color[1]) + c(color[2])


def toggle_color(color):
    if type(color) == str:
        return rgb_2_triple(color)
    else:
        return triple_2_rgb(color)


blue_color = toggle_color((0.2, 0.3, 0.7))
red_color = toggle_color((0.7, 0.3, 0.2))
green_color = toggle_color((0.2, 0.7, 0.3))

