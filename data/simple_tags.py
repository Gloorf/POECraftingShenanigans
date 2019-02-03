import json
"""
Convert a Tags.json exported from the game to a more easy version to work with, SimpleTags.json
"""

class SimpleTag(object):
    def __init__(self, data, headers):
        if not data:
            return
        for index, name in enumerate(headers):
            setattr(self, name, data[index])


def load_tags(filename):
    with open(filename, "r") as f:
        data = json.load(f)
    data = data[0]
    headers_name = []
    for header in data["header"]:
        headers_name.append(header["name"])
    out = [SimpleTag(x, headers_name) for x in data["data"]]
    return out


tags = load_tags("ggpk/Tags.json")
out = []
i = 0
for x in tags:
    out.append({"rowId": i, "Id": x.Id})
    i += 1

with open("SimpleTags.json", "w") as f:
    json.dump(out, f, indent=2)
