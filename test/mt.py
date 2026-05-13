
with open("perlin_noise.txt", "r") as f:
    lines = f.readlines()
for line in lines:
    a = line.split(' ')
    print(len(a))
