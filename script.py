# Script vibecoded apenas para gerar o range de cores necessário
import sys

def parse_vec4(s):
    parts = s.strip().split(',')
    return tuple(int(p) for p in parts)

def lerp(a, b, t):
    return a + (b - a) * t

def lerp_vec4(c1, c2, t):
    return tuple(lerp(c1[i], c2[i], t) for i in range(4))

def format_color(c):
    return f"{{0x{int(c[0]):02x}, 0x{int(c[1]):02x}, 0x{int(c[2]):02x}, 0x{int(c[3]):02x}}},"

while True:
    try:
        color1_str = sys.stdin.readline()
        if not color1_str:
            break
        color2_str = sys.stdin.readline()
        if not color2_str:
            break
        steps_str = sys.stdin.readline()
        if not steps_str:
            break
        
        color1 = parse_vec4(color1_str)
        color2 = parse_vec4(color2_str)
        steps = int(steps_str.strip())
        
        for i in range(steps):
            t = i / (steps - 1) if steps > 1 else 0
            result = lerp_vec4(color1, color2, t)
            print(format_color(result))
    except:
        break
