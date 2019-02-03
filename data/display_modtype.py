import json
import sys


class ModType(object):
    def __init__(self, data, headers):
        if not data:
            return
        for index, name in enumerate(headers):
            setattr(self, name, data[index])
    def __repr__(self):
        out = self.__class__.__name__ + "\n"
        for key, value in self.__dict__.items():
            out += "{} : {}\n".format(key, value)
        return out




def load_modtype(filename):
    with open(filename, "r") as f:
        data = json.load(f)
    data = data[0]
    headers_name = []
    for header in data["header"]:
        headers_name.append(header["name"])
    out = [ModType(x, headers_name) for x in data["data"]]
    return out


mod_type = load_modtype("ggpk/ModType.json")

for index, x in enumerate(mod_type):
    for element in sys.argv[1:]:
        if index == int(element):
            print(x)
