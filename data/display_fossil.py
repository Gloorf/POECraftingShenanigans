import json
import sys


class Fossil(object):
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




def load_fossils(filename):
    with open(filename, "r") as f:
        data = json.load(f)
    data = data[0]
    headers_name = []
    for header in data["header"]:
        headers_name.append(header["name"])
    out = [Fossil(x, headers_name) for x in data["data"]]
    return out


all_mods = load_fossils("ggpk/DelveCraftingModifiers.json")

for index, x in enumerate(all_mods):
    if len(sys.argv) > 1:
        if sys.argv[1] == "all":
            print(x)
        else:
            for element in sys.argv[1:]:
                try:
                    if index == int(element):
                        print(x, index)
                except:
                    pass
