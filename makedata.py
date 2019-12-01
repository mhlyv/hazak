#!/usr/bin/env python3

import random

with open("data.txt", "w") as f:
    for _ in range(10000):
        data = []
        # m^2
        data.append(random.randint(50, 1000))
        # price
        data.append(data[0] * random.randint(70000, 900000))
        # rooms
        data.append(random.randint(1, 3)) # paperwork price


        # OPTIONAL
        # 3 choices: -n not avalible
        #             0 included
        #            +n price
        # extra room price
        data.append(random.randint(-1, 1) * random.randint(20000, 100000))
        # garage price
        data.append(random.randint(-1, 1) * random.randint(20000, 100000))
        # shed price
        data.append(random.randint(-1, 1) * random.randint(20000, 100000))

        for i, d in enumerate(data):
            data[i] = str(d)

        f.write(' '.join(data) + "\n")
